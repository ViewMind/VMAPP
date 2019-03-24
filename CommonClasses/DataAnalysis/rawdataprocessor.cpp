#include "rawdataprocessor.h"

RawDataProcessor::RawDataProcessor(QObject *parent):QObject(parent)
{
}


void RawDataProcessor::initialize(ConfigurationManager *c){
    config = c;

    QString currentDir = config->getString(CONFIG_PATIENT_DIRECTORY);
    if (config->containsKeyword(CONFIG_FILE_READING))
        dataReading = currentDir + "/" + config->getString(CONFIG_FILE_READING);
    else dataReading = "";

    if (config->containsKeyword(CONFIG_FILE_BIDING_BC))
        dataBindingBC = currentDir + "/" + config->getString(CONFIG_FILE_BIDING_BC);
    else dataBindingBC = "";

    if (config->containsKeyword(CONFIG_FILE_BIDING_UC))
        dataBindingUC = currentDir + "/" + config->getString(CONFIG_FILE_BIDING_UC);
    else dataBindingUC = "";

    if (config->containsKeyword(CONFIG_FILE_FIELDING))
        dataFielding = currentDir + "/" + config->getString(CONFIG_FILE_FIELDING);
    else dataFielding = "";

    matrixBindingBC = "";
    matrixReading = "";
    matrixFielding = "";
    matrixBindingUC = "";

    mgeo.distanceToMonitorInMilimiters = config->getReal(CONFIG_DISTANCE_2_MONITOR)*10;
    mgeo.XmmToPxRatio                  = config->getReal(CONFIG_XPX_2_MM);
    mgeo.YmmToPxRatio                  = config->getReal(CONFIG_YPX_2_MM);

    mwp.maxDispersion                  = config->getInt(CONFIG_MOVING_WINDOW_DISP);
    mwp.minimumFixationLength          = config->getReal(CONFIG_MIN_FIXATION_LENGTH);
    mwp.sampleFrequency                = config->getReal(CONFIG_SAMPLE_FREQUENCY);
    mwp.calculateWindowSize();
}


QString RawDataProcessor::formatBindingResultsForPrinting(const EDPImages::BindingAnswers &ans){
    QString report = "";
    report = report + "Number of correct answers in test trials: " + QString::number(ans.testCorrect) + "<br>";
    report = report + "Number of wrong answers in test trials: " + QString::number(ans.testWrong) + "<br>";
    report = report + "Number of correct answers : " + QString::number(ans.correct) + "<br>";
    report = report + "Number of wrong answers : " + QString::number(ans.wrong) + "<br>";
    return report;
}

