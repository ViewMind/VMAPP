#include "experiment.h"


Experiment::Experiment(QWidget *parent, const QString &studyType) : QWidget(parent)
{

    // The Experiment is created in the stopped state.
    state = STATE_STOPPED;

    // Hiding the window
    this->hide();

    // Bye default data needs to be saved.
    debugMode = false;

    this->studyType = studyType;
    metaStudyType = studyType;

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

bool Experiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                 const QVariantMap &studyConfig,
                                 bool useMouse){

    Q_UNUSED(useMouse)

    error = "";
    workingDirectory = workingDir;
    dataFile = experimentFile;

    // Loading the experiment configuration file.
    QFile expfile(this->getExperimentDescriptionFile(studyConfig));
    if (!expfile.open(QFile::ReadOnly)){
        error = "Could not open experiment configuration file: " + expfile.fileName();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    QTextStream reader(&expfile);
    reader.setCodec(ExperimentGlobals::TEXT_CODEC);
    QString contents = reader.readAll();
    expfile.close();

    // We need to load the experiment file as it will contain the subject dat, the meta data already set and the processing parameters.
    if (QFile(dataFile).exists()){
        if (!rawdata.loadFromJSONFile(dataFile)){
            error = "Failed loading existing experiment file: " + experimentFile + "even tough it exists: " + rawdata.getError();
            emit (experimentEndend(ER_FAILURE));
            return false;
        }
    }
    else{
        error = "Set experiment file does not exist: "  + dataFile;
        emit (experimentEndend(ER_FAILURE));
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
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    // Adding the study to the raw data container.
    if (!rawdata.addStudy(studyType,studyConfig,contents,manager->getVersion())){
        error = "Adding the study type: " + rawdata.getError();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }


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

    //qDebug() << "Processing parameters";
    //Debug::prettpPrintQVariantMap(pp);

    // Last thing to do is to actually set the current study to the study type
    if (!rawdata.setCurrentStudy(studyType)){
        error = "Failed setting current study of " + studyType + ". Reason: " + rawdata.getError();
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    if (ExperimentGlobals::SHORT_STUDIES){
        manager->enableDemoMode();
    }

    // Setting up the fixation computer.
    MovingWindowParameters mwp;
    mwp.sampleFrequency = processingParameters.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();
    mwp.minimumFixationLength =  processingParameters.value(VMDC::ProcessingParameter::MIN_FIXATION_DURATION).toReal();
    mwp.maxDispersion =  processingParameters.value(VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW_PX).toReal();
    //mwp.maxDispersion =  35;
    mwp.calculateWindowSize();
    qDebug() << "Processing parameters" << mwp.sampleFrequency << mwp.minimumFixationLength << mwp.maxDispersion << mwp.getStartWindowSize();

    if ((mwp.getStartWindowSize() <= 0) || (mwp.sampleFrequency <= 0) || (mwp.minimumFixationLength <= 0) || (mwp.maxDispersion <= 0)){
        error = "Invalid processing parameeters for MWA. MFL: " + QString::number(mwp.minimumFixationLength)
                + "\nSF: " + QString::number(mwp.sampleFrequency)
                + "\nMD: " + QString::number(mwp.maxDispersion);
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    //Debug::prettpPrintQVariantMap(pp);

    rMWA.parameters = mwp;
    lMWA.parameters = mwp;

    if (ExperimentGlobals::ENABLE_FIX_LOG){
        QFileInfo info(dataFile);
        QString path = info.absolutePath();
        QString basename = info.baseName();
        rMWA.setOnlineFixationAnalysisFileOutput(path + "/" + basename + "_r" + ".log");
        lMWA.setOnlineFixationAnalysisFileOutput(path + "/" + basename + "_l" + ".log");
    }

    // And resetting just in case.
    rMWA.finalizeOnlineFixationCalculation();
    lMWA.finalizeOnlineFixationCalculation();

    return true;
}


void Experiment::newEyeDataAvailable(const EyeTrackerData &data){
    emit(updateEyePositions(data.xRight,data.yRight,data.xLeft,data.yLeft));
    manager->updateGazePoints(data.xRight,data.xLeft,data.yRight,data.yLeft);
}

void Experiment::experimenteAborted(){
    state = STATE_STOPPED;
    this->hide();

    QString newfile = moveDataFileToAborted();
    if (newfile.isEmpty()) emit(experimentEndend(ER_FAILURE));
    else emit (experimentEndend(ER_ABORTED));

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
    if (lastFixationR.isValid()){
        rawdata.addFixationVectorR(fixationToVariantMap(lastFixationR));
    }
    if (lastFixationL.isValid()){
        rawdata.addFixationVectorL(fixationToVariantMap(lastFixationL));
    }
}

void Experiment::computeOnlineFixations(const EyeTrackerData &data, qreal l_schar, qreal l_word, qreal r_schar, qreal r_word){
    lastFixationR = rMWA.calculateFixationsOnline(data.xRight,data.yRight,static_cast<qreal>(data.time),data.pdRight,r_schar,r_word);
    lastFixationL = lMWA.calculateFixationsOnline(data.xLeft,data.yLeft,static_cast<qreal>(data.time),data.pdLeft,l_schar,l_word);

    if (lastFixationR.isValid() && rightEyeEnabled){
        rawdata.addFixationVectorR(fixationToVariantMap(lastFixationR));
    }

    if (lastFixationL.isValid() && leftEyeEnabled){
        rawdata.addFixationVectorL(fixationToVariantMap(lastFixationL));
    }
}

QVariantMap Experiment::fixationToVariantMap(const Fixation &f){
    QVariantMap map;
    map.insert(VMDC::FixationVectorField::X,f.x);
    map.insert(VMDC::FixationVectorField::Y,f.y);
    map.insert(VMDC::FixationVectorField::DURATION,f.duration);
    map.insert(VMDC::FixationVectorField::TIME,f.time);
    map.insert(VMDC::FixationVectorField::START_TIME,f.fixStart);
    map.insert(VMDC::FixationVectorField::END_TIME,f.fixEnd);
    map.insert(VMDC::FixationVectorField::START_INDEX,f.indexFixationStart);
    map.insert(VMDC::FixationVectorField::END_INDEX,f.indexFixationEnd);
    map.insert(VMDC::FixationVectorField::PUPIL,f.pupil);
    map.insert(VMDC::FixationVectorField::ZERO_PUPIL,f.pupilZeroCount);
    if (studyType == VMDC::Study::READING){
        map.insert(VMDC::FixationVectorField::CHAR,f.sentence_char);
        map.insert(VMDC::FixationVectorField::WORD,f.sentence_word);
    }
    return map;
}


bool Experiment::saveDataToHardDisk(){

    // We need to create both the actual file AND the idx file. The idx file contains the EXACT same same information but with out the trial field list of each study.
    // Hence it is much much lighter to quickly load for information reference.
    QFileInfo info(dataFile);
    QString basename = info.baseName();
    QString idxFile = workingDirectory + "/" + basename + ".idx";

    bool ans = rawdata.saveJSONFile(dataFile,ExperimentGlobals::PRETTY_PRINT_OUTPUT_FILES);
    if (!ans) return false;

    rawdata.clearTrialFieldsFromEachStudy();
    ans = rawdata.saveJSONFile(idxFile,ExperimentGlobals::PRETTY_PRINT_OUTPUT_FILES);

    return ans;
}


QImage Experiment::getVRDisplayImage() const{
    return manager->getQImage();
}

void Experiment::keyboardKeyPressed(int keyboardKey){
    keyPressHandler(keyboardKey);
}

void Experiment::keyPressEvent(QKeyEvent *event){
    keyPressHandler(event->key());
}

void Experiment::updateSecondMonitorORHMD(){
    if (Globals::EyeTracker::IS_VR){
        emit(updateVRDisplay());
    }
    else if (debugMode){
        emit(updateBackground(manager->getImage()));
    }
}

void Experiment::keyPressHandler(int keyPressed){
    Q_UNUSED(keyPressed)
}
