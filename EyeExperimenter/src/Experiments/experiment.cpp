#include "experiment.h"

Experiment::Experiment(QWidget *parent) : QWidget(parent)
{

    // The Experiment is created in the stopped state.
    state = STATE_STOPPED;

    // Hiding the window
    this->hide();

    // By default debug mode is true
    debugMode = false;

}

void Experiment::setupView(){
    // Making this window frameless and making sure it stays on top.
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint|Qt::Window);

    // Finding the current desktop resolution
    this->setGeometry(0,0,config->getReal(CONFIG_RESOLUTION_WIDTH),config->getReal(CONFIG_RESOLUTION_HEIGHT));

    // Creating a graphics widget and adding it to the layout
    gview = new QGraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(gview);

    gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

bool Experiment::startExperiment(ConfigurationManager *c){
    config = c;
    error = "";
    workingDirectory = c->getString(CONFIG_PATIENT_DIRECTORY);

    // Saving the patient info file
    QString err;
    QStringList errors;
    QString patientInfoFile = workingDirectory + "/" + QString(FILE_PATIENT_INFO_FILE);

    err = ConfigurationManager::setValue(patientInfoFile,COMMON_TEXT_CODEC,CONFIG_PATIENT_NAME,c->getString(CONFIG_PATIENT_NAME));
    if(!err.isEmpty()) errors << err;
    err = ConfigurationManager::setValue(patientInfoFile,COMMON_TEXT_CODEC,CONFIG_PATIENT_AGE,c->getString(CONFIG_PATIENT_AGE));
    if(!err.isEmpty()) errors << err;
    err = ConfigurationManager::setValue(patientInfoFile,COMMON_TEXT_CODEC,CONFIG_PATIENT_EMAIL,c->getString(CONFIG_PATIENT_EMAIL));
    if(!err.isEmpty()) errors << err;
    if (!errors.isEmpty()){
        error = "WARNING: Could not set data to the patient info file. Reasons: <br>" + errors.join("<br>");
    }

    // Loading the experiment configuration file.
    QFile expfile(config->getString(CONFIG_EXP_CONFIG_FILE));
    if (!expfile.open(QFile::ReadOnly)){
        error = "Could not open experiment configuration file: " + expfile.fileName();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    QTextStream reader(&expfile);
    reader.setCodec(COMMON_TEXT_CODEC);
    QString contents = reader.readAll();
    expfile.close();

    // Initializing the graphical interface and passing on the configuration.
    manager->init(c);
    setupView();
    this->gview->setScene(manager->getCanvas());

    // Configuring the experimet
    if (!manager->parseExpConfiguration(contents)){
        error = "ERROR parsing the configuration file " + expfile.fileName() + ": " + manager->getError();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    // The output data file is set.
    setDataFile(outputDataFile,config->getBool(CONFIG_OVERWRITE_BEHAVIOUR));

    // Deleting the data file if it exists, otherwise the new data will be appended to the old data.
    QFile file(dataFile);
    if (file.exists()){
        if (!file.remove()){
            error = "Could not delete existing data file at: " + dataFile + ". Please make sure the location is writeble.";
            emit (experimentEndend(ER_FAILURE));
            return false;
        }
    }

    // Creating the header of the data file. This will tell the EyeDataProcessor how to treat the data.
    if (!file.open(QFile::WriteOnly)){
        error = "Could not open data file " + dataFile + " for appending data.";
        emit(experimentEndend(ER_FAILURE));
        return false;
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);
    writer << expHeader << "\n"
           << config->getReal(CONFIG_RESOLUTION_WIDTH) << " " << config->getReal(CONFIG_RESOLUTION_HEIGHT) << "\n"
           << contents << "\n"
           << expHeader << "\n";
    file.close();

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
}

void Experiment::keyPressEvent(QKeyEvent *event){
    // FOR ALL Experiments:
    //   ESC = Abort
    //   C   = Request Calibration
    switch (event->key()){
    case Qt::Key_Escape:
        state = STATE_STOPPED;
        this->hide();
        emit (experimentEndend(ER_ABORTED));
        break;
    case Qt::Key_C:
        state = STATE_PAUSED;
        emit(calibrationRequest());
        break;
    }

}

QString Experiment::getNextName(const QString &baseName){

    QFileInfo info(baseName);
    QDir dir(info.absolutePath());
    QStringList filter;
    filter << info.baseName() + "_*.dat";
    QStringList allfiles = dir.entryList(filter,QDir::Files,QDir::Time);

    QString num = QString::number(allfiles.size());
    while (num.size() < 4){
        num = "0" + num;
    }

    return info.absolutePath() + "/" + info.baseName() + "_" + num + ".dat";
}

void Experiment::setDataFile(QString expFileName, bool overwrite){
    if (overwrite){
        dataFile = workingDirectory + "/" + expFileName + "_0000.dat";
    }
    else{
        dataFile = workingDirectory + "/" + expFileName + ".dat";
        //qWarning() << "Entering with the data file" << dataFile;
        dataFile = getNextName(dataFile);
    }
    //qWarning() << "Data file was set to" << dataFile;
}



