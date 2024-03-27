#include "flowcontrol.h"

FlowControl::FlowControl(QObject *parent, LoaderFlowComm *c) : QObject(parent)
{

    // Intializing the communication pointer.
    comm = c;

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

void FlowControl::printLastCalibrationPacketReceived(){
    this->calibrationManager.debugPrintLastCalibrationPacket();
}

bool FlowControl::isSkipETCheckEnabled() {
    bool ans = DBUGBOOL(Debug::Options::SKIP_ET_CHECK);
    if (ans){
        StaticThreadLogger::warning("FlowControl::isSkipETCheckEnabled","DBUG Option to SKIP ET Check is ENABLED");
    }
    return ans;
}

void FlowControl::startRenderServerAndSetWindowID(WId winID){
    mainWindowID = winID;
    // The redner server client can only be started once we have the window ID.
    QString location = Globals::RemoteRenderServerParameters::DIR + "/" + Globals::RemoteRenderServerParameters::EXE;
    if (DBUGBOOL(Debug::Options::NO_RRS)) {
        StaticThreadLogger::warning("startRenderServerAndSetWindowID","STARTING APP WITH NO RRS");
        return;
    }
    renderServerClient.startRenderServer(location,winID,DBUGBOOL(Debug::Options::NO_RRS_KILL));
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

            QString override_packet = DBUGSTR(Debug::Options::CALIB_PACKET_OVERRIDE);
            if (override_packet != ""){
                StaticThreadLogger::log("CalibrationManager::debugLoadFixedCalibrationParameters","[DEBUG] Loading override calibration packet: " + override_packet);
                QString temperror;
                QVariantMap payload = Globals::LoadJSONFileToVariantMap(override_packet,&temperror);
                RenderServerPacket p;
                p.setPacketType(RRS::PacketType::TYPE_CALIB_CONTROL);
                p.setFullPayload(payload);
                calibrationManager.processCalibrationData(p);
            }
            else {
                calibrationManager.processCalibrationData(packet);
            }

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
        comm->configuration()->addKeyValuePair(Globals::Share::HAND_CALIB_RES,result);

        // If we go the hand calibration packet, we can effectively move on.
        emit FlowControl::handCalibrationDone();

    }
    else if (packet.getType() == RRS::PacketType::TYPE_LOG_LOCATION){
        // The return of the log location packet brings about the RRS version of the server and the eye tracking key.
        QString version = packet.getPayloadField(RRS::PacketLogLocation::VERSION).toString();
        QString hmdkey  = packet.getPayloadField(RRS::PacketLogLocation::EYETRACKER).toString();
        StaticThreadLogger::log("FlowControl::onNewPacketArrived","Receieved log location response. RRS Version: " + version + " - " + hmdkey);
        QVariantMap info; info[Globals::FCL::HMD_KEY_RECEIVED] = hmdkey;
        this->HMDKey = hmdkey;
        emit FlowControl::notifyLoader(info);
    }
    else if (packet.getType() == RRS::PacketType::TYPE_STUDY_CONTROL ) {

        // These packets are processed by study control
        studyControl.receiveRenderServerPacket(packet);

    }
    else if (packet.getType() == RRS::PacketType::TYPE_FREQ_UPDATE){
        QVariantMap notification;
        notification[Globals::FCL::UPDATE_AVG_FREQ] = packet.getPayloadField(RRS::PacketFreqUpdate::AVG).toReal();
        notification[Globals::FCL::UPDATE_SAMP_FREQ] = packet.getPayloadField(RRS::PacketFreqUpdate::FRQ).toReal();
        notification[Globals::FCL::UPDATE_MAX_FREQ] = packet.getPayloadField(RRS::PacketFreqUpdate::MAX).toReal();
        emit FlowControl::notifyLoader(notification);
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

    // At this point we check on the number of RRS.exe that are actually running.
    ProcessRecognizer pr;
    if (!pr.refreshProcessList()){
        StaticThreadLogger::error("FlowControl::onReadyToRender","Failed to generate process list. Reasons: \n-> " + pr.getErrors().join("\n-> "));
        StaticThreadLogger::error("FlowControl::onReadyToRender","Printing last output:\n" + pr.getLastTaskTable());
        return;
    }

    qint32 ninstances = pr.getNumberOfInstancesRunningOf(Globals::RemoteRenderServerParameters::EXE,false);
    //qint32 ninstances = pr.getNumberOfInstancesRunningOf("Chrome.exe");

    if (ninstances < 0){
        StaticThreadLogger::error("FlowControl::onReadyToRender","Failed to find remote render server executable. Reasons: \n-> " + pr.getErrors().join("\n-> ") + "\nRetrying in language agnostic manner");
        ninstances = pr.getNumberOfInstancesRunningOf(Globals::RemoteRenderServerParameters::EXE,true);
        if (ninstances < 0){
            StaticThreadLogger::error("FlowControl::onReadyToRender","Failed to find remote render server executable even though search was language agnostic. Reasons: \n-> " + pr.getErrors().join("\n-> ") + "\nRetrying in language agnostic manner");
            StaticThreadLogger::error("FlowControl::onReadyToRender","Printing process table:\n" + pr.getLastTaskTable());
            if (DBUGBOOL(Debug::Options::DISALBE_RRS_FORCE_QUIT)) StaticThreadLogger::warning("FlowControl::onReadyToRender","Disabled RRS Force Quit");
            else emit checkOnRRSFailed();
            return;
        }
    }

    if (ninstances > 1){
        StaticThreadLogger::error("FlowControl::onReadyToRender","Found: " + QString::number(ninstances) + " of remote render server executable. Printing TaskList:\n " + pr.getLastTaskTable());
        StaticThreadLogger::error("FlowControl::onReadyToRender","Printing process table:\n" + pr.getLastTaskTable());
        if (DBUGBOOL(Debug::Options::DISALBE_RRS_FORCE_QUIT)) StaticThreadLogger::warning("FlowControl::onReadyToRender","Disabled RRS Force Quit");
        else emit checkOnRRSFailed();
        return;
    }

    if (ninstances == 0) {
        StaticThreadLogger::error("FlowControl::onReadyToRender","Remote render server executable was not found to be running. Printing TaskList:\n " + pr.getLastTaskTable());
        StaticThreadLogger::error("FlowControl::onReadyToRender","Printing process table:\n" + pr.getLastTaskTable());
        if (DBUGBOOL(Debug::Options::DISALBE_RRS_FORCE_QUIT)) StaticThreadLogger::warning("FlowControl::onReadyToRender","Disabled RRS Force Quit");
        else emit checkOnRRSFailed();
        return;
    }

    StaticThreadLogger::log("FlowControl::onReadyToRender","Success. Only one instance of remote render server is running");
    StaticThreadLogger::log("FlowControl::onReadyToRender","Printing process table:\n" + pr.getLastTaskTable());

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

    // qDebug() << "Keyboard Pressed with study phase" << studyControl.getStudyPhase();

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
    else if (studyControl.getStudyPhase() == StudyControl::SS_NONE){
        // In this case all we need to do is to property set the abort study state and emit the proper signal.
        this->studyControl.forceStudyEndStatusToAborted();
        emit FlowControl::experimentHasFinished();
    }
    else {
        StaticThreadLogger::warning("FlowControl::keyboardKeyPressed","The following key was pressed but the in study command was not sent as no study is running");
    }

}

bool FlowControl::isExperimentEndOk() const{
    return (studyControl.getStudyEndStatus() == StudyControl::SES_OK);
}

/**
 * NOTE: For now the asking if a task requires hand calibration and asking if it's 3D is the same question.
 * It might not be in the future so that is why the functions are separate.
 */

bool FlowControl::doesTaskRequireHandCalibration(const QString &task_code) const {
    if (task_code == VMDC::Study::SPHERES) return true;
    else if (task_code == VMDC::Study::SPHERES_VS) return true;
    return false;
}

bool FlowControl::isTask3D(const QString &task_code) const {
    if (task_code == VMDC::Study::SPHERES) return true;
    else if (task_code == VMDC::Study::SPHERES_VS) return true;
    return false;
}


////////////////////////////// FLOW CONTROL FUNCTIONS ///////////////////////////////////////
void FlowControl::onStudyEndProcessFinished(){

    // We need to update the DB. The information for updating is present in the last QCI File that was created.
    QVariantMap qciData = this->studyEndProcessor.getLastQCIData();
    QString evalID = qciData.value(Globals::QCIFields::EVALUATION_ID).toString();
    QString taskFileName = qciData.value(Globals::QCIFields::TARBALL_FILE).toString();
    qreal qci = qciData.value(Globals::QCIFields::QCI).toReal();
    bool qci_ok = qciData.value(Globals::QCIFields::QCI).toBool();

    if (!this->comm->db()->updateEvaluation(evalID,taskFileName,qci,qci_ok,false)){
        StaticThreadLogger::error("FlowControl::onStudyEndProcessFinished","Failed to update evaluation. Reason: " + this->comm->db()->getError());
    }

    // We are done processing we notify the front end.
    emit FlowControl::studyEndProcessingDone();
}

void FlowControl::requestStudyData() {
    studyControl.requestStudyData();
}

void FlowControl::onNewControlPacketAvailableFromStudy(){
    renderServerClient.sendPacket(studyControl.getControlPacket());
}

void FlowControl::newEvaluation(const QString &evaluationID, const QString &clinician, const QString &selectedProtocol){

    // The subject should be loaded in the shared preferences.
    QString subject    = comm->configuration()->getString(Globals::Share::PATIENT_UID);
    QString evaluator  = comm->configuration()->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR);
    QString instance_prefix = comm->configuration()->getString(Globals::VMConfig::INSTITUTION_ID)
            + "_" + comm->configuration()->getString(Globals::VMConfig::INSTANCE_NUMBER);


    QString newEvaluationID = comm->db()->createNewEvaluation(subject,clinician,evaluator,selectedProtocol,instance_prefix,evaluationID);

    // The system is now ready to start with this evaluation.
    comm->configuration()->addKeyValuePair(Globals::Share::SELECTED_EVALUATION,newEvaluationID);

}

