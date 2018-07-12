#include "rawdataprocessor.h"

RawDataProcessor::RawDataProcessor(QObject *parent):QObject(parent)
{
}

void RawDataProcessor::initialize(ConfigurationManager *c, const QSet<QString> &exps){
    config = c;

    // Getting the corresponding files for processing
    QString fileBC = FILE_OUTPUT_BINDING_BC;
    QString fileUC = FILE_OUTPUT_BINDING_UC;

    if (c->containsKeyword(CONFIG_BC_FILE_FILTER)){
        fileBC = fileBC + c->getString(CONFIG_BC_FILE_FILTER);
    }
    if (c->containsKeyword(CONFIG_UC_FILE_FILTER)){
        fileUC = fileUC + c->getString(CONFIG_UC_FILE_FILTER);
    }

    dataReading = getNewestFile(config->getString(CONFIG_PATIENT_DIRECTORY),FILE_OUTPUT_READING);
    dataBindingBC = getNewestFile(config->getString(CONFIG_PATIENT_DIRECTORY),fileBC);
    dataBindingUC = getNewestFile(config->getString(CONFIG_PATIENT_DIRECTORY),fileUC);
    dataFielding = getNewestFile(config->getString(CONFIG_PATIENT_DIRECTORY),FILE_OUTPUT_FIELDING);
    experiments = exps;

    commonInitialization();

}

void RawDataProcessor::initialize(ConfigurationManager *c, const QStringList &filesToProcess){
    config = c;
    commonInitialization();
    experiments.clear();
    for (qint32 i = 0; i < filesToProcess.size(); i++){
        if (filesToProcess.at(i).contains(FILE_OUTPUT_BINDING_BC)){
            dataBindingBC = filesToProcess.at(i);
            experiments << CONFIG_P_EXP_BIDING_BC;
        }
        else if (filesToProcess.at(i).contains(FILE_OUTPUT_BINDING_UC)){
            dataBindingUC = filesToProcess.at(i);
            experiments << CONFIG_P_EXP_BIDING_UC;
        }
        else if (filesToProcess.at(i).contains(FILE_OUTPUT_READING)){
            dataReading = filesToProcess.at(i);
            experiments << CONFIG_P_EXP_READING;
        }
        else if (filesToProcess.at(i).contains(FILE_OUTPUT_FIELDING)){
            dataFielding = filesToProcess.at(i);
            experiments << CONFIG_P_EXP_FIELDING;
        }
    }
}


