#include "studycontrol.h"

QMap<QString,int> StudyControl::NumberOfExplanationSlides;

StudyControl::StudyControl() {
    this->studyState = SS_NONE;
    this->studyType  = ST_NOT_SET;
    this->expectingAbortACK = false;
    this->studyInDataTransfer = false;
}

/////////////////////////////////////////////// GETTERS //////////////////////////////////////////////////////

RenderServerPacket StudyControl::getControlPacket() const{
    return this->rspacket;
}

StudyControl::StudyEndStatus StudyControl::getStudyEndStatus() const {
    return this->studyEndStatus;
}

StudyControl::StudyType StudyControl::getCurrentStudyType() const {
    return this->studyType;
}

StudyControl::StudyState StudyControl::getStudyPhase() const{
    return this->studyState;
}

bool StudyControl::isStudyInDataTransfer() const {
    return this->studyInDataTransfer;
}

////////////////////////////////////////// CONTROL FUNCTIONS //////////////////////////////////////////////////

void StudyControl::startStudy(const QString &workingDir, const QString &studyFile, const QVariantMap &studyConfig, const QString &studyName){

    // Storing the variables.
    workingDirectory = workingDir;
    fullPathCurrentStudyFile = studyFile;
    expectingAbortACK = false;
    studyInDataTransfer = false;

    bool shortStudies = false;
    if (DBUGBOOL(Debug::Options::SHORT_STUDIES)){
        StaticThreadLogger::warning("StudyControl::startStudy","Short Studies Enabled");
        shortStudies = true;
    }

    // Now we create the configurator.
    StudyConfigurator *configurator = nullptr;
    if (studyName == VMDC::Study::NBACK){
        configurator = new NBackConfigurator();
        if (studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS).toInt() == 3){
            studyExplanationLanguageKey = STUDY_TEXT_KEY_NBACK_3;
        }
        else {
            studyExplanationLanguageKey = STUDY_TEXT_KEY_NBACK_4;
        }
        studyType = ST_2D;
    }
    else if (studyName == VMDC::Study::NBACKVS){
        configurator = new NBackConfigurator();
        studyExplanationLanguageKey = STUDY_TEXT_KEY_NBACKVS;
        studyType = ST_2D;
    }
    else if (studyName == VMDC::Study::BINDING_UC) {
        configurator = new BindingConfigurator(false,studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS).toInt());
        studyExplanationLanguageKey = STUDY_TEXT_KEY_BINDING_UC;
        studyType = ST_2D;
    }
    else if (studyName == VMDC::Study::BINDING_BC) {
        configurator = new BindingConfigurator(true,studyConfig.value(VMDC::StudyParameter::NUMBER_TARGETS).toInt());
        studyExplanationLanguageKey = STUDY_TEXT_KEY_BINDING_BC;
        studyType = ST_2D;
    }
    else if (studyName == VMDC::Study::GONOGO){
        configurator = new GNGConfigurator();
        studyExplanationLanguageKey = STUDY_TEXT_KEY_GONOGO;
        studyType = ST_2D;
    }
    else if (studyName == VMDC::Study::GONOGO_SPHERE){
        configurator = new GNGSpheresConfigurator();
        if (studyConfig.value(VMDC::StudyParameter::SPEED) == 2){
            studyExplanationLanguageKey = STUDY_TEXT_KEY_GONOGO_3D_VS;
        }
        else {
            studyExplanationLanguageKey = STUDY_TEXT_KEY_GONOGO_3D;
        }
        studyType = ST_3D;
    }
    else {
        StaticThreadLogger::error("StudyControl::startStudy","Unimplemented study type: " + studyName);
        emitFailState();
        return;
    }

    // We need to load the experiment file as it will contain the subject dat, the meta data already set and the processing parameters.
    if (QFile(fullPathCurrentStudyFile).exists()){

        if (DBUGBOOL(Debug::Options::DBUG_MSG)){
            StaticThreadLogger::log("StudyControl::startStudy","DBUG: Rawdata is being loaded from file: " + QFileInfo(fullPathCurrentStudyFile).absoluteFilePath());
        }

        if (!rawdata.loadFromJSONFile(fullPathCurrentStudyFile)){
            StaticThreadLogger::error("StudyControl::startStudy","Failed loading existing experiment file: "
                                      + fullPathCurrentStudyFile + "even tough it exists: " + rawdata.getError());
            delete configurator;
            emitFailState();
            return;
        }
    }
    else{
        StaticThreadLogger::error("StudyControl::startStudy","Set study file does not exist: " + fullPathCurrentStudyFile);
        delete configurator;
        emitFailState();
        return;
    }

    // We get the processing parameters.
    QVariantMap processingParameters = rawdata.getProcessingParameters();

    // Adding the default eye for the study to the study configuration.
    QVariantMap enhancedStudyConfig = studyConfig; // This is done so I don't have to change the function interface everywhere.
    enhancedStudyConfig[VMDC::StudyParameter::DEFAULT_EYE] = configurator->getDefaultStudyEye();

    // Last thing to do is to actually set the current study to the study type
    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
        QString dbug = "DBUG: Setting study on starting experiment of type '" + studyName + "'";
        qDebug() << dbug;
        StaticThreadLogger::warning("StudyControl::startStudy",dbug);
    }

    // Setting up the fixation computer.
    qreal sampleFrequency = processingParameters.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();
    qreal md_percent     =  processingParameters.value(VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW).toReal();

    // We need to create the configuration for the study.
    if (!configurator->createStudyConfiguration(studyConfig,sampleFrequency,md_percent,studyName,shortStudies,fullPathCurrentStudyFile)){
        StaticThreadLogger::error("StudyControl::startStudy","Failed in creating a configuration for study of type " + studyName + ". Reason: " + configurator->getError());
        delete configurator;
        emitFailState();
        return;
    }

    // After the configuration is created, we can now add the study to the raw data container.
    // This is because until the configuration is not created the study description contents are empty.
    if (!rawdata.addStudy(studyName,enhancedStudyConfig,configurator->getStudyDescriptionContents(),configurator->getVersionString())){
        StaticThreadLogger::error("StudyControl::startStudy","Adding the study type: " + rawdata.getError());
        delete configurator;
        emitFailState();
        return;
    }

    if (!rawdata.setCurrentStudy(studyName)){
        StaticThreadLogger::error("StudyControl::startStudy","Failed setting current study of " + studyName + ". Reason: " + rawdata.getError());
        delete configurator;
        emitFailState();
        return;
    }

    // We just need to send the study configuration to the server, in order to start the study.
    rspacket.resetForRX();
    rspacket.setPacketType(RRS::PacketType::TYPE_STUDY_CONTROL);
    rspacket.setPayloadField(RRS::PacketStudyControl::STUDY_CNF,configurator->getConfiguration());
    rspacket.setPayloadField(RRS::PacketStudyControl::CMD,RRS::CommandStudyControl::CMD_SETUP_STUDY);

    this->studyState = SS_EXPLAIN;
    this->studyEndStatus = SES_OK;
    this->currentStudyExplanationScreen = 0;

    // Notification that a new packet is available.
    emit StudyControl::newPacketAvailable();

    // Notification to render the first explanation screen.
    emitNewExplanationMessage();

}

