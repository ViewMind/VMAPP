#include "flowcontrol.h"

const char *FlowControl::STUDY_FILE_READING     =  "reading"    ;
const char *FlowControl::STUDY_FILE_BINDING     =  "binding"    ;
const char *FlowControl::STUDY_FILE_NBACKMS     =  "nbackms"    ;
const char *FlowControl::STUDY_FILE_NBACKRT     =  "nbackrt"    ;
const char *FlowControl::STUDY_FILE_PERCEPTION  =  "perception" ;
const char *FlowControl::STUDY_FILE_GONOGO      =  "gonogo"     ;


FlowControl::FlowControl(QWidget *parent, ConfigurationManager *c) : QWidget(parent)
{

    // Intializing the eyetracker pointer
    configuration = c;
    eyeTracker = nullptr;
    connected = false;
    calibrated = false;
    rightEyeCalibrated  = false;
    leftEyeCalibrated = false;
    experiment = nullptr;
    openvrco = nullptr;
    monitor = nullptr;
    renderState = RENDERING_NONE;
    this->setVisible(false);


    // Creating the OpenVR Object if so defined.
    if (Globals::EyeTracker::IS_VR){
        openvrco = new OpenVRControlObject(this);
        connect(openvrco,SIGNAL(requestUpdate()),this,SLOT(onRequestUpdate()));

        // Intializing the OpenVR Solution.
        if (!openvrco->startRendering()){
            logger.appendError("OpenVRControl Object intilization failed. Will not be creating the wait screen");
            return;
        }

        // If rendering was started, is now stopped so as not to consume resources.
        openvrco->stopRendering();

        // Create the wait screen only once
        waitScreenBaseColor = QColor(Qt::gray);

        logo = QImage(":/CommonClasses/PNGWriter/report_text/viewmind.png");
        if (logo.isNull()){
            logger.appendError("The loaded wait screen image is null");
            return;
        }

        waitFont = QFont("Mono",32);

        // We start with a blank wait screen.
        generateWaitScreen("");

    }

}

void FlowControl::generateWaitScreen(const QString &message){

    if (openvrco == nullptr) return;

    QFontMetrics fmetrics(waitFont);
    QRect btextrect = fmetrics.boundingRect(message);

    QSize s = openvrco->getRecommendedSize();
    openvrco->setScreenColor(waitScreenBaseColor);
    logo = logo.scaled(s.width(),s.height(),Qt::KeepAspectRatio);

    waitScreen = QImage(s.width(), s.height(), QImage::Format_RGB32);
    QPainter painter(&waitScreen);
    painter.fillRect(0,0,s.width(),s.height(),QBrush(waitScreenBaseColor));
    qreal xoffset = (s.width() - logo.width())/2;
    qreal yoffset = (s.height() - logo.height())/2;

    QRectF source(0,0,logo.width(),logo.height());
    QRectF target(xoffset,yoffset,logo.width(),logo.height());
    painter.drawImage(target,logo,source);

    // Drawing the text, below the image.
    painter.setPen(QColor(Qt::black));
    painter.setFont(waitFont);
    xoffset = (s.width() - btextrect.width())/2;
    yoffset = yoffset + logo.height() + btextrect.height();
    QRect targetTextRect(0,static_cast<qint32>(yoffset),s.width(),static_cast<qint32>(btextrect.height()*2.2));
    painter.drawText(targetTextRect,Qt::AlignCenter,message);

    painter.end();

}

////////////////////////////// AUXILIARY FUNCTIONS ///////////////////////////////////////

void FlowControl::resolutionCalculations(){
    QDesktopWidget *desktop = QApplication::desktop();
    // This line will assume that the current screen is the one where the experiments will be drawn.

    /// Depracated code.
    /// QRect screen = desktop->screenGeometry(desktop->screenNumber());
    QList<QScreen*> screens = QGuiApplication::screens();
    QRect screen = screens.at(desktop->screenNumber())->geometry();
    configuration->addKeyValuePair(Globals::Share::MONITOR_RESOLUTION_WIDTH,screen.width());
    configuration->addKeyValuePair(Globals::Share::MONITOR_RESOLUTION_HEIGHT,screen.height());

    if (openvrco != nullptr){
        // This menas the resolution for drawing should be the one recommende by OpenVR.
        QSize s = openvrco->getRecommendedSize();
        configuration->addKeyValuePair(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH, s.width());
        configuration->addKeyValuePair(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT,s.height());
    }
    else {
        configuration->addKeyValuePair(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH, screen.width());
        configuration->addKeyValuePair(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT,screen.height());
    }

}

