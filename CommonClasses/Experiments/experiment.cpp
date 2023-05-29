#include "experiment.h"


Experiment::Experiment(QObject *parent, const QString &studyType): QObject(parent) {

    // The Experiment is created in the stopped state.
    state = STATE_STOPPED;

    currentStudyExplanationScreen = 0;

    this->studyType = studyType;
    metaStudyType = studyType;

    // All studies start in manual mode until, it is removed.
    studyPhase = SP_EXPLANATION;

    // Checking if the Override time is necessary.
    overrideTime = DBUGINT(Debug::Options::OVERRIDE_TIME);

    // The default study eye is initialized as the right eye.
    defaultStudyEye = VMDC::Eye::RIGHT;

    // Default is false. Expect 3D Studies to change this.
    isStudy3D = false;
    study3DState = S3S_NONE;

}

QString Experiment::getExperimentDescriptionFile(const QVariantMap &studyConfig){
    Q_UNUSED(studyConfig)
    return "";
}

bool Experiment::startExperiment(const QString &workingDir,
                                 const QString &experimentFile,
                                 const QVariantMap &studyConfig){

    error = "";
    workingDirectory = workingDir;
    dataFile = experimentFile;

    // Loading the experiment configuration file.
    QString experiment_file_name = this->getExperimentDescriptionFile(studyConfig);
    QString contents = "";

    if (experiment_file_name != ""){ // Some studies do not need to load files as the experimet descriptions are created in a differnt way.
        QFile expfile(experiment_file_name);
        if (!expfile.open(QFile::ReadOnly)){
            error = "Could not open experiment configuration file: " + expfile.fileName();
            emit Experiment::experimentEndend(ER_FAILURE);
            return false;
        }

        QTextStream reader(&expfile);
        reader.setEncoding(QStringConverter::Utf8);
        contents = reader.readAll();
        expfile.close();
    }


    // We need to load the experiment file as it will contain the subject dat, the meta data already set and the processing parameters.
    if (QFile(dataFile).exists()){

        if (DBUGBOOL(Debug::Options::DBUG_MSG)){
            StaticThreadLogger::log("Experiment::startExperiment","DBUG: Rawdata is being loaded from file: " + QFileInfo(dataFile).absoluteFilePath());
        }

        if (!rawdata.loadFromJSONFile(dataFile)){
            error = "Failed loading existing experiment file: " + experimentFile + "even tough it exists: " + rawdata.getError();
            emit Experiment::experimentEndend(ER_FAILURE);
            return false;
        }
    }
    else{
        error = "Set experiment file does not exist: "  + dataFile;
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    processingParameters = rawdata.getProcessingParameters();

    // Initializing the experiment data painters in general.
    manager->init(processingParameters.value(VMDC::ProcessingParameter::RESOLUTION_WIDTH).toInt(),
                  processingParameters.value(VMDC::ProcessingParameter::RESOLUTION_HEIGHT).toInt());

    // Configuring the experimet
    if (!manager->parseExpConfiguration(contents)){
        error = "ERROR parsing the configuration file " + experiment_file_name + ": " + manager->getError();
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    // Adding the default eye for the study to the study configuration.
    QVariantMap enhancedStudyConfig = studyConfig; // This is done so I don't have to change the function interface everywhere.
    enhancedStudyConfig[VMDC::StudyParameter::DEFAULT_EYE] = defaultStudyEye;

    // Adding the study to the raw data container.

    //qDebug() << "On Experiment Start. Printing the studies in the current raw data file before adding a study" << rawdata.getStudies();

    if (!rawdata.addStudy(studyType,enhancedStudyConfig,contents,manager->getVersion())){
        error = "Adding the study type: " + rawdata.getError();
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    //qDebug() << "On Experiment Start. Printing the studies in the current raw data file after adding a study" << rawdata.getStudies();


    QString validEye = studyConfig.value(VMDC::StudyParameter::VALID_EYE).toString();
    if (validEye == VMDC::Eye::BOTH){
        leftEyeEnabled = true;
        rightEyeEnabled = true;
    }
    else if (validEye == VMDC::Eye::RIGHT){
        leftEyeEnabled = false;
        rightEyeEnabled = true;
    }
    else if (validEye == VMDC::Eye::LEFT){
        leftEyeEnabled = true;
        rightEyeEnabled = false;
    }

    //qDebug() << "Valid Eye was " << validEye << " so L: " << leftEyeEnabled << " and R: " << rightEyeEnabled;
    //qDebug() << "Processing parameters";
    //Debug::prettpPrintQVariantMap(pp);

    // Last thing to do is to actually set the current study to the study type

    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
        QString dbug = "DBUG: Setting study on starting experiment of type '" + studyType + "'";
        qDebug() << dbug;
        StaticThreadLogger::warning("Experiment::startExperiment",dbug);
    }

    if (!rawdata.setCurrentStudy(studyType)){
        error = "Failed setting current study of " + studyType + ". Reason: " + rawdata.getError();
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    // Base setup for the request study description packet.
    if (isStudy3D){
        //qDebug() << "Creating the Request for study description packet";
        rrsControlPacket.resetForRX();
        rrsControlPacket.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        rrsControlPacket.setPayloadField(Packet3DStudyControl::COMMAND, Study3DControlCommands::CMD_REQUEST_STUDY_DESCRIPTION);

    }

    if (DBUGBOOL(Debug::Options::SHORT_STUDIES)){
        QString dbug = "DBUG: Short Studies Enabled";
        qDebug() << dbug;
        StaticThreadLogger::warning("Experiment::startExperiment",dbug);
        if (isStudy3D){
            rrsControlPacket.setPayloadField(Packet3DStudyControl::SHORT_STUDIES,true);
        }
        else manager->enableShortStudyMode();
    }
    else {
        if (isStudy3D){
            rrsControlPacket.setPayloadField(Packet3DStudyControl::SHORT_STUDIES,false);
        }
    }

    // Setting up the fixation computer.
    MovingWindowParameters mwp;
    mwp.sampleFrequency = processingParameters.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();
    mwp.minimumFixationLength =  processingParameters.value(VMDC::ProcessingParameter::MIN_FIXATION_DURATION).toReal();
    mwp.maxDispersion =  processingParameters.value(VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW_PX).toReal();
    //mwp.maxDispersion =  35;
    mwp.calculateWindowSize();
    //qDebug() << "Processing parameters" << mwp.sampleFrequency << mwp.minimumFixationLength << mwp.maxDispersion << mwp.getStartWindowSize();

    if ((mwp.getStartWindowSize() <= 0) || (mwp.sampleFrequency <= 0) || (mwp.minimumFixationLength <= 0) || (mwp.maxDispersion <= 0)){
        error = "Invalid processing parameeters for MWA.\nMFL: " + QString::number(mwp.minimumFixationLength)
                + "\nSF: " + QString::number(mwp.sampleFrequency)
                + "\nMD: " + QString::number(mwp.maxDispersion);
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    //Debug::prettpPrintQVariantMap(pp);

    rMWA.parameters = mwp;
    lMWA.parameters = mwp;

    if (DBUGBOOL(Debug::Options::FIX_LOG_ENABLED)){
        QFileInfo info(dataFile);
        QString path = info.absolutePath();
        QString basename = info.baseName();
        rMWA.setOnlineFixationAnalysisFileOutput(path + "/" + basename + "_r" + ".log");
        lMWA.setOnlineFixationAnalysisFileOutput(path + "/" + basename + "_l" + ".log");
    }

    // And resetting just in case.
    rMWA.finalizeOnlineFixationCalculation();
    lMWA.finalizeOnlineFixationCalculation();

    // The experiment is ALWAYS started in the explanation phase
    rawdata.setExplanationPhaseStart();

    return true;
}

void Experiment::setCalibrationValidationData(const QVariantMap &calibrationValidationData){
    rawdata.setCalibrationValidationData(calibrationValidationData);
}

void Experiment::process3DStudyControlPacket(const RenderServerPacket &p) {

    if (study3DState == S3S_EXPECTING_STUDY_DESCRIPTION){

        if (p.getType() != RenderServerPacketType::TYPE_STUDY_DESCRIPTION){
            StaticThreadLogger::error("Experiment::process3DStudyControlPacket","In study '" + this->studyType  + "' on state expecting the study description. But Got Packet of Type: " + p.getType());
            return;
        }

        // The first step is to parse the study description back to a map.
        QVariantMap map = p.getPayload();
        QString study_description = map.value(PacketStudyDescription::STUDY_DESC).toString();

        // Parsing the JSON of the map.
        QVariantMap study_desc_as_map;
        QJsonParseError json_error;
        QJsonDocument doc = QJsonDocument::fromJson(study_description.toUtf8(),&json_error);
        if (doc.isNull()){
            StaticThreadLogger::error("Experiment::process3DStudyControlPacket","Error parsing the JSON string of the study description: " + json_error.errorString());
        }
        else {
            study_desc_as_map = doc.object().toVariantMap();
        }

        map[PacketStudyDescription::STUDY_DESC] = study_desc_as_map;

        // Now we need to store this in the experiment_description field of the current study.
        if (!rawdata.setExperimentDescriptionMap(map)){
            StaticThreadLogger::error("Experiment::process3DStudyControlPacket","Error setting the study description: " + rawdata.getError());
        }

        StaticThreadLogger::log("Experiment::process3DStudyControlPacket","Received 3D Study Description Packet");
        study3DState = S3S_WAITING_FOR_STUDY_END;

    }
    else if (study3DState == S3S_WAITING_FOR_STUDY_END){

        QString packetType = p.getType();

        if (packetType == RenderServerPacketType::TYPE_3DSTUDY_CONTROL){

            qint32 cmd = p.getPayloadField(Packet3DStudyControl::COMMAND).toInt();
            if (cmd == Study3DControlCommands::CMD_UPDATE_MESSAGES){
                newDataForStudyMessageFor3DStudies(p.getPayloadField(Packet3DStudyControl::STUDY_MESSAGES).toList());
            }
            else if (cmd == Study3DControlCommands::CMD_STUDY_END){
                StaticThreadLogger::log("Experiment::process3DStudyControlPacket","Received 3D Study End");

                // Before we go we need to ta make sure that the file name is saved.
                rawdata.finalizeStudy(QVariantMap(), 0); // We finalized the study with empty data so that all metadata is stored.

                rMWA.finalizeOnlineFixationLog();
                lMWA.finalizeOnlineFixationLog();
                state = STATE_STOPPED;

                ExperimentResult er;
                if (error.isEmpty()) er = ER_NORMAL;

                if (!saveDataToHardDisk()){
                    emit Experiment::experimentEndend(ER_FAILURE);
                }
                else emit Experiment::experimentEndend(ER_NORMAL);


            }
            else{
                StaticThreadLogger::error("Experiment::process3DStudyControlPacket","In study '" + this->studyType  + "' on state expecting study's end. But received study control packet of with command " + QString::number(cmd));
            }
        }

        else {
            StaticThreadLogger::error("Experiment::process3DStudyControlPacket","In study '" + this->studyType  + "' on state expecting study's end. But Got Packet of Type: " + p.getType());
        }


    }

}

void Experiment::setStudyPhaseToEvaluation(){
    studyPhase = SP_EVALUATION;
    rawdata.setCurrentStudyStart();
    if (isStudy3D){
        // Send the packet indicating the study start.
        rrsControlPacket.resetForRX();
        rrsControlPacket.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        rrsControlPacket.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_STUDY_START);
        emit Experiment::remoteRenderServerPacketAvailable();
    }
    else {
        manager->setTrialCountLoopValue(-1);
        resetStudy();
    }
}

void Experiment::setStudyPhaseToExamples(){
    studyPhase = SP_EXAMPLE;
    rawdata.setExamplePhaseStart();
    if (isStudy3D){
        // Send the packet indicating the start of the example phase.
        rrsControlPacket.resetForRX();
        rrsControlPacket.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        rrsControlPacket.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_START_EXAMPLES);
        emit Experiment::remoteRenderServerPacketAvailable();
    }
    else {
        manager->setTrialCountLoopValue(NUMBER_OF_TRIALS_IN_MANUAL_MODE);
        resetStudy();
    }
}

void Experiment::newEyeDataAvailable(const EyeTrackerData &data){
    Q_UNUSED(data)
    updateDisplay();
}

void Experiment::experimenteAborted(){
    state = STATE_STOPPED;
    QString newfile = moveDataFileToAborted();
    if (newfile.isEmpty()) emit Experiment::experimentEndend(ER_FAILURE);
    else emit Experiment::experimentEndend(ER_ABORTED);
}

QString Experiment::moveDataFileToAborted(){

    // Before moving we need to check if the file has finalized studies.
    ViewMindDataContainer rdc;
    if (!rdc.loadFromJSONFile(dataFile)){
        error = "Could not load experiment data file " + dataFile + " from disk to check wheter it can be deleted or not";
        return "";
    }

    // Checking the if there are nay studies in it.
    if (rdc.getStudies().size() > 0){
        // This means that it was saved before and hence we need to simply not save in order to not loose any data.
        // We return the curent data path so as to avoid an experiment failed message.
        return dataFile;
    }

    QString abortDir;
    QDir(workingDirectory).mkdir(ExperimentGlobals::SUBJECT_DIR_ABORTED);
    abortDir = workingDirectory + "/" + QString(ExperimentGlobals::SUBJECT_DIR_ABORTED);
    if (QDir(abortDir).exists()){
        QFileInfo info(dataFile);
        QString destination = abortDir + "/" + info.baseName() + "." + info.suffix();
        if (!QFile::copy(dataFile,destination)) {
            error = "Could not copy data file " + dataFile + " to abort directory inside patient directory: " + abortDir;
            return "";
        }
        else {
            if (!QFile(dataFile).remove()){
                error = "Could not delete orginal data file " + dataFile + " after copying to the abort directory";
                return "";
            }
            else return destination;
        }
    }
    else {
        error = "Could not create abort directory inside patient directory: " + workingDirectory;
        return "";
    }
}


void Experiment::finalizeOnlineFixations(){
    lastFixationR = rMWA.finalizeOnlineFixationCalculation();
    lastFixationL = lMWA.finalizeOnlineFixationCalculation();

    if (lastFixationR.hasFinished() && rightEyeEnabled){
        rawdata.addFixationVectorR(fixationToVariantMap(lastFixationR));
    }
    if (lastFixationL.hasFinished() && leftEyeEnabled){
        rawdata.addFixationVectorL(fixationToVariantMap(lastFixationL));
    }

    this->storeStudyLogicFixation();

}

void Experiment::computeOnlineFixations(const EyeTrackerData &data){
    lastFixationR = rMWA.calculateFixationsOnline(data.xr(),data.yr(),static_cast<qreal>(data.timestamp()),data.pr());
    lastFixationL = lMWA.calculateFixationsOnline(data.xl(),data.yl(),static_cast<qreal>(data.timestamp()),data.pl());

    if (lastFixationR.hasFinished() && rightEyeEnabled){
        rawdata.addFixationVectorR(fixationToVariantMap(lastFixationR));
    }

    if (lastFixationL.hasFinished() && leftEyeEnabled){
        rawdata.addFixationVectorL(fixationToVariantMap(lastFixationL));
    }

    this->storeStudyLogicFixation();

}

void Experiment::storeStudyLogicFixation(){
    if (!leftEyeEnabled){
        studyLogicFixation = lastFixationR;
    }
    else if (!rightEyeEnabled){
        studyLogicFixation = lastFixationL;
    }
    else {
        if (defaultStudyEye == VMDC::Eye::RIGHT){
            studyLogicFixation = lastFixationR;
        }
        else {
            studyLogicFixation = lastFixationL;
        }
    }
}

QVariantMap Experiment::fixationToVariantMap(const Fixation &f){
    QVariantMap map;
    map.insert(VMDC::FixationVectorField::X,f.getX());
    map.insert(VMDC::FixationVectorField::Y,f.getY());
    map.insert(VMDC::FixationVectorField::DURATION,f.getDuration());
    map.insert(VMDC::FixationVectorField::TIME,f.getTime());
    map.insert(VMDC::FixationVectorField::START_TIME,f.getStartTime());
    map.insert(VMDC::FixationVectorField::END_TIME,f.getEndTime());
    map.insert(VMDC::FixationVectorField::START_INDEX,0);   // DEPRACATED. Left for legacy reasons.
    map.insert(VMDC::FixationVectorField::END_INDEX,0);     // DEPRACATED. Left for legacy reasons.
    map.insert(VMDC::FixationVectorField::PUPIL,f.getPupil());
    map.insert(VMDC::FixationVectorField::ZERO_PUPIL,f.getBlinks());
    if (studyType == VMDC::Study::READING){
        map.insert(VMDC::FixationVectorField::CHAR,f.getChar());
        map.insert(VMDC::FixationVectorField::WORD,f.getWord());
    }
    return map;
}


bool Experiment::saveDataToHardDisk(){

    // We need to create both the actual file AND the idx file. The idx file contains the EXACT same same information but with out the trial field list of each study.
    // Hence it is much much lighter to quickly load for information reference.
    QFileInfo info(dataFile);
    QString basename = info.baseName();
    idxFile = workingDirectory + "/" + basename + ".idx";

    bool ans = rawdata.saveJSONFile(dataFile);
    if (!ans) return false;

    rawdata.clearFieldsForIndexFileCreation();
    ans = rawdata.saveJSONFile(idxFile);

    return ans;
}

QStringList Experiment::getDataFilesLocation() const{
    QStringList ans;
    ans << dataFile << idxFile;
    return ans;
}


RenderServerScene Experiment::getVRDisplayImage() const{
    return manager->getImage();
}

RenderServerPacket Experiment::getControlPacket() const {
    return rrsControlPacket;
}

void Experiment::keyboardKeyPressed(int keyboardKey){
    if (studyPhase == SP_EXPLANATION){
        this->moveStudyExplanationScreen(keyboardKey);
    }
    else {
        keyPressHandler(keyboardKey);
    }
}

//void Experiment::keyPressEvent(QKeyEvent *event){
//    if (studyPhase == SP_EXPLANATION){
//        this->moveStudyExplanationScreen(event->key());
//    }
//    else {
//       keyPressHandler(event->key());
//    }
//}

void Experiment::newDataForStudyMessageFor3DStudies(const QVariantList &msg){
    Q_UNUSED(msg)
}

void Experiment::moveStudyExplanationScreen(int key_pressed){
    if (key_pressed == Qt::Key_B){
        if (currentStudyExplanationScreen > 0){
            currentStudyExplanationScreen--;
            renderCurrentStudyExplanationScreen(false);
        }
    }
    else if (key_pressed == Qt::Key_N){
        if (currentStudyExplanationScreen < manager->getNumberOfStudyExplanationScreens()-1){
            currentStudyExplanationScreen++;
            renderCurrentStudyExplanationScreen(true);
        }
        else {
            // Going forward at the end loops back to the beginning
            currentStudyExplanationScreen = 0;
            renderCurrentStudyExplanationScreen(true);
        }
    }

    // Abort should still be possible, if required.
    else if (key_pressed == Qt::Key_Escape){
        keyPressHandler(key_pressed);
    }
}

void Experiment::updateDisplay(){
    if (!isStudy3D) emit Experiment::updateVRDisplay();
}

void Experiment::keyPressHandler(int keyPressed){
    Q_UNUSED(keyPressed)
}

void Experiment::renderCurrentStudyExplanationScreen(bool forward){
    if (isStudy3D){
        StaticThreadLogger::log("Experiment::renderCurrentStudyExplanationScreen","Sending next explanation packet");
        rrsControlPacket.resetForRX();
        rrsControlPacket.setPacketType(RenderServerPacketType::TYPE_3DSTUDY_CONTROL);
        if (forward){
            rrsControlPacket.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_NEXT_EXPLANATION);
        }
        else {
            rrsControlPacket.setPayloadField(Packet3DStudyControl::COMMAND,Study3DControlCommands::CMD_PREVIOUS_EXPLANATION);
        }
        emit Experiment::remoteRenderServerPacketAvailable();
    }
    else {
        manager->renderStudyExplanationScreen(currentStudyExplanationScreen);
        updateDisplay();
        // Now we send the signal for the proper message.
    }

    // No matter the study type we need to update the messages.
    QVariantMap stringToDisplay;
    stringToDisplay[manager->getStringKeyForStudyExplanationList()] = currentStudyExplanationScreen;
    // qDebug() << "Rendering Current Explanation Screen";
    emit Experiment::updateStudyMessages(stringToDisplay);
}

void Experiment::resetStudy(){

}

