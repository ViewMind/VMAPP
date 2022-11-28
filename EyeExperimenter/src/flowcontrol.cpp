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
    this->setVisible(false);
    vrOK = false;
    usingVR = false;

    connect(&delayTimer,&QTimer::timeout,this,&FlowControl::onDelayTimerUp);

    // Creating the OpenVR Object if so defined.
    if ((Globals::EyeTracker::IS_VR) && (!DBUGBOOL(Debug::Options::USE_MOUSE))){

        //Making all the connections to the render server.
        connect(&renderServerClient,&RenderServerClient::newPacketArrived,this,&FlowControl::onNewPacketArrived);
        connect(&renderServerClient,&RenderServerClient::connectionEstablished,this,&FlowControl::onConnectionEstablished);
        connect(&renderServerClient,&RenderServerClient::newMessage,this,&FlowControl::onNewMessage);
        connect(&renderServerClient,&RenderServerClient::readyToRender,this,&FlowControl::onReadyToRender);
        usingVR = true;

    }
    else {
        if (DBUGBOOL(Debug::Options::USE_MOUSE)){
            qDebug() << "DBUG: Mouse Use Enabled";
            StaticThreadLogger::warning("FlowControl::FlowControl","DBUG: Using Mouse as EyeTracker");
        }
        vrOK = true;
    }

    if (DBUGINT(Debug::Options::OVERRIDE_TIME) != 0){
        QString msg = "DBUG: Override Time has been set to " + QString::number(DBUGINT(Debug::Options::OVERRIDE_TIME));
        qDebug() << msg;
        StaticThreadLogger::warning("FlowControl::FlowControl",msg);
    }


}

void FlowControl::startRenderServerAndSetWindowID(WId winID){
    mainWindowID = winID;
    // The redner server client can only be started once we have the window ID.
    QString location = Globals::RemoteRenderServerParameters::DIR + "/" + Globals::RemoteRenderServerParameters::EXE;
    renderServerClient.startRenderServer(location,winID);
}


void FlowControl::setRenderWindowGeometry(int target_x, int target_y, int target_w, int target_h){

    // We need to use the Window Function to get DPI multiplication being used by the system as it affects the sizes and positions we need to set
    // to the Unity Render Server Window Geometry.
    quint32 DPI = GetDpiForWindow((HWND) mainWindowID);

    float dpi_multiplier = static_cast<float>(DPI)/96.0f;

    target_x = static_cast<qint32>( static_cast<float>(target_x)*dpi_multiplier );
    target_y = static_cast<qint32>( static_cast<float>(target_y)*dpi_multiplier );
    target_w = static_cast<qint32>( static_cast<float>(target_w)*dpi_multiplier );
    target_h = static_cast<qint32>( static_cast<float>(target_h)*dpi_multiplier );

    renderServerClient.resizeRenderWindow(target_x,target_y,target_w,target_h);

}

void FlowControl::hideRenderWindow(){
    renderServerClient.hideRenderWindow();
}

void FlowControl::showRenderWindow(){
    emit FlowControl::requestWindowGeometry();
    renderServerClient.showRenderWindow();
}

void FlowControl::setRenderWindowState(bool hidden){
    renderServerClient.setRenderWindowHiddenFlag(hidden);
}



void FlowControl::onNewMessage(const QString &msg, const quint8 &msgType){
    if (msgType == RenderServerClient::MSG_TYPE_ERROR){
        StaticThreadLogger::error("FlowControl::onNewMessage",msg);
    }
    else if (msgType == RenderServerClient::MSG_TYPE_INFO){
        StaticThreadLogger::log("FlowControl::onNewMessage",msg);
    }
    else if (msgType == RenderServerClient::MSG_TYPE_WARNING){
        StaticThreadLogger::warning("FlowControl::onNewMessage",msg);
    }

    // vrOK needs to be checked as we might get several messages at startup before the render server is working.
    if (!renderServerClient.isRenderServerWorking() && (vrOK)){
        emit FlowControl::renderServerDisconnect();
    }

}