void FlowControl::setupSecondMonitor(){

    /// Depracated code
    /// QDesktopWidget *desktop = QApplication::desktop();
    QList<QScreen*> screens = QGuiApplication::screens();

    if (screens.size() < 2) {
        // There is nothing to do as there is no second monitor.
        if (monitor == nullptr) return;
        // In case the monitor was disconnected.
        logger.appendWarning("Attempting to restore situation in which second monitor seems to have been disconnected after it was created");
        if (monitor != nullptr){
            if (experiment != nullptr){
                disconnect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
                disconnect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
                disconnect(experiment,&Experiment::addFixations,monitor,&MonitorScreen::addFixations);
                disconnect(experiment,&Experiment::addDebugMessage,monitor,&MonitorScreen::addMessages);
            }
            delete monitor;
            monitor = nullptr;
        }
        return;
    }

    // Nothing to do.
    if (monitor != nullptr) return;

    // Creating the second monitor.
    if (!configuration->getBool(Globals::VMPreferences::DUAL_MONITOR_MODE)) return;

    // Setting up the monitor. Assuming 1 is the SECONDARY MONITOR.
    QRect secondScreen = screens.at(1)->geometry();
    monitor = new MonitorScreen(Q_NULLPTR,secondScreen,configuration->getReal(Globals::Share::MONITOR_RESOLUTION_WIDTH),configuration->getReal(Globals::Share::MONITOR_RESOLUTION_HEIGHT));
}

void FlowControl::keyboardKeyPressed(int key){
    if (experiment != nullptr){
        experiment->keyboardKeyPressed(key);
    }
}

void FlowControl::stopRenderingVR(){
    //qDebug() << "On FlowControl::stopRenderingVR()";
    if (openvrco != nullptr){
        openvrco->stopRendering();
    }
    if (eyeTracker != nullptr){
        eyeTracker->enableUpdating(false); // This is specially important to stop the VIVE EYE Poller
    }
}


////////////////////////////// FLOW CONTROL FUNCTIONS ///////////////////////////////////////

void FlowControl::onRequestUpdate(){
    if (renderState == RENDERING_EXPERIMENT){
        if (eyeTracker == nullptr) {
            logger.appendWarning("VR Update Request: Rendering experiment with a non existant eyeTracker object");
            return;
        }
        if (experiment == nullptr) {
            logger.appendWarning("VR Update Request: Rendering experiment with a non existant experiment object");
            return;
        }
        EyeTrackerData data = eyeTracker->getLastData();
        QImage image = experiment->getVRDisplayImage();
        displayImage = image;
        QPainter painter(&displayImage);
        qreal r = 0.01*displayImage.width();
        painter.setBrush(QBrush(QColor(0,255,0,100)));
        painter.drawEllipse(static_cast<qint32>(data.xLeft-r),static_cast<qint32>(data.yLeft-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r));
        painter.setBrush(QBrush(QColor(0,0,255,100)));
        painter.drawEllipse(static_cast<qint32>(data.xRight-r),static_cast<qint32>(data.yRight-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r));
        openvrco->setImage(&image);
        emit(newImageAvailable());
    }
    if (renderState == RENDER_WAIT_SCREEN){
        if (displayImage != waitScreen){
            displayImage = waitScreen;
            openvrco->setScreenColor(waitScreenBaseColor);
            openvrco->setImage(&displayImage);
            emit(newImageAvailable());
        }
    }
}

QImage FlowControl::image() const{
    return displayImage;
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

    logger.appendStandard("Creating new EyeTracker Object");

    // The new et is NOT calibrated.
    calibrated = false;

    //qWarning() << "CONNECTING TO ET. Selected" << eyeTrackerSelected;

    if (configuration->getBool(Globals::VMPreferences::USE_MOUSE)){
        eyeTracker = new MouseInterface();
        logger.appendStandard("Connecting to the Mouse ... ");
    }
    else{
#ifdef EYETRACKER_HTCVIVEPRO
        eyeTracker = new HTCViveEyeProEyeTrackingInterface(this,configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH),configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH));
