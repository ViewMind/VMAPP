#include "flowcontrol.h"

FlowControl::FlowControl(QWidget *parent, LogInterface *l, ConfigurationManager *c) : QWidget(parent)
{
    // Intializing the eyetracker pointer
    configuration = c;
    eyeTracker = nullptr;
    logger = l;
    connected = false;
    experiment = nullptr;
    monitor = nullptr;
    this->setVisible(false);

    // Launching the server if it was configured.
    QString server = configuration->getString(CONFIG_SSLSERVER_PATH);
    if (QFile(server).exists()){

        QFileInfo info(server);
        QString sOldPath = QDir::currentPath();
        QDir::setCurrent( info.absolutePath() );

        //Run it!
        //obProcess.startDetached(m_sLaunchFilename, obList);
        sslServer.start(server);

        QDir::setCurrent( sOldPath );

    }
    else{
        logger->appendWarning("SSL Server configured in path, but the file does not exist.");
    }

    // Creating the sslclient
    sslclient = new SSLClient(this,c,l);

    // Connection to the "finished" slot.
    connect(sslclient,SIGNAL(transactionFinished(bool)),this,SLOT(onSLLTransactionFinished(bool)));

}

void FlowControl::saveReport(){
    ImageReportDrawer reportDrawer;
    reportDrawer.drawReport(&reportData,configuration);
}

void FlowControl::saveReportAs(const QString &title){

    QString newFileName = QFileDialog::getSaveFileName(nullptr,title,"","*.png");
    if (newFileName.isEmpty()) return;
    if (!QFile::copy(configuration->getString(CONFIG_IMAGE_REPORT_PATH),newFileName)){
        logger->appendError("Could not save report from " + configuration->getString(CONFIG_IMAGE_REPORT_PATH) + " to " + newFileName);
    }
}

void FlowControl::resolutionCalculations(){
    QDesktopWidget *desktop = QApplication::desktop();
    // This line will assume that the current screen is the one where the experiments will be drawn.
    QRect screen = desktop->screenGeometry(desktop->screenNumber());
    configuration->addKeyValuePair(CONFIG_RESOLUTION_WIDTH,screen.width());
    configuration->addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,screen.height());
}

void FlowControl::onSLLTransactionFinished(bool allOk){
    sslTransactionAllOk = allOk;
    if (allOk){
        // Loading the report
        reportData.clear();
        // The ssl client should have set up the config report path.
        if (!reportData.loadConfiguration(configuration->getString(CONFIG_REPORT_PATH),COMMON_TEXT_CODEC)){
            logger->appendError("Error loading the report data file: " + reportData.getError());
            sslTransactionAllOk = false;
        }
    }
    emit(sslTransactionFinished());
}

QString FlowControl::getReportDataField(const QString &key){
    if (reportData.containsKeyword(key)){
        return reportData.getString(key);
    }
    else return "N/A";
}

int FlowControl::getReportResultBarPosition(const QString &key){

    qreal value = reportData.getReal(key);
    QList<qreal> list;
    bool largerBetter;

    if (key == CONFIG_RESULTS_ATTENTIONAL_PROCESSES){
        list << 450 << 550 << 650 << 750;
        largerBetter = false;
    }
    else if (key == CONFIG_RESULTS_EXECUTIVE_PROCESSES){
        list << -4 << 18 << 40 << 62;
        largerBetter = false;
    }
    else if (key == CONFIG_RESULTS_MEMORY_ENCODING){
        list << -0.991 << 0.009 << 1.009;
        largerBetter = true;
    }
    else if (key == CONFIG_RESULTS_RETRIEVAL_MEMORY){
        list << 7 << 15 << 23 << 31;
        largerBetter = true;
    }
    else if (key == CONFIG_RESULTS_WORKING_MEMORY){
        list << 50 << 60 << 70 << 80;
        largerBetter = true;
    }
    else{
        return 0;
    }

    return calculateSegment(value,list,largerBetter);

}