void RawDataProcessor::commonInitialization(){
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

    EyeMatrixProcessor::DBHash dbdata;

    QStringList studyID;

    // Each of the processing fucntions is called. In this way, processing of just one can be accomplished
    // just by running this complete function but it will keep going if a file does not exist.
    if (experiments.contains(CONFIG_P_EXP_READING)) {        
        emit(appendMessage("========== STARTED READING PROCESSING ==========",MSG_TYPE_SUCC));
        EDPReading reading(config);
        matrixReading = csvGeneration(&reading,"Reading",dataReading,HEADER_READING_EXPERIMENT);
        fixations[CONFIG_P_EXP_READING] = reading.getEyeFixations();
        QString report = emp.processReading(matrixReading,&dbdata);
        /// TODO: Get versions of experiment from somewhere else.
        studyID << "rdv_1";
        if (!report.isEmpty()){
            emit(appendMessage(report,MSG_TYPE_STD));
        }
        else emit(appendMessage(emp.getError(),MSG_TYPE_ERR));
    }


    if (experiments.contains(CONFIG_P_EXP_BIDING_BC)) {
        emit(appendMessage("========== STARTED BINDING BC PROCESSING ==========",MSG_TYPE_SUCC));
        EDPImages images(config);
        matrixBindingBC = csvGeneration(&images,"Binding BC",dataBindingBC,HEADER_IMAGE_EXPERIMENT);
        fixations[CONFIG_P_EXP_BIDING_BC] = images.getEyeFixations();
        QString report = emp.processBinding(matrixBindingBC,true,&dbdata);
        /// TODO: Get versions of experiment from somewhere else.
        studyID << "bcv_1";
        if (!report.isEmpty()){
            emit(appendMessage(report,MSG_TYPE_STD));
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


    if (experiments.contains(CONFIG_P_EXP_BIDING_UC)){
        emit(appendMessage("========== STARTED BINDING UC PROCESSING ==========",MSG_TYPE_SUCC));
        EDPImages images(config);
        matrixBindingUC = csvGeneration(&images,"Binding UC",dataBindingUC,HEADER_IMAGE_EXPERIMENT);
        fixations[CONFIG_P_EXP_BIDING_UC] = images.getEyeFixations();
        QString report = emp.processBinding(matrixBindingUC,false,&dbdata);
        /// TODO: Get versions of experiment from somewhere else.
        studyID << "ucv_1";
        if (!report.isEmpty()){
            emit(appendMessage(report,MSG_TYPE_STD));
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

    if (experiments.contains(CONFIG_P_EXP_FIELDING)) {
        emit(appendMessage("========== STARTED FIELDING PROCESSING ==========",MSG_TYPE_SUCC));
        EDPFielding fielding(config);
        matrixFielding = csvGeneration(&fielding,"Fielding",dataFielding,HEADER_FIELDING_EXPERIMENT);
        fixations[CONFIG_P_EXP_FIELDING] = fielding.getEyeFixations();
        QString report = emp.processFielding(matrixFielding,fielding.getNumberOfTrials());
        /// TODO: Get versions of experiment from somewhere else.
        studyID << "fdv_1";
        if (!report.isEmpty()){
            emit(appendMessage(report,MSG_TYPE_STD));
        }
        else emit(appendMessage(emp.getError(),MSG_TYPE_ERR));
    }

    // Generating the resport based on available data.
    QHash<qint32,bool> what2Add;
    what2Add[STAT_ID_ENCODING_MEM_VALUE] = !matrixBindingBC.isEmpty() && !matrixBindingUC.isEmpty();
    what2Add[STAT_ID_TOTAL_FIXATIONS] = !matrixReading.isEmpty();
    generateReportFile(emp.getResults(),what2Add);
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
    dbdatafile.close();
    emit(appendMessage("DB Data File Generated to: " + dbdatafile.fileName(),MSG_TYPE_SUCC));

}

void RawDataProcessor::generateReportFile(const DataSet::ProcessingResults &res, const QHash<qint32,bool> whatToAdd){

    reportFileOutput = config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_REPORT_NAME;
    reportFileOutput = reportFileOutput + "_" + QDateTime::currentDateTime().toString("yyyy_MM_dd") + ".rep";

    // Adding the required report data
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_PATIENT_NAME,config->getString(CONFIG_PATIENT_NAME));
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_PATIENT_AGE,config->getString(CONFIG_PATIENT_AGE));
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_DOCTOR_NAME,config->getString(CONFIG_DOCTOR_NAME));
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_REPORT_DATE,QDateTime::currentDateTime().toString("dd/MM/yyyy"));

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

QString RawDataProcessor::csvGeneration(EDPBase *processor, const QString &id, const QString &dataFile, const QString &header){
    QString data;
    QString exp;

    if (dataFile.isEmpty()){
        emit(appendMessage(id + " processing was configured but no " + id + " file was found",MSG_TYPE_ERR));
        return "";
    }

    if (!separateInfoByTag(dataFile,header,&data,&exp)){
        return "";
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


    return processor->getOuputMatrixFileName();

}

bool RawDataProcessor::separateInfoByTag(const QString &file, const QString &tag, QString *data, QString *experiment){

    QFile f(file);
    if (!f.open(QFile::ReadOnly)){
        emit(appendMessage("Could not open " + file + " for reading",2));
        return false;
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
            if (line.contains(tag,Qt::CaseSensitive)) tagFound++;
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

    if (tagFound == 2) return true;
    else{
        emit(appendMessage("Could not find the tag " + tag + " in the input file: " + file,2));
        return false;
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