void RawDataProcessor::run(){

    EyeMatrixProcessor emp((quint8)config->getInt(CONFIG_VALID_EYE));
    reportFileOutput = "";

    QStringList studyID;
    EyeMatrixProcessor::DBHash dbdata;
    RawDataProcessor::TagParseReturn tagRet;

    QString dateForReport;
    QStringList reportInfoText;

    bool freqErrorsOK = true;

    // Each of the processing fucntions is called. In this way, processing of just one can be accomplished
    // just by running this complete function but it will keep going if a file does not exist.
    if (!dataReading.isEmpty()) {
        emit(appendMessage("========== STARTED READING PROCESSING ==========",MSG_TYPE_SUCC));
        EDPReading reading(config);
        tagRet = csvGeneration(&reading,"Reading",dataReading,HEADER_READING_EXPERIMENT);
        matrixReading = tagRet.filePath;
        fixations[CONFIG_P_EXP_READING] = reading.getEyeFixations();
        QString report = emp.processReading(matrixReading,&dbdata);

        QFileInfo info(dataReading);
        DatFileInfoInDir::DatInfo datInfo = DatFileInfoInDir::getReadingInformation(info.baseName());
        dateForReport = datInfo.date + "_" + datInfo.hour;
        reportInfoText << "r";

        if (!tagRet.freqCheckErrors) freqErrorsOK = false;

        if (!report.isEmpty()){
            studyID << "rd" + tagRet.version;
            emit(appendMessage(report,MSG_TYPE_STD));
        }
        else emit(appendMessage(emp.getError(),MSG_TYPE_ERR));
    }


    if (!dataBindingBC.isEmpty()) {
        emit(appendMessage("========== STARTED BINDING BC PROCESSING ==========",MSG_TYPE_SUCC));

        EDPImages images(config);
        // This function generates the tag for the DB but ALSO sets whether the targets use are small or large.
        QString bindingVersion = getVersionForBindingExperiment(true);
        tagRet = csvGeneration(&images,"Binding BC",dataBindingBC,HEADER_IMAGE_EXPERIMENT);
        matrixBindingBC = tagRet.filePath;
        fixations[CONFIG_P_EXP_BIDING_BC] = images.getEyeFixations();
        QString report = emp.processBinding(matrixBindingBC,true,&dbdata);

        // The code needs to be saved only once as it should be the same for both BC and UC.
        QFileInfo info(dataBindingBC);
        DatFileInfoInDir::DatInfo datInfo = DatFileInfoInDir::getBindingFileInformation(info.baseName());
        reportInfoText << "b" + datInfo.extraInfo;
        if (dateForReport.isEmpty()) dateForReport = datInfo.date + "_" + datInfo.hour;
        if (!tagRet.freqCheckErrors) freqErrorsOK = false;

        if (!report.isEmpty()){
            emit(appendMessage(report,MSG_TYPE_STD));
            studyID << bindingVersion + tagRet.version;
            EDPImages::BindingAnswers ans = images.getExperimentAnswers();

            emp.addExtraToResults(STAT_ID_BC_WRONG,ans.wrong);

            // Saving the binding answers
            dbdata[TEYERES_COL_BCCORRECT] = ans.correct;
            dbdata[TEYERES_COL_BCWRONGANS] = ans.wrong;
            dbdata[TEYERES_COL_BCTESTCORRECTANS] = ans.testCorrect;
            dbdata[TEYERES_COL_BCTESTWRONGANS] = ans.testWrong;

            emit(appendMessage(formatBindingResultsForPrinting(ans),MSG_TYPE_STD));
        }
        else emit(appendMessage(emp.getError(),MSG_TYPE_ERR));
    }


    if (!dataBindingUC.isEmpty()){
        emit(appendMessage("========== STARTED BINDING UC PROCESSING ==========",MSG_TYPE_SUCC));

        EDPImages images(config);
        // This function generates the tag for the DB but ALSO sets whether the targets use are small or large.
        QString bindingVersion = getVersionForBindingExperiment(false);
        tagRet = csvGeneration(&images,"Binding UC",dataBindingUC,HEADER_IMAGE_EXPERIMENT);
        matrixBindingUC = tagRet.filePath;
        fixations[CONFIG_P_EXP_BIDING_UC] = images.getEyeFixations();
        QString report = emp.processBinding(matrixBindingUC,false,&dbdata);

        // The code needs to be saved only once as it should be the same for both BC and UC.
        QFileInfo info(dataBindingUC);
        DatFileInfoInDir::DatInfo datInfo = DatFileInfoInDir::getBindingFileInformation(info.baseName());
        if (dateForReport.isEmpty()) dateForReport = datInfo.date + "_" + datInfo.hour;
        if (!tagRet.freqCheckErrors) freqErrorsOK = false;

        if (!report.isEmpty()){
            emit(appendMessage(report,MSG_TYPE_STD));
            studyID << bindingVersion + tagRet.version;
            EDPImages::BindingAnswers ans = images.getExperimentAnswers();

            emp.addExtraToResults(STAT_ID_UC_WRONG,ans.wrong);

            // Saving the binding answers
            dbdata[TEYERES_COL_UCCORRECT] = ans.correct;
            dbdata[TEYERES_COL_UCWRONGANS] = ans.wrong;
            dbdata[TEYERES_COL_UCTESTCORRECTANS] = ans.testCorrect;
            dbdata[TEYERES_COL_UCTESTWRONGANS] = ans.testWrong;

            emit(appendMessage(formatBindingResultsForPrinting(ans),MSG_TYPE_STD));

        }
        else emit(appendMessage(emp.getError(),MSG_TYPE_ERR));
    }

    if (!dataFielding.isEmpty()) {
        emit(appendMessage("========== STARTED FIELDING PROCESSING ==========",MSG_TYPE_SUCC));
        EDPFielding fielding(config);
        tagRet = csvGeneration(&fielding,"Fielding",dataFielding,HEADER_FIELDING_EXPERIMENT);
        matrixFielding = tagRet.filePath;
        fixations[CONFIG_P_EXP_FIELDING] = fielding.getEyeFixations();
        QString report = emp.processFielding(matrixFielding,fielding.getNumberOfTrials());
        if (!tagRet.freqCheckErrors) freqErrorsOK = false;
        if (!report.isEmpty()){
            studyID << "fd" + tagRet.version;
            emit(appendMessage(report,MSG_TYPE_STD));
        }
        else emit(appendMessage(emp.getError(),MSG_TYPE_ERR));
    }

    // Generating the report based on available data.
    QHash<qint32,bool> what2Add;
    what2Add[STAT_ID_ENCODING_MEM_VALUE] = !matrixBindingBC.isEmpty() && !matrixBindingUC.isEmpty();
    what2Add[STAT_ID_TOTAL_FIXATIONS] = !matrixReading.isEmpty();

    if (reportInfoText.isEmpty()){
        emit(appendMessage("Nothing selected to process. Exiting." + reportFileOutput,MSG_TYPE_STD));
        return;
    }

    // Setting the report date to the date of the data.
    QStringList dateParts = dateForReport.split("_");

    // The check is done in case of an unforseen bug. Otherwise the program will crash attempting to access non existent values on a list.
    if (dateParts.size() != 5) config->addKeyValuePair(CONFIG_REPORT_DATE,dateForReport);
    else {
        config->addKeyValuePair(CONFIG_REPORT_DATE,dateParts.at(2) + "/" + dateParts.at(1) + "/" + dateParts.at(0) + " " + dateParts.at(3) + ":" + dateParts.at(4) );
        // Date for report will now be used in legacy versions of report generator. Need to have a name with no hour part.
        dateForReport = dateParts.at(0) + "_" + dateParts.at(1) + "_" + dateParts.at(2);
    }

    generateReportFile(emp.getResults(),what2Add,reportInfoText.join("_") + "_" + dateForReport,freqErrorsOK);
    emit(appendMessage("Report Generated: " + reportFileOutput,MSG_TYPE_SUCC));

    // Saving the database data to text file
    QFile dbdatafile(config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_DBDATA_FILE) ;
    if (!dbdatafile.open(QFile::WriteOnly)){
        emit(appendMessage("Could not open " + dbdatafile.fileName() + " for writing the db data file.",MSG_TYPE_ERR));
        return;
    }

    QTextStream writer(&dbdatafile);
    writer.setCodec(COMMON_TEXT_CODEC);
    QStringList cols = dbdata.keys();
    for (qint32 i = 0; i < cols.size(); i++){
        writer << cols.at(i) + " = " + QString::number(dbdata.value(cols.at(i))) + ";\n";
    }
    writer << QString(TEYERES_COL_STUDY_ID) + " = " + studyID.join("_") + ";\n";
    writer << QString(TEYERES_COL_PROTOCOL) + " =  " + config->getString(CONFIG_PROTOCOL_NAME) + ";\n";
    dbdatafile.close();
    emit(appendMessage("DB Data File Generated to: " + dbdatafile.fileName(),MSG_TYPE_SUCC));

}

