#include "eyereportui.h"
#include "ui_eyereportui.h"

EyeReportUI::EyeReportUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EyeReportUI)
{
    ui->setupUi(this);

    this->setWindowTitle("EyeReport Generator  - " + QString(EYE_REP_GEN_VERSION));

    log.setLogInterface(ui->plainTextEdit);
    ui->progressBar->setValue(0);

    connect(&pthread,&EyeDataProcessingThread::finished,this,&EyeReportUI::onPThreadFinished);
    connect(&pthread,&EyeDataProcessingThread::appendMessage,this,&EyeReportUI::onAppendMsg);
    connect(&pthread,&EyeDataProcessingThread::updateProgressBar,this,&EyeReportUI::onUpdatePBar);

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
    cmd.optional = true;
    cv[CONFIG_PATIENT_AGE] = cmd;

    cmd.clear();
    cv[CONFIG_REPORT_LANGUAGE] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_BOOL;
    cmd.optional = true;
    cv[CONFIG_ENABLE_REPORT_GEN] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_BOOL;
    cmd.optional = true;
    cv[CONFIG_DEMO_MODE] = cmd;

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

    // Attempting to load the configuration.
    if (!configuration.loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        log.appendError("Configuration file errors:<br>"+configuration.getError());
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

    // Deleting the tex file if it exists;
    QString outImage = configuration.getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_REPORT_NAME  + ".png";
    QFile(outImage).remove();

#ifdef TEST_REPORT
    testReport();
#else
    pthread.initialize(&configuration,experimentsToExec);
    pthread.start();
#endif

}


void EyeReportUI::onPThreadFinished(){
    // This means that I can show the report if exists.
    QString reportImage = configuration.getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_REPORT_NAME  + ".png";

    if (!configuration.getBool(CONFIG_RUN_AND_QUIT)){
        if (configuration.getBool(CONFIG_ENABLE_REPORT_GEN)){
            if (QFile(reportImage).exists()){
                ReportViewer *viewer = new ReportViewer(this);
                viewer->loadReport(reportImage);
                viewer->show();
            }
        }
    }
    else{
        this->close();
    }
}

void EyeReportUI::onUpdatePBar(qint32 value){
    ui->progressBar->setValue(value);
}

void EyeReportUI::onAppendMsg(const QString &msg, qint32 type){
    if (type == MSG_TYPE_STD) log.appendStandard(msg);
    else if (type == MSG_TYPE_SUCC) log.appendSuccess(msg);
    else if (type == MSG_TYPE_ERR) log.appendError(msg);
    else log.appendWarning(msg);
}

EyeReportUI::~EyeReportUI()
{
    delete ui;
}

void EyeReportUI::testReport(){

    qWarning() << "Test Report";

    DataSet::ProcessingResults res;
    res[STAT_ID_ENCODING_MEM_VALUE] = 0.00987;
    res[STAT_ID_FIRST_STEP_FIXATIONS] = 65;
    res[STAT_ID_MULTIPLE_FIXATIONS] = 16.62;
    res[STAT_ID_TOTAL_FIXATIONS] = 600;
    res[STAT_ID_SINGLE_FIXATIONS] = 45.55;

    QHash<qint32,bool> what2Add;
    what2Add[STAT_ID_TOTAL_FIXATIONS] = true;
    what2Add[STAT_ID_ENCODING_MEM_VALUE] = true;

    ImageReportDrawer drawer;
    drawer.drawReport(res,&configuration,what2Add);

    QString reportImage = configuration.getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_REPORT_NAME  + ".png";
    if (QFile(reportImage).exists()){
        ReportViewer *viewer = new ReportViewer(this);
        viewer->loadReport(reportImage);
        viewer->show();
    }
}

void EyeReportUI::loadArguments(){
    QStringList arguments = QApplication::arguments();

    // Parsing arguments to the command line
    QString field = "";
    QString value = "";

    // Bye default it is not a quit and run
    configuration.addKeyValuePair(CONFIG_RUN_AND_QUIT,false);

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


void EyeReportUI::addToConfigFromCmdLine(const QString &field, const QString &value){
    if (!field.isEmpty()){
        if (field == CONFIG_PATIENT_DIRECTORY){
            // This should be "run-and-quit" type run
            configuration.addKeyValuePair(CONFIG_RUN_AND_QUIT,true);
            // Also ensures fast processing
            configuration.addKeyValuePair(CONFIG_FAST_PROCESSING,true);
        }
    }
    //qWarning() << "Adding" << field << "<|>" <<  value;
    configuration.addKeyValuePair(field,value);
}
