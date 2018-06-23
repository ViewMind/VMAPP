#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{

    connect(&data_processor,&RawDataProcessor::appendMessage,this,&Control::onAppendMsg);

    // Creating the configuration verifier
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_REAL;
    cv[CONFIG_DISTANCE_2_MONITOR] = cmd;

    cmd.clear();
    cmd.optional = true;
    cv[CONFIG_DOCTOR_NAME] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_BOOL;
    cmd.optional = true;
    cv[CONFIG_FAST_PROCESSING] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_LATENCY_ESCAPE_RAD] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_REAL;
    cv[CONFIG_MARGIN_TARGET_HIT] = cmd;
    cv[CONFIG_SAMPLE_FREQUENCY] = cmd;
    cv[CONFIG_XPX_2_MM] = cmd;
    cv[CONFIG_YPX_2_MM] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_REAL;
    cv[CONFIG_MIN_FIXATION_LENGTH] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_MOVING_WINDOW_DISP] = cmd;

    cmd.clear();
    cmd.optional = true;
    cv[CONFIG_PATIENT_DIRECTORY] = cmd;

    cmd.fields << ConfigurationManager::VT_STRING;
    cmd.type = ConfigurationManager::VT_LIST;
    cmd.optional = true;
    cv[CONFIG_EXP_LIST] = cmd;

    cmd.clear();
    cmd.optional = true;
    cv[CONFIG_PATIENT_NAME] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_VALID_EYE] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cmd.optional = true;
    cv[CONFIG_PATIENT_AGE] = cmd;

    cmd.clear();
    cmd.optional = true;
    cv[CONFIG_REPORT_LANGUAGE] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_BOOL;
    cmd.optional = true;
    cv[CONFIG_REPORT_NO_LOGO] = cmd;

    cmd.clear();
    cmd.optional = true;
    cv[CONFIG_BC_FILE_FILTER] = cmd;
    cv[CONFIG_UC_FILE_FILTER] = cmd;

    cmd.clear();
    cmd.optional = true;
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_DAT_TIME_FILTER_THRESHOLD] = cmd;

    configuration.setupVerification(cv);

}

void Control::exitProgram(){
    log.write(QString(EYE_REP_GEN_NAME) + ".html",QString(EYE_REP_GEN_NAME) + " - " + QString(EYE_REP_GEN_VERSION));
    emit(done());
}

void Control::run(){

    // Attempting to load the configuration.
    if (!configuration.loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        log.appendError("Configuration file errors:<br>"+configuration.getError());
        exitProgram();
        return;
    }

    // Checking which experiments to run
    QSet<QString> validExperiments;
    validExperiments << CONFIG_P_EXP_BIDING_UC;
    validExperiments << CONFIG_P_EXP_BIDING_BC;
    validExperiments << CONFIG_P_EXP_FIELDING;
    validExperiments << CONFIG_P_EXP_READING;
    QSet<QString> experimentsToExec;

    QStringList exp = configuration.getStringList(CONFIG_EXP_LIST);
    if (!exp.isEmpty()){
        for (qint32 i = 0; i < exp.size(); i++){
            if (validExperiments.contains(exp.at(i))){
                experimentsToExec << exp.at(i);
            }
            else{
                log.appendError("Unknown experiment found: " + exp.at(i) + ".");
                exitProgram();
                return;
            }
        }
    }

    // Do all experiments.
    else experimentsToExec = validExperiments;

    // Checking about the time filter.
    if (!configuration.containsKeyword(CONFIG_DAT_TIME_FILTER_THRESHOLD)){
        log.appendWarning("No DAT time filter threshold has been set. Using 0. This means the file will not be filtered");
        configuration.addKeyValuePair(CONFIG_DAT_TIME_FILTER_THRESHOLD,(qint32)0);
    }

    // Checking if any of the parameters are overwritten via command line.
    loadArguments();

    // Starting the data processing.
    data_processor.initialize(&configuration,experimentsToExec);
    data_processor.run();

    // All is done, the program exits.
    exitProgram();
}


void Control::onAppendMsg(const QString &msg, qint32 type){
    if (type == MSG_TYPE_STD) log.appendStandard(msg);
    else if (type == MSG_TYPE_SUCC) log.appendSuccess(msg);
    else if (type == MSG_TYPE_ERR) log.appendError(msg);
    else log.appendWarning(msg);
}


void Control::loadArguments(){
    QStringList arguments = QCoreApplication::arguments();

    // Parsing arguments to the command line
    QString field = "";
    QString value = "";

    // Not many checks are done as if nothing is replaced or the arguments are incorrect, the
    // report will be generated with the values from the file or an error will be generated.
    // Also the first value is skipped as it is simply the program itself.
    for (qint32 i = 1; i < arguments.size(); i++){
        QString val = arguments.at(i);
        //qWarning() << "val is" << val;
        if (val.startsWith('-')){
            // This a field, which means that the last one needs to be saved.
            addToConfigFromCmdLine(field,value);
            value = "";
            field = val.remove(0,1);
        }
        else{
            // Anything without an - is concatenated as part of the value.
            if (value.isEmpty()) value = val;
            else value = value +" " + val;
        }
    }

    // The last field value pair needs to be added after the loop
    addToConfigFromCmdLine(field,value);


}


void Control::addToConfigFromCmdLine(const QString &field, const QString &value){
    if (!field.isEmpty()){
        if (field == CONFIG_PATIENT_DIRECTORY){
            // Also ensures fast processing
            configuration.addKeyValuePair(CONFIG_FAST_PROCESSING,true);
        }
    }
    configuration.addKeyValuePair(field,value);
}
