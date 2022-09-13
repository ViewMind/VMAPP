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
    renderState = RENDERING_NONE;
    this->setVisible(false);
    vrOK = false;

    // Creating the OpenVR Object if so defined.
    if ((Globals::EyeTracker::IS_VR) && (!DBUGBOOL(Debug::Options::USE_MOUSE))){
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

        logo = QImage(":/images/logo.png");
        if (logo.isNull()){
            logger.appendError("The loaded wait screen image is null");
            return;
        }

        waitFont = QFont("Mono",static_cast<qint32>(32*Globals::EyeTracker::VRSCALING));

        // We start with a blank wait screen.
        generateWaitScreen("");

    }
    else {
        if (DBUGBOOL(Debug::Options::USE_MOUSE)){
            qDebug() << "DBUG: Mouse Use Enabled";
            logger.appendWarning("DBUG: Using Mouse as EyeTracker");
        }
        vrOK = true;
    }

    if (DBUGINT(Debug::Options::OVERRIDE_TIME) != 0){
        QString msg = "DBUG: Override Time has been set to " + QString::number(DBUGINT(Debug::Options::OVERRIDE_TIME));
        qDebug() << msg;
        logger.appendWarning(msg);
    }

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

    QRect screen = QGuiApplication::primaryScreen()->geometry();
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
            logger.appendError("VR Update Request: Rendering experiment with a non existant eyeTracker object. Force stopping rendering");
            openvrco->stopRendering();
            return;
        }
        if (experiment == nullptr) {
            logger.appendError("VR Update Request: Rendering experiment with a non existant experiment object. Force stopping rendering");
            openvrco->stopRendering();
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

        if (!DBUGBOOL(Debug::Options::SHOW_EYES_IN_STUDY)){
            openvrco->setImage(&image);
        }
        else{
            openvrco->setImage(&displayImage);

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

        if (!DBUGBOOL(Debug::Options::USE_MOUSE)){

            if (Globals::EyeTracker::PROCESSING_PARAMETER_KEY == Globals::HPReverb::PROCESSING_PARAMETER_KEY){

                eyeTracker = new HPOmniceptInterface(this,configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH),
                                                     configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT));
            }
            else if (Globals::EyeTracker::PROCESSING_PARAMETER_KEY == Globals::HTC::PROCESSING_PARAMETER_KEY){
                eyeTracker = new HTCViveEyeProEyeTrackingInterface(this,
                                                                   configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH),
                                                                   configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT));
            }
            else if (Globals::EyeTracker::PROCESSING_PARAMETER_KEY == Globals::GP3HD::PROCESSING_PARAMETER_KEY){
                eyeTracker = new OpenGazeInterface(this,configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH),
                                                   configuration->getReal(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT));
            }
            else{
                logger.appendError("Failed connecting to ET. Unknown key: " + Globals::EyeTracker::PROCESSING_PARAMETER_KEY);
            }

            logger.appendStandard("Connecting to the EyeTracker " + Globals::EyeTracker::PROCESSING_PARAMETER_KEY);
        }
        else{
            logger.appendWarning("WARNING: Using Mouse EyeTracker DBug Option");
            eyeTracker = new MouseInterface();
            static_cast<MouseInterface*>(eyeTracker)->overrideCalibration();
        }

    }

    connect(eyeTracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));

    if (openvrco != nullptr){
        connect(openvrco,SIGNAL(newProjectionMatrixes(QMatrix4x4,QMatrix4x4)),eyeTracker,SLOT(updateProjectionMatrices(QMatrix4x4,QMatrix4x4)));
    }

    eyeTracker->connectToEyeTracker();
}

