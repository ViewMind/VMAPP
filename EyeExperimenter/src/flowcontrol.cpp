#include "flowcontrol.h"

FlowControl::FlowControl(QWidget *parent, ConfigurationManager *c) : QWidget(parent)
{
    // Intializing the eyetracker pointer
    configuration = c;
    eyeTracker = nullptr;
    connected = false;
    calibrated = false;
    experiment = nullptr;
    monitor = nullptr;
    demoTransaction = false;
    this->setVisible(false);

    // For debugging.
    connect(&sslServer,SIGNAL(errorOccurred(QProcess::ProcessError)),this,SLOT(onErrorOccurred(QProcess::ProcessError)));
    connect(&sslServer,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(onStateChanged(QProcess::ProcessState)));

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
        if (!server.isEmpty())
            logger.appendWarning("SSL Server configured in path, but the file does not exist.");
    }

    // Creating the sslclient
    sslDataProcessingClient = new SSLDataProcessingClient(this,c);

    // Connection to the "finished" slot.
    //connect(sslDataProcessingClient,SIGNAL(transactionFinished(bool)),this,SLOT(onSLLTransactionFinished(bool)));
    connect(sslDataProcessingClient,SIGNAL(diconnectionFinished()),this,SLOT(onDisconnectionFinished()));

}

void FlowControl::startDemoTransaction(){
    demoTransaction = true;
    onNextFileSet(QStringList());
}

void FlowControl::onErrorOccurred(QProcess::ProcessError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ProcessError>();
    logger.appendError(QString("PROCESS ERROR: ") + metaEnum.valueToKey(error));
}

void FlowControl::onStateChanged(QProcess::ProcessState newState){
    QMetaEnum metaEnum = QMetaEnum::fromType<QProcess::ProcessState>();
    logger.appendStandard(QString("SSLServer Process state changed to: ") + metaEnum.valueToKey(newState));
}

void FlowControl::saveReport(){
    ImageReportDrawer reportDrawer;
    reportDrawer.drawReport(&reportData,configuration);
}

void FlowControl::saveReportAs(const QString &title){

    QString newFileName = QFileDialog::getSaveFileName(nullptr,title,"","*.png");
    if (newFileName.isEmpty()) return;
    if (!QFile::copy(configuration->getString(CONFIG_IMAGE_REPORT_PATH),newFileName)){
        logger.appendError("Could not save report from " + configuration->getString(CONFIG_IMAGE_REPORT_PATH) + " to " + newFileName);
    }
}

void FlowControl::resolutionCalculations(){
    QDesktopWidget *desktop = QApplication::desktop();
    // This line will assume that the current screen is the one where the experiments will be drawn.
    QRect screen = desktop->screenGeometry(desktop->screenNumber());
    configuration->addKeyValuePair(CONFIG_RESOLUTION_WIDTH,screen.width());
    configuration->addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,screen.height());
}

void FlowControl::requestReportData(){
    sslTransactionAllOk = false;
    emit(requestNextFileSet());
}

void FlowControl::onNextFileSet(const QStringList &fileSetAndName){

    //qWarning() << "NEXT FILE SET: " << fileSetAndName;

    if (fileSetAndName.isEmpty() && !demoTransaction){
        emit(sslTransactionFinished());
        return;
    }


    // The first value is the expected report name.
    QStringList fileSet = fileSetAndName;
    if (!demoTransaction){
        logger.appendStandard("Expected Report is: " + fileSetAndName.first());
        fileSet.removeFirst();
    }
    else{
        fileSet.clear();
        fileSet << "binding_bc_2018_06_03.dat" << "binding_uc_2018_06_03.dat" << "reading_2018_06_03.dat";
    }

    //qWarning() << "File set" << fileSet;

    // Generating the configuration file required to send to the server.
    QString expgenfile = configuration->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_EYE_REP_GEN_CONFIGURATION;
    // Making sure previous version of the file are not present so as to ensure that no garbage data is used in this current configuration.
    QFile(expgenfile).remove();

    QString error;
    QStringList toSave;

    toSave << CONFIG_DOCTOR_NAME << CONFIG_PATIENT_AGE << CONFIG_PATIENT_NAME << CONFIG_MOVING_WINDOW_DISP
           << CONFIG_MIN_FIXATION_LENGTH << CONFIG_SAMPLE_FREQUENCY << CONFIG_DISTANCE_2_MONITOR << CONFIG_XPX_2_MM << CONFIG_YPX_2_MM
           << CONFIG_LATENCY_ESCAPE_RAD << CONFIG_MARGIN_TARGET_HIT;

    for (qint32 i = 0; i < toSave.size(); i++){
        error = configuration->saveValueToFile(expgenfile,COMMON_TEXT_CODEC,toSave.at(i));
        if (!error.isEmpty()){
            logger.appendError("WRITING EYE REP GEN FILE: " + error);
            sslTransactionAllOk = false;
            emit(sslTransactionFinished());
            return;
        }
    }

    //qWarning() << "Saved expgen";

    qint32 validEye;
    if (!demoTransaction) validEye = DatFileInfoInDir::getValidEyeForDatList(fileSet);
    else validEye = EYE_BOTH;

    error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_VALID_EYE,QString::number(validEye));
    if (!error.isEmpty()){
        logger.appendError("WRITING EYE REP GEN FILE: " + error);
        sslTransactionAllOk = false;
        emit(sslTransactionFinished());
        return;
    }

    error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_DAT_TIME_FILTER_THRESHOLD,"0");
    if (!error.isEmpty()){
        logger.appendError("WRITING EYE REP GEN FILE: " + error);
        sslTransactionAllOk = false;
        emit(sslTransactionFinished());
        return;
    }

    for (qint32 i = 0; i < fileSet.size(); i++){
        if (fileSet.at(i).startsWith(FILE_OUTPUT_READING)){
            error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_FILE_READING,fileSet.at(i));
            if (!error.isEmpty()){
                logger.appendError("WRITING EYE REP GEN FILE: " + error);
                sslTransactionAllOk = false;
                emit(sslTransactionFinished());
                return;
            }
        }
        else if (fileSet.at(i).startsWith(FILE_OUTPUT_BINDING_BC)){
            error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_FILE_BIDING_BC,fileSet.at(i));
            if (!error.isEmpty()){
                logger.appendError("WRITING EYE REP GEN FILE: " + error);
                sslTransactionAllOk = false;
                emit(sslTransactionFinished());
                return;
            }
        }
        else if (fileSet.at(i).startsWith(FILE_OUTPUT_BINDING_UC)){
            error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_FILE_BIDING_UC,fileSet.at(i));
            if (!error.isEmpty()){
                logger.appendError("WRITING EYE REP GEN FILE: " + error);
                sslTransactionAllOk = false;
                emit(sslTransactionFinished());
                return;
            }
        }
    }

    //qWarning() << "Requesting report";
    sslDataProcessingClient->requestReport(!demoTransaction);

}

