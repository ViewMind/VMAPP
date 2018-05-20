#include "maingui.h"
#include "ui_maingui.h"

MainGUI::MainGUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainGUI)
{
    ui->setupUi(this);

    // Setting the title
    this->setWindowTitle("EyeExperimenter " + QString(EXPERIMENTER_VERSION) + EXPANDED_VERSION);

    // Null experiment to begin with
    experiment = nullptr;

    // Intialize the logger
    logger.setLogInterface(ui->pteLog);

    // Initialize eye tracker pointer.
    eyeTracker = nullptr;
    ui->actionCalibrate_Eye_Tracker->setEnabled(false);
    ui->actionStart_Experiment->setEnabled(false);

    // Loading configuration.
    if (!configuration.loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        logger.appendError("ERROR Loading configuration file: " + configuration.getError() + ". Please correct the error and restart the program");
        ui->actionStart_Experiment->setEnabled(false);
    }

    // Checking the output directory exists.
    if (!QDir(configuration.getString(CONFIG_OUTPUT_DIR)).exists()){
        logger.appendError("ERROR: Output directory: " + configuration.getString(CONFIG_OUTPUT_DIR) + " could not be found. Please correct this and restart the program");
        ui->actionStart_Experiment->setEnabled(false);
    }

    // Connecting the processor thread to this one
    connect(&thread,&ProcessorThread::finished,this,&MainGUI::on_pthread_finished);
    thread.setConfiguration(&configuration);

    // Second monitor handle.
    monitor = nullptr;

    // Moving the windows if necessary
    setWidgetPositions();

#ifdef SHOW_CONSOLE
    qWarning() << "Starting application";
#endif



}


MainGUI::~MainGUI()
{
    delete ui;
}


//********************************* GUI Methods ************************************

void MainGUI::on_actionConfigure_triggered()
{
    SettingsDialog diag(this,&configuration);
    diag.exec();
}

void MainGUI::on_actionStart_Experiment_triggered()
{

    if (!checkResolution()) return;

    ExperimentChooserDialog chooser(this);

    quint8 age = configuration.getInt(CONFIG_PATIENT_AGE);
    QString pname = configuration.getString(CONFIG_PATIENT_NAME);
    if (pname.isEmpty()) pname = "No name set";

    // Due to the casting this is enough of a check for default values
    if ((age < 6) || (age > 130)) age = 30;

    chooser.setPatientData(pname,age,configuration.getString(CONFIG_PATIENT_EMAIL));

    if (chooser.exec() != QDialog::Accepted){
        // The user pressed cancel, so nothing to do.
        return;
    }

    // Setting the values to the configuration.
    QString outDir = configuration.getString(CONFIG_OUTPUT_DIR);
    QDir repo(outDir);
    pname = chooser.getPatientName();
    QString patientDir = chooser.getPatientName().replace(" ","_");
    outDir = outDir + "/" + patientDir;

    QString err;
    QStringList errors;
    err = ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_PATIENT_NAME,pname,&configuration);
    if(!err.isEmpty()) errors << err;
    err = ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_PATIENT_EMAIL,chooser.getEmail(),&configuration);
    if(!err.isEmpty()) errors << err;
    err = ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_PATIENT_AGE,chooser.getPatientsAge(),&configuration);    
    if(!err.isEmpty()) errors << err;
    err = ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_PATIENT_DIRECTORY,outDir,&configuration);
    if(!err.isEmpty()) errors << err;


    if (!errors.isEmpty()){
        logger.appendWarning("WARNING: Could not set data to the configuration file. Reasons: <br>" + errors.join("<br>"));
    }

    experimentSequence = chooser.getSelectedSequence();
    expInSeq = 0;

    if (patientDir.isEmpty()){
        logger.appendError("Cannot continue without a patient name");
        return;
    }

    configuration.addKeyValuePair(CONFIG_OUTPUT_DIR,outDir);

    // Creating the patient directory if it doesn't exist
    if (!QDir(outDir).exists()){
        repo.mkdir(patientDir);
    }

    // It should exist now.
    if (!QDir(outDir).exists()){
        logger.appendError("Could not create patient directory: " + outDir);
        return;
    }

    startExperiment();
}

