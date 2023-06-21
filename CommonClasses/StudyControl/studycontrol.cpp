#include "studycontrol.h"

QMap<QString,int> StudyControl::NumberOfExplanationSlides;

StudyControl::StudyControl() {
    this->studyState = SS_NONE;
    this->studyType  = ST_NOT_SET;
    this->expectingAbortACK = false;
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

////////////////////////////////////////// CONTROL FUNCTIONS //////////////////////////////////////////////////

void StudyControl::startStudy(const QString &workingDir, const QString &studyFile, const QVariantMap &studyConfig, const QString &studyName){

    // Storing the variables.
    workingDirectory = workingDir;
    fullPathCurrentStudyFile = studyFile;
    expectingAbortACK = false;

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
    else if (studyName == VMDC::Study::GONOGO_SPHERE){
        configurator = new GNGSpheresConfigurator();
        studyExplanationLanguageKey = STUDY_TEXT_KEY_GONOGO_3D;
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

    // Adding the study to the raw data container.
    if (!rawdata.addStudy(studyName,enhancedStudyConfig,configurator->getStudyDescriptionContents(),configurator->getVersionString())){
        StaticThreadLogger::error("StudyControl::startStudy","Adding the study type: " + rawdata.getError());
        delete configurator;
        emitFailState();
        return;
    }

    // Last thing to do is to actually set the current study to the study type
    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
        QString dbug = "DBUG: Setting study on starting experiment of type '" + studyName + "'";
        qDebug() << dbug;
        StaticThreadLogger::warning("StudyControl::startStudy",dbug);
    }

    if (!rawdata.setCurrentStudy(studyName)){
        StaticThreadLogger::error("StudyControl::startStudy","Failed setting current study of " + studyName + ". Reason: " + rawdata.getError());
        delete configurator;
        emitFailState();
        return;
    }

    // Setting up the fixation computer.
    qreal sampleFrequency = processingParameters.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();
    qreal md_percent     =  processingParameters.value(VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW).toReal();

    // We now, finally, create teh study configuraiton.
    if (!configurator->createStudyConfiguration(studyConfig,sampleFrequency,md_percent,studyName,shortStudies)){
        StaticThreadLogger::error("StudyControl::startStudy","Failed in creating a configuration for study of type " + studyName + ". Reason: " + configurator->getError());
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

    if (control.getType() == RRS::PacketType::TYPE_STUDY_CONTROL){
        QString command = control.getPayloadField(RRS::PacketStudyControl::CMD).toString();

        if (expectingAbortACK){
            if (command != RRS::CommandStudyControl::CMD_ACK){
                StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Study was expectin ACK from abort request. Will do nothing");
                return;
            }
            else {
                StaticThreadLogger::log("StudyControl::receiveRenderServerPacket","Received Abort ACK");
            }
            emitFailState();
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
        else if ( (this->studyState == SS_EVAL) && (command == RRS::CommandStudyControl::CMD_STUDY_STATUS_UPDATES) ){
            //qDebug() << "Got Stats Updates For Evaluation" << control.getPayloadField(RRS::PacketStudyControl::STATUS_UPDATE).toMap();
            emit StudyControl::updateStudyMessages(control.getPayloadField(RRS::PacketStudyControl::STATUS_UPDATE).toMap());
            return;
        }
        else {
            // Unexpected command. - We abort.
            StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Recieved unpexcted study control command " + command + " for the current study phase. Aborting");
            emitFailState();
            return;
        }

    }
    else if (control.getType() == RRS::PacketType::TYPE_STUDY_DATA){

        // This is the end of the study, we should only receive when we are in the evaluation phase.
        if (this->studyState != SS_EVAL){
            StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Received packet with study data, however the study is NOT in it's evaluation phase");
            return;
        }

        QVariantList trialList = control.getPayloadField(RRS::PacketStudyData::TRIAL_LIST_2D).toList();
        QVariantMap  studyData = control.getPayloadField(RRS::PacketStudyData::ELEMENTS_3D).toMap();
        qint32 explanationDuration = control.getPayloadField(RRS::PacketStudyData::EXPLAIN_LENGTH).toInt();
        qint32 exampleDuration = control.getPayloadField(RRS::PacketStudyData::EXAMPLE_LENGTH).toInt();
        qint32 evalDuration = control.getPayloadField(RRS::PacketStudyData::EVAL_LENGTH).toInt();
        qint32 pauseDuration = control.getPayloadField(RRS::PacketStudyData::PAUSE_LEGTH).toInt();
        qint32 res_width = control.getPayloadField(RRS::PacketStudyData::RES_W).toInt();
        qint32 res_height = control.getPayloadField(RRS::PacketStudyData::RES_H).toInt();

        // First we set the processing parameter missing fields.
        QVariantMap pp = rawdata.getProcessingParameters();
        pp[VMDC::ProcessingParameter::RESOLUTION_WIDTH] = res_width;
        pp[VMDC::ProcessingParameter::RESOLUTION_HEIGHT] = res_height;
        if (!rawdata.setProcessingParameters(pp)){
            StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Failed to set processing parameters in raw data. Reason: " + rawdata.getError());
            this->studyEndStatus = SES_FAILED;
            emit StudyControl::studyEnd();
            return;
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
                return;
            }

        }
        else {
            // This should be a 3D study.
            if (!rawdata.setFullElement3D(studyData,
                                          exampleDuration,exampleDuration,pauseDuration,evalDuration,
                                          evaluationStartTime)){
                StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Failed to set the full 3d element data in raw data. Reason: " + rawdata.getError());
                emitFailState();
                return;
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
        this->studyType = ST_NOT_SET;
        emit StudyControl::studyEnd();

    }
    else {
        StaticThreadLogger::error("StudyControl::receiveRenderServerPacket","Received packet of unexpected type: " + control.getType());
        return;
    }

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
    if (this->studyState != SS_EVAL){
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
    fullPathCurrentIDXFile = workingDirectory + "/" + basename + ".idx";

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

void StudyControl::emitFailState(){
    this->expectingAbortACK = false;
    this->studyEndStatus = SES_FAILED;
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
            << STUDY_TEXT_KEY_PASS_BALL;
    for (qint32 i = 0; i < allkeys.size(); i++){
        NumberOfExplanationSlides[allkeys.at(i)] = language->getStringList(allkeys.at(i)).size();
    }
}