int FlowControl::calculateSegment(qreal value, QList<qreal> segments, bool largerBetter){
    int result = 0;
    result = 0;
    for (qint32 i = 0; i < segments.size()-1; i++){
        if (value < segments.at(i)){
            break;
        }
        result = i;
    }
    // If larger is better then the indexes are inverted as 0 represents green which is good and 2 represents red.
    if (largerBetter){
        if (segments.size() == 4){
            if (result == 0) result = 2;
            else if (result == 2) result = 0;
        }
        else{
            if (result == 0) result = 1;
            else if (result == 1) result = 0;
        }
    }

    return result;
}

void FlowControl::eyeTrackerChanged(){
    if (eyeTracker != nullptr){
        if (experiment != nullptr){
            disconnect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
        }
        delete eyeTracker;
        eyeTracker = nullptr;
    }
    connected = false;
}

bool FlowControl::connectToEyeTracker(){

    // Creating the connection with the EyeTracker
    if (eyeTracker != nullptr) delete eyeTracker;

    QString eyeTrackerSelected = configuration->getString(CONFIG_SELECTED_ET);

    if (eyeTrackerSelected == CONFIG_P_ET_MOUSE){
        eyeTracker = new MouseInterface();
    }
    else if (eyeTrackerSelected == CONFIG_P_ET_REDM){
        eyeTracker = new REDInterface();
    }
    else{
        logger->appendError("Selected unknown EyeTracker: " + eyeTrackerSelected);
        return false;
    }

    EyeTrackerInterface::ExitStatus es = eyeTracker->connectToEyeTracker();
    switch (es){
    case EyeTrackerInterface::ES_FAIL:
        logger->appendError("Error Connecting to the EyeTracker: " + eyeTracker->getMessage());
        delete eyeTracker;
        eyeTracker = nullptr;
        return false;
    case EyeTrackerInterface::ES_SUCCESS:
        logger->appendSuccess("Connection to EyeTracker established successfully!");
        break;
    case EyeTrackerInterface::ES_WARNING:
        logger->appendWarning("Connecting to the EyeTracker, " + eyeTracker->getMessage());
        break;
    }

    connected = true;
    return true;

}

bool FlowControl::calibrateEyeTracker(){
    EyeTrackerCalibrationParameters calibrationParams;
    calibrationParams.forceCalibration = true;
    calibrationParams.name = "";
    EyeTrackerInterface::ExitStatus es = eyeTracker->calibrate(calibrationParams);
    switch (es){
    case EyeTrackerInterface::ES_FAIL:
        logger->appendError("Error Calibrating the EyeTracker: " + eyeTracker->getMessage());
        return false;
        break;
    case EyeTrackerInterface::ES_SUCCESS:
        logger->appendSuccess("Calibration completed successfully!");
        break;
    case EyeTrackerInterface::ES_WARNING:
        logger->appendError("Warning: Calibrating the EyeTracker, " + eyeTracker->getMessage());
        break;
    }
    return true;
}

bool FlowControl::startNewExperiment(qint32 experimentID){

    QBrush background;
    experimentIsOk = true;

    // Making sure no old reports are stored.
    configuration->addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,"");

    // Using the polimorphism, the experiment object is created according to the selected index.
    QString readingQuestions;
    switch (experimentID){
    case EXP_READING:
        readingQuestions = ":/experiment_data/Reading_" + configuration->getString(CONFIG_REPORT_LANGUAGE) + ".dat";
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,readingQuestions);
        experiment = new ReadingExperiment();
        background = QBrush(Qt::gray);
        break;
    case EXP_BINDING_BC:
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,getBindingExperiment(true));
        experiment = new ImageExperiment(true);
        background = QBrush(Qt::gray);
        break;
    case EXP_BINDING_UC:
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,getBindingExperiment(false));
        experiment = new ImageExperiment(false);
        background = QBrush(Qt::gray);
        break;
    case EXP_FIELDNG:
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,":/experiment_data/fielding.dat");
        experiment = new FieldingExperiment();
        background = QBrush(Qt::black);
        break;
    default:
        logger->appendError("Unknown experiment was selected " + QString::number(experimentID));
        return false;
    }

    // Making sure that that both experiment signals are connected.
    // Eyetracker should be connected by this point.
    connect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    connect(experiment,&Experiment::calibrationRequest,this,&FlowControl::requestCalibration);
    connect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);

    if (monitor != nullptr){
        connect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
        connect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
        monitor->setBackgroundBrush(background);
    }

    // Setting the debug mode according to the current configuration.
    experiment->setDebugMode(configuration->getBool(CONFIG_DUAL_MONITOR_MODE));

    // Making sure that the eyetracker is sending data.
    eyeTracker->enableUpdating(true);

    // Start the experiment.
    experiment->startExperiment(configuration);

    if (monitor != nullptr){
        if (configuration->getBool(CONFIG_DUAL_MONITOR_MODE)) {
            monitor->show();
        }
        else monitor->hide();
    }

    return true;

}