void MainGUI::on_actionConnect_to_EyeTracker_triggered()
{
    EyeTrackerSelectionDialog diag(this);

    // If the user pressed Cancel, then there is nothing to do.
    if (diag.exec() != QDialog::Accepted){
        return;
    }

    qint32 eyeTrackerSelected = diag.getSelectedEyeTracker();

    // Using polymorphism, the inteface for the eyetracker is created.
    switch (eyeTrackerSelected){
    case EYE_TRACKER_MOUSE:
        eyeTracker = new MouseInterface();
        break;
    case EYE_TRACKER_REDM:
        eyeTracker = new REDInterface();
        break;
    default:
        logger.appendError("Eye Tracker not supported.");
        return;
    }

    EyeTrackerInterface::ExitStatus es = eyeTracker->connectToEyeTracker();
    switch (es){
    case EyeTrackerInterface::ES_FAIL:
        logger.appendError("Error Connecting to the EyeTracker: " + eyeTracker->getMessage());
        delete eyeTracker;
        eyeTracker = nullptr;
        return;
    case EyeTrackerInterface::ES_SUCCESS:
        logger.appendSuccess("Connection to EyeTracker established successfully!");
        break;
    case EyeTrackerInterface::ES_WARNING:
        logger.appendStandard("Warning: Connecting to the EyeTracker, " + eyeTracker->getMessage());
        break;
    }

    // If all went well, calibration is enabled, and the connect action is disabled, to force a restart of the program
    // if the Eye Tracker wants to be changed.
    ui->actionCalibrate_Eye_Tracker->setEnabled(true);
    ui->actionConnect_to_EyeTracker->setEnabled(false);

    logger.appendStandard("Before starting any experiments, please perform a calibration of the eye tracker");
}

void MainGUI::on_actionCalibrate_Eye_Tracker_triggered()
{

    if (!checkResolution()) return;

    EyeTrackerCalibrationParameters calibrationParams;
    calibrationParams.forceCalibration = true;
    calibrationParams.name = "";
    EyeTrackerInterface::ExitStatus es = eyeTracker->calibrate(calibrationParams);
    switch (es){
    case EyeTrackerInterface::ES_FAIL:
        logger.appendError("Error Calibrating the EyeTracker: " + eyeTracker->getMessage());
        break;
    case EyeTrackerInterface::ES_SUCCESS:
        logger.appendSuccess("Calibration completed successfully!");
        break;
    case EyeTrackerInterface::ES_WARNING:
        logger.appendError("Warning: Calibrating the EyeTracker, " + eyeTracker->getMessage());
        break;
    }

    if (es == EyeTrackerInterface::ES_SUCCESS){
        if (experiment != nullptr){
            experiment->togglePauseExperiment();
        }
    }
    else if (experiment != nullptr){

        // Experiment should be aborted, as calibration is not possible.
        disconnect(experiment,&Experiment::experimentEndend,this,&MainGUI::on_experimentFinished);
        disconnect(experiment,&Experiment::calibrationRequest,this,&MainGUI::on_actionCalibrate_Eye_Tracker_triggered);
        disconnect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
        delete experiment;
        experiment = nullptr;
        ui->actionStart_Experiment->setEnabled(true);

        logger.appendError("Experiment aborted, as calibration has failed.");

    }

    // The action to start an experiment is now enabled.
    ui->actionStart_Experiment->setEnabled(true);

}

//********************************* Experiment Init ************************************
void MainGUI::startExperiment(){

    qint32 expSelected = experimentSequence.at(expInSeq);
    QBrush background;
    QString instruction = "";

    // Using the polimorphism, the experiment object is created according to the selected index.
    QString readingQuestions;
    switch (expSelected){
    case EXP_READING:
        readingQuestions = ":/expdata/experiment_data/Reading_" + configuration.getString(CONFIG_REPORT_LANGUAGE) + ".dat";
        configuration.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,readingQuestions);
        experiment = new ReadingExperiment();
        background = QBrush(Qt::gray);
        instruction = ":/images/images/reading";
        break;
    case EXP_BINDING_BC:
        configuration.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,getBindingExperiment(true));
        experiment = new ImageExperiment(true);
        background = QBrush(Qt::gray);
        instruction = ":/images/images/binding_bc";
        break;
    case EXP_BINDING_UC:
        configuration.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,getBindingExperiment(false));
        experiment = new ImageExperiment(false);
        background = QBrush(Qt::gray);
        instruction = ":/images/images/binding_uc";
        break;
    case EXP_FIELDNG:
        configuration.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,":/expdata/experiment_data/fielding.dat");
        experiment = new FieldingExperiment();
        background = QBrush(Qt::black);
        instruction = ":/images/images/fielding";
        break;
    default:
        logger.appendError("Experiment not supported yet");
        return;
    }

    instruction = instruction + "_" + configuration.getString(CONFIG_REPORT_LANGUAGE) + ".jpeg";

    // Showing the title card.
    if (!instruction.isEmpty()){
        InstructionDialog diag(this);
        diag.setInstruction(instruction);
        if (monitor != nullptr){
            monitor->show();
            monitor->updateBackground(diag.getPixmap());
        }
        diag.exec();
    }

    // Making sure that that both experiment signals are connected.
    // Eyetracker should be connected by this point.
    connect(experiment,&Experiment::experimentEndend,this,&MainGUI::on_experimentFinished);
    connect(experiment,&Experiment::calibrationRequest,this,&MainGUI::on_actionCalibrate_Eye_Tracker_triggered);
    connect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);

    if (monitor != nullptr){
        connect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
        connect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
        monitor->setBackgroundBrush(background);
    }

    // Once everything has been set, the experiment can continue, and this can't be called until the experiment is over.
    ui->actionStart_Experiment->setEnabled(false);

    // Setting the debug mode according to the current configuration.
    experiment->setDebugMode(configuration.getBool(CONFIG_DUAL_MONITOR_MODE));

    // Making sure that the eyetracker is sending data.
    eyeTracker->enableUpdating(true);

    // Start the experiment.
    experiment->startExperiment(&configuration);

    if (monitor != nullptr){
        monitor->show();
    }

}