void FlowControl::onDisconnectionFinished(){
    sslTransactionAllOk = sslDataProcessingClient->getTransactionStatus();

    // If this is a demo transaction there is nothing else to do.
    if (demoTransaction){
        demoTransaction = false;
        emit(sslTransactionFinished());
        return;
    }

    if (!sslTransactionAllOk) emit(sslTransactionFinished());
    else emit(requestNextFileSet());
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

void FlowControl::connectToEyeTracker(){

    // Creating the connection with the EyeTracker
    if (eyeTracker != nullptr) delete eyeTracker;

    // The new et is NOT calibrated.
    calibrated = false;

    QString eyeTrackerSelected = configuration->getString(CONFIG_SELECTED_ET);

    if (eyeTrackerSelected == CONFIG_P_ET_MOUSE){
        eyeTracker = new MouseInterface();
    }
    else if (eyeTrackerSelected == CONFIG_P_ET_REDM){
        eyeTracker = new REDInterface();
    }
    else if (eyeTrackerSelected == CONFIG_P_ET_GP3HD){
        eyeTracker = new OpenGazeInterface(this,configuration->getReal(CONFIG_RESOLUTION_WIDTH),configuration->getReal(CONFIG_RESOLUTION_HEIGHT));
    }
    else{
        logger.appendError("Selected unknown EyeTracker: " + eyeTrackerSelected);
        return;
    }

    connect(eyeTracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
    eyeTracker->connectToEyeTracker();
}

void FlowControl::calibrateEyeTracker(){
    EyeTrackerCalibrationParameters calibrationParams;
    calibrationParams.forceCalibration = true;
    calibrationParams.name = "";
    calibrated = false;
    eyeTracker->calibrate(calibrationParams);
}

void FlowControl::onEyeTrackerControl(quint8 code){
    switch(code){
    case EyeTrackerInterface::ET_CODE_CALIBRATION_ABORTED:
        calibrated = false;
        emit(calibrationDone(false));
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_DONE:
        calibrated = true;
        emit(calibrationDone(true));
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_FAILED:
        calibrated = false;
        emit(calibrationDone(false));
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_FAIL:
        connected = false;
        emit(connectedToEyeTracker(false));
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_SUCCESS:
        connected = true;
        emit(connectedToEyeTracker(true));
        break;
    case EyeTrackerInterface::ET_CODE_DISCONNECTED_FROM_ET:
        logger.appendError("Disconnected from EyeTracker");
        connected = false;
        break;
    }
}

bool FlowControl::startNewExperiment(qint32 experimentID){

    QBrush background;
    experimentIsOk = true;

    // Making sure no old reports are stored.
    configuration->addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,"");

    //logger.appendStandard("Starting experiment with ID " + QString::number(experimentID));

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
        logger.appendError("Unknown experiment was selected " + QString::number(experimentID));
        return false;
    }

    // Hiding cursor UNLESS the ET is in mouse mode.
    if (configuration->getString(CONFIG_SELECTED_ET) != CONFIG_P_ET_MOUSE){
        experiment->hideCursor();
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

    // Making sure the right eye is used.
    eyeTracker->setEyeToTransmit(configuration->getInt(CONFIG_VALID_EYE));

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
        logger.appendStandard("Experiment aborted");
        experimentIsOk = false;
        break;
    case Experiment::ER_FAILURE:
        logger.appendError("EXPERIMENT FAILURE: " + experiment->getError());
        experimentIsOk = false;
        break;
    case Experiment::ER_NORMAL:
        break;
    case Experiment::ER_WARNING:
        logger.appendWarning("EXPERIMENT WARNING: " + experiment->getError());
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
    if (configuration->getInt(CONFIG_BINDING_NUMBER_OF_TARGETS) == 2){
        if (bc) return ":/experiment_data/bc.dat";
        else return ":/experiment_data/uc.dat";
    }
    else{
        if (bc) return ":/experiment_data/bc_3.dat";
        else return ":/experiment_data/uc_3.dat";
    }
}

void FlowControl::setupSecondMonitor(){

    QDesktopWidget *desktop = QApplication::desktop();
    if (desktop->screenCount() < 2) {
        // There is nothing to do as there is no second monitor.
        if (monitor == nullptr) return;
        // In case the monitor was disconnected.
        logger.appendWarning("Attempting to restore situation in which second monitor seems to have been disconnected after it was created");
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