#endif
#ifdef EYETRACKER_GAZEPOINT
        eyeTracker = new OpenGazeInterface(this,configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH),configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH));
#endif
        logger.appendStandard("Connecting to the EyeTracker ... ");
    }


    connect(eyeTracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
    if (openvrco != nullptr)
        connect(openvrco,SIGNAL(newProjectionMatrixes(QMatrix4x4,QMatrix4x4)),eyeTracker,SLOT(updateProjectionMatrices(QMatrix4x4,QMatrix4x4)));
    eyeTracker->connectToEyeTracker();
}

void FlowControl::calibrateEyeTracker(quint8 eye_to_use){

    //qDebug() << "In FlowControl::calibrateEyeTracker";

    EyeTrackerCalibrationParameters calibrationParams;
    calibrationParams.forceCalibration = true;
    calibrationParams.name = "";
    calibrated = false;
    rightEyeCalibrated  = false;
    leftEyeCalibrated = false;

    // Making sure the right eye is used, in both the calibration and the experiment.
    selected_eye_to_use = RDC::Eye::fromInt(eye_to_use);
    eyeTracker->setEyeToTransmit(selected_eye_to_use);

    // Required during calibration.
    renderState = RENDERING_NONE;

    logger.appendStandard("Calibrating the eyetracker....");
    if (openvrco != nullptr) {
        if (!openvrco->startRendering()){
            qDebug() << "Failed to start rendering upon calibration";
        }
        else{
            openvrco->setScreenColor(QColor(Qt::gray));
        }
    }

    //qDebug() << "In FlowwControl::calibrateEyeTracker, is VR ENABLED?"  <<configuration->getBool(Globals::Share::VR_ENABLED);

    if (Globals::EyeTracker::IS_VR) eyeTracker->enableUpdating(true); // To ensure that eyetracking data is gathered.
    eyeTracker->calibrate(calibrationParams);
}

bool FlowControl::isCalibrated() const{
    return calibrated;
}

bool FlowControl::isRightEyeCalibrated() const{
    return rightEyeCalibrated;
}

bool FlowControl::isLeftEyeCalibrated() const{
    return leftEyeCalibrated;
}

void FlowControl::onEyeTrackerControl(quint8 code){
    //qDebug() << "ON FlowControl::EYETRACKER CONTROL" << code;

    if (eyeTracker == nullptr){
        logger.appendError("Possible crash alert: Entering onEyeTrackerControl with a null pointer eyeTracker object");
    }

    switch(code){
    case EyeTrackerInterface::ET_CODE_CALIBRATION_ABORTED:
        logger.appendWarning("EyeTracker Control: Calibration aborted");
        calibrated = false;
        if (Globals::EyeTracker::IS_VR) eyeTracker->enableUpdating(false);
        renderState = RENDER_WAIT_SCREEN;
        emit(calibrationDone(false));
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_DONE:
        logger.appendStandard("EyeTracker Control: Calibration done successfully");
        calibrated = true;
        if (Globals::EyeTracker::IS_VR) eyeTracker->enableUpdating(false);
        renderState = RENDER_WAIT_SCREEN;
        switch (eyeTracker->getCalibrationFailureType()){
        case EyeTrackerInterface::ETCFT_NONE:
            calibrated = true;
            rightEyeCalibrated  = true;
            leftEyeCalibrated = true;
            break;
        case EyeTrackerInterface::ETCFT_UNKNOWN:
            calibrated = false;
            rightEyeCalibrated  = false;
            leftEyeCalibrated = false;
            break;
        case EyeTrackerInterface::ETCFT_FAILED_BOTH:
            calibrated = false;
            rightEyeCalibrated  = false;
            leftEyeCalibrated = false;
            break;
        case EyeTrackerInterface::ETCFT_FAILED_LEFT:
            rightEyeCalibrated  = true;
            leftEyeCalibrated = false;
            if (selected_eye_to_use == RDC::Eye::LEFT){
                // The eye that failed was the one that was configured.
                calibrated = false;
            }
            else calibrated = true;
            break;
        case EyeTrackerInterface::ETCFT_FAILED_RIGHT:
            rightEyeCalibrated  = false;
            leftEyeCalibrated = true;
            if (selected_eye_to_use == RDC::Eye::RIGHT){
                // The eye that failed was the one that was configured.
                calibrated = false;
            }
            else calibrated = true;
            break;
        }
        emit(calibrationDone(calibrated));
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_FAIL:
        logger.appendError("EyeTracker Control: Connection to EyeTracker Failed");
        connected = false;
        emit(connectedToEyeTracker(false));
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_SUCCESS:
        logger.appendStandard("EyeTracker Control: Connection to EyeTracker Established");
        connected = true;
        emit(connectedToEyeTracker(true));
        break;
    case EyeTrackerInterface::ET_CODE_DISCONNECTED_FROM_ET:
        logger.appendError("EyeTracker Control: Disconnected from EyeTracker");
        connected = false;
        break;
    case EyeTrackerInterface::ET_CODE_NEW_CALIBRATION_IMAGE_AVAILABLE:
        displayImage = eyeTracker->getCalibrationImage();
        openvrco->setImage(&displayImage);
        emit(newImageAvailable());
        break;
    }
}

