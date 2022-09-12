#include "experiment.h"


Experiment::Experiment(QWidget *parent, const QString &studyType) : QWidget(parent)
{

    // The Experiment is created in the stopped state.
    state = STATE_STOPPED;

    currentStudyExplanationScreen = 0;

    this->studyType = studyType;
    metaStudyType = studyType;

    // All studies start in manual mode until, it is removed.
    studyPhase = SP_EXPLANATION;

    // Basically the screen is show if we ever setup an eyetracker that uses the main screen or we use the mouse debug option
    activateScreenView = !Globals::EyeTracker::IS_VR || DBUGBOOL(Debug::Options::USE_MOUSE);

    // Checking if the Override time is necessary.
    overrideTime = DBUGINT(Debug::Options::OVERRIDE_TIME);

    // The default study eye is initialized as the right eye.
    defaultStudyEye = VMDC::Eye::RIGHT;

}

void Experiment::setupView(qint32 monitor_resolution_width, qint32 monitor_resolution_height){
    // Making this window frameless and making sure it stays on top.
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint|Qt::Window);

    // Setting the geometry to the current desktop resolution.
    this->setGeometry(0,0,monitor_resolution_width,monitor_resolution_height);

    // Creating a graphics widget and adding it to the layout
    gview = new QGraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(gview);

    gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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
    QFile expfile(this->getExperimentDescriptionFile(studyConfig));
    if (!expfile.open(QFile::ReadOnly)){
        error = "Could not open experiment configuration file: " + expfile.fileName();
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    QTextStream reader(&expfile);
    reader.setEncoding(QStringConverter::Utf8);
    QString contents = reader.readAll();
    expfile.close();

    // We need to load the experiment file as it will contain the subject dat, the meta data already set and the processing parameters.
    if (QFile(dataFile).exists()){

        if (DBUGBOOL(Debug::Options::DBUG_MSG)){
            LogInterface logger;
            logger.appendStandard("DBUG: Rawdata is being loaded from file: " + QFileInfo(dataFile).absoluteFilePath());
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

    // Initializing the graphical interface and passing on the configuration.
    /// WARNING: Should be using the MONITOR display values, but they are not available in this function. This quick fix, SHOULD be the same.
    setupView(processingParameters.value(VMDC::ProcessingParameter::RESOLUTION_WIDTH).toInt(),
              processingParameters.value(VMDC::ProcessingParameter::RESOLUTION_HEIGHT).toInt());
    manager->init(processingParameters.value(VMDC::ProcessingParameter::RESOLUTION_WIDTH).toInt(),
                  processingParameters.value(VMDC::ProcessingParameter::RESOLUTION_HEIGHT).toInt());
    this->gview->setScene(manager->getCanvas());


    // Configuring the experimet
    if (!manager->parseExpConfiguration(contents)){
        error = "ERROR parsing the configuration file " + expfile.fileName() + ": " + manager->getError();
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
        LogInterface logger;
        qDebug() << dbug;
        logger.appendWarning(dbug);
    }

    if (!rawdata.setCurrentStudy(studyType)){
        error = "Failed setting current study of " + studyType + ". Reason: " + rawdata.getError();
        emit Experiment::experimentEndend(ER_FAILURE);
        return false;
    }

    if (DBUGBOOL(Debug::Options::SHORT_STUDIES)){
        QString dbug = "DBUG: Short Studies Enabled";
        LogInterface logger;
        qDebug() << dbug;
        logger.appendWarning(dbug);
        manager->enableShortStudyMode();
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

    return true;
}

void Experiment::setCalibrationValidationData(const QVariantMap &calibrationValidationData){
    rawdata.setCalibrationValidationData(calibrationValidationData);
}

void Experiment::setStudyPhaseToEvaluation(){
    manager->setTrialCountLoopValue(-1);
    studyPhase = SP_EVALUATION;
    resetStudy();
}

void Experiment::setStudyPhaseToExamples(){
    manager->setTrialCountLoopValue(NUMBER_OF_TRIALS_IN_MANUAL_MODE);
    studyPhase = SP_EXAMPLE;
    resetStudy();
}

void Experiment::newEyeDataAvailable(const EyeTrackerData &data){
    emit Experiment::updateEyePositions(data.xRight,data.yRight,data.xLeft,data.yLeft);
    manager->updateGazePoints(data.xRight,data.xLeft,data.yRight,data.yLeft);
}

void Experiment::experimenteAborted(){
    state = STATE_STOPPED;
    this->hide();

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

void Experiment::computeOnlineFixations(const EyeTrackerData &data, qreal l_schar, qreal l_word, qreal r_schar, qreal r_word){
    lastFixationR = rMWA.calculateFixationsOnline(data.xRight,data.yRight,static_cast<qreal>(data.time),data.pdRight,r_schar,r_word);
    lastFixationL = lMWA.calculateFixationsOnline(data.xLeft,data.yLeft,static_cast<qreal>(data.time),data.pdLeft,l_schar,l_word);

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
    QString idxFile = workingDirectory + "/" + basename + ".idx";

    bool ans = rawdata.saveJSONFile(dataFile,true);
    if (!ans) return false;

    rawdata.clearTrialFieldsFromEachStudy();
    ans = rawdata.saveJSONFile(idxFile,true);

    return ans;
}


QImage Experiment::getVRDisplayImage() const{
    return manager->getQImage();
}

void Experiment::keyboardKeyPressed(int keyboardKey){
    if (studyPhase == SP_EXPLANATION){
        this->moveStudyExplanationScreen(keyboardKey);
    }
    else {
       keyPressHandler(keyboardKey);
    }
}

void Experiment::keyPressEvent(QKeyEvent *event){
    if (studyPhase == SP_EXPLANATION){
        this->moveStudyExplanationScreen(event->key());
    }
    else {
       keyPressHandler(event->key());
    }
}

void Experiment::moveStudyExplanationScreen(int key_pressed){
    if (key_pressed == Qt::Key_B){
        if (currentStudyExplanationScreen > 0){
            currentStudyExplanationScreen--;
            renderCurrentStudyExplanationScreen();
        }
    }
    else if (key_pressed == Qt::Key_N){
        if (currentStudyExplanationScreen < manager->getNumberOfStudyExplanationScreens()-1){
            currentStudyExplanationScreen++;
            renderCurrentStudyExplanationScreen();
        }
        else {
            // Going forward at the end loops back to the beginning
            currentStudyExplanationScreen = 0;
            renderCurrentStudyExplanationScreen();
        }
    }

    // Abort should still be possible, if required.
    else if (key_pressed == Qt::Key_Escape){
        keyPressHandler(key_pressed);
    }
}

void Experiment::updateSecondMonitorORHMD(){
    if (Globals::EyeTracker::IS_VR){
        emit Experiment::updateVRDisplay();
    }
}

void Experiment::keyPressHandler(int keyPressed){
    Q_UNUSED(keyPressed)
}

void Experiment::renderCurrentStudyExplanationScreen(){
    manager->renderStudyExplanationScreen(currentStudyExplanationScreen);
    updateSecondMonitorORHMD();
    // Now we send the signal for the proper message.
    QVariantMap stringToDisplay;
    stringToDisplay[manager->getStringKeyForStudyExplanationList()] = currentStudyExplanationScreen;
    emit Experiment::updateStudyMessages(stringToDisplay);
}

void Experiment::resetStudy(){

}