void StudyControl::receiveRenderServerPacket(const RenderServerPacket &control){

    //StaticThreadLogger::log("StudyControl::receiveRenderServerPacket","Received a packet " + control.getType());
    //StaticThreadLogger::log("StudyControl::receiveRenderServerPacket",Debug::QVariantMapToString(control.getPayload()));

    if (control.getType() == RRS::PacketType::TYPE_STUDY_CONTROL){
        QString command = control.getPayloadField(RRS::PacketStudyControl::CMD).toString();

        if (expectingAbortACK){
            if (command != RRS::CommandStudyControl::CMD_ACK){
                StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Study was expecting ACK from abort request. Will do nothing");
                return;
            }
            else {
                expectingAbortACK = false;
                StaticThreadLogger::log("StudyControl::receiveRenderServerPacket","Received Abort ACK");
            }
            emitFailState(true);
            return;
        }

        if (this->studyState == SS_EXPLAIN){
            // We can receive either an acknowledge, an error
            if (command == RRS::CommandStudyControl::CMD_ACK){
                // All is good, we need to do nothing.
                StaticThreadLogger::log("StudyControl::receiveRenderServerPacket","Study start ACK received");
                return;
            }
            else if (command == RRS::CommandStudyControl::CMD_ERROR){
                // Something went wrong.We need to finish.
                StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Received ERROR for study start. Aborting");
                emitFailState();
                return;
            }
            else if (command == RRS::CommandStudyControl::CMD_STUDY_STATUS_UPDATES){
                // At the very beginning we can expect this command, asi it sets up the zero values for the whatever is printed below the unity window.
                emit StudyControl::updateStudyMessages(control.getPayloadField(RRS::PacketStudyControl::STATUS_UPDATE).toMap());
                return;
            }
            else {
                // Unexpected command. - We abort.
                StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Recieved unpexcted study control command while on explanation phase: " + command);
                emitFailState();
                return;
            }
        }
        else if (this->studyState == SS_EVAL) {
            if (command == RRS::CommandStudyControl::CMD_STUDY_STATUS_UPDATES) {
                //qDebug() << "Got Stats Updates For Evaluation" << control.getPayloadField(RRS::PacketStudyControl::STATUS_UPDATE).toMap();
                emit StudyControl::updateStudyMessages(control.getPayloadField(RRS::PacketStudyControl::STATUS_UPDATE).toMap());
                return;
            }
            else if (command == RRS::CommandStudyControl::CMD_STUDY_END){
                // The study has finalized. We need to setup the wait screen both in the app and in the HMD.
                this->studyState = SS_WAITING_FOR_STUDY_DATA;
                this->studyEndStatus = SES_OK; // This might change once we get the data. But up to this point it's all OK.
                emit StudyControl::studyEnd();
                return;
            }
        }
        else if (this->studyState == SS_WAITING_FOR_STUDY_DATA){

            // Finally we check that the command is the correct one.
            if (command != RRS::CommandStudyControl::CMD_STUDY_DATA_STORED){
                StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Study was expecting study data stored command, but got '" + command + "' instead");
                emitFailState();
                return;
            }

//            qDebug() << "Printing payload of study end command";
//            Debug::prettyPrintQVariantMap(control.getPayload());

            // First we check we got the file.
            QString study_data_filename = control.getPayloadField(RRS::PacketStudyControl::STUDY_DATA_FILE).toString();
            if (study_data_filename == ""){
                StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Study has ended but no file with stored study data is provided. Aborting");
                emitFailState();
                return;
            }

            // Now we check the file exists.
            if (!QFile(study_data_filename).exists()){
                StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Study has ended but no file with storoed study data is provided. Aborting");
                emitFailState();
                return;
            }

            StaticThreadLogger::log("StudyControl::receiveRenderServerPacket","Loading JSON Data ....");

            QString error = "";
            QVariantMap data = Globals::LoadJSONFileToVariantMap(study_data_filename,&error);
            if (error != ""){
                StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Unable to load study file '" + study_data_filename + "' for reading. Reason: " + error);
                emitFailState();
                return;
            }

            StaticThreadLogger::log("StudyControl::receiveRenderServerPacket","Processing loaded data ....");

            // Process and store the data.
            if (this->processAndStoreStudyData(data)){
                // If all is OK we delete the file.
                QFile(study_data_filename).remove();
            }
            else {
                StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Something went wrong while processing file '" + study_data_filename + "'. Not deleting");
            }

            StaticThreadLogger::log("StudyControl::receiveRenderServerPacket","Finished processing");

            return;

        }

        // If we got here it's an unexpected command Unexpected command. - We abort.
        StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Recieved unpexcted study control command " + command + " for the current study phase. Aborting");
        emitFailState();
        return;


    }
    else {
        StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Received packet of unexpected type: " + control.getType());
        return;
    }

}