bool FlowControl::startNewExperiment(QVariantMap study_config){

    // So to start a new experiment we need:
    // 1) Know the patient directory.
    // 2) Know the study configuration
    // 3) If Binding we need to figure out if new file or load an existing one.
    // 4) If Perception same thing.

    // configuration->getString(CONFIG_PATIENT_DIRECTORY)

    QBrush background;
    experimentIsOk = true;

    // Using the polimorphism, the experiment object is created according to the selected index.

    switch (study_config.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toInt()){
    case Globals::StudyConfiguration::INDEX_READING:
        logger.appendStandard("STARTING READING EXPERIMENT");
        experiment = new ReadingExperiment();
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray).darker(110));
        break;

    case Globals::StudyConfiguration::INDEX_BINDING_BC:
        logger.appendStandard("STARTING BINDING BC EXPERÏMENT");
        experiment = new ImageExperiment();
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray));
        break;
    case Globals::StudyConfiguration::INDEX_BINDING_UC:
        logger.appendStandard("STARTING BINDING UC EXPERÏMENT");
        experiment = new ImageExperiment(false);
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray));
        break;
    case Globals::StudyConfiguration::INDEX_NBACKMS:
        logger.appendStandard("STARTING N BACK TRACE FOR MS");
        experiment = new FieldingExperiment();
        background = QBrush(Qt::black);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::black));
        break;
    case Globals::StudyConfiguration::INDEX_NBACKRT:
        logger.appendStandard("STARTING NBACK TRACE FOR RESPONSE TIME");
        experiment = new NBackRTExperiment(RDC::Study::NBACKRT);
        background = QBrush(Qt::black);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::black));
        break;
        //    case EXP_PARKINSON:
        //        logger.appendStandard("STARTING PARKINSON MAZE");
        //        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,":/experiment_data/parkinson.dat");
        //        experiment = new ParkinsonExperiment();
        //        background = QBrush(Qt::black);
        //        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::black));
        //        break;
    case Globals::StudyConfiguration::INDEX_GONOGO:
        logger.appendStandard("STARTING GO - NO GO");
        experiment = new GoNoGoExperiment();
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray).darker(110));
        break;
    case Globals::StudyConfiguration::INDEX_NBACKVS:
        logger.appendStandard("STARTING N BACK VS");
        experiment = new NBackRTExperiment(RDC::Study::NBACKVS);
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray).darker(110));
        break;
    case Globals::StudyConfiguration::INDEX_PERCEPTION:
        logger.appendStandard("STARTING PERCEPTION");
        experiment = new PerceptionExperiment(nullptr);
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray).darker(110));
        break;

    default:
        logger.appendError("Unknown experiment was selected " + study_config.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toString());
        return false;
    }


    // Hiding cursor UNLESS the ET is in mouse mode.
    if (!configuration->getBool(Globals::VMPreferences::USE_MOUSE)){
        experiment->hideCursor();
    }

    // Making sure that that both experiment signals are connected.
    // Eyetracker should be connected by this point.
    connect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    connect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
    connect(experiment,&Experiment::updateVRDisplay,this,&FlowControl::onRequestUpdate);

    if ( (monitor != nullptr) && (!Globals::EyeTracker::IS_VR) ){
        connect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
        connect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
        connect(experiment,&Experiment::addFixations,monitor,&MonitorScreen::addFixations);
        connect(experiment,&Experiment::addDebugMessage,monitor,&MonitorScreen::addMessages);
        monitor->setBackgroundBrush(background);
    }

    // Setting the debug mode according to the current configuration.
    experiment->setDebugMode(configuration->getBool(Globals::VMPreferences::DUAL_MONITOR_MODE));

    // Making sure that the eyetracker is sending data.
    eyeTracker->enableUpdating(true);

    // Creating the processing parameters structure, but we must check that all values are here. Its possible for the processing parameters to be missing.
    QVariantMap pp;
    if (!checkAllProcessingParameters()) return false;

    pp.insert(RDC::ProcessingParameter::MAX_DISPERSION_WINDOW,configuration->getInt(Globals::Share::MAX_DISPERSION_SIZE));
    pp.insert(RDC::ProcessingParameter::MIN_FIXATION_DURATION,configuration->getInt(Globals::Share::MINIMUM_FIX_DURATION));
    pp.insert(RDC::ProcessingParameter::SAMPLE_FREQUENCY,configuration->getInt(Globals::Share::SAMPLE_FREQUENCY));
    pp.insert(RDC::ProcessingParameter::RESOLUTION_HEIGHT,configuration->getInt(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT));
    pp.insert(RDC::ProcessingParameter::RESOLUTION_WIDTH,configuration->getInt(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH));

    // Start the experiment.
    experiment->startExperiment(configuration->getString(Globals::Share::PATIENT_DIRECTORY),
                                configuration->getString(Globals::Share::PATIENT_STUDY_FILE),
                                study_config,
                                configuration->getBool(Globals::VMPreferences::USE_MOUSE),
                                pp);

    if (monitor != nullptr){
        if (configuration->getBool(Globals::VMPreferences::DUAL_MONITOR_MODE)) {
            monitor->show();
        }
        else monitor->hide();
    }

    if (openvrco != nullptr){
        if (!configuration->getBool(Globals::VMPreferences::USE_MOUSE)){
            if (!openvrco->startRendering()){
                logger.appendError("Could not start rendering upon starting experiment");
                return false;
            }
            renderState = RENDERING_EXPERIMENT;
        }
    }

    return true;
}