void RawDataProcessor::generateReportFile(const DataSet::ProcessingResults &res, const QHash<qint32,bool> whatToAdd, const QString &repFileCode, bool freqErrorsOk){

    if (config->containsKeyword(CONFIG_REPORT_FILENAME)){
        reportFileOutput = config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + config->getString(CONFIG_REPORT_FILENAME);
    }
    else {
        reportFileOutput = config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_REPORT_NAME;
        reportFileOutput = reportFileOutput + "_" + repFileCode + ".rep";
    }

    // Deleting the resport if it exists.
    QFile::remove(reportFileOutput);

    QString freqErrValue;
    if (freqErrorsOk) freqErrValue = "false";
    else freqErrValue = "true";

    // Adding the required report data
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_PATIENT_NAME,config->getString(CONFIG_PATIENT_NAME));
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_PATIENT_AGE,config->getString(CONFIG_PATIENT_AGE));
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_DOCTOR_NAME,config->getString(CONFIG_DOCTOR_NAME));
    //QDateTime::currentDateTime().toString("dd/MM/yyyy")
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_REPORT_DATE,config->getString(CONFIG_REPORT_DATE));
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_RESULTS_FREQ_ERRORS_PRESENT,freqErrValue);

    // Adding the actual results
    if (whatToAdd.value(STAT_ID_TOTAL_FIXATIONS)){
        ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_RESULTS_ATTENTIONAL_PROCESSES,
                                       QString::number(res.value(STAT_ID_TOTAL_FIXATIONS),'f',0));

        ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_RESULTS_EXECUTIVE_PROCESSES,
                                       QString::number(res.value(STAT_ID_MULTIPLE_FIXATIONS)*100.0/res.value(STAT_ID_TOTAL_FIXATIONS),'f',0));

        ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_RESULTS_WORKING_MEMORY,
                                       QString::number(res.value(STAT_ID_FIRST_STEP_FIXATIONS)*100.0/res.value(STAT_ID_TOTAL_FIXATIONS),'f',0));

        ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_RESULTS_RETRIEVAL_MEMORY,
                                       QString::number(res.value(STAT_ID_SINGLE_FIXATIONS)*100.0/res.value(STAT_ID_TOTAL_FIXATIONS),'f',0));
    }

    if (whatToAdd.value(STAT_ID_ENCODING_MEM_VALUE)){
        ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_RESULTS_MEMORY_ENCODING,
                                       QString::number(res.value(STAT_ID_ENCODING_MEM_VALUE),'f',3));

        // Adding the answers incorrect answers as a String List.
        QStringList list;
        list << QString::number(res.value(STAT_ID_BC_WRONG)) << QString::number(res.value(STAT_ID_UC_WRONG));
        ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_RESULTS_BEHAVIOURAL_RESPONSE,list.join("|"));

    }
}

