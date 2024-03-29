#include "flowcontrol.h"

FlowControl::FlowControl(QObject *parent, ConfigurationManager *c) : QObject(parent)
{

    // Intializing the eyetracker pointer
    configuration = c;

    // Making all the connections to the render server.
    connect(&renderServerClient,&RenderServerClient::newPacketArrived,this,&FlowControl::onNewPacketArrived);
    connect(&renderServerClient,&RenderServerClient::connectionEstablished,this,&FlowControl::onConnectionEstablished);
    connect(&renderServerClient,&RenderServerClient::newMessage,this,&FlowControl::onNewMessage);
    connect(&renderServerClient,&RenderServerClient::readyToRender,this,&FlowControl::onReadyToRender);

    // Making all the connection to the calibration manager.
    connect(&calibrationManager,&CalibrationManager::calibrationDone,this,&FlowControl::onCalibrationDone);
    connect(&calibrationManager,&CalibrationManager::newPacketAvailable,this,&FlowControl::onNewCalibrationPacketAvailable);

    // Making the connection to the end of the study processor.
    connect(&studyEndProcessor,&StudyEndOperations::finished,this,&FlowControl::onStudyEndProcessFinished);

    // Making the connections with study control.
    connect(&studyControl,&StudyControl::newPacketAvailable,this,&FlowControl::onNewControlPacketAvailableFromStudy);
    connect(&studyControl,&StudyControl::studyEnd,this,&FlowControl::onStudyEnd);
    connect(&studyControl,&StudyControl::updateStudyMessages,this,&FlowControl::onUpdatedExperimentMessages);

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

    if (!renderServerClient.isRenderServerWorking() && renderServerClient.isReadyToRender()){
        emit FlowControl::renderServerDisconnect();
    }

}

void FlowControl::onNewCalibrationPacketAvailable(){
    renderServerClient.sendPacket(calibrationManager.getRenderServerPacket());
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

    if (DBUGBOOL(Debug::Options::PRINT_INCOMMING)){
        QString message = "Incomming Packet of type '" + packet.getType() + "' :\n" + Debug::QVariantMapToString(packet.getPayload());
        StaticThreadLogger::warning("FlowControl::onNewPacketArrived",message);
    }


    if (packet.getType() == RRS::PacketType::TYPE_CALIB_CONTROL){
        // The only packet regarding calibration that we should be receiving is the one with the calibration data to compute the coefficients.
        QString command = packet.getPayloadField(RRS::PacketCalibrationControl::COMMAND).toString();
        if ( (command == RRS::CommandCalibrationControls::CMD_DATA_2D) || (command == RRS::CommandCalibrationControls::CMD_DATA_3D) ){
            calibrationManager.processCalibrationData(packet);
        }
        else {
            StaticThreadLogger::error("FlowControl::onNewPacketArrived","Received unepected calibration command of " + command);
        }
    }
    else if (packet.getType() == RRS::PacketType::TYPE_HAND_CALIB_CONTROL){

        // This should be the packet which contains the hand calibration values.
        // They should be added to the processing parameters when a new study is created.
        QVariantList result;
        qreal hres = packet.getPayloadField(RRS::PacketHandCalibrationControl::H_CALIB_RESULT).toReal();
        qreal vres = packet.getPayloadField(RRS::PacketHandCalibrationControl::V_CALIB_RESULT).toReal();
        StaticThreadLogger::log("FlowControl::onNewPacketArrived","Received hand calibration results. H: " + QString::number(hres) + ". V: " + QString::number(vres));
        result << hres << vres;
        configuration->addKeyValuePair(Globals::Share::HAND_CALIB_RES,result);

        // If we go the hand calibration packet, we can effectively move on.
        emit FlowControl::handCalibrationDone();

    }
    else if ( (packet.getType() == RRS::PacketType::TYPE_STUDY_CONTROL ) ||
              (packet.getType() == RRS::PacketType::TYPE_STUDY_DATA) ){

        // These packets are processed by study control
        studyControl.receiveRenderServerPacket(packet);

    }
    else {
        StaticThreadLogger::warning("FlowControl::onNewPacketArrived","Unexpected packet arrival of type '" + packet.getType() + "'");
    }

}