bool StudyControl::processAndStoreStudyData(const QVariantMap &control) {

    QVariantList trialList = control.value(RRS::PacketStudyData::TRIAL_LIST_2D).toList();
    QVariantMap  studyData = control.value(RRS::PacketStudyData::ELEMENTS_3D).toMap();
    qint32 explanationDuration = control.value(RRS::PacketStudyData::EXPLAIN_LENGTH).toInt();
    qint32 exampleDuration = control.value(RRS::PacketStudyData::EXAMPLE_LENGTH).toInt();
    qint32 evalDuration = control.value(RRS::PacketStudyData::EVAL_LENGTH).toInt();
    qint32 pauseDuration = control.value(RRS::PacketStudyData::PAUSE_LEGTH).toInt();

    /**
     * The list of study data fields that need to be added as processing parameters.
     * The values are required for actually processing each study file. Whether they have content or not
     * will depend on the study.
     */
    QVariantMap addToPP;
    addToPP[RRS::PacketStudyData::FOV]          = VMDC::ProcessingParameter::FOV_3D;
    addToPP[RRS::PacketStudyData::MESH_FILES]   = VMDC::ProcessingParameter::MESH_FILES;
    addToPP[RRS::PacketStudyData::MESH_STRUCTS] = VMDC::ProcessingParameter::MESH_STRUCT;
    addToPP[RRS::PacketStudyData::ORIGIN_PTS]   = VMDC::ProcessingParameter::ORIGIN_POINTS;
    addToPP[RRS::PacketStudyData::SPHERE_R]     = VMDC::ProcessingParameter::SPHERE_RADIOUS;
    addToPP[RRS::PacketStudyData::RES_W]        = VMDC::ProcessingParameter::RESOLUTION_WIDTH;
    addToPP[RRS::PacketStudyData::RES_H]        = VMDC::ProcessingParameter::RESOLUTION_HEIGHT;

    // The hitboxes can be go no go hitboxes or NBAck. depending on the study.
    if ( (rawdata.getCurrentStudy() == VMDC::Study::NBACK) ||
         (rawdata.getCurrentStudy() == VMDC::Study::NBACKVS) ||
         (rawdata.getCurrentStudy() == VMDC::Study::NBACKRT) ){
        addToPP[RRS::PacketStudyData::HITBOXES] = VMDC::ProcessingParameter::NBACK_HITBOXES;
    }
    else if (rawdata.getCurrentStudy() == VMDC::Study::GONOGO){
        addToPP[RRS::PacketStudyData::HITBOXES] = VMDC::ProcessingParameter::GONOGO_HITBOXES;
    }

    // To do a bit of control and that is it.
    QString ndata_points = control.value(RRS::PacketStudyData::N_DP_DURING_EVAL).toString();
    StaticThreadLogger::log("StudyControl::receiveRenderServerPacket","Finalized evaluation. Number of datapoints gathered is: " + ndata_points);

    // First we set the processing parameter missing fields.
    QVariantMap pp = rawdata.getProcessingParameters();

    // We now add all fields.
    QStringList packet_keys = addToPP.keys();
    for (qint32 i = 0; i < packet_keys.size(); i++){
        QString packetKey = packet_keys.at(i);
        QString ppKey     = addToPP.value(packetKey).toString();
        pp[ppKey] = control.value(packetKey);
    }

    if (!rawdata.setProcessingParameters(pp)){
        StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Failed to set processing parameters in raw data. Reason: " + rawdata.getError());
        this->studyEndStatus = SES_FAILED;
        emit StudyControl::studyEnd();
        return false;
    }

    // We need to figure out if this is file we need to finalize.
    bool shouldBeFinalized = this->studyFinalizationLogic();

    // If the trial list field is a list that is NOT empty, this a 2D study file.
    if (trialList.size() > 0){
        // We have a 2D file.
        if (!rawdata.setFullTrialList(trialList,
                                      explanationDuration,exampleDuration,pauseDuration,evalDuration,
                                      evaluationStartTime)){
            StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Failed to set the full trial list in raw data. Reason: " + rawdata.getError());
            emitFailState();
            return false;
        }
    }
    else {
        // This should be a 3D study.
        if (!rawdata.setFullElement3D(studyData,
                                      exampleDuration,exampleDuration,pauseDuration,evalDuration,
                                      evaluationStartTime)){
            StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Failed to set the full 3d element data in raw data. Reason: " + rawdata.getError());
            emitFailState();
            return false;
        }
    }

    // Now that we are ready, then we mark the file as finalized if it is.
    if (shouldBeFinalized){
        rawdata.markFileAsFinalized();
    }

    // Now we save the data to hard disk.
    if (!saveDataToHardDisk()){
        StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Failed to save data to HD.");
        this->studyEndStatus = SES_FAILED;
    }

    this->studyState = SS_NONE;
    this->studyEndStatus = SES_OK;
    //this->studyEndStatus = SES_FAILED; // Was put here to test the data transfer error situation.
    this->studyType = ST_NOT_SET;
    emit StudyControl::studyEnd();
    return true;
}