void FlowControl::on_experimentFinished(const Experiment::ExperimentResult &er){

    // Make the experiment windown invisible.
    experiment->hide();

    // Hide monitor if showing
    if (monitor != nullptr) monitor->hide();

    switch (er){
    case Experiment::ER_ABORTED:
        logger->appendStandard("Experiment aborted");
        experimentIsOk = false;
        break;
    case Experiment::ER_FAILURE:
        logger->appendError("EXPERIMENT FAILURE: " + experiment->getError());
        experimentIsOk = false;
        break;
    case Experiment::ER_NORMAL:
        logger->appendSuccess("Experiment finished sucessfully!");
        break;
    case Experiment::ER_WARNING:
        logger->appendStandard("EXPERIMENT WARNING: " + experiment->getError());
        break;
    }

    // Destroying the experiment and reactivating the start experiment.
    disconnect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    disconnect(experiment,&Experiment::calibrationRequest,this,&FlowControl::requestCalibration);
    disconnect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
    if (monitor != nullptr){
        disconnect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
        disconnect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
    }
    delete experiment;
    experiment = nullptr;

    // Notifying the QML.
    emit(experimentHasFinished());

}

void FlowControl::requestCalibration(){
    /// TODO Enable calibration in the middle of the experiment.
}

QString FlowControl::getBindingExperiment(bool bc){

    QString defaultExp;
    if (bc) defaultExp = ":/experiment_data/bc.dat";
    else defaultExp = ":/experiment_data/uc.dat";

    if (!configuration->containsKeyword(CONFIG_BINDING_DEFAULT)) {
        configuration->addKeyValuePair(CONFIG_BINDING_DEFAULT,false);
        return defaultExp;
    }

    if (configuration->getBool(CONFIG_BINDING_DEFAULT)) return defaultExp;

    // If it got here, then that means it is one of the expanede experiment
    QString exp = ":/expdata/experiment_data/expanded_binding/";
    if (bc) exp = exp + "bc_";
    else exp = exp + "uc_";

    // The number of targets
    exp = exp + QString::number(configuration->getInt(CONFIG_BINDING_NUM_TARGETS)) + "_";

    if (configuration->getBool(CONFIG_BINDING_USE_NUMBERS)) exp = exp + "n.dat";
    else exp = exp + "x.dat";

    return exp;

}

void FlowControl::setupSecondMonitor(){

    QDesktopWidget *desktop = QApplication::desktop();
    if (desktop->screenCount() < 2) {
        // There is nothing to do as there is no second monitor.
        if (monitor == nullptr) return;
        // In case the monitor was disconnected.
        logger->appendWarning("Attempting to restore situation in which second monitor seems to have been disconnected after it was created");
        if (monitor != nullptr){
            if (experiment != nullptr){
                disconnect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
                disconnect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
            }
            delete monitor;
            monitor = nullptr;
        }
        return;
    }

    // Nothing to do.
    if (monitor != nullptr) return;

    // Creating the second monitor.
    if (!configuration->getBool(CONFIG_DUAL_MONITOR_MODE)) return;

    // Setting up the monitor
    QRect secondScreen = desktop->screenGeometry(1);
    monitor = new MonitorScreen(Q_NULLPTR,secondScreen,configuration->getReal(CONFIG_RESOLUTION_WIDTH),configuration->getReal(CONFIG_RESOLUTION_HEIGHT));
}

FlowControl::~FlowControl(){
    sslServer.kill();
}

