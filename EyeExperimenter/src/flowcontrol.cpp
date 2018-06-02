#include "flowcontrol.h"

FlowControl::FlowControl(QObject *parent, LogInterface *l, ConfigurationManager *c) : QObject(parent)
{
    // Intializing the eyetracker pointer
    configuration = c;
    eyeTracker = nullptr;
    logger = l;
    connected = false;
    experiment = nullptr;
    monitor = nullptr;

    // Calling the screen resolution in order to fix those values.
//    QDesktopWidget *desktop = QApplication::desktop();
//    QRect screen = desktop->screenGeometry(this);
//    configuration.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,screen.width());
//    configuration.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,screen.height());

}


bool FlowControl::connectToEyeTracker(){

    // Creating the connection with the EyeTracker
    if (eyeTracker != nullptr) delete eyeTracker;

    QString eyeTrackerSelected = configuration->getString(CONFIG_SELECTED_ET);

    if (eyeTrackerSelected == CONFIG_P_ET_MOUSE){
        eyeTracker = new MouseInterface();
    }
    else if (eyeTrackerSelected == CONFIG_P_ET_REDM){
        return false;
        //eyeTracker = new REDInterface();
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
        monitor->show();
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
