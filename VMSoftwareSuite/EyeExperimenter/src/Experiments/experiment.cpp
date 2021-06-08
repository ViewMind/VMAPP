#include "experiment.h"

Experiment::Experiment(QWidget *parent) : QWidget(parent)
{

    // The Experiment is created in the stopped state.
    state = STATE_STOPPED;

    // Hiding the window
    this->hide();

    // Bye default data needs to be saved.
    debugMode = false;

}

void Experiment::setupView(qint32 monitor_resolution_width, qint32 monitor_resolution_height){
    // Making this window frameless and making sure it stays on top.
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint|Qt::Window);

    // Finding the current desktop resolution
    this->setGeometry(0,0,monitor_resolution_width,monitor_resolution_height);

    // Creating a graphics widget and adding it to the layout
    gview = new QGraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(gview);

    gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

bool Experiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                 const QVariantMap &studyConfig,
                                 bool useMouse,
                                 QVariantMap pp){

    Q_UNUSED(useMouse);

    error = "";
    workingDirectory = workingDir;
    studyConfiguration = studyConfig;
    dataFile = workingDir + "/" + experimentFile;

    // Loading the experiment configuration file.
    QFile expfile(getExperimentDescriptionFile());
    if (!expfile.open(QFile::ReadOnly)){
        error = "Could not open experiment configuration file: " + expfile.fileName();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    QTextStream reader(&expfile);
    reader.setCodec(Globals::Share::TEXT_CODEC);
    QString contents = reader.readAll();
    expfile.close();

    // Initializing the graphical interface and passing on the configuration.
    /// WARNING: Should be using the MONITOR display values, but they are not available in this function. This quick fix, SHOULD be the same.
    setupView(pp.value(RDC::ProcessingParameter::RESOLUTION_WIDTH).toInt(),pp.value(RDC::ProcessingParameter::RESOLUTION_HEIGHT).toInt());
    manager->init(pp.value(RDC::ProcessingParameter::RESOLUTION_WIDTH).toInt(),pp.value(RDC::ProcessingParameter::RESOLUTION_HEIGHT).toInt());
    this->gview->setScene(manager->getCanvas());

    // Configuring the data conllection.
    if (!rawdata.addStudy(studyType,studyConfig,contents,manager->getVersion())){
        error = "Configuring data collection: " + rawdata.getError();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    // Configuring the experimet
    if (!manager->parseExpConfiguration(contents)){
        error = "ERROR parsing the configuration file " + expfile.fileName() + ": " + manager->getError();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    // We need to load the experiment file as it will contain the subject data and the meta data already set.
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

    // The processing parameters now need to be stored. Any processing parameters need to have been set BEFORE calling this function.
    if (!rawdata.setProcessingParameters(pp)){
        error = "Failed to set the processing parameters for " + dataFile + ". Reason: " + rawdata.getError();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    // Last thing to do is to actually set the current study to the study type
    if (!rawdata.setCurrentStudy(studyType)){
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    return true;
}

// If the experiment is in a stopped state this function does nothing.
void Experiment::togglePauseExperiment(){
    if (state == STATE_RUNNING){
        state = STATE_PAUSED;
    }
    else if (state == STATE_PAUSED){
        state = STATE_RUNNING;
    }
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
    QString abortDir;
    QDir(workingDirectory).mkdir(Globals::Paths::SUBJECT_DIR_ABORTED);
    abortDir = workingDirectory + "/" + QString(Globals::Paths::SUBJECT_DIR_ABORTED);
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


bool Experiment::saveDataToHardDisk(){
    return rawdata.saveJSONFile(dataFile,Globals::Debug::PRETTY_PRINT_JSON);
}

QString Experiment::getExperimentDescriptionFile() const{
    QString lang;

    if (studyType == RDC::Study::BINDING){
        if (studyConfiguration.value(RDC::StudyParameter::NUMBER_TARGETS).toString() == RDC::BindingTargetCount::TWO){
            if (studyConfiguration.value(RDC::StudyParameter::BINDING_TYPE).toString() == RDC::BindingType::BOUND) return ":/experiment_data/bc.dat";
            else return ":/experiment_data/uc.dat";
        }
        else{
            if (studyConfiguration.value(RDC::StudyParameter::BINDING_TYPE).toString() == RDC::BindingType::BOUND) return ":/experiment_data/bc_3.dat";
            else return ":/experiment_data/uc_3.dat";
        }
    }
    else if (studyType == RDC::Study::GONOGO){
        return ":/experiment_data/go_no_go.dat";
    }
    else if (studyType == RDC::Study::NBACKMS){
        return ":/experiment_data/fielding.dat";
    }
    else if (studyType == RDC::Study::NBACKRT){
        return ":/experiment_data/fielding.dat";
    }
    else if (studyType == RDC::Study::NBACKVS){
        return ":/experiment_data/fielding.dat";
    }
    else if (studyType == RDC::Study::PERCEPTION){
        return ":/experiment_data/perception_study.dat";
    }
    else if (studyType == RDC::Study::READING){
        if (studyConfiguration.value(RDC::StudyParameter::LANGUAGE) == RDC::ReadingLanguage::GERMAN){
            lang = "de";
        }
        else if (studyConfiguration.value(RDC::StudyParameter::LANGUAGE) == RDC::ReadingLanguage::ENGLISH){
            lang = "en";
        }
        else if (studyConfiguration.value(RDC::StudyParameter::LANGUAGE) == RDC::ReadingLanguage::SPANISH){
            lang = "es";
        }
        else if (studyConfiguration.value(RDC::StudyParameter::LANGUAGE) == RDC::ReadingLanguage::ISELANDIC){
            lang = "fr";
        }
        else if (studyConfiguration.value(RDC::StudyParameter::LANGUAGE) == RDC::ReadingLanguage::FRENCH){
            lang = "is";
        }
        return ":/experiment_data/Reading_" + lang + ".dat";
    }

    return "";
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