RawDataProcessor::TagParseReturn RawDataProcessor::csvGeneration(EDPBase *processor, const QString &id, const QString &dataFile, const QString &header){
    QString data;
    QString exp;

    RawDataProcessor::TagParseReturn tagRet;

    if (dataFile.isEmpty()){
        emit(appendMessage(id + " processing was configured but no " + id + " file was found",MSG_TYPE_ERR));
        tagRet.filePath = "";
        return tagRet;
    }

    tagRet = separateInfoByTag(dataFile,header,&data,&exp);

    if (!tagRet.ok){
        tagRet.filePath = "";
        return tagRet;
    }

    processor->setFieldingMarginInMM(config->getReal(CONFIG_MARGIN_TARGET_HIT));
    processor->setMonitorGeometry(mgeo);
    processor->setMovingWindowParameters(mwp);
    processor->calculateWindowSize();
    processor->setPixelsInSacadicLatency(config->getInt(CONFIG_LATENCY_ESCAPE_RAD));

    // Configuring the experiment
    processor->configure(dataFile,exp);

    emit(appendMessage("-> Generating  " + id + " CSV file",0));
    if (!processor->doEyeDataProcessing(data)){
        emit(appendMessage("ERROR: " + processor->getError(),2));
    }
    else{

        if (!processor->getWarnings().isEmpty()){
            emit(appendMessage("WARNING: " + id + ": " + processor->getWarnings(),MSG_TYPE_WARN));
        }

        if (!processor->getFilteredLines().isEmpty()){
            emit(appendMessage("WARNING: " + id + ". A total of " + QString::number(processor->getFilteredLines().size()) + " lines were ignored from the dat file",MSG_TYPE_WARN));
        }

        emit(appendMessage("DONE!: " + id + " Matrix Generated. DAT File used: " + dataFile,1));
    }

    tagRet.freqCheckErrors = getFrequencyCheckErrors(processor->getSamplingFrequencyCheck());

    tagRet.filePath = processor->getOuputMatrixFileName();
    return tagRet;

}