//********************************* Custom Slots ************************************
void MainGUI::on_experimentFinished(const Experiment::ExperimentResult &er){

    // Make the experiment windown invisible.
    experiment->hide();

    // Do after stuff
    bool continueSequence = false;

    // Hide monitor if showing
    if (monitor != nullptr) monitor->hide();

    switch (er){
    case Experiment::ER_ABORTED:
        logger.appendStandard("Experiment aborted");
        break;
    case Experiment::ER_FAILURE:
        logger.appendError("EXPERIMENT FAILURE: " + experiment->getError());
        break;
    case Experiment::ER_NORMAL:
        logger.appendSuccess("Experiment finished sucessfully!");
        continueSequence = true;
        break;
    case Experiment::ER_WARNING:
        logger.appendStandard("EXPERIMENT WARNING: " + experiment->getError());
        continueSequence = true;
        break;
    }

    // Destroying the experiment and reactivating the start experiment.
    disconnect(experiment,&Experiment::experimentEndend,this,&MainGUI::on_experimentFinished);
    disconnect(experiment,&Experiment::calibrationRequest,this,&MainGUI::on_actionCalibrate_Eye_Tracker_triggered);
    disconnect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
    if (monitor != nullptr){
        disconnect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
        disconnect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
    }
    delete experiment;
    experiment = nullptr;

    // Checking if the sequence is over    
    bool done = true;    
    if (continueSequence){
        if (expInSeq < experimentSequence.size()-1){
            expInSeq++;
            done = false;
        }
    }

    // Continuing if need be.
    if (done){
        ui->actionStart_Experiment->setEnabled(true);
        // Also making sure that the eyetracker stops receiving data.
        eyeTracker->enableUpdating(false);
        if (thread.isProcessingEnabled()) thread.start();
    }
    else{
        startExperiment();
    }
}

void MainGUI::on_pthread_finished(){
    if (!thread.didItStart()){
        logger.appendError("ERROR: Could not start processing program. Please make sure that the path is correct in "
                           + QString(SCRIPT_CALL_PROCESSOR) + " script.");
    }
    else if (thread.didItCrash()){
        logger.appendError("ERROR: The processor program crashed.");
    }
    else{
        if (!thread.getError().isEmpty()){
            logger.appendError("ERROR: " + thread.getError());
        }
    }
}

bool MainGUI::checkResolution(){
    // Checking correct resolution.
    QDesktopWidget *desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry(this);
    if ((screen.width() != SCREEN_W) || (screen.height() != SCREEN_H)){
        logger.appendError("Cannot start experiment with the resolution: " + QString::number(screen.width()) + "x" + QString::number(screen.height())
                           + ". Required is: " +
                           QString::number(SCREEN_W) + "x" + QString::number(SCREEN_H)
                           + ". Please change your screen resolution.");
        return false;
    }
    return true;
}

void MainGUI::setWidgetPositions(){
    QDesktopWidget *desktop = QApplication::desktop();
    if (desktop->screenCount() < 2) return;
    if (!configuration.getBool(CONFIG_DUAL_MONITOR_MODE)) return;

    // Moving the main window
    QRect mainScreen = desktop->screenGeometry(0);
    this->move(QPoint(mainScreen.x(),mainScreen.y()));

    // Setting up the monitor
    QRect secondScreen = desktop->screenGeometry(1);
    monitor = new MonitorScreen(this,secondScreen);
}

QString MainGUI::getBindingExperiment(bool bc){

    QString defaultExp;
    if (bc) defaultExp = ":/expdata/experiment_data/bc.dat";
    else defaultExp = ":/expdata/experiment_data/uc.dat";

    if (!configuration.containsKeyword(CONFIG_BINDING_DEFAULT)) {
        configuration.addKeyValuePair(CONFIG_BINDING_DEFAULT,false);
        return defaultExp;
    }

    if (configuration.getBool(CONFIG_BINDING_DEFAULT)) return defaultExp;

    // If it got here, then that means it is one of the expanede experiment
    QString exp = ":/expdata/experiment_data/expanded_binding/";
    if (bc) exp = exp + "bc_";
    else exp = exp + "uc_";

    // The number of targets
    exp = exp + QString::number(configuration.getInt(CONFIG_BINDING_NUM_TARGETS)) + "_";

    if (configuration.getBool(CONFIG_BINDING_USE_NUMBERS)) exp = exp + "n.dat";
    else exp = exp + "x.dat";

    return exp;

}


