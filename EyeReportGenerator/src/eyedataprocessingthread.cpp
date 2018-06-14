#include "eyedataprocessingthread.h"

EyeDataProcessingThread::EyeDataProcessingThread()
{
}

void EyeDataProcessingThread::initialize(ConfigurationManager *c, const QSet<QString> &exps){
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

    experiments = exps;

}


void EyeDataProcessingThread::run(){

    EyeMatrixProcessor emp;
    reportFileOutput = "";

    // Each of the processing fucntions is called. In this way, processing of just one can be accomplished
    // just by running this complete function but it will keep going if a file does not exist.
    if (experiments.contains(CONFIG_P_EXP_READING)) {
        emit(appendMessage("========== STARTED READING PROCESSING ==========",MSG_TYPE_SUCC));
        EDPReading reading(config);
        matrixReading = csvGeneration(&reading,"Reading",dataReading,HEADER_READING_EXPERIMENT);
        QString report = emp.processReading(matrixReading);
        if (!report.isEmpty()){
            emit(appendMessage(report,MSG_TYPE_STD));
        }
        else emit(appendMessage(emp.getError(),MSG_TYPE_ERR));
    }

    emit(updateProgressBar(0));
    if (experiments.contains(CONFIG_P_EXP_BIDING_BC)) {
        emit(appendMessage("========== STARTED BINDING BC PROCESSING ==========",MSG_TYPE_SUCC));
        EDPImages images(config);
        matrixBindingBC = csvGeneration(&images,"Binding BC",dataBindingBC,HEADER_IMAGE_EXPERIMENT);
        QString report = emp.processBinding(matrixBindingBC,true);
        if (!report.isEmpty()){
            emit(appendMessage(report,MSG_TYPE_STD));
        }
        else emit(appendMessage(emp.getError(),MSG_TYPE_ERR));
    }


    emit(updateProgressBar(0));
    if (experiments.contains(CONFIG_P_EXP_BIDING_UC)){
        emit(appendMessage("========== STARTED BINDING UC PROCESSING ==========",MSG_TYPE_SUCC));
        EDPImages images(config);
        matrixBindingUC = csvGeneration(&images,"Binding UC",dataBindingUC,HEADER_IMAGE_EXPERIMENT);
        QString report = emp.processBinding(matrixBindingUC,false);
        if (!report.isEmpty()){
            emit(appendMessage(report,MSG_TYPE_STD));
        }
        else emit(appendMessage(emp.getError(),MSG_TYPE_ERR));
    }

    emit(updateProgressBar(0));
    if (experiments.contains(CONFIG_P_EXP_FIELDING)) {
        emit(appendMessage("========== STARTED FIELDING PROCESSING ==========",MSG_TYPE_SUCC));
        EDPFielding fielding(config);
        matrixFielding = csvGeneration(&fielding,"Fielding",dataFielding,HEADER_FIELDING_EXPERIMENT);
        QString report = emp.processFielding(matrixFielding);
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

}

void EyeDataProcessingThread::generateReportFile(const DataSet::ProcessingResults &res, const QHash<qint32,bool> whatToAdd){

    QString patientFile = config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_PATIENT_INFO_FILE;
    // Loading the patient data
    ConfigurationManager patientData;
    patientData.loadConfiguration(patientFile,COMMON_TEXT_CODEC);

    // Setting default values, so that there will always be information.
    if (!patientData.containsKeyword(CONFIG_PATIENT_NAME)){
        patientData.addKeyValuePair(CONFIG_PATIENT_NAME,"N/A");
    }
    if (!patientData.containsKeyword(CONFIG_PATIENT_AGE)){
        patientData.addKeyValuePair(CONFIG_PATIENT_AGE,"N/A");
    }
    if (!config->containsKeyword(CONFIG_DOCTOR_NAME)){
        config->addKeyValuePair(CONFIG_DOCTOR_NAME,"N/A");
    }
    reportFileOutput = config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_REPORT_NAME;
    reportFileOutput = reportFileOutput + "_" + QDateTime::currentDateTime().toString("yyyy_MM_dd") + ".rep";

    // Adding the required report data
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_PATIENT_NAME,patientData.getString(CONFIG_PATIENT_NAME));
    ConfigurationManager::setValue(reportFileOutput,COMMON_TEXT_CODEC,CONFIG_PATIENT_AGE,patientData.getString(CONFIG_PATIENT_AGE));
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

    }
}

QString EyeDataProcessingThread::csvGeneration(EDPBase *processor, const QString &id, const QString &dataFile, const QString &header){
    QString data;
    QString exp;

    if (dataFile.isEmpty()){
        emit(appendMessage(id + " processing was configured but no " + id + " file was found",MSG_TYPE_ERR));
        return "";
    }

    //emit(appendMessage("-> Separating experiment data from recorded data from Binding " + ids + " experiment file",0));

    if (!separateInfoByTag(dataFile,header,&data,&exp)){
        return "";
    }

    // Resolution is now known.
    processor->initManager(config);

    processor->setFastProcessing(config->getBool(CONFIG_FAST_PROCESSING));

    //emit(appendMessage("-> Configuring " + id  + " data processor",MSG_TYPE_STD));

    connect(processor,&EDPBase::updateProgress,this,&EyeDataProcessingThread::onUpdateProgress);

    processor->setFieldingMarginInMM(config->getReal(CONFIG_MARGIN_TARGET_HIT));
    processor->setMonitorGeometry(mgeo);
    processor->setMovingWindowParameters(mwp);
    processor->calculateWindowSize();
    processor->setPixelsInSacadicLatency(config->getInt(CONFIG_LATENCY_ESCAPE_RAD));

    if (!processor->configure(dataFile,exp)){
        emit(appendMessage("ERROR: Configuring " + id + " experiment: " + processor->getError(),MSG_TYPE_ERR));
        return "";
    }

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

bool EyeDataProcessingThread::separateInfoByTag(const QString &file, const QString &tag, QString *data, QString *experiment){

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

bool EyeDataProcessingThread::getResolutionToConfig(const QString &firstline){
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

void EyeDataProcessingThread::onUpdateProgress(qint32 v){
    emit(updateProgressBar(v));
}