bool RawDataProcessor::getFrequencyCheckErrors(const QStringList &ferrors){
    LogInterface temp_log;
    temp_log.setLogFileLocation("freq_errors.log");
    for (qint32 i = 0; i < ferrors.size(); i++){
        temp_log.appendError("DR: " + config->getString(CONFIG_DOCTOR_NAME) + ". PAT: " + config->getString(CONFIG_PATIENT_NAME) + ". " +  ferrors.at(i));
    }
    return ferrors.isEmpty();
}

RawDataProcessor::TagParseReturn RawDataProcessor::separateInfoByTag(const QString &file, const QString &tag, QString *data, QString *experiment){

    TagParseReturn ans;
    ans.ok = false;

    QFile f(file);
    if (!f.open(QFile::ReadOnly)){
        emit(appendMessage("Could not open " + file + " for reading",2));
        return ans;
    }

    QTextStream reader(&f);
    reader.setCodec(COMMON_TEXT_CODEC);

    *data = "";
    *experiment = "";
    QStringList datalist;
    QStringList explist;

    qint32 tagFound = 0;

    while (!reader.atEnd()){
        QString line = reader.readLine();
        line = line.trimmed();

        if (line.isEmpty()) continue;

        // Until the first tag is found, everything else is ignored
        if (tagFound == 0){
            if (line.contains(tag,Qt::CaseSensitive)) {
                tagFound++;
                // Separating by space. By definition the last non empty string is the version string.
                QStringList parts = line.split(" ",QString::SkipEmptyParts);
                if (parts.size() == 1){
                    // There is no version string so it is version 1.
                    ans.version = "v1";
                }
                else ans.version = parts.last();

            }
            else continue;
        }
        // Until the second tag is found everything else is the experiment description.
        else if (tagFound == 1){
            if (line.contains(tag,Qt::CaseSensitive)) tagFound++;
            else explist << line;
        }
        else{
            datalist << line;
        }
    }

    if (getResolutionToConfig(datalist.first())){
        datalist.removeFirst();
    }
    *data       = datalist.join('\n');
    *experiment = explist.join('\n');

    if (tagFound == 2){
        ans.ok = true;
        return ans;
    }
    else{
        emit(appendMessage("Could not find the tag " + tag + " in the input file: " + file,2));
        return ans;
    }
}

bool RawDataProcessor::getResolutionToConfig(const QString &firstline){

    // Getting the resolution for the experiment
    // Default values.
    config->addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,768);
    config->addKeyValuePair(CONFIG_RESOLUTION_WIDTH,1024);

    QStringList tokens = firstline.split(" ",QString::SkipEmptyParts);
    if (tokens.size() != 2) return false;

    bool ok;
    qreal w = tokens.first().toUInt(&ok);
    if (!ok) return false;
    qreal h = tokens.last().toUInt(&ok);
    if (!ok) return false;

    // Setting the obtained resolution
    config->addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,h);
    config->addKeyValuePair(CONFIG_RESOLUTION_WIDTH,w);

    return true;
}

QString RawDataProcessor::getVersionForBindingExperiment(bool bound){
    QString base;
    QString fileName;
    if (bound) {
        base = "bc";
        fileName = dataBindingBC;
    }
    else{
        base = "uc";
        fileName = dataBindingUC;
    }

    QFileInfo info(fileName);
    DatFileInfoInDir::DatInfo datinfo = DatFileInfoInDir::getBindingFileInformation(info.baseName());
    base = base + datinfo.extraInfo;

    //qWarning() << "Base" << base;

    return base;
}