void FlowControl::calibrateEyeTracker(bool useSlowCalibration){

    EyeTrackerCalibrationParameters calibrationParams;
    calibrationParams.forceCalibration = true;
    calibrationParams.name = "";

    int required_number_of_accepted_pts = 7;
    if (DBUGEXIST(Debug::Options::FORCE_N_CALIB_PTS)){
        calibrationParams.number_of_calibration_points = DBUGINT(Debug::Options::FORCE_N_CALIB_PTS);
        required_number_of_accepted_pts = 2;
    }
    else{
        calibrationParams.number_of_calibration_points = 9;
    }

    calibrated = false;
    rightEyeCalibrated  = false;
    leftEyeCalibrated = false;

    // Making sure the right eye is used, in both the calibration and the experiment.
    eyeTracker->setEyeToTransmit(VMDC::Eye::BOTH);

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

    if (DBUGSTR(Debug::Options::LOAD_CALIBRATION_K) != ""){
        // Check if the file exists

        QString coefficient_file = DBUGSTR(Debug::Options::LOAD_CALIBRATION_K);
        calibrationParams.name = coefficient_file;

        if (QFile(coefficient_file).exists()){
            calibrationParams.forceCalibration = false;
            QString dbug = "DBUG: Loading calibration coefficients " + coefficient_file;
            qDebug() << dbug;
            logger.appendWarning(dbug);
        }
        else {
            QString dbug = "DBUG: Calibration coefficients do no exist. File "  + calibrationParams.name + " will be created after next calibration";
            qDebug() << dbug;
            logger.appendWarning(dbug);
        }
    }

    qint32 validation_point_acceptance_threshold = 70;
    if (useSlowCalibration){
        logger.appendStandard("Requested use of slow calibration");
        validation_point_acceptance_threshold = 60;
        calibrationParams.gather_time = CALIB_PT_GATHER_TIME_SLOW;
        calibrationParams.wait_time = CALIB_PT_WAIT_TIME_SLOW;
    }
    else {
        validation_point_acceptance_threshold = 60;
        calibrationParams.gather_time = CALIB_PT_GATHER_TIME_NORMAL;
        calibrationParams.wait_time = CALIB_PT_WAIT_TIME_NORMAL;
    }

    // Testing validations parameters.
    QVariantMap calibrationValidationParameters;
    calibrationValidationParameters[VMDC::CalibrationFields::REQ_NUMBER_OF_ACCEPTED_POINTS] = required_number_of_accepted_pts;
    calibrationValidationParameters[VMDC::CalibrationFields::VALIDATION_POINT_ACCEPTANCE_THRESHOLD] = 70;
    calibrationValidationParameters[VMDC::CalibrationFields::VALIDATION_POINT_HIT_TOLERANCE] = 0;
    calibrationValidationParameters[VMDC::CalibrationFields::CALIBRATION_POINT_GATHERTIME] = calibrationParams.gather_time;
    calibrationValidationParameters[VMDC::CalibrationFields::CALIBRATION_POINT_WAITTIME] = calibrationParams.wait_time;

    if (DBUGEXIST(Debug::Options::CONFIG_CALIB_VALID)){
        QVariantMap configCalibValidDebugOptons = Debug::parseMultipleDebugOptionLine(DBUGSTR(Debug::Options::CONFIG_CALIB_VALID));

        qDebug() << "DBUG: Configuration For Calibration Options Set Externally. Final set of options";

        QStringList allkeys = configCalibValidDebugOptons.keys();
        for (qint32 i = 0; i < allkeys.size(); i++){
            QString key = allkeys.at(i);
            qDebug() << "   Setting '" << key << "' -> " << configCalibValidDebugOptons.value(key);
            calibrationValidationParameters[key] = configCalibValidDebugOptons[key];
        }

        //Debug::prettpPrintQVariantMap(calibrationValidationParameters);
    }

    eyeTracker->configureCalibrationValidation(calibrationValidationParameters);
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
        logger.appendStandard("EyeTracker Control: Calibration and validation finished");

        if (DBUGBOOL(Debug::Options::DBUG_MSG)){
            logger.appendStandard("VALIDATION REPORT:\n" + eyeTracker->getCalibrationValidationReport());
        }

        calibrated = true;
        if (Globals::EyeTracker::IS_VR) eyeTracker->enableUpdating(false);
        renderState = RENDER_WAIT_SCREEN;

        // At this point the calibration validation data structure should be available.
        // The structure should be used in order to determine which eye will be the eye used in the study.

        // We can now get the selected eye to use.
        selected_eye_to_use = eyeTracker->getCalibrationRecommendedEye();
        eyeTracker->setEyeToTransmit(selected_eye_to_use);
        logger.appendStandard("Calibration Validation Report After Calibration Done");
        logger.appendStandard(eyeTracker->getCalibrationValidationReport());
        logger.appendStandard("Selected Eye: " + selected_eye_to_use);

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
        break;
    }
}


