#include "flowcontrol.h"

FlowControl::FlowControl(QWidget *parent, ConfigurationManager *c) : QWidget(parent)
{

    // Intializing the eyetracker pointer
    configuration = c;
    experiment = nullptr;
    vrOK = false;
    this->setVisible(false);

    // Making all the connections to the render server.
    connect(&renderServerClient,&RenderServerClient::newPacketArrived,this,&FlowControl::onNewPacketArrived);
    connect(&renderServerClient,&RenderServerClient::connectionEstablished,this,&FlowControl::onConnectionEstablished);
    connect(&renderServerClient,&RenderServerClient::newMessage,this,&FlowControl::onNewMessage);
    connect(&renderServerClient,&RenderServerClient::readyToRender,this,&FlowControl::onReadyToRender);

    // Making all the connection to the calibration manager.
    connect(&calibrationManager,&CalibrationManager::calibrationDone,this,&FlowControl::onCalibrationDone);
    connect(&calibrationManager,&CalibrationManager::newPacketAvailable,this,&FlowControl::onNewCalibrationRenderServerPacketAvailable);

    // Making the connection to the end of the study processor.
    connect(&studyEndProcessor,&StudyEndOperations::finished,this,&FlowControl::onStudyEndProcessFinished);

    if (DBUGINT(Debug::Options::OVERRIDE_TIME) != 0){
        QString msg = "DBUG: Override Time has been set to " + QString::number(DBUGINT(Debug::Options::OVERRIDE_TIME));
        qDebug() << msg;
        StaticThreadLogger::warning("FlowControl::FlowControl",msg);
    }

    StaticThreadLogger::log("FlowControl::connectToEyeTracker","Creating new EyeTracker Object");

    if (Globals::EyeTracker::PROCESSING_PARAMETER_KEY == Globals::HPReverb::PROCESSING_PARAMETER_KEY){
        eyeTracker = new HPOmniceptInterface();
    }
    else{
        StaticThreadLogger::error("FlowControl::connectToEyeTracker","Failed creating to ET Object. Unknown key: " + Globals::EyeTracker::PROCESSING_PARAMETER_KEY);
    }

    StaticThreadLogger::log("FlowControl::connectToEyeTracker","Connecting to the EyeTracker " + Globals::EyeTracker::PROCESSING_PARAMETER_KEY);

    connect(eyeTracker,&EyeTrackerInterface::newDataAvailable,this,&FlowControl::onNewEyeDataAvailable);

    eyeTracker->connectToEyeTracker();

    calibrationHistory.reset();

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

    // StaticThreadLogger::log("FlowControl::onNewPacketArrived" , "Received Render Server Packet of Type: '" + packet.getType() + "'");


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
    else if (packet.getType() == RenderServerPacketType::TYPE_CALIB_CONTROL){
        // The only packet regarding calibration that we should be receiving is the one wiht the calibration data to compute the coefficients.
        calibrationManager.process3DCalibrationEyeTrackingData(packet);
    }
    else if (packet.getType() == RenderServerPacketType::TYPE_HAND_CALIB_CONTROL){
        // This should be the packet which contains the hand calibration values. They should be added to the processing parameters when a new study is created.
        QVariantList result;
        qreal hres = packet.getPayloadField(PacketHandCalibrationControl::H_CALIB_RESULT).toReal();
        qreal vres = packet.getPayloadField(PacketHandCalibrationControl::V_CALIB_RESULT).toReal();
        StaticThreadLogger::log("FlowControl::onNewPacketArrived","Received hand calibration results. H: " + QString::number(hres) + ". V: " + QString::number(vres));
        result << hres << vres;
        configuration->addKeyValuePair(Globals::Share::HAND_CALIB_RES,result);

        // If we go the hand calibration packet, we can effectively move on.
        emit FlowControl::handCalibrationDone();

    }
    else if ( (packet.getType() == RenderServerPacketType::TYPE_3DSTUDY_CONTROL) ||
              (packet.getType() == RenderServerPacketType::TYPE_STUDY_DATA) ||
              (packet.getType() == RenderServerPacketType::TYPE_STUDY_DESCRIPTION) ){

        if (experiment == nullptr){
            StaticThreadLogger::error("FlowControl::onNewPacketArrived","Received packet of type " + packet.getType() + " however no study is being run");
            return;
        }

        experiment->process3DStudyControlPacket(packet);

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
    // renderServerClient.sendEnable2DRenderPacket(true);
    // renderWaitScreen("");

    // Setting the remote debug options.
    bool logRender2DPackets = DBUGBOOL(Debug::Options::RENDER_PACKET_DBUG);
    bool enableGazeFollow   = DBUGBOOL(Debug::Options::ENABLE_GAZE_FOLLOW);
    QStringList handCalibValues = DBUGSLIST(Debug::Options::HAND_CALIB_RESULTS);

    //qDebug() << "HAND CALIB VALUES" << handCalibValues;

    bool sendDebugPacket = (logRender2DPackets || sendDebugPacket || !handCalibValues.isEmpty());

    if (sendDebugPacket){
        if (logRender2DPackets) StaticThreadLogger::warning("FlowControl::onReadyToRender","Render Packet DBug Enabled");
        if (enableGazeFollow) StaticThreadLogger::warning("FlowControl::onReadyToRender","Gaze Following in HMD Enabled");
        if (!handCalibValues.isEmpty()) StaticThreadLogger::warning("FlowControl::onReadyToRender","Will force set hand calibration to " + handCalibValues.join("|"));
        RenderServerPacket p;
        p.setPacketType(RenderServerPacketType::TYPE_DBUG_CONTROL);
        QVariantMap map;
        map[PacketDebugControl::ENABLE_RENDER_2D_LOG] = logRender2DPackets;
        map[PacketDebugControl::ENABLE_3D_HMD_LASER_SIGHT] = enableGazeFollow;
        if (!handCalibValues.isEmpty()){
            map[PacketDebugControl::ENABLE_FORCE_HAND_CALIB] = handCalibValues.join("|");
        }
        p.setPayloadField(PacketDebugControl::JSON_DICT_FIELD,map);
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

void FlowControl::resetCalibrationHistory(){
    calibrationHistory.reset();
}

void FlowControl::renderWaitScreen(const QString &message){

    //qDebug() << "Render Wait Screen";

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
        text->setAlignment(QString(text->ALIGN_CENTER));
        QRectF br = text->boundingRect();
        QRectF imgbr = img->boundingRect();

        qreal x = (screen.width() - br.width())/2;
        qreal y = imgbr.top() + imgbr.height() + 50;
        text->setPos(x,y);
    }

    renderServerClient.sendPacket(waitScreen.render());
}

void FlowControl::closeApplication(){
    StaticThreadLogger::kill();
    renderServerClient.closeRenderServer();
}


bool FlowControl::isVROk() const{
    return (vrOK && renderServerClient.isRenderServerWorking());
}


////////////////////////////// AUXILIARY FUNCTIONS ///////////////////////////////////////

void FlowControl::resolutionCalculations(){

    QRect screen = QGuiApplication::primaryScreen()->geometry();
    configuration->addKeyValuePair(Globals::Share::MONITOR_RESOLUTION_WIDTH,screen.width());
    configuration->addKeyValuePair(Globals::Share::MONITOR_RESOLUTION_HEIGHT,screen.height());

    QSize s = renderServerClient.getRenderResolution();
    configuration->addKeyValuePair(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH, s.width());
    configuration->addKeyValuePair(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT,s.height());

}

void FlowControl::keyboardKeyPressed(int key){
    if (experiment != nullptr){
        experiment->keyboardKeyPressed(key);
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
    data = calibrationManager.correct2DData(data);

    qreal r = 0.01*displayImage.width();
    RenderServerCircleItem *  left  = displayImage.addEllipse(static_cast<qint32>(data.xl()-r),static_cast<qint32>(data.yl()-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r),QPen(),QBrush(QColor(0,255,0,100)));
    RenderServerCircleItem *  right = displayImage.addEllipse(static_cast<qint32>(data.xr()-r),static_cast<qint32>(data.yr()-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r),QPen(),QBrush(QColor(0,0,255,100)));

    //StaticThreadLogger::log("FlowControl::onRequestUpdate","Sending experiment image rendering packet");

    if (!DBUGBOOL(Debug::Options::ENABLE_GAZE_FOLLOW)){
        left->setDisplayOnly(true);
        right->setDisplayOnly(true);
    }

    renderServerClient.sendPacket(displayImage.render(true));
    //renderServerClient.sendPacket(displayImage.render());

}

void FlowControl::onNewEyeDataAvailable(const EyeTrackerData &data){


    if (calibrationManager.requires2DCalibrationDataPointSamples()){

        calibrationManager.addEyeDataToCalibrationPoint(static_cast<float>(data.xl()),
                                                        static_cast<float>(data.xr()),
                                                        static_cast<float>(data.yl()),
                                                        static_cast<float>(data.yr()),
                                                        static_cast<float>(data.zl()),
                                                        static_cast<float>(data.zr()));
        return;

    }

    if (experiment != nullptr){

        EyeTrackerData corrected = calibrationManager.correct2DData(data);
        experiment->newEyeDataAvailable(corrected);
    }
}

void FlowControl::onStudyEndProcessFinished(){
    // We are done processing we notify the front end.
    emit FlowControl::studyEndProcessingDone();
}

void FlowControl::finalizeStudyOperations() {
    // We start the end of operations processing for the last saved files.
    studyEndProcessor.start();
}


void FlowControl::onNewControlPacketAvailableFromStudy(){
    renderServerClient.sendPacket(experiment->getControlPacket());
}

void FlowControl::handCalibrationControl(qint32 command, const QString &which_hand){

    //qDebug() << "Hand Calibration Command" << command << " hand " << which_hand;
    qint32 hand_sel = -1;
    if (which_hand == VMDC::Hand::LEFT){
        hand_sel = 0;
    }
    else if (which_hand == VMDC::Hand::RIGHT){
        hand_sel = 1;
    }
    else if (which_hand == VMDC::Hand::BOTH){
        hand_sel = 2;
    }
    else {
        StaticThreadLogger::error("FlowControl::handCalibrationControl","Received unknown hand parameter of which hadnd to caibrate: " + which_hand);
    }

    RenderServerPacket p;
    p.setPacketType(RenderServerPacketType::TYPE_HAND_CALIB_CONTROL);

    switch (command) {
    case HAND_CALIB_END:
        p.setPayloadField(PacketHandCalibrationControl::HORIZONTAL,false);
        p.setPayloadField(PacketHandCalibrationControl::VERTICAL,false);
        break;
    case HAND_CALIB_START_H:
        p.setPayloadField(PacketHandCalibrationControl::HORIZONTAL,true);
        p.setPayloadField(PacketHandCalibrationControl::VERTICAL,false);
        p.setPayloadField(PacketHandCalibrationControl::HANDS,hand_sel);
        break;
    case HAND_CALIB_START_V:
        p.setPayloadField(PacketHandCalibrationControl::HORIZONTAL,false);
        p.setPayloadField(PacketHandCalibrationControl::VERTICAL,true);
        p.setPayloadField(PacketHandCalibrationControl::HANDS,hand_sel);
        break;
    default:
        StaticThreadLogger::error("FlowControl::handCalibrationControl","Received unknown hand calibration control command: " + QString::number(command));
        return;
    }

    renderServerClient.sendPacket(p);

}

void FlowControl::calibrateEyeTracker(bool useSlowCalibration, bool mode3D){

    int required_number_of_accepted_pts = 7;
    int ncalibration_points = 9;
    if (DBUGEXIST(Debug::Options::FORCE_N_CALIB_PTS)){
        ncalibration_points = DBUGINT(Debug::Options::FORCE_N_CALIB_PTS);
        required_number_of_accepted_pts = 2;
    }

    // Making sure the right eye is used, in both the calibration and the experiment.
    eyeTracker->setEyeToTransmit(VMDC::Eye::BOTH);

    // qDebug() << "Selected eye to use number" << eye_to_use << "translated to" << selected_eye_to_use;

    QString strmode = "2D";
    if (mode3D) strmode = "3D";
    StaticThreadLogger::log("FlowControl::calibrateEyeTracker","Calibrating the eyetracker: " + strmode);

    if (!mode3D) { // Local eyetracking is required only for 2D Calibration
        eyeTracker->enableUpdating(true); // To ensure that eyetracking data is gathered.
    }


    QString coefficient_file = "";    
    qint32 validation_point_acceptance_threshold = 70;
    qint32 gather_time, wait_time;

    if (useSlowCalibration){
        StaticThreadLogger::log("FlowControl::calibrateEyeTracker","Requested use of slow calibration");
        validation_point_acceptance_threshold = 60;
        gather_time = CALIB_PT_GATHER_TIME_SLOW;
        wait_time = CALIB_PT_WAIT_TIME_SLOW;
    }
    else {
        validation_point_acceptance_threshold = 60;
        gather_time = CALIB_PT_GATHER_TIME_NORMAL;
        wait_time = CALIB_PT_WAIT_TIME_NORMAL;
    }

    // Testing validations parameters.
    QVariantMap calibrationValidationParameters;
    calibrationValidationParameters[VMDC::CalibrationConfigurationFields::REQ_NUMBER_OF_ACCEPTED_POINTS] = required_number_of_accepted_pts;
    calibrationValidationParameters[VMDC::CalibrationConfigurationFields::VALIDATION_POINT_HIT_TOLERANCE] = 0;
    calibrationValidationParameters[VMDC::CalibrationConfigurationFields::NUMBER_OF_CALIBRAION_POINTS] = ncalibration_points;
    calibrationValidationParameters[VMDC::CalibrationAttemptFields::CALIBRATION_POINT_GATHERTIME] = gather_time;
    calibrationValidationParameters[VMDC::CalibrationAttemptFields::CALIBRATION_POINT_WAITTIME] = wait_time;
    calibrationValidationParameters[VMDC::CalibrationAttemptFields::VALIDATION_POINT_ACCEPTANCE_THRESHOLD] = 70;

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

    QSize s = renderServerClient.getRenderResolution();

    calibrationManager.startCalibration(s.width(),s.height(),                                        
                                        mode3D,
                                        calibrationValidationParameters,coefficient_file);

}

// Signals that a new packet needs to be sent to the Remote Render Server.
void FlowControl::onNewCalibrationRenderServerPacketAvailable(){
    renderServerClient.sendPacket(calibrationManager.getRenderServerPacket());
}

bool FlowControl::isConnected() const{
    return eyeTracker->isEyeTrackerConnected();
}

// When the calibration process is finished.
void FlowControl::onCalibrationDone(qint32 code){

    // We now store the current calibration attempt.
    calibrationHistory.setConfiguration(calibrationManager.getCalibrationConfigurationParameters());
    calibrationHistory.addCalibrationAttempt(calibrationManager.getCalibrationAttemptData());


    // For Debugging only
    // calibrationManager.debugSaveCalibrationValidationData("zz.json");

    if (code == CalibrationManager::CALIBRATION_SUCCESSFUL){
        StaticThreadLogger::log("FlowControl::onCalibrationDone","EyeTracker Control: Calibration and validation finished");

        // At this point the calibration validation data structure should be available.
        // The structure should be used in order to determine which eye will be the eye used in the study.

        // We can now get the selected eye to use.
        eyeTracker->setEyeToTransmit(calibrationManager.getRecommendedEye());
        StaticThreadLogger::log("FlowControl::onCalibrationDone","Calibration Validation Report After Calibration Done");
        StaticThreadLogger::log("FlowControl::onCalibrationDone",calibrationManager.getCalibrationValidationReport());
        StaticThreadLogger::log("FlowControl::onCalibrationDone","Selected Eye: " + calibrationManager.getRecommendedEye());

        // We need to wait a few milliseconds otherwise the end screen does NOT get shown.
        emit FlowControl::calibrationDone(true);
    }
    else {
        StaticThreadLogger::error("FlowControl::onCalibrationDone","Calibration could not be completed");

        renderWaitScreen("");

        // We need to wait a few milliseconds otherwise the end screen does NOT get shown.
        emit FlowControl::calibrationDone(false);
    }

}


QVariantMap FlowControl::getCalibrationValidationData() const {

    QVariantMap map = calibrationHistory.getMapOfLastAttempt();

    // We add the resolution as it is required for plotting the calibration data.
    map["W"] = configuration->getInt(Globals::Share::STUDY_DISPLAY_RESOLUTION_WIDTH);
    map["H"] = configuration->getInt(Globals::Share::STUDY_DISPLAY_RESOLUTION_HEIGHT);

    return map;

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

    QColor backgroundForVRScreen;
    experimentIsOk = true;

    // Forcing the valid eye to the calibration selected eye.
    study_config[VMDC::StudyParameter::VALID_EYE] = calibrationManager.getRecommendedEye();
    //Debug::prettpPrintQVariantMap(study_config);

    // Using the polimorphism, the experiment object is created according to the selected index.
    switch (study_config.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toInt()){

    case Globals::StudyConfiguration::INDEX_BINDING_BC:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING BINDING BC EXPERIMENT");
        experiment = new BindingExperiment(nullptr,VMDC::Study::BINDING_BC);
        backgroundForVRScreen = QColor(Qt::gray);
        break;
    case Globals::StudyConfiguration::INDEX_BINDING_UC:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING BINDING UC EXPERIMENT");
        experiment = new BindingExperiment(nullptr,VMDC::Study::BINDING_UC);
        backgroundForVRScreen = QColor(Qt::gray);
        break;

    case Globals::StudyConfiguration::INDEX_NBACKRT:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING NBACK TRACE FOR RESPONSE TIME");
        if (configuration->getString(Globals::VMPreferences::UI_LANGUAGE) == Globals::UILanguage::ES){
            study_config[VMDC::StudyParameter::LANGUAGE] = VMDC::UILanguage::SPANISH;
        }
        else {
            study_config[VMDC::StudyParameter::LANGUAGE] = VMDC::UILanguage::ENGLISH;
        }
        experiment = new NBackRTExperiment(nullptr,VMDC::Study::NBACKRT);
        backgroundForVRScreen = QColor(Qt::black);
        break;        
    case Globals::StudyConfiguration::INDEX_NBACK:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING NBACK SLOW DEFAULT VERSION");
        if (configuration->getString(Globals::VMPreferences::UI_LANGUAGE) == Globals::UILanguage::ES){
            study_config[VMDC::StudyParameter::LANGUAGE] = VMDC::UILanguage::SPANISH;
        }
        else {
            study_config[VMDC::StudyParameter::LANGUAGE] = VMDC::UILanguage::ENGLISH;
        }
        experiment = new NBackRTExperiment(nullptr,VMDC::Study::NBACK);
        backgroundForVRScreen = QColor(Qt::black);
        break;
    case Globals::StudyConfiguration::INDEX_GONOGO:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING GO - NO GO");
        experiment = new GoNoGoExperiment(nullptr,VMDC::Study::GONOGO);
        backgroundForVRScreen = QColor(Qt::gray);
        break;
    case Globals::StudyConfiguration::INDEX_NBACKVS:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING N BACK VS");
        if (configuration->getString(Globals::VMPreferences::UI_LANGUAGE) == Globals::UILanguage::ES){
            study_config[VMDC::StudyParameter::LANGUAGE] = VMDC::UILanguage::SPANISH;
        }
        else {
            study_config[VMDC::StudyParameter::LANGUAGE] = VMDC::UILanguage::ENGLISH;
        }
        experiment = new NBackRTExperiment(nullptr,VMDC::Study::NBACKVS);
        backgroundForVRScreen = QColor(Qt::black);
        break;
    case Globals::StudyConfiguration::INDEX_GNG_SPHERE:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING GO - NO GO SPHERES");
        experiment = new GoNoGoSphereExperiment(nullptr,VMDC::Study::GONOGO_SPHERE);
        backgroundForVRScreen = QColor(Qt::black); // This is ignore in all 3D studies.
        break;
    case Globals::StudyConfiguration::INDEX_PASSBALL:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING PASSBALL");
        experiment = new PassBallExperiment(nullptr,VMDC::Study::PASSBALL);
        backgroundForVRScreen = QColor(Qt::black); // This is ignore in all 3D studies.
        break;
    default:
        StaticThreadLogger::error("FlowControl::startNewExperiment","Unknown experiment was selected " + study_config.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toString());
        return false;
    }

    // Making sure that that both experiment signals are connected.
    // Eyetracker should be connected by this point.
    connect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    connect(experiment,&Experiment::updateVRDisplay,this,&FlowControl::onRequestUpdate);
    connect(experiment,&Experiment::updateStudyMessages,this,&FlowControl::onUpdatedExperimentMessages);
    connect(experiment,&Experiment::remoteRenderServerPacketAvailable,this,&FlowControl::onNewControlPacketAvailableFromStudy);

    // Making sure that the eyetracker is sending data, but only if it's a 2D study.
    bool studyIs3D = study_config.value("is_3d_study").toBool();
    //qDebug() << "IS STARTING STUDY 3D" << studyIs3D;
    if (!studyIs3D) eyeTracker->enableUpdating(true);

    // Start the experiment.
    qDebug() << "Calling start experiment";
    if (!experiment->startExperiment(configuration->getString(Globals::Share::PATIENT_DIRECTORY),
                                configuration->getString(Globals::Share::PATIENT_STUDY_FILE),
                                study_config)){
        return false;
    }


    // Since experiments starts in the explanation phase, it is then necessary to render the first screen.
    // This will actually render the very first screen and update the text in the UI properly.
    experiment->renderCurrentStudyExplanationScreen();

    // We add the calibration history as this function is called once the calibration is approved.
    experiment->setCalibrationValidationData(calibrationHistory.getHistory());

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

    // Making sure we are no longer receing EyeTracker info, even if it's only for 3D Studies.
    eyeTracker->enableUpdating(false);

    // If there was a hand calibration result, we need to remove it.
    configuration->removeKey(Globals::Share::HAND_CALIB_RES);

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
    disconnect(experiment,&Experiment::updateVRDisplay,this,&FlowControl::onRequestUpdate);
    disconnect(experiment,&Experiment::updateStudyMessages,this,&FlowControl::onUpdatedExperimentMessages);
    disconnect(experiment,&Experiment::remoteRenderServerPacketAvailable,this,&FlowControl::onNewCalibrationRenderServerPacketAvailable);

    // We get the files for this experiment and set them. The processing will start AFTER we can get the wait screen up.
    QStringList files = experiment->getDataFilesLocation();
    studyEndProcessor.setFileToProcess(files.first(),files.last());

    delete experiment;
    experiment = nullptr;

    // Notifying the QML.
    emit(experimentHasFinished());

}

///////////////////////////////////////////////////////////////////
FlowControl::~FlowControl(){

}