void FlowControl::onReadyToRender() {

    StaticThreadLogger::log("FlowControl::onReadyToRender","Ready to render");

    // Setting the remote debug options.
    bool enableGazeFollow   = DBUGBOOL(Debug::Options::ENABLE_GAZE_FOLLOW);
    bool enablePacketPrintout = DBUGBOOL(Debug::Options::RRS_PACKET_PRINT);
    QStringList handCalibValues = DBUGSLIST(Debug::Options::HAND_CALIB_RESULTS);

    //qDebug() << "HAND CALIB VALUES" << handCalibValues;

    bool sendDebugPacket = (enableGazeFollow || !handCalibValues.isEmpty() || enablePacketPrintout);

    if (sendDebugPacket){

        RenderServerPacket p;
        p.setPacketType(RRS::PacketType::TYPE_DBUG_CONTROL);

        QVariantMap map;
        if (enableGazeFollow) {
            map[RRS::PacketDebugControl::ENABLE_HMD_EYE_FOLLOW] = "true";
            StaticThreadLogger::warning("FlowControl::onReadyToRender","Gaze Following in HMD Enabled");
        }
        if (!handCalibValues.isEmpty()){
            StaticThreadLogger::warning("FlowControl::onReadyToRender","Will force set hand calibration to " + handCalibValues.join("|"));
            map[RRS::PacketDebugControl::ENABLE_FORCE_HAND_CALIB] = handCalibValues.join("|");
        }
        if (enablePacketPrintout){
            StaticThreadLogger::warning("FlowControl::onReadyToRender","Enablign RRS Received Packet Printout");
            map[RRS::PacketDebugControl::ENABLE_RRS_PACKET_PRINT] = "true";
        }
        p.setPayloadField(RRS::PacketDebugControl::JSON_DICT_FIELD,map);
        renderServerClient.sendPacket(p);
    }

    // And now we hide the render window.
    this->hideRenderWindow();

    // We can now enable the rendering of the waitscreen. No message.
    this->renderWaitScreen("");

}

void FlowControl::resetCalibrationHistory(){
    calibrationHistory.reset();
}

void FlowControl::renderWaitScreen(const QString &message, bool renderAsCornerTargets){
    RenderServerPacket waitScreen;
    waitScreen.setPacketType(RRS::PacketType::TYPE_WAIT_MSG);
    waitScreen.setPayloadField(RRS::PacketWaitMsg::MSG,message);
    waitScreen.setPayloadField(RRS::PacketWaitMsg::RENDER_BORDERS,renderAsCornerTargets);
    renderServerClient.sendPacket(waitScreen);
}

void FlowControl::closeApplication(){
    StaticThreadLogger::kill();
    renderServerClient.closeRenderServer();
}


bool FlowControl::isRenderServerWorking() const{
    return renderServerClient.isRenderServerWorking();
}


////////////////////////////// AUXILIARY FUNCTIONS ///////////////////////////////////////
void FlowControl::keyboardKeyPressed(int key){

    if (studyControl.getStudyPhase() == StudyControl::SS_EVAL){
        QString phase = "Eval";
        switch (key){
        case Qt::Key_S:
            studyControl.sendInStudyCommand(StudyControl::InStudyCommand::ISC_BINDING_SAME);
            break;
        case Qt::Key_D:
            studyControl.sendInStudyCommand(StudyControl::InStudyCommand::ISC_BINDING_DIFF);
            break;
        case Qt::Key_Escape:
            studyControl.sendInStudyCommand(StudyControl::InStudyCommand::ISC_ABORT);
            break;
        case Qt::Key_G:
            studyControl.sendInStudyCommand(StudyControl::InStudyCommand::ISC_CONTINUE);
            break;
        default:
            StaticThreadLogger::warning("FlowControl::keyboardKeyPressed","The following key was pressed but the in study command was not sent as the key was not recognized: " + QString::number(key) + " in state " + phase);
            break;
        }
    }
    else if (studyControl.getStudyPhase() == StudyControl::SS_EXAMPLE){
        QString phase = "Example";
        switch (key){
        case Qt::Key_N:
            studyControl.nextExampleSlide();
            break;
        case Qt::Key_Escape:
            studyControl.sendInStudyCommand(StudyControl::ISC_ABORT);
            break;
        default:
            StaticThreadLogger::warning("FlowControl::keyboardKeyPressed","The following key was pressed but the in study command was not sent as the key was not recognized: " + QString::number(key) + " in state " + phase);
            break;
        }
    }
    else if (studyControl.getStudyPhase() == StudyControl::SS_EXPLAIN){
        QString phase = "Explanation";
        switch (key){
        case Qt::Key_N:
            studyControl.nextExplanationSlide();
            break;
        case Qt::Key_B:
            studyControl.previousExplanationSlide();
            break;
        case Qt::Key_Escape:
            studyControl.sendInStudyCommand(StudyControl::ISC_ABORT);
            break;
        default:
            StaticThreadLogger::warning("FlowControl::keyboardKeyPressed","The following key was pressed but the in study command was not sent as the key was not recognized: " + QString::number(key) + " in state " + phase);
            break;
        }
    }
    else {
        StaticThreadLogger::warning("FlowControl::keyboardKeyPressed","The following key was pressed but the in study command was not sent as no study is running");
    }

}