void StudyControl::startEvaluation(){
    if (this->studyState != SS_EXAMPLE){
        StaticThreadLogger::error("StudyControl::startEvaluation","Will not send start evaluation command as current state is NOT example state.");
        return;
    }
    evaluationStartTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    rspacket.resetForRX();
    rspacket.setPacketType(RRS::PacketType::TYPE_STUDY_CONTROL);
    rspacket.setPayloadField(RRS::PacketStudyControl::CMD,RRS::CommandStudyControl::CMD_STUDY_START);
    emit StudyControl::newPacketAvailable();
    this->studyState = SS_EVAL;
}

void StudyControl::startExamplesPhase(){
    if (this->studyState != SS_EXPLAIN){
        StaticThreadLogger::error("StudyControl::startExamplesPhase","Will not send start examples command as current state is NOT explanation state.");
        return;
    }
    rspacket.resetForRX();
    rspacket.setPacketType(RRS::PacketType::TYPE_STUDY_CONTROL);
    rspacket.setPayloadField(RRS::PacketStudyControl::CMD,RRS::CommandStudyControl::CMD_START_EXAMPLE);
    emit StudyControl::newPacketAvailable();
    this->studyState = SS_EXAMPLE;
}

void StudyControl::nextExampleSlide(){
    if (this->studyState != SS_EXAMPLE){
        StaticThreadLogger::error("StudyControl::nextExampleSlide","Will not send next explanation command as current state is NOT example state.");
        return;
    }
    rspacket.resetForRX();
    rspacket.setPacketType(RRS::PacketType::TYPE_STUDY_CONTROL);
    rspacket.setPayloadField(RRS::PacketStudyControl::CMD,RRS::CommandStudyControl::CMD_NEXT_EXAMPLE);
    emit StudyControl::newPacketAvailable();
}