QVariantMap FlowControl::getCalibrationValidationData() const {

    if (DBUGSTR(Debug::Options::LOAD_FOR_CALIB_VERIF) != ""){

        // Debug Code to Test Validation Graphics.
        QString file_to_load = DBUGSTR(Debug::Options::LOAD_FOR_CALIB_VERIF);
        QFile file(file_to_load);
        file.open(QFile::ReadOnly);
        QString content = file.readAll();
        file.close();

        QJsonParseError json_error;
        QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8(),&json_error);
        QVariantMap data = doc.object().toVariantMap();
        QVariantMap pp = data.value("processing_parameters").toMap();
        data = data.value("calibration_validation").toMap();
        data["W"] = pp.value("resolution_width");
        data["H"] = pp.value("resolution_height");

        return data;
    }

    if (eyeTracker != nullptr){

        QVariantMap map = eyeTracker->getCalibrationValidationData();

        // We add the resolution as it is required for plotting the calibration data.
        map["W"] = configuration->getInt(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH);
        map["H"] = configuration->getInt(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT);

        return map;

    }
    return QVariantMap();

}

bool FlowControl::autoValidateCalibration() const {
    if (DBUGSTR(Debug::Options::LOAD_FOR_CALIB_VERIF) != "") return false; // In this case we need the dialog since that the entire point of the debug option.
    return (DBUGBOOL(Debug::Options::USE_MOUSE) || (DBUGSTR(Debug::Options::LOAD_CALIBRATION_K) != ""));
}

void FlowControl::onUpdatedExperimentMessages(const QVariantMap &string_value_map){
    // This is simply a pass through from the experiment object to the QML front end
    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
        QStringList keys = string_value_map.keys();
        for (qint32 i = 0; i < keys.size(); i++){
            qDebug() << "DBUG: Update Message Key " << keys.at(i) << " => " << string_value_map.value(keys.at(i));
        }
    }
    emit FlowControl::newExperimentMessages(string_value_map);
}