QVariantList FlowControl::setupCurrentTaskList() {

    // First we must get the current evaluation ID
    QString evaluationID = comm->configuration()->getString(Globals::Share::SELECTED_EVALUATION);
    this->currentEvaluationTaskList.clear();

    // Each of the objects in this list contains a task type and file.
    this->currentEvaluationTaskList = comm->db()->getRemainingTasksForEvaluation(evaluationID);
    return this->currentEvaluationTaskList;

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
                                        coefficient_file,
                                        this->HMDKey);

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

        renderWaitScreen("",true);

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

bool FlowControl::startTask(qint32 taskIndexInCurrentList){

    // So we need to get task type and final output file.
    QString task_code      = this->currentEvaluationTaskList.at(taskIndexInCurrentList).toMap().value("type").toString();
    QString task_filename  = this->currentEvaluationTaskList.at(taskIndexInCurrentList).toMap().value("file").toString();

    // Now we need to get the evaluation
    QString evalID             = this->comm->configuration()->getString(Globals::Share::SELECTED_EVALUATION);
    QVariantMap evalDefinition = this->comm->db()->getEvaluation(evalID);
    if (evalDefinition.empty()){
        StaticThreadLogger::error("FlowControl::startTask", "The evaluation with id '" + evalID + " for task of type '" + task_code + "' could not be found");
        return false;
    }

    // We put togethere the FULL filename.
    QString task_directory = this->comm->configuration()->getString(Globals::Share::PATIENT_DIRECTORY);
    task_filename = QDir(task_directory).absolutePath() + "/" + task_filename;

    // Now we load the default configuration depending on the task type.
    QVariantMap study_config = DefaultConfiguration(task_code);

    if (study_config.empty()){
        StaticThreadLogger::error("FlowControl::startTask", "The evaluation with id '" + evalID + " has a task of unknown origin '" + task_code + "'");
        return false;
    }

    // Forcing the valid eye to the calibration selected eye.
    study_config[VMDC::StudyParameter::VALID_EYE] = calibrationManager.getRecommendedEye();

    StaticThreadLogger::log("FlowControl::startTask","STARTING TASK '" + task_code + "'");

    QVariantMap processingParameters = comm->db()->getProcessingParameters();

    ViewMindDataContainer vmdc;
    if (!initDataContainerForNewTask(evalDefinition,&vmdc)){
        StaticThreadLogger::error("FlowControl::startTask", "Failed initializaing VMDC for eval '" + evalID + " and task '" + task_code + "'");
        return false;
    }

    studyControl.startTask(task_directory,
                           task_filename,
                           study_config,
                           processingParameters,
                           task_code,vmdc);

    // We add the calibration history as this function is called once the calibration is approved.
    studyControl.setCalibrationValidationData(calibrationHistory.getHistory());


    return true;
}