void StudyControl::previousExplanationSlide(){
    if (this->studyState != SS_EXPLAIN){
        StaticThreadLogger::error("StudyControl::previousExplanationSlide","Will not send previous explanation command as current state is NOT explanation state.");
        return;
    }
    rspacket.resetForRX();
    rspacket.setPacketType(RRS::PacketType::TYPE_STUDY_CONTROL);
    rspacket.setPayloadField(RRS::PacketStudyControl::CMD,RRS::CommandStudyControl::CMD_PREVIOUS_EXPLANATION);
    emit StudyControl::newPacketAvailable();

    // We do the index loop logic, and update the explanation message.
    currentStudyExplanationScreen--;
    if (currentStudyExplanationScreen < 0){
        currentStudyExplanationScreen = NumberOfExplanationSlides[studyExplanationLanguageKey]-1;
    }
    emitNewExplanationMessage();

}

void StudyControl::nextExplanationSlide(){
    if (this->studyState != SS_EXPLAIN){
        StaticThreadLogger::error("StudyControl::nextExplanationSlide","Will not send next explanation command as current state is NOT explanation state.");
        return;
    }
    rspacket.resetForRX();
    rspacket.setPacketType(RRS::PacketType::TYPE_STUDY_CONTROL);
    rspacket.setPayloadField(RRS::PacketStudyControl::CMD,RRS::CommandStudyControl::CMD_NEXT_EXPLANATION);
    emit StudyControl::newPacketAvailable();

    // We do the index loop logic, and update the explanation message.
    currentStudyExplanationScreen++;
    if (currentStudyExplanationScreen == NumberOfExplanationSlides[studyExplanationLanguageKey]){
        currentStudyExplanationScreen = 0;
    }
    emitNewExplanationMessage();

}

void StudyControl::sendInStudyCommand(InStudyCommand cmd){

    bool isOk = false;
    isOk = isOk || (this->studyState == SS_EVAL);
    isOk = isOk || ( (this->studyState != SS_NONE) && (cmd == ISC_ABORT) );

    if (!isOk){
        StaticThreadLogger::error("StudyControl::sendInStudyCommand","Cannot send in study command as study is NOT in evaluation state");
        return;
    }

    rspacket.resetForRX();
    rspacket.setPacketType(RRS::PacketType::TYPE_STUDY_CONTROL);
    rspacket.setPayloadField(RRS::PacketStudyControl::CMD,RRS::CommandStudyControl::CMD_IN_STUDY_COMMAND);

    switch (cmd) {
    case ISC_ABORT:
        rspacket.setPayloadField(RRS::PacketStudyControl::STUDY_CMD,RRS::CommandInStudy::CMD_ABORT);
        expectingAbortACK = true;
        break;
    case ISC_BINDING_DIFF:
        rspacket.setPayloadField(RRS::PacketStudyControl::STUDY_CMD,RRS::CommandInStudy::CMD_BINDING_DIFFERENT);
        break;
    case ISC_BINDING_SAME:
        rspacket.setPayloadField(RRS::PacketStudyControl::STUDY_CMD,RRS::CommandInStudy::CMD_BINDING_SAME);
        break;
    case ISC_CONTINUE:
        rspacket.setPayloadField(RRS::PacketStudyControl::STUDY_CMD,RRS::CommandInStudy::CMD_CONTINUE);
        break;
    default:
        StaticThreadLogger::error("StudyControl::sendInStudyCommand","Unknown in study command: " + QString::number(cmd));
        return;
    }

    emit StudyControl::newPacketAvailable();
}