bool FlowControl::isExperimentEndOk() const{
    return (studyControl.getStudyEndStatus() == StudyControl::SES_OK);
}

////////////////////////////// FLOW CONTROL FUNCTIONS ///////////////////////////////////////
void FlowControl::onStudyEndProcessFinished(){
    // We are done processing we notify the front end.
    emit FlowControl::studyEndProcessingDone();
}

void FlowControl::requestStudyData() {
    studyControl.requestStudyData();
}

void FlowControl::onNewControlPacketAvailableFromStudy(){
    renderServerClient.sendPacket(studyControl.getControlPacket());
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
    p.setPacketType(RRS::PacketType::TYPE_HAND_CALIB_CONTROL);

    switch (command) {
    case HAND_CALIB_END:
        p.setPayloadField(RRS::PacketHandCalibrationControl::HORIZONTAL,false);
        p.setPayloadField(RRS::PacketHandCalibrationControl::VERTICAL,false);
        break;
    case HAND_CALIB_START_H:
        p.setPayloadField(RRS::PacketHandCalibrationControl::HORIZONTAL,true);
        p.setPayloadField(RRS::PacketHandCalibrationControl::VERTICAL,false);
        p.setPayloadField(RRS::PacketHandCalibrationControl::HANDS,hand_sel);
        break;
    case HAND_CALIB_START_V:
        p.setPayloadField(RRS::PacketHandCalibrationControl::HORIZONTAL,false);
        p.setPayloadField(RRS::PacketHandCalibrationControl::VERTICAL,true);
        p.setPayloadField(RRS::PacketHandCalibrationControl::HANDS,hand_sel);
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

    QString strmode = "2D";
    if (mode3D) strmode = "3D";
    StaticThreadLogger::log("FlowControl::calibrateEyeTracker","Calibrating the eyetracker: " + strmode);

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
    calibrationValidationParameters[VMDC::CalibrationAttemptFields::VALIDATION_POINT_ACCEPTANCE_THRESHOLD] = validation_point_acceptance_threshold;

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

    calibrationManager.startCalibration(mode3D,
                                        calibrationValidationParameters,
                                        coefficient_file);

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

    // The calibration target locations need to be there, as there are checked with QML

    // We add the resolution as it is required for plotting the calibration data.
    QSize s = calibrationManager.getResolution();
    map["W"] = s.width();
    map["H"] = s.height();

    return map;

}

void FlowControl::storeCalibrationHistoryAsFailedCalibration() {

    // Here we must do the following
    // 1) Make sure that the destination directory exists.

    QDir dir(".");
    dir.mkdir(Globals::Paths::FAILED_CALIBRATION_DIR);
    QDir dirinfo(Globals::Paths::FAILED_CALIBRATION_DIR);
    if (!dirinfo.exists()){
        StaticThreadLogger::error("FlowControl::storeCalibrationHistoryAsFailedCalibration","Failed in creating the failed calibration directory");
        return;
    }

    QString filename_base = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    QString json_basename = filename_base + + ".json";
    QString tar_basename  = filename_base + + ".tar.gz";

    QString filename = Globals::Paths::FAILED_CALIBRATION_DIR + "/" + json_basename;
    QString filename_tar = Globals::Paths::FAILED_CALIBRATION_DIR + "/" + tar_basename;

    // 2) Stored the failed calibration with a file name indicative of the date in that directory.
    QVariantMap history = calibrationHistory.getHistory();
    if (!Globals::SaveVariantMapToJSONFile(filename,history,true)){
        StaticThreadLogger::error("FlowControl::storeCalibrationHistoryAsFailedCalibration","Failed in storing failed calibration history at: '" + filename + "'");
        return;
    }

    QFileInfo info(filename);

    // 3) Compress the file so that it doesn't take up space.
    QStringList arguments;
    arguments << "-c";
    arguments << "-z";
    arguments << "-f";
    arguments << tar_basename;
    arguments << json_basename;

    //qDebug() << "jsonFile" << jsonFile << "zip" << zipfile;

    QProcess tar;
    tar.setWorkingDirectory(info.absolutePath());
    tar.start(Globals::Paths::TAR_EXE,arguments);
    tar.waitForFinished();

    tar.exitCode();
    if (!QFile().exists(filename_tar)){
        StaticThreadLogger::error("FlowControl::storeCalibrationHistoryAsFailedCalibration","Failed in compressing failed calibration history at: '" + filename_tar + "'");
        return;
    }

    // 4) Delete the json file
    QFile(filename).remove();

    // 5) Clear calibration history.
    calibrationHistory.reset();

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

    // Forcing the valid eye to the calibration selected eye.
    study_config[VMDC::StudyParameter::VALID_EYE] = calibrationManager.getRecommendedEye();
    QString studyName;

    // We now set the study
    switch (study_config.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toInt()){

    case Globals::StudyConfiguration::INDEX_BINDING_BC:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING BINDING BC EXPERIMENT");
        studyName = VMDC::Study::BINDING_BC;
        break;
    case Globals::StudyConfiguration::INDEX_BINDING_UC:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING BINDING UC EXPERIMENT");
        studyName = VMDC::Study::BINDING_UC;
        break;
    case Globals::StudyConfiguration::INDEX_NBACK:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING NBACK SLOW DEFAULT VERSION");
        study_config[VMDC::StudyParameter::WAIT_MESSAGE] = configuration->getString(Globals::Share::NBACK_WAIT_MSG);
        studyName = VMDC::Study::NBACK;
        break;
    case Globals::StudyConfiguration::INDEX_GONOGO:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING GO - NO GO");
        studyName = VMDC::Study::GONOGO;
        break;
    case Globals::StudyConfiguration::INDEX_NBACKVS:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING N BACK VS");
        study_config[VMDC::StudyParameter::WAIT_MESSAGE] = configuration->getString(Globals::Share::NBACK_WAIT_MSG);
        studyName = VMDC::Study::NBACKVS;
        break;
    case Globals::StudyConfiguration::INDEX_GNG_SPHERE:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING GO - NO GO SPHERES");
        studyName = VMDC::Study::GONOGO_SPHERE;
        break;
    case Globals::StudyConfiguration::INDEX_PASSBALL:
        StaticThreadLogger::log("FlowControl::startNewExperiment","STARTING PASSBALL");
        studyName = VMDC::Study::PASSBALL;
        break;
    default:
        StaticThreadLogger::error("FlowControl::startNewExperiment","Unknown experiment was selected " + study_config.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toString());
        return false;
    }

    studyControl.startStudy(configuration->getString(Globals::Share::PATIENT_DIRECTORY),
                            configuration->getString(Globals::Share::PATIENT_STUDY_FILE),
                            study_config,studyName);

    // We add the calibration history as this function is called once the calibration is approved.
    studyControl.setCalibrationValidationData(calibrationHistory.getHistory());


    return true;
}

void FlowControl::startStudyEvaluationPhase(){
    studyControl.startEvaluation();
}

void FlowControl::startStudyExamplePhase(){
    studyControl.startExamplesPhase();
}

void FlowControl::onStudyEnd(){

    // If there was a hand calibration result, we need to remove it.
    configuration->removeKey(Globals::Share::HAND_CALIB_RES);

    StudyControl::StudyEndStatus ses = studyControl.getStudyEndStatus();

    QString message_prefix = "EXPERIMENT";
    bool all_good = false;
    if (studyControl.isStudyInDataTransfer()){
        message_prefix = "STUDY DATA RECEPTION";
    }

    switch (ses){
    case StudyControl::SES_ABORTED:
        StaticThreadLogger::log("FlowControl::onStudyEnd",message_prefix +  " aborted");
        break;
    case StudyControl::SES_FAILED:
        StaticThreadLogger::error("FlowControl::onStudyEnd",message_prefix + " was ended by failure");
        break;
    case StudyControl::SES_OK:
        all_good = true;
        StaticThreadLogger::log("FlowControl::onStudyEnd",message_prefix + " Finshed Successfully");
        break;
    }

    if (!studyControl.isStudyInDataTransfer()){
        // We need to tell the UI that the study has finished so that it can put the wait screen up and request the study data.
        emit FlowControl::experimentHasFinished();
    }
    else {
        // This is the second time the study end came as the SS_NONE means we are done. If all is good, then we start file processing.
        // Other wise we need to notify the UI that something went wrong with the data transfer.
        if (all_good){
            // We get the files for this experiment and set them.
            QStringList files = studyControl.getDataFilesLocation();
            studyEndProcessor.setFileToProcess(files.first(),files.last());
            studyEndProcessor.start();
        }
        else {
            // This is the signal the UI is expecting. However it must check the experiment state in order to make sure all is good.
            emit FlowControl::studyEndProcessingDone();
        }
    }

}

///////////////////////////////////////////////////////////////////
FlowControl::~FlowControl(){

}

