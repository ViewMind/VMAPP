#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{

    connect(&data_processor,&RawDataProcessor::appendMessage,this,&Control::onAppendMsg);

    // Creating the configuration verifier. This information MUST be passed in the configuration file.
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_REAL;
    cv[CONFIG_DISTANCE_2_MONITOR] = cmd;
    cv[CONFIG_MARGIN_TARGET_HIT] = cmd;
    cv[CONFIG_SAMPLE_FREQUENCY] = cmd;
    cv[CONFIG_XPX_2_MM] = cmd;
    cv[CONFIG_YPX_2_MM] = cmd;
    cv[CONFIG_MIN_FIXATION_LENGTH] = cmd;

    // The frequency check paramters
    cv[CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL] = cmd;
    cv[CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES] = cmd;
    cv[CONFIG_TOL_MAX_PERIOD_TOL] = cmd;
    cv[CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL] = cmd;
    cv[CONFIG_TOL_MIN_PERIOD_TOL] = cmd;
    cv[CONFIG_TOL_NUM_ALLOWED_FAILED_DATA_SETS] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_LATENCY_ESCAPE_RAD] = cmd;
    cv[CONFIG_MOVING_WINDOW_DISP] = cmd;
    cv[CONFIG_VALID_EYE] = cmd;
    cv[CONFIG_DAT_TIME_FILTER_THRESHOLD] = cmd;
    cv[CONFIG_PATIENT_AGE] = cmd;

    cmd.clear();
    cv[CONFIG_REPORT_FILENAME] = cmd;
    cv[CONFIG_PROTOCOL_NAME]   = cmd;

    cmd.optional = true;
    cv[CONFIG_FILE_BIDING_BC]        = cmd;
    cv[CONFIG_FILE_BIDING_UC]        = cmd;
    cv[CONFIG_FILE_READING]          = cmd;
    cv[CONFIG_FILE_FIELDING]         = cmd;
    cv[CONFIG_INST_ETSERIAL]         = cmd;
    cv[CONFIG_RESULT_ENTRY_ID]       = cmd;
    cv[CONFIG_TRANSACTION_ID]        = cmd;
    cv[CONFIG_DEMO_MODE]             = cmd;
    cv[CONFIG_REPROCESS_REQUEST]     = cmd;

    configuration.setupVerification(cv);

    // Adding the version to the log file
    log.appendStandard(QString(EYE_REP_GEN_NAME) + " - " + QString(EYE_REP_GEN_VERSION));

}

void Control::exitProgram(){
    emit(done());
}

void Control::run(){

    // Loading the configuration file from the command line.
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() != 2){
        // The only argument should be the full path of the configuration file.
        log.appendError("Wrong number of arguments: " + QString::number(arguments.size()) + ". The only argument should be the configuration file");
        exitProgram();
    }
    QString configFile = arguments.last();

    // Attempting to load the configuration.
    if (!configuration.loadConfiguration(configFile,COMMON_TEXT_CODEC)){
        log.appendError("Configuration file errors:<br>"+configuration.getError());
        exitProgram();
        return;
    }

    // Checking about the time filter.
    if (!configuration.containsKeyword(CONFIG_DAT_TIME_FILTER_THRESHOLD)){
        log.appendWarning("No DAT time filter threshold has been set. Using 0. This means the file will not be filtered");
        configuration.addKeyValuePair(CONFIG_DAT_TIME_FILTER_THRESHOLD,(qint32)0);
    }

    // Getting the work directory.
    QFileInfo fileInfo(configFile);

    if (configuration.containsKeyword(CONFIG_TRANSACTION_ID)){
        log.setID(configuration.getString(CONFIG_TRANSACTION_ID));
    }

    // Starting the data processing.
    configuration.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,fileInfo.absolutePath());
    data_processor.initialize(&configuration);
    data_processor.run();

    // All is done, the program exits.
    exitProgram();
}


void Control::onAppendMsg(const QString &msg, qint32 type){

    // Transforming HTML to text.
    QString plainText = msg;
    plainText = plainText.replace("<br>","\n");
    if (type == MSG_TYPE_STD) log.appendStandard(plainText);
    else if (type == MSG_TYPE_SUCC) log.appendSuccess(plainText);
    else if (type == MSG_TYPE_ERR) log.appendError(plainText);
    else log.appendWarning(plainText);
}


