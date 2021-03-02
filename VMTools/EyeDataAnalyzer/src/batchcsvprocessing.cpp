#include "batchcsvprocessing.h"

BatchCSVProcessing::BatchCSVProcessing()
{
    mrListEvals << "MMSE" << "ACE-R" << "IFS" << "BOSTON" << "Rey" << "RMN";
    mrListMeds << "Difenhidramina" << "Fenobarbital" << "Cannabinoides( CBD, THC, otros)" << "Amiodorona"
               << "Atenolol" << "Topiromato" << "Venlafaxina" << "Fluoxetina" << "Paroxetina" << "Sertralina"
               << "Rivastigmina" << "Memantina" << "Donepecilo" << "Diazepan" << "Alprazolam" << "Clonazepam"
               << "Lorazepan" << "Midazolan";

    mrListRNM <<  "RMN_Amyg"  <<  "RMN_HOC"  <<  "RMN_Hipp" <<
                  "RMN_InfLatVent" <<  "RMN_SupLatVent" <<
                  "RMN_LAmyg" <<  "RMN_LCorThick"  <<  "RMN_LEntCort"  <<  "RMN_LHip" <<
                  "RMN_RAmyg" <<  "RMN_RCorThick"  <<  "RMN_REntCort"  <<  "RMN_RHip" <<
                  "RMN_TotCertCort" <<  "RMN_TotGrayMatt"  <<  "RMN_TotWhiteMatt";
    headerMedRec << "medical_record_date" << "years_formation" << mrListMeds <<  mrListEvals << mrListRNM;

    // PATCH TO REMOVE COMMAS FROM HEADER
    headerMedRec[4] = "Cannabinoides_CBD_THC_otros";
    maxDayDiff = 60;

    // Default value for global maximum dispersion is -1 indicating not to use.
    maximumDispersionWindow = -1;

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
        int expType = -1;
        if (cats.at(i).contains("RD")) expType = EXP_READING;
        else if (cats.at(i).contains("BC")) expType = EXP_BINDING_BC;
        else if (cats.at(i).contains("UC")) expType = EXP_BINDING_UC;
        else if (cats.at(i).contains("NB")) expType = EXP_NBACKRT;
        else if (cats.at(i).contains("GN")) expType = EXP_GONOGO;
        else {
            qDebug() << "Unknown category: " + cats.at(i);
            errors << "Unknown category: " + cats.at(i);
        }

        if (expType == -1) continue;

        for (qint32 j = 0; j < processingList.value(cats.at(i)).size(); j++){

            DatFileProcessingStruct dfps = processingList.value(cats.at(i)).value(j);
            //qWarning() << "Generating csv for file" << dfps.filePath << ". ISREADING" << isReading;
            QString csvFile = generateLocalCSV(dfps,expType);
            if (csvFile.isEmpty()) continue;

            //qWarning() << "Appending data of file" << csvFile;
            outputCSVContents = appendCSV(csvFile,dfps,outputCSVContents);

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

QStringList BatchCSVProcessing::getClosestMedicalRecord(const QString &dbpuid, const QString &studyDate){
    //    QDate date2 = QDate::fromString("2019-03-25","yyyy-MM-dd");
    //    QDate date1 = QDate::fromString("2019-03-08","yyyy-MM-dd");
    //    qint64 dayDiff = qAbs(date1.toJulianDay() - date2.toJulianDay());
    //    qDebug() << "DAY DIFF is" << dayDiff;
    qint64 evaluationDate = QDate::fromString(studyDate,"yyyy-MM-dd").toJulianDay();
    QVariantMap medRecord = patdata->getClosestMedicalRecord(dbpuid,evaluationDate,maxDayDiff);
    QVariantMap patientInfo = patdata->getPatientDatabaseRecord(dbpuid);
    QStringList ans;

    if (medRecord.contains(TPATMEDREC_COL_DATE)) ans << medRecord.value(TPATMEDREC_COL_DATE).toString();
    else ans << "";

    if (patientInfo.contains(TPATDATA_COL_FORMATIVE_YEARS)) ans << patientInfo.value(TPATDATA_COL_FORMATIVE_YEARS).toString();
    else ans << "";

    if (medRecord.contains(TPATMEDREC_COL_MEDICATION)){
        QString json = medRecord.value(TPATMEDREC_COL_MEDICATION).toString();
        QJsonDocument doc = QJsonDocument::fromJson(json.toLatin1());
        QVariantMap map = doc.object().toVariantMap();
        for (qint32 i = 0; i < mrListMeds.size(); i++){
            if (map.contains(mrListMeds.at(i))) ans << map.value(mrListMeds.at(i)).toString();
            else ans << "";
        }
    }
    else {
        for (qint32 i = 0; i < mrListMeds.size(); i++){
            ans << "";
        }
    }

    if (medRecord.contains(TPATMEDREC_COL_EVALS)){
        QString json = medRecord.value(TPATMEDREC_COL_EVALS).toString();
        QJsonDocument doc = QJsonDocument::fromJson(json.toLatin1());
        QVariantMap map = doc.object().toVariantMap();
        for (qint32 i = 0; i < mrListEvals.size(); i++){
            if (map.contains(mrListEvals.at(i))) ans << map.value(mrListEvals.at(i)).toString();
            else ans << "";
        }
    }
    else {
        for (qint32 i = 0; i < mrListEvals.size(); i++){
            ans << "";
        }
    }

    if (medRecord.contains(TPATMEDREC_COL_RNM)){
        QString json = medRecord.value(TPATMEDREC_COL_RNM).toString();
        QJsonDocument doc = QJsonDocument::fromJson(json.toLatin1());
        QVariantMap map = doc.object().toVariantMap();
        for (qint32 i = 0; i < mrListRNM.size(); i++){
            if (map.contains(mrListRNM.at(i))) ans << map.value(mrListRNM.at(i)).toString();
            else ans << "";
        }
    }
    else {
        for (qint32 i = 0; i < mrListRNM.size(); i++){
            ans << "";
        }
    }




    return ans;
}

QString BatchCSVProcessing::generateLocalCSV(BatchCSVProcessing::DatFileProcessingStruct dfps, int exp_type){
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


    if (maximumDispersionWindow == -1){
        mwp.maxDispersion              = config.getInt(CONFIG_MOVING_WINDOW_DISP);
    }
    else{
        qDebug() << "Using forced dispersion of"  << maximumDispersionWindow;
        mwp.maxDispersion              = maximumDispersionWindow;
    }

    mwp.minimumFixationLength          = config.getReal(CONFIG_MIN_FIXATION_LENGTH);
    mwp.sampleFrequency                = config.getReal(CONFIG_SAMPLE_FREQUENCY);

    mwp.calculateWindowSize();

    QString exp;
    QString data;
    QString genCSV;

    EDPBase *processor;
    RawDataProcessor rdp;
    rdp.initialize(&config);


    if (exp_type == EXP_READING){
        processor = new EDPReading(&config);
        rdp.separateInfoByTag(dfps.filePath,HEADER_READING_EXPERIMENT,&data,&exp);
    }
    else if ((exp_type == EXP_BINDING_UC) || (exp_type == EXP_BINDING_BC)){
        processor = new EDPImages(&config);
        rdp.separateInfoByTag(dfps.filePath,HEADER_IMAGE_EXPERIMENT,&data,&exp);
    }
    else if (exp_type == EXP_NBACKRT){
        processor = new EDPNBackRT(&config);
        rdp.separateInfoByTag(dfps.filePath,HEADER_NBACKRT_EXPERIMENT,&data,&exp);
    }
    else if (exp_type == EXP_GONOGO){
        processor = new EDPGoNoGo(&config);
        rdp.separateInfoByTag(dfps.filePath,HEADER_GONOGO_EXPERIMENT,&data,&exp);
    }
    else{
        errors << "Unsuported experiment type for mass csv generator: " + QString::number(exp_type);
        return "";
    }

    mgeo.resolutionHeight =  rdp.getConfiguration()->getReal(CONFIG_RESOLUTION_HEIGHT);
    mgeo.resolutionWidth  =  rdp.getConfiguration()->getReal(CONFIG_RESOLUTION_WIDTH);

    processor->setFieldingMargin(config.getInt(CONFIG_MARGIN_TARGET_HIT));
    processor->setMonitorGeometry(mgeo);
    processor->setMovingWindowParameters(mwp);
    processor->calculateWindowSize();
    processor->setPixelsInSacadicLatency(config.getInt(CONFIG_LATENCY_ESCAPE_RAD));
    processor->configure(dfps.filePath,exp);

    if (!data.isEmpty()){
        if (!processor->doEyeDataProcessing(data)){
            errors << processor->getError();
            return "";
        }

        genCSV = processor->getOuputMatrixFileName();
    }
    else{
        //qWarning() << "Could not get any data from" << dfps.filePath;
        messages << "EMPTY FILE: " + dfps.filePath;
        genCSV = "";
    }

    delete processor;
    return genCSV;

}

QString BatchCSVProcessing::appendCSV(const QString &fileToAppend, const DatFileProcessingStruct &dfps, const QString &csvdata){


    // Reading the input file.
    QFile inFile(fileToAppend);
    if (!inFile.open(QFile::ReadOnly)){
        errors << "Could not open input csv " + fileToAppend + " for reading and appending";
        return csvdata;
    }
    QTextStream reader(&inFile);
    reader.setCodec(COMMON_TEXT_CODEC);
    QString allcsv = reader.readAll();
    inFile.close();


    QStringList lines = allcsv.split("\n");
    if (lines.size() <= 1) {
        errors << "CSV File " + fileToAppend + " had 1 or less lines";
        return csvdata;
    }

    // Getting medical record information.


    QStringList csvLines;
    if (csvdata.isEmpty()) csvLines << "display_id,age,"+lines.first() + "," + headerMedRec.join(",");
    else csvLines << csvdata;

    QStringList medicalRecord = getClosestMedicalRecord(dfps.dbpuid,dfps.studyDate);

    for (qint32 i = 1; i < lines.size(); i++){
        csvLines << dfps.displayID + "," + dfps.age  + "," + lines.at(i) + "," + medicalRecord.join(",");
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

            ConfigurationManager temp;
            if (!temp.loadConfiguration(confFile,COMMON_TEXT_CODEC)){
                errors << "Could not load configuration file " + confFile + " with error: " + temp.getError() + " for getting patient age";
                s.age = "0";
            }
            else s.age = temp.getString(CONFIG_PATIENT_AGE);

            s.filePath = dir + "/" + datfiles.at(i);
            ConfigurationManager config = patdata->getPatientNameFromDirname(dir);
            if (idToUse == UNIFIED_CSV_ID_DID) s.displayID = config.getString(ID_DID);
            else if (idToUse == UNIFIED_CSV_ID_DBPUID) s.displayID = config.getString(ID_DBUID);
            else if (idToUse == UNIFIED_CSV_ID_HPUID) s.displayID = config.getString(ID_HPUID);
            else if (idToUse == UNIFIED_CSV_ID_NAME) s.displayID = config.getString(ID_NAME);
            else if (idToUse == UNIFIED_CSV_ID_PUID) s.displayID = config.getString(ID_PUID);
            s.dbpuid = config.getString(ID_DBUID);

            // Getting the directory name, in order to get the time stamp.
            QStringList dirnameParts = qdir.dirName().split("_");
            if (dirnameParts.size() == 6){
                s.studyDate = dirnameParts.at(0) + "-" + dirnameParts.at(1) + "-" + dirnameParts.at(2);
            }
            else s.studyDate = "";

            DatFileInfoInDir::DatInfo dinfo = DatFileInfoInDir::getDatFileInformation(datfiles.at(i));
            processingList[dinfo.category].append(s);

            if (dinfo.category == "MS"){
                qDebug() << datfiles.at(i) << "@" << dir;
            }

        }

    }

    for (qint32 i = 0; i < directories.size(); i++){
        recursiveFillProcessingList(dir + "/" + directories.at(i));
    }

}