bool FlowControl::initDataContainerForNewTask(const QVariantMap &evaluationDefintion, ViewMindDataContainer *rdc){

    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString hour = QDateTime::currentDateTime().toString("HH:mm:ss");

    QString medic = evaluationDefintion.value(LocalDB::EVAL_CLINICIAN).toString();

    // Creating the metadata.
    QVariantMap metadata;
    metadata.insert(VMDC::MetadataField::DATE,date);
    metadata.insert(VMDC::MetadataField::HOUR,hour);
    metadata.insert(VMDC::MetadataField::INSTITUTION_ID,comm->configuration()->getString(Globals::VMConfig::INSTITUTION_ID));
    metadata.insert(VMDC::MetadataField::INSTITUTION_INSTANCE,comm->configuration()->getString(Globals::VMConfig::INSTANCE_NUMBER));
    metadata.insert(VMDC::MetadataField::INSTITUTION_NAME,comm->configuration()->getString(Globals::VMConfig::INSTITUTION_NAME));
    metadata.insert(VMDC::MetadataField::PROC_PARAMETER_KEY,comm->configuration()->getString(Globals::Share::API_PARAMETER_KEY));
    metadata.insert(VMDC::MetadataField::PROTOCOL,evaluationDefintion.value(LocalDB::EVAL_PROTOCOL).toString());
    metadata.insert(VMDC::MetadataField::DISCARD_REASON,"");
    metadata.insert(VMDC::MetadataField::COMMENTS,"");
    metadata.insert(VMDC::MetadataField::APP_VERSION,Globals::Share::EXPERIMENTER_VERSION_NUMBER);
    metadata.insert(VMDC::MetadataField::EVALUATION_ID,evaluationDefintion.value(LocalDB::EVAL_ID).toString());
    metadata.insert(VMDC::MetadataField::EVALUATION_TYPE,evaluationDefintion.value(LocalDB::EVAL_TYPE).toString());

    // Creating the subject data
    QVariantMap subject_data;
    subject_data.insert(VMDC::SubjectField::BIRTH_COUNTRY,comm->db()->getSubjectFieldValue(comm->configuration()->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_BIRTHCOUNTRY));
    subject_data.insert(VMDC::SubjectField::BIRTH_DATE,comm->db()->getSubjectFieldValue(comm->configuration()->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_BIRTHDATE));
    subject_data.insert(VMDC::SubjectField::GENDER,comm->db()->getSubjectFieldValue(comm->configuration()->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_GENDER));
    subject_data.insert(VMDC::SubjectField::INSTITUTION_PROVIDED_ID,comm->db()->getSubjectFieldValue(comm->configuration()->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_INSTITUTION_ID));
    subject_data.insert(VMDC::SubjectField::LASTNAME,comm->db()->getSubjectFieldValue(comm->configuration()->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_LASTNAME));
    subject_data.insert(VMDC::SubjectField::LOCAL_ID,comm->configuration()->getString(Globals::Share::PATIENT_UID));
    subject_data.insert(VMDC::SubjectField::NAME,comm->db()->getSubjectFieldValue(comm->configuration()->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_NAME));
    subject_data.insert(VMDC::SubjectField::YEARS_FORMATION,comm->db()->getSubjectFieldValue(comm->configuration()->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_YEARS_FORMATION));
    subject_data.insert(VMDC::SubjectField::EMAIL,comm->db()->getSubjectFieldValue(comm->configuration()->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_EMAIL));

    // Setting the evaluator info. We do it here becuase it is required for the data file comparisons.
    QVariantMap evaluator;
    //qDebug() << "Creating study file for evaluator" << comm->configuration()->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR);
    QVariantMap evaluator_local_data = comm->db()->getEvaluatorData(comm->configuration()->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR));
    //Debug::prettpPrintQVariantMap(evaluator_local_data);
    evaluator.insert(VMDC::AppUserField::EMAIL,evaluator_local_data.value(LocalDB::APPUSER_EMAIL));
    evaluator.insert(VMDC::AppUserField::NAME,evaluator_local_data.value(LocalDB::APPUSER_NAME));
    evaluator.insert(VMDC::AppUserField::LASTNAME,evaluator_local_data.value(LocalDB::APPUSER_LASTNAME));
    evaluator.insert(VMDC::AppUserField::LOCAL_ID,"");
    evaluator.insert(VMDC::AppUserField::VIEWMIND_ID,"");

    // Setting the selected doctor info.
    QVariantMap medic_to_store;
    QVariantMap medic_local_data = comm->db()->getMedicData(medic);
    if (medic_local_data.empty()){
        StaticThreadLogger::error("FlowControl::initDataContainerForNewTask","Failed to retrieve medic local data: " + medic);
        return false;
    }
    medic_to_store.insert(VMDC::AppUserField::EMAIL,medic_local_data.value(LocalDB::APPUSER_EMAIL));
    medic_to_store.insert(VMDC::AppUserField::NAME,medic_local_data.value(LocalDB::APPUSER_NAME));
    medic_to_store.insert(VMDC::AppUserField::LASTNAME,medic_local_data.value(LocalDB::APPUSER_LASTNAME));
    medic_to_store.insert(VMDC::AppUserField::LOCAL_ID,"");
    medic_to_store.insert(VMDC::AppUserField::VIEWMIND_ID,medic_local_data.value(LocalDB::APPUSER_VIEWMIND_ID));

    // Need to insert them as in in the file.
    QVariantMap pp = comm->db()->getProcessingParameters();

    // Check if we need to add the hand calibration data.
    if (comm->configuration()->containsKeyword(Globals::Share::HAND_CALIB_RES)){
        pp.insert(VMDC::ProcessingParameter::HAND_CALIB_RESULTS,comm->configuration()->getVariant(Globals::Share::HAND_CALIB_RES));
    }

    // Computing the actual maximum dispersion to use.
    qreal reference_for_md = qMax(pp.value(VMDC::ProcessingParameter::RESOLUTION_WIDTH).toReal(),pp.value(VMDC::ProcessingParameter::RESOLUTION_HEIGHT).toReal());
    qreal md_percent = pp.value(VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW).toReal();
    qint32 md_px = qRound(md_percent*reference_for_md/100.0);
    //qDebug() << "Setting the MD from" << reference_for_md << md_percent << " to " << md_px;
    pp.insert(VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW_PX,md_px);

    // Computing the effective minimum fixation size to use.
    qreal sample_frequency = pp.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();
    qreal sample_period = 1/sample_frequency;
    qreal minimum_fixation_length = qRound(sample_period*1000*MINIMUM_NUMBER_OF_DATAPOINTS_IN_FIXATION);
    pp.insert(VMDC::ProcessingParameter::MIN_FIXATION_DURATION,minimum_fixation_length);
    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
        QString dbug = "DBUG: Effective Minimum Fixation Computed At " + QString::number(minimum_fixation_length);
        qDebug() << dbug;
        StaticThreadLogger::warning("FlowControl::initDataContainerForNewTask",dbug);
    }
    // These values will be properly set with the end study data.
    pp[VMDC::ProcessingParameter::RESOLUTION_HEIGHT] = 0;
    pp[VMDC::ProcessingParameter::RESOLUTION_WIDTH]  = 0;

    // Setting the QC Parameters that will be used.
    QVariantMap qc = comm->db()->getQCParameters();

    // We store this information the raw data container and leave it ready for the study to start.

    rdc->setQCParameters(qc);

    if (!rdc->setSubjectData(subject_data)){
        StaticThreadLogger::error("FlowControl::initDataContainerForNewTask","Failed setting subject data to new study. Reason: " + rdc->getError());
        return false;
    }
    if (!rdc->setMetadata(metadata)){
        StaticThreadLogger::error("FlowControl::initDataContainerForNewTask","Failed setting study metadata to new study. Reason: " + rdc->getError());
        return false;
    }

    if (!rdc->setApplicationUserData(VMDC::AppUserType::EVALUATOR,evaluator)){
        StaticThreadLogger::error("FlowControl::initDataContainerForNewTask","Failed to store evaluator data: " + rdc->getError());
        return false;
    }

    if (!rdc->setApplicationUserData(VMDC::AppUserType::MEDIC,medic_to_store)){
        StaticThreadLogger::error("FlowControl::initDataContainerForNewTask","Failed to store medic data: " + rdc->getError());
        return false;
    }

    if (!rdc->setProcessingParameters(pp)){
        StaticThreadLogger::error("FlowControl::initDataContainerForNewTask","Failed to store processing parameters: " + rdc->getError());
        return false;
    }

    // We add the system specifications to the file.
    rdc->setSystemSpecs(comm->hw()->getHardwareSpecsAsVariantMap());

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
    comm->configuration()->removeKey(Globals::Share::HAND_CALIB_RES);

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

