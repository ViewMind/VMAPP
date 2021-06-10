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

bool Experiment::startExperiment(const QString &workingDir, const QString &experimentFile,
                                 const QVariantMap &studyConfig,
                                 bool useMouse,
                                 QVariantMap pp){

    Q_UNUSED(useMouse);

    error = "";
    workingDirectory = workingDir;
    studyConfiguration = studyConfig;
    dataFile = experimentFile;

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


    // Adding the study to the raw data container.
    if (!rawdata.addStudy(studyType,studyConfig,contents,manager->getVersion())){
        error = "Adding the study type: " + rawdata.getError();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    // The processing parameters now need to be stored. Any processing parameters need to have been set BEFORE calling this function.
    if (!rawdata.setProcessingParameters(pp)){
        error = "Failed to set the processing parameters for " + dataFile + ". Reason: " + rawdata.getError();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    //qDebug() << "Processing parameters";
    //Debug::prettpPrintQVariantMap(pp);

    // Last thing to do is to actually set the current study to the study type
    if (!rawdata.setCurrentStudy(studyType)){
        error = "Failed setting current study of " + studyType + ". Reason: " + rawdata.getError();
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    if (Globals::Debug::SHORT_STUDIES){
        manager->enableDemoMode();
    }

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
    RawDataContainer rdc;
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

    if (studyType == RDC::Study::BINDING_UC){
        if (studyConfiguration.value(RDC::StudyParameter::NUMBER_TARGETS).toString() == RDC::BindingTargetCount::TWO){
            return ":/experiment_data/uc.dat";
        }
        else{
            return ":/experiment_data/uc_3.dat";
        }
    }
    else if (studyType == RDC::Study::BINDING_BC){
        if (studyConfiguration.value(RDC::StudyParameter::NUMBER_TARGETS).toString() == RDC::BindingTargetCount::TWO){
            return ":/experiment_data/bc.dat";
        }
        else{
            return ":/experiment_data/bc_3.dat";
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
    else if (studyType.contains(RDC::MultiPartStudyBaseName::PERCEPTION)){
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
            lang = "is";
        }
        else if (studyConfiguration.value(RDC::StudyParameter::LANGUAGE) == RDC::ReadingLanguage::FRENCH){
            lang = "fr";
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