void FlowControl::onConnectionEstablished(){

    StaticThreadLogger::log("FlowControl::onConnectionEstablished","Render Server Connection established");

    RenderServerPacket packet;
    packet.setPacketType(RenderServerPacketType::TYPE_LOG_LOCATION);
    packet.setPayloadField(PacketLogLocation::LOG_LOCATION,StaticThreadLogger::getFullLogFilePath());
    packet.setPayloadField(PacketLogLocation::APP_NAME,"RenderServer");
    renderServerClient.sendPacket(packet);

    StaticThreadLogger::log("FlowControl::onConnectionEstablished","Sent log relocation packet");

    emit FlowControl::requestWindowGeometry();

}

void FlowControl::onNewPacketArrived(){

    RenderServerPacket packet = renderServerClient.getPacket();

    StaticThreadLogger::log("FlowControl::onNewPacketArrived" , "Received Render Server Packet of Type: '" + packet.getType() + "'");

    if (packet.getType() == RenderServerPacketType::TYPE_IMG_SIZE_REQ){
        QString name = packet.getPayloadField(PacketImgSizeRequest::NAME).toString();
        qreal w = packet.getPayloadField(PacketImgSizeRequest::WIDTH).toReal();
        qreal h = packet.getPayloadField(PacketImgSizeRequest::HEIGHT).toReal();

        if ((w < 1) || (h < 1)){
            StaticThreadLogger::error("FlowControl::onNewPacketArrived","Got BAD image size of " + QString::number(w) + "x" + QString::number(h) + " for image " + name);
            return;
        }

        if (name == RenderServerImageNames::BACKGROUND_LOGO){
            backgroundLogoSize.setWidth(w);
            backgroundLogoSize.setHeight(h);
            QString size = QString::number(w) + "x" + QString::number(h);
            StaticThreadLogger::log("FlowControl::onNewPacketArrived" , "Background image resolution of " + size);
            renderWaitScreen("");
        }
        else {
            StaticThreadLogger::warning("FlowControl::onNewPacketArrived","Image size request of unexpected image of name '" + name + "'");
        }
    }
    else {
        StaticThreadLogger::warning("FlowControl::onNewPacketArrived","Unexpected packet arrival of type '" + packet.getType() + "'");
    }

}

void FlowControl::onReadyToRender() {

    StaticThreadLogger::log("FlowControl::onReadyToRender","Enabling 2D Rendering and Sending background image request");

    // A this point we know that the server is ready to render and we know the rendering resolution.
    // So we add the resolutions to the configuration
    this->resolutionCalculations();
    vrOK = true;

    // We send the packet to render server, and also we enable 2D rendering
    renderServerClient.sendEnable2DRenderPacket(true);
    // renderWaitScreen("");

    if (DBUGBOOL(Debug::Options::RENDER_PACKET_DBUG)){
        StaticThreadLogger::warning("FlowControl::onReadyToRender","Render Packet DBug Enabled");
        RenderServerPacket p;
        p.setPacketType(RenderServerPacketType::TYPE_2D_DBUG_ENABLE);
        p.setPayloadField(RenderControlPacketFields::ENABLE_RENDER_P_DBUG,true);
        renderServerClient.sendPacket(p);
    }

    // We need to request the image dimensions.
    RenderServerPacket logoDimRequest;
    logoDimRequest.setPacketType(RenderServerPacketType::TYPE_IMG_SIZE_REQ);
    logoDimRequest.setPayloadField(PacketImgSizeRequest::HEIGHT,0);
    logoDimRequest.setPayloadField(PacketImgSizeRequest::WIDTH,0);
    logoDimRequest.setPayloadField(PacketImgSizeRequest::NAME,RenderServerImageNames::BACKGROUND_LOGO);
    renderServerClient.sendPacket(logoDimRequest);

    // And now we hide the render window.
    this->hideRenderWindow();

}

