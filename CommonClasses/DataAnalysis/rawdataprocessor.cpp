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

    if (config->containsKeyword(CONFIG_FILE_NBACKRT))
        dataNBackRT = currentDir + "/" + config->getString(CONFIG_FILE_NBACKRT);
    else dataNBackRT = "";

    matrixBindingBC = "";
    matrixReading = "";
    matrixFielding = "";
    matrixBindingUC = "";
    matrixNBackRT = "";

    mgeo.distanceToMonitorInMilimiters = config->getReal(CONFIG_DISTANCE_2_MONITOR)*10;
    mgeo.XmmToPxRatio                  = config->getReal(CONFIG_XPX_2_MM);
    mgeo.YmmToPxRatio                  = config->getReal(CONFIG_YPX_2_MM);

    mwp.maxDispersion                  = config->getInt(CONFIG_MOVING_WINDOW_DISP);
    mwp.minimumFixationLength          = config->getReal(CONFIG_MIN_FIXATION_LENGTH);
    mwp.sampleFrequency                = config->getReal(CONFIG_SAMPLE_FREQUENCY);
    mwp.calculateWindowSize();

}


QString RawDataProcessor::formatBindingResultsForPrinting(const EDPImages::BindingAnswers &ans, const QString &ID){
    QString report = "ANSWER Summary For: " + ID + "<br>";
    report = report + "Number of correct answers in test trials: " + QString::number(ans.testCorrect) + "<br>";
    report = report + "Number of wrong answers in test trials: " + QString::number(ans.testWrong) + "<br>";
    report = report + "Number of correct answers : " + QString::number(ans.correct) + "<br>";
    report = report + "Number of wrong answers : " + QString::number(ans.wrong) + "<br>";
    return report;
}