bool FlowControl::startNewExperiment(QVariantMap study_config){

    // So to start a new experiment we need:
    // 1) Know the patient directory.
    // 2) Know the study configuration
    // 3) If Binding we need to figure out if new file or load an existing one.
    // 4) If Perception same thing.

    // configuration->getString(CONFIG_PATIENT_DIRECTORY)

    QBrush background;
    QColor backgroundForVRScreen;
    experimentIsOk = true;
    QString finalStudyName;

    // Forcing the valid eye to the calibration selected eye.
    study_config[VMDC::StudyParameter::VALID_EYE] = selected_eye_to_use;
    //Debug::prettpPrintQVariantMap(study_config);

    // Using the polimorphism, the experiment object is created according to the selected index.
    switch (study_config.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toInt()){

    case Globals::StudyConfiguration::INDEX_BINDING_BC:
        logger.appendStandard("STARTING BINDING BC EXPERIMENT");
        experiment = new BindingExperiment(nullptr,VMDC::Study::BINDING_BC);

        if (openvrco != nullptr) backgroundForVRScreen = QColor(Qt::gray);
        break;
    case Globals::StudyConfiguration::INDEX_BINDING_UC:
        logger.appendStandard("STARTING BINDING UC EXPERIMENT");
        experiment = new BindingExperiment(nullptr,VMDC::Study::BINDING_UC);

        if (openvrco != nullptr) backgroundForVRScreen = QColor(Qt::gray);
        break;

    case Globals::StudyConfiguration::INDEX_NBACKRT:
        logger.appendStandard("STARTING NBACK TRACE FOR RESPONSE TIME");
        experiment = new NBackRTExperiment(nullptr,VMDC::Study::NBACKRT);

        if (openvrco != nullptr) backgroundForVRScreen = QColor(Qt::black);
        break;
    case Globals::StudyConfiguration::INDEX_GONOGO:
        logger.appendStandard("STARTING GO - NO GO");
        experiment = new GoNoGoExperiment(nullptr,VMDC::Study::GONOGO);

        if (openvrco != nullptr) backgroundForVRScreen = QColor(Qt::gray);
        break;
    case Globals::StudyConfiguration::INDEX_NBACKVS:
        logger.appendStandard("STARTING N BACK VS");
        experiment = new NBackRTExperiment(nullptr,VMDC::Study::NBACKVS);

        if (openvrco != nullptr) backgroundForVRScreen = QColor(Qt::black);
        break;

    default:
        logger.appendError("Unknown experiment was selected " + study_config.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toString());
        return false;
    }

    // Hiding cursor UNLESS the ET is in mouse mode.
    if (!DBUGBOOL(Debug::Options::USE_MOUSE)){
        experiment->hideCursor();
    }

    // Making sure that that both experiment signals are connected.
    // Eyetracker should be connected by this point.
    connect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    connect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
    connect(experiment,&Experiment::updateVRDisplay,this,&FlowControl::onRequestUpdate);
    connect(experiment,&Experiment::updateStudyMessages,this,&FlowControl::onUpdatedExperimentMessages);

    // Making sure that the eyetracker is sending data.
    //qDebug() << "EyeTracker Enable Updating";
    eyeTracker->enableUpdating(true);

    if (DBUGSTR(Debug::Options::LOAD_CALIBRATION_K) != ""){
        // The eye needs to be forced in this instance
        study_config[VMDC::StudyParameter::VALID_EYE] = VMDC::Eye::BOTH;
    }

    // Start the experiment.
    experiment->startExperiment(configuration->getString(Globals::Share::PATIENT_DIRECTORY),
                                configuration->getString(Globals::Share::PATIENT_STUDY_FILE),
                                study_config);

    // Since experiments starts in the explanation phase, it is then necessary to render the first screen.
    // This will actually render the very first screen and update the text in the UI properly.
    experiment->renderCurrentStudyExplanationScreen();

    if (DBUGSTR(Debug::Options::LOAD_CALIBRATION_K) == ""){
       experiment->setCalibrationValidationData(eyeTracker->getCalibrationValidationData());
    }

    if (openvrco != nullptr){
        if (!configuration->getBool(Globals::VMPreferences::USE_MOUSE)){
            if (!openvrco->startRendering()){
                logger.appendError("Could not start rendering upon starting experiment");
                return false;
            }
            renderState = RENDERING_EXPERIMENT;
            openvrco->setScreenColor(backgroundForVRScreen);
        }
    }

    return true;
}

void FlowControl::startStudyEvaluationPhase(){
    if (experiment != nullptr)
        experiment->setStudyPhaseToEvaluation();
}

void FlowControl::startStudyExamplePhase(){
    if (experiment != nullptr)
        experiment->setStudyPhaseToExamples();
}

void FlowControl::on_experimentFinished(const Experiment::ExperimentResult &er){

    // Make the experiment windown invisible.
    experiment->hide();

    // Getting the information for the generated data file.
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
    disconnect(experiment,&Experiment::updateStudyMessages,this,&FlowControl::onUpdatedExperimentMessages);

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