void FlowControl::renderWaitScreen(const QString &message){

    RenderServerScene waitScreen;
    waitScreen.setBackgroundBrush(QBrush(Qt::gray));

    QSize screen = renderServerClient.getRenderResolution();
    waitScreen.setSceneRect(0,0,screen.width(),screen.height());

    qreal w = backgroundLogoSize.width();
    qreal h = backgroundLogoSize.height();

    qreal tw = 0.7*screen.width();

    RenderServerImageItem *img = waitScreen.addImage(RenderServerImageNames::BACKGROUND_LOGO,true,w,h,tw);
    img->setPos(0.15*screen.width(),0.3*screen.height());

    if (message != ""){
        QFont font;
        font.setPointSizeF(80);
        font.setWeight(QFont::Normal);
        RenderServerTextItem *text = waitScreen.addText(message,font);
        text->setBrush(QBrush(QColor("#2A3990")));
        text->setAlignment(text->ALIGN_CENTER);
        QRectF br = text->boundingRect();
        QRectF imgbr = img->boundingRect();

        qreal x = (screen.width() - br.width())/2;
        qreal y = imgbr.top() + imgbr.height() + 50;
        text->setPos(x,y);
    }

    renderServerClient.sendPacket(waitScreen.render());
}


bool FlowControl::isVROk() const{
    if (!usingVR) return true;
    return (vrOK && renderServerClient.isRenderServerWorking());
}


////////////////////////////// AUXILIARY FUNCTIONS ///////////////////////////////////////