void RawDataProcessor::run(){

    RDataProcessor rdataProcessor;
    rdataProcessor.setWorkDirectory(config->getString(CONFIG_PATIENT_DIRECTORY));

    reportFileOutput = "";
    frequencyErrorMailBody = "";

    QStringList studyID;
    RawDataProcessor::TagParseReturn tagRet;
    QHash<QString,qreal> bindingAns;

    QString dateForReport;
    QStringList reportInfoText;

    bool freqErrorsOK = true;

    // Each of the processing fucntions is called. In this way, processing of just one can be accomplished
    // just by running this complete function but it will keep going if a file does not exist.
    if (!dataReading.isEmpty()) {
        emit(appendMessage("========== STARTED READING PROCESSING ==========",MSG_TYPE_SUCC));
        EDPReading reading(config);
        tagRet = csvGeneration(&reading,"Reading",dataReading,HEADER_READING_EXPERIMENT);
        if (!tagRet.ok) return;
        matrixReading = tagRet.filePath;

        fixations[CONFIG_P_EXP_READING] = reading.getEyeFixations();
        barGraphOptionsFromFixationList(reading.getEyeFixations(),dataReading);
        bool temp = generateFDBFile(dataReading,reading.getEyeFixations(),false);
        freqErrorsOK = freqErrorsOK && temp;

        QString report = rdataProcessor.processReading(matrixReading);

        QFileInfo info(dataReading);
        DatFileInfoInDir::DatInfo datInfo = DatFileInfoInDir::getReadingInformation(info.baseName());
        dateForReport = datInfo.date + "_" + datInfo.hour;
        reportInfoText << "r";

        if (!report.isEmpty()){
            studyID << "rd" + tagRet.version;
            emit(appendMessage(report,MSG_TYPE_STD));
        }
        else {
            emit(appendMessage(rdataProcessor.getError(),MSG_TYPE_ERR));
            return;
        }
    }


    if (!dataBindingBC.isEmpty() && !dataBindingUC.isEmpty()) {
        emit(appendMessage("========== STARTED BINDING PROCESSING ==========",MSG_TYPE_SUCC));

        EDPImages imagesBC(config);
        // This function generates the tag for the DB but ALSO sets whether the targets use are small or large.
        QString bindingVersion = getVersionForBindingExperiment(true);
        tagRet = csvGeneration(&imagesBC,"Binding BC",dataBindingBC,HEADER_IMAGE_EXPERIMENT);
        if (!tagRet.ok) return;
        matrixBindingBC = tagRet.filePath;

        fixations[CONFIG_P_EXP_BIDING_BC] = imagesBC.getEyeFixations();
        barGraphOptionsFromFixationList(imagesBC.getEyeFixations(),dataBindingBC);
        bool temp = generateFDBFile(dataBindingBC,imagesBC.getEyeFixations(),false);
        freqErrorsOK = freqErrorsOK && temp;

        EDPImages imagesUC(config);
        // This function generates the tag for the DB but ALSO sets whether the targets use are small or large.
        tagRet = csvGeneration(&imagesUC,"Binding UC",dataBindingUC,HEADER_IMAGE_EXPERIMENT);
        matrixBindingUC = tagRet.filePath;

        fixations[CONFIG_P_EXP_BIDING_UC] = imagesUC.getEyeFixations();
        barGraphOptionsFromFixationList(imagesUC.getEyeFixations(),dataBindingUC);
        temp = generateFDBFile(dataBindingUC,imagesUC.getEyeFixations(),false);
        if (!tagRet.ok) return;
        freqErrorsOK = freqErrorsOK && temp;

        EDPImages::BindingAnswers bcans = imagesBC.getExperimentAnswers();
        EDPImages::BindingAnswers ucans = imagesUC.getExperimentAnswers();

        // Saving the binding answers
        bindingAns[TEYERES_COL_BCCORRECT] = bcans.correct;
        bindingAns[TEYERES_COL_BCWRONGANS] = bcans.wrong;
        bindingAns[TEYERES_COL_BCTESTCORRECTANS] = bcans.testCorrect;
        bindingAns[TEYERES_COL_BCTESTWRONGANS] = bcans.testWrong;
        bindingAns[TEYERES_COL_UCCORRECT] = ucans.correct;
        bindingAns[TEYERES_COL_UCWRONGANS] = ucans.wrong;
        bindingAns[TEYERES_COL_UCTESTCORRECTANS] = ucans.testCorrect;
        bindingAns[TEYERES_COL_UCTESTWRONGANS] = ucans.testWrong;

        QString report = rdataProcessor.processBinding(matrixBindingBC,matrixBindingUC,bcans.correct,ucans.correct);

        // The code needs to be saved only once as it should be the same for both BC and UC.
        QFileInfo info(dataBindingBC);
        DatFileInfoInDir::DatInfo datInfo = DatFileInfoInDir::getBindingFileInformation(info.baseName());
        reportInfoText << "b" + datInfo.extraInfo;
        if (dateForReport.isEmpty()) dateForReport = datInfo.date + "_" + datInfo.hour;

        if (!report.isEmpty()){
            emit(appendMessage(report,MSG_TYPE_STD));
            studyID << bindingVersion + tagRet.version;
            emit(appendMessage(formatBindingResultsForPrinting(bcans,"BC"),MSG_TYPE_STD));
            emit(appendMessage(formatBindingResultsForPrinting(ucans,"UC"),MSG_TYPE_STD));
        }
        else {
            emit(appendMessage(rdataProcessor.getError(),MSG_TYPE_ERR));
            return;
        }
    }

    if (!dataFielding.isEmpty()) {
        emit(appendMessage("========== STARTED FIELDING PROCESSING ==========",MSG_TYPE_SUCC));
        EDPFielding fielding(config);
        tagRet = csvGeneration(&fielding,"Fielding",dataFielding,HEADER_FIELDING_EXPERIMENT);
        if (!tagRet.ok) return;
        matrixFielding = tagRet.filePath;

        fixations[CONFIG_P_EXP_FIELDING] = fielding.getEyeFixations();
        /// TODO ADD FIELDING PROCESSING

        QFileInfo info(dataFielding);
        DatFileInfoInDir::DatInfo datInfo = DatFileInfoInDir::getFieldingInformation(info.baseName());
        dateForReport = datInfo.date + "_" + datInfo.hour;
        reportInfoText << "f";

        bool temp = generateFDBFile(dataFielding,fielding.getEyeFixations(),true);
        freqErrorsOK = freqErrorsOK && temp;

        studyID << "fd" + tagRet.version;
        emit(appendMessage("Fielding CSV GENERATED",MSG_TYPE_STD));
    }

    if (!dataNBackRT.isEmpty()) {
        emit(appendMessage("========== STARTED NBACK RT PROCESSING ==========",MSG_TYPE_SUCC));
        EDPNBackRT nbackrt(config);
        tagRet = csvGeneration(&nbackrt,"NBack RT",dataNBackRT,HEADER_NBACKRT_EXPERIMENT);
        if (!tagRet.ok) return;
        matrixNBackRT = tagRet.filePath;

        fixations[CONFIG_P_EXP_NBACKRT] = nbackrt.getEyeFixations();
        barGraphOptionsFromFixationList(nbackrt.getEyeFixations(),dataNBackRT);
        bool temp = generateFDBFile(dataNBackRT,nbackrt.getEyeFixations(),false);
        freqErrorsOK = freqErrorsOK && temp;

        QString report = rdataProcessor.processNBackRT(matrixNBackRT);

        QFileInfo info(dataNBackRT);
        DatFileInfoInDir::DatInfo datInfo = DatFileInfoInDir::getNBackRTInformation(info.baseName());
        dateForReport = datInfo.date + "_" + datInfo.hour;
        reportInfoText << "t";

        if (!report.isEmpty()){
            studyID << "nbrt" + tagRet.version;
            emit(appendMessage(report,MSG_TYPE_STD));
        }
        else {
            emit(appendMessage(rdataProcessor.getError(),MSG_TYPE_ERR));
            return;
        }
    }

    // Generating the report based on available data.
    if (reportInfoText.isEmpty()){
        emit(appendMessage("Nothing selected to process. Exiting." + reportFileOutput,MSG_TYPE_STD));
        return;
    }

    // Setting the report date to the date of the data.
    QStringList dateParts = dateForReport.split("_");
    QString client_date_for_db;

    // The check is done in case of an unforseen bug. Otherwise the program will crash attempting to access non existent values on a list.
    if (dateParts.size() != 5) {
        config->addKeyValuePair(CONFIG_REPORT_DATE,dateForReport);
        client_date_for_db = "0000-00-00 00:00";
    }
    else {
        config->addKeyValuePair(CONFIG_REPORT_DATE,dateParts.at(2) + "/" + dateParts.at(1) + "/" + dateParts.at(0) + " " + dateParts.at(3) + ":" + dateParts.at(4) );
        // Date for report will now be used in legacy versions of report generator. Need to have a name with no hour part.
        dateForReport = dateParts.at(0) + "_" + dateParts.at(1) + "_" + dateParts.at(2);
        client_date_for_db = dateParts.at(0) + "-" + dateParts.at(1) + "-" + dateParts.at(2) + " " + dateParts.at(3) + ":" + dateParts.at(4);
    }

    generateReportFile(rdataProcessor.getResults(),reportInfoText.join("_") + "_" + dateForReport,freqErrorsOK);
    emit(appendMessage("Report Generated: " + reportFileOutput,MSG_TYPE_SUCC));

    // Saving the database data to text file
    QFile dbdatafile(config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_DBDATA_FILE) ;
    if (!dbdatafile.open(QFile::WriteOnly)){
        emit(appendMessage("Could not open " + dbdatafile.fileName() + " for writing the db data file.",MSG_TYPE_ERR));
        return;
    }

    QTextStream writer(&dbdatafile);
    writer.setCodec(COMMON_TEXT_CODEC);

    // Joining all data that goes to the DB.
    QHash<QString,qreal> dbdata = rdataProcessor.getDBData();
    QStringList bansKeys = bindingAns.keys();
    for (qint32 i = 0; i < bansKeys.size(); i++){
        dbdata[bansKeys.at(i)] = bindingAns.value(bansKeys.at(i));
    }

    QStringList cols = dbdata.keys();
    for (qint32 i = 0; i < cols.size(); i++){
        writer << cols.at(i) + " = " + QString::number(dbdata.value(cols.at(i))) + ";\n";
    }
    QString ferrorvalue;
    if (freqErrorsOK) ferrorvalue = "0";
    else ferrorvalue = "1";

    writer << QString(TEYERES_COL_STUDY_ID) + " = " + studyID.join("_") + ";\n";
    writer << QString(TEYERES_COL_PROTOCOL) + " =  " + config->getString(CONFIG_PROTOCOL_NAME) + ";\n";
    writer << QString(TEYERES_COL_FERROR) + " = " + ferrorvalue + ";\n";
    writer << QString(TEYERES_COL_EVALUATION_ID) + " = " + config->getString(CONFIG_RESULT_ENTRY_ID) + ";\n";
    writer << QString(TEYERES_COL_CLIENT_STUDY_DATE) + " = " + client_date_for_db + ";\n";
    dbdatafile.close();
    emit(appendMessage("DB Data File Generated to: " + dbdatafile.fileName(),MSG_TYPE_SUCC));

    // Generating mail body if necessary.
    if  (!frequencyErrorMailBody.isEmpty()){
        QFile mailFile(config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_MAIL_ERROR_LOG);
        if (!mailFile.open(QFile::WriteOnly)){
            emit(appendMessage("Could not open " + mailFile.fileName() + " for writing the mail ferror file.",MSG_TYPE_ERR));
            return;
        }

        QTextStream mailWriter(&mailFile);
        mailWriter.setCodec(COMMON_TEXT_CODEC);
        mailWriter << "<p>" << frequencyErrorMailBody << "</p>";
        mailFile.close();
        emit(appendMessage("Mail body File Generated to: " + mailFile.fileName(),MSG_TYPE_SUCC));
    }

    // Generating the graphs file.
    QString graphFile = config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_GRAPHS_FILE;
    BarGrapher bgrapher;
    for (qint32 i = 0; i < graphValues.size(); i++){
        bgrapher.addGraphToDo(graphValues.at(i));
    }
    if (!bgrapher.createBarGraphHTML(graphFile)){
        emit(appendMessage("When creating fixation graphs: " + bgrapher.getLastError(),MSG_TYPE_ERR));
    }

}

