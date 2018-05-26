#include "processorthread.h"

ProcessorThread::ProcessorThread(){
    config = nullptr;
}

void ProcessorThread::setConfiguration(ConfigurationManager *c){
    config = c;
}

bool ProcessorThread::isProcessingEnabled() const{
    if (config == nullptr) return false;
    return !config->getString(CONFIG_EYEPROCESSOR_PATH).isEmpty();
}

void ProcessorThread::run(){

    if (config == nullptr) return;

    // Creating the batch file
    QFile file(SCRIPT_CALL_PROCESSOR);
    if (!file.open(QFile::WriteOnly)){
        error = "Could not open " + QString(SCRIPT_CALL_PROCESSOR) + " for writing";
        return;
    }

    QString processorPath = config->getString(CONFIG_EYEPROCESSOR_PATH);

    QTextStream writer(&file);
    writer << "cd \"" << processorPath << "\"\n";
    writer << SCRIPT_EYEPROCESSOR_EXE;
    file.close();

    // Replacing the in the configuration file.
    QString confFile = processorPath + "/" + FILE_CONFIGURATION;
    QStringList errors;

    writeIntoConfiguration(confFile,CONFIG_PATIENT_DIRECTORY,&errors);
    writeIntoConfiguration(confFile,CONFIG_PATIENT_AGE,&errors);
    writeIntoConfiguration(confFile,CONFIG_DOCTOR_NAME,&errors);
    writeIntoConfiguration(confFile,CONFIG_PATIENT_NAME,&errors);
    writeIntoConfiguration(confFile,CONFIG_REPORT_LANGUAGE,&errors);
    writeIntoConfiguration(confFile,CONFIG_DEMO_MODE,&errors);

    //qWarning() << "STARTING";
    if (errors.isEmpty()){
        finishCode = 0;
        finishCode = QProcess::execute(SCRIPT_CALL_PROCESSOR);
    }
}


void ProcessorThread::writeIntoConfiguration(const QString &confFile, const QString &configName2Copy, QStringList *errors){
    QString e =  ConfigurationManager::setValue(confFile,
                                                COMMON_TEXT_CODEC,
                                                configName2Copy,
                                                config->getString(configName2Copy),
                                                nullptr);
    if (!e.isEmpty()) errors->append(e);
}