void FlowControl::resolutionCalculations(){

    QRect screen = QGuiApplication::primaryScreen()->geometry();
    configuration->addKeyValuePair(Globals::Share::MONITOR_RESOLUTION_WIDTH,screen.width());
    configuration->addKeyValuePair(Globals::Share::MONITOR_RESOLUTION_HEIGHT,screen.height());

    if (usingVR){
        // This menas the resolution for drawing should be the one recommende by OpenVR.
        //QSize s = openvrco->getRecommendedSize();
        QSize s = renderServerClient.getRenderResolution();
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
    // We could stop rendering in 2D but that only makes sense when we are going to show ONLY 3D stuff.
    //    if (usingVR){
    //        renderServerClient.sendEnable2DRenderPacket(false);
    //    }
    if (eyeTracker != nullptr){
        eyeTracker->enableUpdating(false);
    }
}


////////////////////////////// FLOW CONTROL FUNCTIONS ///////////////////////////////////////

void FlowControl::onRequestUpdate(){

    if (eyeTracker == nullptr) {
        StaticThreadLogger::error("FlowControl::onRequestUpdate","VR Update Request: Rendering experiment with a non existant eyeTracker object. Force stopping rendering");
        renderWaitScreen("");
        return;
    }
    if (experiment == nullptr) {
        StaticThreadLogger::error("FlowControl::onRequestUpdate","VR Update Request: Rendering experiment with a non existant experiment object. Force stopping rendering");
        renderWaitScreen("");
        return;
    }

    //QImage image = experiment->getVRDisplayImage();
    RenderServerScene displayImage = experiment->getVRDisplayImage();

    // On experiment images, the eye position is added.
    EyeTrackerData data = eyeTracker->getLastData();

    qreal r = 0.01*displayImage.width();
    RenderServerCircleItem *  left  = displayImage.addEllipse(static_cast<qint32>(data.xLeft-r),static_cast<qint32>(data.yLeft-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r),QPen(),QBrush(QColor(0,255,0,100)));
    RenderServerCircleItem *  right = displayImage.addEllipse(static_cast<qint32>(data.xRight-r),static_cast<qint32>(data.yRight-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r),QPen(),QBrush(QColor(0,0,255,100)));

    StaticThreadLogger::log("FlowControl::onRequestUpdate","Sending experiment image rendering packet");
    if (!DBUGBOOL(Debug::Options::SHOW_EYES_IN_STUDY)){
        left->setDisplayOnly(true);
        right->setDisplayOnly(true);
    }

    renderServerClient.sendPacket(displayImage.render());

}

void FlowControl::onDelayTimerUp() {
    delayTimer.stop();
    emit FlowControl::calibrationDone(calibrated);
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

    StaticThreadLogger::log("FlowControl::connectToEyeTracker","Creating new EyeTracker Object");

    // The new et is NOT calibrated.
    calibrated = false;


    if (configuration->getBool(Globals::VMPreferences::USE_MOUSE)){
        eyeTracker = new MouseInterface();
        StaticThreadLogger::log("FlowControl::connectToEyeTracker","Connecting to the Mouse ... ");
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
                StaticThreadLogger::error("FlowControl::connectToEyeTracker","Failed connecting to ET. Unknown key: " + Globals::EyeTracker::PROCESSING_PARAMETER_KEY);
            }

            StaticThreadLogger::log("FlowControl::connectToEyeTracker","Connecting to the EyeTracker " + Globals::EyeTracker::PROCESSING_PARAMETER_KEY);
        }
        else{
            StaticThreadLogger::warning("FlowControl::connectToEyeTracker","Using Mouse EyeTracker DBug Option");
            eyeTracker = new MouseInterface();
            static_cast<MouseInterface*>(eyeTracker)->overrideCalibration();
        }

    }

    connect(eyeTracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));

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

    StaticThreadLogger::log("FlowControl::calibrateEyeTracker","Calibrating the eyetracker....");

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
            StaticThreadLogger::warning("FlowControl::calibrateEyeTracker",dbug);
        }
        else {
            QString dbug = "DBUG: Calibration coefficients do no exist. File "  + calibrationParams.name + " will be created after next calibration";
            qDebug() << dbug;
            StaticThreadLogger::warning("FlowControl::calibrateEyeTracker",dbug);
        }
    }

    qint32 validation_point_acceptance_threshold = 70;
    if (useSlowCalibration){
        StaticThreadLogger::log("FlowControl::calibrateEyeTracker","Requested use of slow calibration");
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
        StaticThreadLogger::error("FlowControl::onEyeTrackerControl","Possible crash alert: Entering onEyeTrackerControl with a null pointer eyeTracker object");
    }

    switch(code){
    case EyeTrackerInterface::ET_CODE_CALIBRATION_ABORTED:
        StaticThreadLogger::warning("FlowControl::onEyeTrackerControl","EyeTracker Control: Calibration aborted");
        calibrated = false;

        if (Globals::EyeTracker::IS_VR) eyeTracker->enableUpdating(false);
        renderWaitScreen("");

        // We need to wait a few milliseconds otherwise the end screen does NOT get shown.
        emit FlowControl::calibrationDone(calibrated);//delayTimer.start(10);

        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_DONE:
        StaticThreadLogger::log("FlowControl::onEyeTrackerControl","EyeTracker Control: Calibration and validation finished");

        calibrated = true;

        if (Globals::EyeTracker::IS_VR) eyeTracker->enableUpdating(false);

        // At this point the calibration validation data structure should be available.
        // The structure should be used in order to determine which eye will be the eye used in the study.

        // We can now get the selected eye to use.
        selected_eye_to_use = eyeTracker->getCalibrationRecommendedEye();
        eyeTracker->setEyeToTransmit(selected_eye_to_use);
        StaticThreadLogger::log("FlowControl::onEyeTrackerControl","Calibration Validation Report After Calibration Done");
        StaticThreadLogger::log("FlowControl::onEyeTrackerControl",eyeTracker->getCalibrationValidationReport());
        StaticThreadLogger::log("FlowControl::onEyeTrackerControl","Selected Eye: " + selected_eye_to_use);

        // We need to wait a few milliseconds otherwise the end screen does NOT get shown.
        emit FlowControl::calibrationDone(calibrated); //delayTimer.start(10);

        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_FAIL:
        StaticThreadLogger::error("FlowControl::onEyeTrackerControl","EyeTracker Control: Connection to EyeTracker Failed");
        connected = false;
        emit(connectedToEyeTracker(false));
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_SUCCESS:
        StaticThreadLogger::log("FlowControl::onEyeTrackerControl","EyeTracker Control: Connection to EyeTracker Established");
        connected = true;
        emit(connectedToEyeTracker(true));
        break;
    case EyeTrackerInterface::ET_CODE_DISCONNECTED_FROM_ET:
        StaticThreadLogger::error("FlowControl::onEyeTrackerControl","EyeTracker Control: Disconnected from EyeTracker");
        connected = false;
        break;
    case EyeTrackerInterface::ET_CODE_NEW_CALIBRATION_IMAGE_AVAILABLE:
        //qDebug() << "Got a new calibration image";
        renderServerClient.sendPacket(eyeTracker->getCalibrationImage().render());
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
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING BINDING BC EXPERIMENT");
        experiment = new BindingExperiment(nullptr,VMDC::Study::BINDING_BC);
        if (!usingVR) backgroundForVRScreen = QColor(Qt::gray);
        break;
    case Globals::StudyConfiguration::INDEX_BINDING_UC:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING BINDING UC EXPERIMENT");
        experiment = new BindingExperiment(nullptr,VMDC::Study::BINDING_UC);

        if (!usingVR) backgroundForVRScreen = QColor(Qt::gray);
        break;

    case Globals::StudyConfiguration::INDEX_NBACKRT:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING NBACK TRACE FOR RESPONSE TIME");
        experiment = new NBackRTExperiment(nullptr,VMDC::Study::NBACKRT);

        if (!usingVR) backgroundForVRScreen = QColor(Qt::black);
        break;
    case Globals::StudyConfiguration::INDEX_GONOGO:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING GO - NO GO");
        experiment = new GoNoGoExperiment(nullptr,VMDC::Study::GONOGO);

        if (!usingVR) backgroundForVRScreen = QColor(Qt::gray);
        break;
    case Globals::StudyConfiguration::INDEX_NBACKVS:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING N BACK VS");
        experiment = new NBackRTExperiment(nullptr,VMDC::Study::NBACKVS);

        if (!usingVR) backgroundForVRScreen = QColor(Qt::black);
        break;

    default:
        StaticThreadLogger::error("FlowControl::startNewExperiment","Unknown experiment was selected " + study_config.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toString());
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

    //renderServerClient.sendEnable2DRenderPacket(true);

    // Since experiments starts in the explanation phase, it is then necessary to render the first screen.
    // This will actually render the very first screen and update the text in the UI properly.
    experiment->renderCurrentStudyExplanationScreen();

    if (DBUGSTR(Debug::Options::LOAD_CALIBRATION_K) == ""){
        experiment->setCalibrationValidationData(eyeTracker->getCalibrationValidationData());
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
        StaticThreadLogger::log("FlowControl::on_experimentFinished","EXPERIMENT aborted");
        experimentIsOk = false;
        break;
    case Experiment::ER_FAILURE:
        StaticThreadLogger::error("FlowControl::on_experimentFinished","EXPERIMENT FAILURE: " + experiment->getError());
        experimentIsOk = false;
        break;
    case Experiment::ER_NORMAL:
        StaticThreadLogger::log("FlowControl::on_experimentFinished","EXPERIMENT Finshed Successfully");
        break;
    case Experiment::ER_WARNING:
        StaticThreadLogger::warning("FlowControl::on_experimentFinished","EXPERIMENT WARNING: " + experiment->getError());
        break;
    }

    // Destroying the experiment and reactivating the start experiment.
    disconnect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    disconnect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
    disconnect(experiment,&Experiment::updateVRDisplay,this,&FlowControl::onRequestUpdate);
    disconnect(experiment,&Experiment::updateStudyMessages,this,&FlowControl::onUpdatedExperimentMessages);

    delete experiment;
    experiment = nullptr;

    // Notifying the QML.
    emit(experimentHasFinished());

}

///////////////////////////////////////////////////////////////////
FlowControl::~FlowControl(){

}