bool RawDataProcessor::generateFDBFile(const QString &datFile, const FixationList &fixList, bool isFielding){

    //qWarning() << "FDB WITH" << datFile;

    FreqAnalysis::FreqAnalysisResult fres;
    fres = FreqAnalysis::doFrequencyAnalysis(config,datFile);

    if (!fres.fileError.isEmpty()){
        emit(appendMessage("Error while doing freq analysis for FDB file (" + datFile + "): " + fres.fileError,MSG_TYPE_ERR));
        return true;
    }

    bool freqError = datFile.endsWith(".datf");
    if (freqError == fres.errorList.isEmpty()){
        // This should not happen. Hence a warning is emitted
        emit(appendMessage("Difference in frequency check results (" + datFile + "): But found" + fres.errorList.join("<br>   "),MSG_TYPE_ERR));
    }

    // Info on the file name.
    QFileInfo info(datFile);

    // Freq error flag
    QString ferrorVal;
    if (fres.errorList.isEmpty()) ferrorVal = "0";
    else {
        ferrorVal = "1";
        QString html = "<br><b>" + info.baseName() + ":</b><ul>";
        html = html + "<li>" + fres.errorList.join("</li><li>") + "</li></ul>";
        frequencyErrorMailBody = frequencyErrorMailBody + "<br>" + html;
    }

    // Freq Tol Params;
    QStringList freqTolParams;
    freqTolParams << "HAP:" + config->getString(CONFIG_TOL_MAX_PERIOD_TOL);
    freqTolParams << "LAP:" + config->getString(CONFIG_TOL_MIN_PERIOD_TOL);
    freqTolParams << "HFG:" + config->getString(CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL);
    if (isFielding){
        freqTolParams << "LNI:" + config->getString(CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL);
    }
    else{
        freqTolParams << "LNI:" + config->getString(CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL);
    }
    freqTolParams << "HPI:" + config->getString(CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES);
    freqTolParams << "AFD:" + config->getString(CONFIG_TOL_NUM_ALLOWED_FAILED_DATA_SETS);

    // Fixation List values
    QStringList fixCountValues;
    if (fixList.checkCoherenceInListSizes()){
        QStringList left, right;
        for (qint32 i = 0; i < fixList.trialID.size(); i++){
            QString name = fixList.trialID.at(i).join("_");
            left << name + ":" + QString::number(fixList.left.at(i).size());
            right << name + ":" + QString::number(fixList.right.at(i).size());
        }
        fixCountValues << left.join(",") << right.join(",");
    }
    else{
        emit(appendMessage("Fixation list for (" + datFile + "): is size incoherent. Not adding the data",MSG_TYPE_ERR));
    }

    QString fdbfile = info.path() + "/" + info.baseName() + ".fdb";
    QFile file(fdbfile);
    if (!file.open(QFile::WriteOnly)){
        emit(appendMessage("Could not open FDB file " + fdbfile + " for writing",MSG_TYPE_ERR));
        return fres.errorList.isEmpty();
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);
    writer << TFDATA_COL_FILENAME << " = " << info.baseName() + "." + info.suffix() << ";\n";
    writer << TFDATA_COL_FERROR << " = " << ferrorVal << ";\n";
    writer << TFDATA_COL_FREQ_TOL_PARAMS << " = " << freqTolParams.join(",") << ";\n";
    writer << TFDATA_COL_EYE_FIX_COUNT << " = " << fixCountValues.join("-") << ";\n";
    writer << TFDATA_COL_GLITCHED_SET_COUNT << " = " << QString::number(fres.numberOfDataSetsWithTooManyFreqGlitches) << ";\n";
    writer << TFDATA_COL_NODATA_SET_COUNT << " = " << QString::number(fres.numberOfDataSetsWithLittleDataPoints) << ";\n";

    file.close();

    return fres.errorList.isEmpty();
}

