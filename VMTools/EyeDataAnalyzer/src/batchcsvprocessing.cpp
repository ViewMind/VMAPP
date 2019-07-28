#include "batchcsvprocessing.h"

BatchCSVProcessing::BatchCSVProcessing()
{

}


void BatchCSVProcessing::run(){

    errors.clear();
    processingList.clear();

    recursiveFillProcessingList(workingDirectory);
    /// DEBUG Printing results.
    //    QStringList cats = processingList.keys();
    //    for (qint32 i = 0; i < cats.size(); i++){
    //        qDebug() << "CATEGORY: " + cats.at(i);
    //        for (qint32 j = 0; j < processingList.value(cats.at(i)).size(); j++){
    //            qDebug() << "   " +  processingList.value(cats.at(i)).at(j).toString();
    //        }
    //    }

    // Processing the information

    // Getting the total number of files to process.
    qreal total = 0;
    QStringList cats = processingList.keys();
    for (qint32 i = 0; i < cats.size(); i++){
        total = total + processingList.value(cats.at(i)).size();
    }

    qint32 counter = 0;
    for (qint32 i = 0; i < cats.size(); i++){

        QString outputCSVFile = workingDirectory + "/" + cats.at(i) + ".csv";
        QString outputCSVContents = "";
        bool isReading = cats.at(i).contains("RD");

        for (qint32 j = 0; j < processingList.value(cats.at(i)).size(); j++){

            DatFileProcessingStruct dfps = processingList.value(cats.at(i)).value(j);
            //qWarning() << "Generating csv for file" << dfps.filePath << ". ISREADING" << isReading;
            QString csvFile = generateLocalCSV(dfps,isReading);
            if (csvFile.isEmpty()) continue;

            //qWarning() << "Appending data of file" << csvFile;
            outputCSVContents = appendCSV(csvFile,dfps.displayID,outputCSVContents);

            counter++;
            qint32 percent = counter*100/total;
            emit(updateAdvance(percent));

        }

        if (!outputCSVContents.isEmpty()){
            QFile outFile(outputCSVFile);
            if (!outFile.open(QFile::WriteOnly)){
                errors << "Could not open output csv " + outputCSVFile + " for appending";
                continue;
            }
            QTextStream writer(&outFile);
            writer << outputCSVContents;
            outFile.close();
            messages << "Generated output file " + outputCSVFile;
        }
        else errors << "No content to fill file " << outputCSVFile;

    }

}


QString BatchCSVProcessing::generateLocalCSV(BatchCSVProcessing::DatFileProcessingStruct dfps, bool isReading){
    ConfigurationManager config;
    if (!config.loadConfiguration(dfps.configurationFile,COMMON_TEXT_CODEC)){
        errors << "Error Loading configuration: " << config.getError();
        return "";
    }

    MonitorGeometry mgeo;
    MovingWindowParameters mwp;

    mgeo.distanceToMonitorInMilimiters = config.getReal(CONFIG_DISTANCE_2_MONITOR)*10;
    mgeo.XmmToPxRatio                  = config.getReal(CONFIG_XPX_2_MM);
    mgeo.YmmToPxRatio                  = config.getReal(CONFIG_YPX_2_MM);

    mwp.maxDispersion                  = config.getInt(CONFIG_MOVING_WINDOW_DISP);
    mwp.minimumFixationLength          = config.getReal(CONFIG_MIN_FIXATION_LENGTH);
    mwp.sampleFrequency                = config.getReal(CONFIG_SAMPLE_FREQUENCY);
    mwp.calculateWindowSize();

    QString exp;
    QString data;
    QString genCSV;

    EDPBase *processor;
    RawDataProcessor rdp;
    rdp.initialize(&config);

    if (isReading){
        processor = new EDPReading(&config);
        rdp.separateInfoByTag(dfps.filePath,HEADER_READING_EXPERIMENT,&data,&exp);
    }
    else{
        processor = new EDPImages(&config);
        rdp.separateInfoByTag(dfps.filePath,HEADER_IMAGE_EXPERIMENT,&data,&exp);
    }

    processor->setFieldingMarginInMM(config.getReal(CONFIG_MARGIN_TARGET_HIT));
    processor->setMonitorGeometry(mgeo);
    processor->setMovingWindowParameters(mwp);
    processor->calculateWindowSize();
    processor->setPixelsInSacadicLatency(config.getInt(CONFIG_LATENCY_ESCAPE_RAD));
    processor->configure(dfps.filePath,exp);

    if (!processor->doEyeDataProcessing(data)){
        errors << processor->getError();
        return "";
    }
    genCSV = processor->getOuputMatrixFileName();

    delete processor;
    return genCSV;

}

QString BatchCSVProcessing::appendCSV(const QString &fileToAppend, const QString &displayID, const QString &csvdata){


    // Reading the input file.
    QFile inFile(fileToAppend);
    if (!inFile.open(QFile::ReadOnly)){
        errors << "Could not open input csv " + fileToAppend + " for reading and appending";
        return csvdata;
    }
    QTextStream reader(&inFile);
    reader.setCodec(COMMON_TEXT_CODEC);
    QString allcsv = reader.readAll();
    inFile.close();;


    QStringList lines = allcsv.split("\n");
    if (lines.size() <= 1) {
        errors << "CSV File " + fileToAppend + " had 1 or less lines";
        return csvdata;
    }

    QStringList csvLines;
    if (csvdata.isEmpty()) csvLines << "display_id,"+lines.first();
    else csvLines << csvdata;

    for (qint32 i = 1; i < lines.size(); i++){
        csvLines << displayID + "," + lines.at(i);
    }

    return csvLines.join("\n");

}

void BatchCSVProcessing::recursiveFillProcessingList(const QString &dir){

    //qWarning() << "Analyzing directory" << dir;

    QDir qdir(dir);
    QStringList filters; filters << "*.dat" << "*.datf";
    QStringList directories = qdir.entryList(QStringList(),QDir::NoDotAndDotDot|QDir::Dirs);
    QStringList datfiles = qdir.entryList(filters,QDir::Files);
    filters.clear(); filters << "eye_rep_gen_conf*";
    QStringList conf = qdir.entryList(filters,QDir::Files);

    if (conf.size() >= 1){

        if (conf.size() > 1){
            errors << "Directory " + dir  + " has more than one configuration file. Using: " << conf.first();
        }

        QString confFile = dir + "/" + conf.first();
        //qWarning() << "FOUND CONF: " << confFile;

        for (qint32 i = 0; i < datfiles.size(); i++){
            BatchCSVProcessing::DatFileProcessingStruct s;
            s.configurationFile = confFile;
            s.filePath = dir + "/" + datfiles.at(i);
            ConfigurationManager config = patdata->getPatientNameFromDirname(dir);
            if (idToUse == UNIFIED_CSV_ID_DID) s.displayID = config.getString(ID_DID);
            else if (idToUse == UNIFIED_CSV_ID_DBPUID) s.displayID = config.getString(ID_DBUID);
            else if (idToUse == UNIFIED_CSV_ID_HPUID) s.displayID = config.getString(ID_HPUID);
            else if (idToUse == UNIFIED_CSV_ID_NAME) s.displayID = config.getString(ID_NAME);
            else if (idToUse == UNIFIED_CSV_ID_PUID) s.displayID = config.getString(ID_PUID);
            DatFileInfoInDir::DatInfo dinfo = DatFileInfoInDir::getDatFileInformation(datfiles.at(i));
            processingList[dinfo.category].append(s);
        }

    }

    for (qint32 i = 0; i < directories.size(); i++){
        recursiveFillProcessingList(dir + "/" + directories.at(i));
    }

}
