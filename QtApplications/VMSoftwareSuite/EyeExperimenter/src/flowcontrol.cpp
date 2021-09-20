#include "flowcontrol.h"

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
    vrOK = false;

    // Creating the OpenVR Object if so defined.
    if (Globals::EyeTracker::IS_VR){
        openvrco = new OpenVRControlObject(this);
        connect(openvrco,SIGNAL(requestUpdate()),this,SLOT(onRequestUpdate()));

        // Intializing the OpenVR Solution.
        if (!openvrco->startRendering()){
            logger.appendError("OpenVRControl Object intilization failed. Will not be creating the wait screen");
            return;
        }

        vrOK = true;

        // If rendering was started, is now stopped so as not to consume resources.
        openvrco->stopRendering();

        // Create the wait screen only once
        waitScreenBaseColor = QColor(Qt::gray);

        logo = QImage(":/images/viewmind_wait_logo.png");
        if (logo.isNull()){
            logger.appendError("The loaded wait screen image is null");
            return;
        }

        waitFont = QFont("Mono",static_cast<qint32>(32*Globals::EyeTracker::VRSCALING));

        // We start with a blank wait screen.
        generateWaitScreen("");

    }
    else vrOK = true;

}

bool FlowControl::isVROk() const{
    return vrOK;
}

void FlowControl::generateWaitScreen(const QString &message){

    if (openvrco == nullptr) return;

    QFontMetrics fmetrics(waitFont);
    QRect btextrect = fmetrics.boundingRect(message);

    QSize s = openvrco->getRecommendedSize();
    openvrco->setScreenColor(waitScreenBaseColor);

    qint32 swidth  = static_cast<qint32>(s.width()*Globals::EyeTracker::VRSCALING);
    qint32 sheight = static_cast<qint32>(s.height()*Globals::EyeTracker::VRSCALING);

    logo = logo.scaled(swidth,sheight,Qt::KeepAspectRatio);

    // Drawing the logo in the center of the screen.
    waitScreen = QImage(swidth, sheight, QImage::Format_RGB32);
    QPainter painter(&waitScreen);
    painter.fillRect(0,0,swidth,sheight,QBrush(waitScreenBaseColor));
    qreal xoffset = (swidth - logo.width())/2;
    qreal yoffset = (sheight - logo.height())/2;

    QRectF source(0,0,logo.width(),logo.height());
    QRectF target(xoffset,yoffset,logo.width(),logo.height());
    painter.drawImage(target,logo,source);

    // Drawing the text, below the image.
    painter.setPen(QColor(Qt::black));
    painter.setFont(waitFont);
    xoffset = (swidth - btextrect.width())/2;
    yoffset = yoffset + logo.height() + btextrect.height();
    QRect targetTextRect(0,static_cast<qint32>(yoffset),swidth,static_cast<qint32>(btextrect.height()*2.2));
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
        configuration->addKeyValuePair(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH, s.width()*Globals::EyeTracker::VRSCALING);
        configuration->addKeyValuePair(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT,s.height()*Globals::EyeTracker::VRSCALING);
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
        eyeTracker->enableUpdating(false);
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
        QImage image = experiment->getVRDisplayImage();

        // On experiment images, the eye position is added.
        EyeTrackerData data = eyeTracker->getLastData();
        displayImage = image;
        QPainter painter(&displayImage);
        qreal r = 0.01*displayImage.width();
        painter.setBrush(QBrush(QColor(0,255,0,100)));
        painter.drawEllipse(static_cast<qint32>(data.xLeft-r),static_cast<qint32>(data.yLeft-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r));
        painter.setBrush(QBrush(QColor(0,0,255,100)));
        painter.drawEllipse(static_cast<qint32>(data.xRight-r),static_cast<qint32>(data.yRight-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r));

        if (Globals::Debug::SHOW_EYE_POSITION){
            openvrco->setImage(&displayImage);
        }
        else{
            openvrco->setImage(&image);
        }

        emit(newImageAvailable());
    }
    else if (renderState == RENDER_WAIT_SCREEN){
        if (displayImage != waitScreen){
            displayImage = waitScreen;
            openvrco->setScreenColor(waitScreenBaseColor);
            openvrco->setImage(&displayImage);
            emit(newImageAvailable());
        }
    }
    else if (renderState == RENDERING_CALIBRATION_SCREEN){
        openvrco->setImage(&displayImage);
        emit(newImageAvailable());
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


    if (configuration->getBool(Globals::VMPreferences::USE_MOUSE)){
        eyeTracker = new MouseInterface();
        logger.appendStandard("Connecting to the Mouse ... ");
    }
    else{

#ifdef EYETRACKER_HPOMNICENT
        eyeTracker = new HPOmniceptInterface(this,configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH),
                                             configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT));
#endif
#ifdef EYETRACKER_HTCVIVEPRO
        eyeTracker = new HTCViveEyeProEyeTrackingInterface(this,
                                                           configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH),
                                                           configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT));
#endif
#ifdef EYETRACKER_GAZEPOINT
        eyeTracker = new OpenGazeInterface(this,configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH),configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT));