void RawDataProcessor::generateReportFile(const ConfigurationManager &res, const QString &repFileCode, bool freqErrorsOk){

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

    ConfigurationManager results;

    // Adding the report values
    results.merge(res);

    // Adding all configuration parameters as they were presented.
    results.merge(*config);

    // The algorithm versions are added based solely on the what variables are present.
    if (results.containsKeyword(CONFIG_FILE_READING)){
        results.addKeyValuePair(CONFIG_READING_ALG_VERSION,EYE_REP_GEN_READING_ALGORITHM_VERSION);
    }

    if ((results.containsKeyword(CONFIG_FILE_BIDING_BC) && results.containsKeyword(CONFIG_FILE_BIDING_UC))){
        results.addKeyValuePair(CONFIG_BINDING_ALG_VERSION,EYE_REP_GEN_BINDING_ALGORITHM_VERSION);
    }

    if (results.containsKeyword(CONFIG_FILE_FIELDING)){
        results.addKeyValuePair(CONFIG_FIELDING_ALG_VERSION,EYE_REP_GEN_FIELDING_ALGORITHM_VERSION);
    }

    // Removing patient directory as it contains sensitive information
    results.removeKey(CONFIG_PATIENT_DIRECTORY);

    // Adding the frecuency error flag
    results.addKeyValuePair(CONFIG_RESULTS_FREQ_ERRORS_PRESENT,freqErrValue);

    // Saving the results to disk.
    results.saveToFile(reportFileOutput,COMMON_TEXT_CODEC);

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

    // VR Constants. Should be kept the same for ANY VR Implementations.
    if (config->getBool(CONFIG_VR_ENABLED) && config->containsKeyword(CONFIG_VR_ENABLED)){
        mgeo.distanceToMonitorInMilimiters = MonitorGeometry::VR_VIEW_DISTANCE_TO_MONITOR;
        mgeo.XmmToPxRatio                  = MonitorGeometry::VR_VIEW_WIDTH/config->getReal(CONFIG_RESOLUTION_WIDTH);
        mgeo.YmmToPxRatio                  = MonitorGeometry::VR_VIEW_HEIGHT/config->getReal(CONFIG_RESOLUTION_HEIGHT);
        QString msg = "->  Using VR Constants for geometry calculations. W = ";
        msg = msg + config->getString(CONFIG_RESOLUTION_WIDTH) + ". H = " + config->getString(CONFIG_RESOLUTION_HEIGHT);
        emit(appendMessage(msg,MSG_TYPE_STD));
    }

    mgeo.resolutionWidth = config->getReal(CONFIG_RESOLUTION_WIDTH);
    mgeo.resolutionHeight = config->getReal(CONFIG_RESOLUTION_HEIGHT);

    processor->setFieldingMargin(static_cast<qint32>(config->getReal(CONFIG_MARGIN_TARGET_HIT)));
    processor->setMonitorGeometry(mgeo);
    processor->setMovingWindowParameters(mwp);
    processor->calculateWindowSize();
    processor->setPixelsInSacadicLatency(config->getInt(CONFIG_LATENCY_ESCAPE_RAD));

    // Configuring the experiment
    processor->configure(dataFile,exp);

    emit(appendMessage("-> Generating  " + id + " CSV file",0));
    if (!processor->doEyeDataProcessing(data)){
        tagRet.ok = false;
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

    //tagRet.freqCheckErrors = getFrequencyCheckErrors(processor->getSamplingFrequencyCheck());

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

void RawDataProcessor::barGraphOptionsFromFixationList(const FixationList &fixlist, const QString &fileName){

    QList<BarGrapher::BarGraphOptions> bgo;

    if (fileName.contains(CONFIG_P_EXP_BIDING_BC) || fileName.contains(CONFIG_P_EXP_BIDING_UC)){
        QString title;
        if (fileName.contains(CONFIG_P_EXP_BIDING_BC)) title = "Binding BC";
        else title = "Binding UC";

        bgo << BarGrapher::BarGraphOptions();  // Left Eye.
        bgo << BarGrapher::BarGraphOptions();  // Right Eye
        for (qint32 i = 0; i < 2; i++){
            bgo[i].width = 1280;
            bgo[i].height = 720;
            bgo[i].associatedFileName = fileName;
            bgo[i].drawValuesOnBars = false;
            bgo[i].fontSize = 12;
            bgo[i].ylabel = "Number of Total Unfiltered Fixations";
            bgo[i].associatedFileName = fileName;
            bgo[i].colors << "#2022a2" << "#a21d3c";
            bgo[i].dataSets << QList<qreal>() << QList<qreal>();
            bgo[i].legend << "Show" << "Decision";
        }
        for (qint32 i = 0; i < fixlist.trialID.size(); i++){
            QStringList l = fixlist.trialID.at(i);
            // This check should alwasy pass. However if for some future bug the list does not contain any values the code inside would crash the program
            if (l.size() == 2){
                qint32 index = 0;
                if (l.last() == "1") index = 1;
                if (index == 0){
                    bgo[EYE_L].xtext << l.first();
                    bgo[EYE_R].xtext << l.first();
                }
                bgo[EYE_L].dataSets[index].append(fixlist.left.at(i).size());
                bgo[EYE_R].dataSets[index].append(fixlist.right.at(i).size());
            }
        }

        QFileInfo info(fileName);

        bgo[EYE_L].title = "Left Eye Fixations for " + title + ": " + info.baseName();
        bgo[EYE_R].title = "Right Eye Fixations for " + title + ": " + info.baseName();
    }
    else if (fileName.contains(CONFIG_P_EXP_READING)){
        bgo << BarGrapher::BarGraphOptions(); // Sentences longer than 7 words
        bgo << BarGrapher::BarGraphOptions(); // Sentences shorter or equal to 7 words.

        for (qint32 i = 0; i < 2; i++){
            bgo[i].width = 1280;
            bgo[i].height = 720;
            bgo[i].associatedFileName = fileName;
            bgo[i].drawValuesOnBars = false;
            bgo[i].fontSize = 12;
            bgo[i].ylabel = "Number of Total Unfiltered Fixations";
            bgo[i].associatedFileName = fileName;
            bgo[i].colors << "#2022a2" << "#a21d3c";
            bgo[i].dataSets << QList<qreal>() << QList<qreal>();
            bgo[i].legend << "Left Eye" << "Right Eye";
        }
        for (qint32 i = 0; i < fixlist.trialID.size(); i++){
            QStringList l = fixlist.trialID.at(i);
            // This check should alwasy pass. However if for some future bug the list does not contain any values the code inside would crash the program
            if (l.size() == 2){
                qint32 sl = l.last().toInt();
                qint32 index = 0;
                if (sl <= 7) index = 1;
                bgo[index].dataSets[EYE_L].append(fixlist.left.at(i).size());
                bgo[index].dataSets[EYE_R].append(fixlist.right.at(i).size());
                bgo[index].xtext << l.first();
            }
        }
        QFileInfo info(fileName);
        bgo[0].title = "Fixations on Reading Sentences longer than 7 for : " + info.baseName();
        bgo[1].title = "Fixations on Reading Sentences shorter or equal to 7 for: " + info.baseName();
    }
    else if (fileName.contains(CONFIG_P_EXP_NBACKRT)){
        bgo << BarGrapher::BarGraphOptions();  // The number of fixation during the three parts of the encoding
        bgo << BarGrapher::BarGraphOptions();  // The number of fixations during the the retrieval.

        for (qint32 i = 0; i < 2; i++){
            bgo[i].width = 1280;
            bgo[i].height = 720;
            bgo[i].associatedFileName = fileName;
            bgo[i].drawValuesOnBars = false;
            bgo[i].fontSize = 12;
            bgo[i].ylabel = "Number of Total Unfiltered Fixations";
            bgo[i].associatedFileName = fileName;
            bgo[i].colors << "#2022a2" << "#a21d3c";
            bgo[i].dataSets << QList<qreal>() << QList<qreal>();
            bgo[i].legend << "Left Eye" << "Right Eye";
        }

        qint32 nFixL, nFixR;
        nFixL = 0; nFixR = 0;

        for (qint32 i = 0; i < fixlist.trialID.size(); i++){
            QStringList l = fixlist.trialID.at(i);
            // This check should always pass. However if for some future bug the list does not contain any values the code inside would crash the program
            if (l.size() == 2){
                if (l.first() == "4"){
                    bgo[0].dataSets[EYE_L].append(nFixL);
                    bgo[0].dataSets[EYE_R].append(nFixR);
                    bgo[0].xtext << l.last();
                    nFixL = 0;
                    nFixR = 0;
                    bgo[1].dataSets[EYE_L].append(fixlist.left.at(i).size());
                    bgo[1].dataSets[EYE_R].append(fixlist.right.at(i).size());
                    bgo[1].xtext << l.last();
                }
                else{
                   nFixR = nFixR + fixlist.right.at(i).size();
                   nFixL = nFixL + fixlist.left.at(i).size();
                }
            }
        }
        QFileInfo info(fileName);
        bgo[0].title = "Fixations on NBACK RT Encoding for : " + info.baseName();
        bgo[1].title = "Fixations on NBACK RT Retrieval for: " + info.baseName();

    }
    else{
        emit(appendMessage("BarGraph Option Creator: Unknown file type: " + fileName,MSG_TYPE_ERR));
    }

    graphValues << bgo;
}