void FlowControl::on_experimentFinished(const Experiment::ExperimentResult &er){

    // Make the experiment windown invisible.
    experiment->hide();

    // Hide monitor if showing
    if (monitor != nullptr) monitor->hide();
    QFileInfo info(experiment->getDataFileLocation());

    switch (er){
    case Experiment::ER_ABORTED:
        logger.appendStandard("EXPERIMENT aborted");
        experimentIsOk = false;
        break;
    case Experiment::ER_FAILURE:
        logger.appendError("EXPERIMENT FAILURE: " + experiment->getError());
        experimentIsOk = false;
        break;
    case Experiment::ER_NORMAL:
        logger.appendSuccess("EXPERIMENT Finshed Successfully");
        break;
    case Experiment::ER_WARNING:
        logger.appendWarning("EXPERIMENT WARNING: " + experiment->getError());
        break;
    }

    // Destroying the experiment and reactivating the start experiment.
    disconnect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    disconnect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
    disconnect(experiment,&Experiment::updateVRDisplay,this,&FlowControl::onRequestUpdate);

    if (monitor != nullptr){
        disconnect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
        disconnect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
        disconnect(experiment,&Experiment::addFixations,monitor,&MonitorScreen::addFixations);
        disconnect(experiment,&Experiment::addDebugMessage,monitor,&MonitorScreen::addMessages);
    }
    delete experiment;
    experiment = nullptr;

    // This should make the user of the HMD see the wait screen until the next experiment.
    renderState = RENDER_WAIT_SCREEN;

    // Notifying the QML.
    emit(experimentHasFinished());

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool FlowControl::checkAllProcessingParameters(){
    QStringList checkForAllprocessingParameters;
    checkForAllprocessingParameters << Globals::Share::MAX_DISPERSION_SIZE << Globals::Share::MINIMUM_FIX_DURATION << Globals::Share::SAMPLE_FREQUENCY << Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH
                                    << Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH;

    for (qint32 i = 0; i < checkForAllprocessingParameters.size(); i++){
        if (!configuration->containsKeyword(checkForAllprocessingParameters.at(i))){
            logger.appendError("Missing processing " + checkForAllprocessingParameters.at(i) + ". Unable to start study");
            return false;
        }
    }
    return true;
}

///////////////////////////////////////////////////////////////////
FlowControl::~FlowControl(){

}