#endif

        logger.appendStandard("Connecting to the EyeTracker ... ");
    }

    connect(eyeTracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));

    if (openvrco != nullptr){
        connect(openvrco,SIGNAL(newProjectionMatrixes(QMatrix4x4,QMatrix4x4)),eyeTracker,SLOT(updateProjectionMatrices(QMatrix4x4,QMatrix4x4)));
    }

    eyeTracker->connectToEyeTracker();
}

void FlowControl::calibrateEyeTracker(quint8 eye_to_use){

    EyeTrackerCalibrationParameters calibrationParams;
    calibrationParams.forceCalibration = true;
    calibrationParams.name = "";
    calibrationParams.number_of_calibration_points = 9;
    calibrated = false;
    rightEyeCalibrated  = false;
    leftEyeCalibrated = false;

    // Making sure the right eye is used, in both the calibration and the experiment.
    selected_eye_to_use = VMDC::Eye::fromInt(eye_to_use);
    eyeTracker->setEyeToTransmit(selected_eye_to_use);

   // qDebug() << "Selected eye to use number" << eye_to_use << "translated to" << selected_eye_to_use;

    // Required during calibration.
    renderState = RENDERING_CALIBRATION_SCREEN;

    logger.appendStandard("Calibrating the eyetracker....");
    if (openvrco != nullptr) {
        if (!openvrco->startRendering()){
            logger.appendError("Failed to start rendering upon calibration");
        }
        else{
            openvrco->setScreenColor(QColor(Qt::gray));
        }
    }

    //qDebug() << "In FlowwControl::calibrateEyeTracker, is VR ENABLED?"  <<configuration->getBool(Globals::Share::VR_ENABLED);

    if (Globals::EyeTracker::IS_VR) {
        eyeTracker->enableUpdating(true); // To ensure that eyetracking data is gathered.
    }
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
            if (selected_eye_to_use == VMDC::Eye::LEFT){
                // The eye that failed was the one that was configured.
                calibrated = false;
            }
            else calibrated = true;
            break;
        case EyeTrackerInterface::ETCFT_FAILED_RIGHT:
            rightEyeCalibrated  = false;
            leftEyeCalibrated = true;
            if (selected_eye_to_use == VMDC::Eye::RIGHT){
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
        //qDebug() << "Got a new calibration image";
        displayImage = eyeTracker->getCalibrationImage();
        //openvrco->setImage(&displayImage);
        //emit(newImageAvailable());
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
    QString finalStudyName;

    // Using the polimorphism, the experiment object is created according to the selected index.

    switch (study_config.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toInt()){
    case Globals::StudyConfiguration::INDEX_READING:
        logger.appendStandard("STARTING READING EXPERIMENT");
        experiment = new ReadingExperiment(nullptr,VMDC::Study::READING);
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray).darker(110));
        break;

    case Globals::StudyConfiguration::INDEX_BINDING_BC:
        logger.appendStandard("STARTING BINDING BC EXPERÏMENT");
        experiment = new ImageExperiment(nullptr,VMDC::Study::BINDING_BC);
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray));
        break;
    case Globals::StudyConfiguration::INDEX_BINDING_UC:
        logger.appendStandard("STARTING BINDING UC EXPERÏMENT");
        experiment = new ImageExperiment(nullptr,VMDC::Study::BINDING_UC);
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray));
        break;
    case Globals::StudyConfiguration::INDEX_NBACKMS:
        logger.appendStandard("STARTING N BACK TRACE FOR MS");
        experiment = new FieldingExperiment(nullptr,VMDC::Study::NBACKMS);
        background = QBrush(Qt::black);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::black));
        break;
    case Globals::StudyConfiguration::INDEX_NBACKRT:
        logger.appendStandard("STARTING NBACK TRACE FOR RESPONSE TIME");
        experiment = new NBackRTExperiment(nullptr,VMDC::Study::NBACKRT);
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
        experiment = new GoNoGoExperiment(nullptr,VMDC::Study::GONOGO);
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray));
        break;
    case Globals::StudyConfiguration::INDEX_NBACKVS:
        logger.appendStandard("STARTING N BACK VS");
        experiment = new NBackRTExperiment(nullptr,VMDC::Study::NBACKVS);
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray).darker(110));
        break;
    case Globals::StudyConfiguration::INDEX_PERCEPTION:
        logger.appendStandard("STARTING PERCEPTION");
        finalStudyName = VMDC::MultiPartStudyBaseName::PERCEPTION + " " + study_config.value(VMDC::StudyParameter::PERCEPTION_PART).toString();
        experiment = new PerceptionExperiment(nullptr,finalStudyName);
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
    //qDebug() << "Connecting for experiment finished";
    connect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    //qDebug() << "Connecting for new data available" << (eyeTracker == nullptr) << (experiment == nullptr);
    connect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
    //qDebug() << "Connecting for update VR Display";
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
    //qDebug() << "EyeTracker Enable Updating";
    eyeTracker->enableUpdating(true);

    // Start the experiment.
    experiment->startExperiment(configuration->getString(Globals::Share::PATIENT_DIRECTORY),
                                configuration->getString(Globals::Share::PATIENT_STUDY_FILE),
                                study_config,
                                configuration->getBool(Globals::VMPreferences::USE_MOUSE));


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

///////////////////////////////////////////////////////////////////
FlowControl::~FlowControl(){

}