void StudyControl::requestStudyData() {
    if (this->studyState != SS_WAITING_FOR_STUDY_DATA){
        StaticThreadLogger::error("StudyControl::requestStudyData","Will not send the request data command as state is NOT the waiting for study data state.");
        return;
    }
    rspacket.resetForRX();
    rspacket.setPacketType(RRS::PacketType::TYPE_STUDY_CONTROL);
    rspacket.setPayloadField(RRS::PacketStudyControl::CMD,RRS::CommandStudyControl::CMD_SEND_STUDY_DATA);
    studyInDataTransfer = true;
    emit StudyControl::newPacketAvailable();
}

///////////////////////////////////////////// AUX FUNCTIONS ///////////////////////////////////////////////////////

void StudyControl::setCalibrationValidationData(const QVariantMap &calib_data){
    rawdata.setCalibrationValidationData(calib_data);
}

QStringList StudyControl::getDataFilesLocation() const{
    QStringList ans;
    ans << fullPathCurrentStudyFile << fullPathCurrentIDXFile;
    return ans;
}

bool StudyControl::saveDataToHardDisk(){

    // We need to create both the actual file AND the idx file. The idx file contains the EXACT same same information but with out the trial field list of each study.
    // Hence it is much much lighter to quickly load for information reference.
    QFileInfo info(fullPathCurrentStudyFile);
    QString basename = info.baseName();
    fullPathCurrentIDXFile = workingDirectory + "/" + Globals::BaseFileNames::MakeMetdataFileName(basename);  //basename + "_metadata.json";

    bool ans = rawdata.saveJSONFile(fullPathCurrentStudyFile);
    if (!ans) return false;

    rawdata.clearFieldsForIndexFileCreation();
    ans = rawdata.saveJSONFile(fullPathCurrentIDXFile);

    return ans;

}

bool StudyControl::studyFinalizationLogic() {

    QStringList studyList = rawdata.getStudies();
    if (studyList.size() == 1){
        if ( (studyList.first() == VMDC::Study::BINDING_BC) ||
             (studyList.first() == VMDC::Study::BINDING_UC) ) {
            // We have only 1 study and it is one fo the Bindings.
            return false;
        }
    }

    // In any other case we can return true.
    return true;

}

void StudyControl::emitFailState(bool wasAborted){
    this->expectingAbortACK = false;
    if (wasAborted) this->studyEndStatus = SES_ABORTED;
    else this->studyEndStatus = SES_FAILED;
    this->studyState = SS_NONE;
    this->studyType = ST_NOT_SET;
    emit StudyControl::studyEnd();
}

void StudyControl::emitNewExplanationMessage(){
    QVariantMap stringToDisplay;
    stringToDisplay[studyExplanationLanguageKey] = currentStudyExplanationScreen;
    emit StudyControl::updateStudyMessages(stringToDisplay);
}


void StudyControl::FillNumberOfSlidesInExplanations(ConfigurationManager *language){
    NumberOfExplanationSlides.clear();
    QStringList allkeys;
    allkeys << STUDY_TEXT_KEY_BINDING_BC << STUDY_TEXT_KEY_BINDING_UC << STUDY_TEXT_KEY_GONOGO << STUDY_TEXT_KEY_GONOGO_3D
            << STUDY_TEXT_KEY_NBACKRT << STUDY_TEXT_KEY_NBACKVS << STUDY_TEXT_KEY_NBACK_3 << STUDY_TEXT_KEY_NBACK_4
            << STUDY_TEXT_KEY_PASS_BALL << STUDY_TEXT_KEY_GONOGO_3D_VS;
    for (qint32 i = 0; i < allkeys.size(); i++){
        //qDebug() << language->getStringList(allkeys.at(i));
        NumberOfExplanationSlides[allkeys.at(i)] = language->getStringList(allkeys.at(i)).size();
    }
}
