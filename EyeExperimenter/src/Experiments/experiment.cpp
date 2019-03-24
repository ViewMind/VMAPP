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

    // Loading the experiment configuration file.
    //qWarning() << "Reading the configuration file" << config->getString(CONFIG_EXP_CONFIG_FILE);
    QFile expfile(config->getString(CONFIG_EXP_CONFIG_FILE));
    if (!expfile.open(QFile::ReadOnly)){
        error = "Could not open experiment configuration file: " + expfile.fileName();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    bool saveData = !(config->getBool(CONFIG_DEMO_MODE) || config->getBool(CONFIG_USE_MOUSE));

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
    if (!saveData) outputDataFile = "demo" + outputDataFile;
    dataFile = workingDirectory + "/" + outputDataFile + "_" + config->getString(CONFIG_VALID_EYE) + "_"
            + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm") + ".dat";

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

    if (!saveData) return true;
    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);
    writer << expHeader + " " + manager->getVersion() << "\n"
           << contents << "\n"
           << expHeader << "\n"
           << config->getReal(CONFIG_RESOLUTION_WIDTH) << " " << config->getReal(CONFIG_RESOLUTION_HEIGHT) << "\n";
    file.close();

    return true;

}

bool Experiment::doFrequencyCheck(){
    FreqAnalysis freqChecker;
    FreqAnalysis::FreqAnalysisResult fres;
    fres = freqChecker.analyzeFile(dataFile);

    QString freqReport;
    bool ans = true;

    if (!fres.errorList.isEmpty()){
        freqReport = "FREQ ANALYSIS ERROR: \n   " + fres.errorList.join("\n   ");
        ans = false;
    }
    else {

        FreqAnalysis::FreqCheckParameters fcp;
        fcp.fexpected                        = config->getReal(CONFIG_SAMPLE_FREQUENCY);
        fcp.periodMax                        = config->getReal(CONFIG_TOL_MAX_PERIOD_TOL);
        fcp.periodMin                        = config->getReal(CONFIG_TOL_MIN_PERIOD_TOL);
        fcp.maxAllowedFreqGlitchesPerTrial   = config->getReal(CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL);
        fcp.maxAllowedPercentOfInvalidValues = config->getReal(CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES);
        fcp.minNumberOfDataItems             = config->getReal(CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL);

        fres.analysisValid(fcp);

        if (!fres.errorList.isEmpty()){
            freqReport = "FREQ ANALYSIS REPORT: Avg Frequency: " + QString::number(fres.averageFrequency) + "\n   ";
            freqReport = freqReport + fres.errorList.join("\n   ");
            freqReport = freqReport  + "\n   Individual Freq Errors:\n   " + fres.individualErrorList.join("\n   ");
            ans = false;
        }

    }

    // If there was aproblem with the frequency check, the frequency report is saved to its own log. This way the common
    // log is NOT polluted.
    if (!ans){

        QString movedFile = moveDataFileToAborted();
        if (movedFile.isEmpty()){
            error = "ERROR while moving aborted file in frequency check: " + error;
            error = error + "\n" +  freqReport;
        }
        else{
            LogInterface freqerrlogger;
            error = "Frequency Check Failed: Log can be seen at: " + movedFile + ".flog";
            freqerrlogger.setLogFileLocation(movedFile + ".flog");
            freqerrlogger.appendStandard(freqReport);
        }
    }

    return ans;
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
    QDir(workingDirectory).mkdir(DIRNAME_ABORTED);
    abortDir = workingDirectory + "/" + QString(DIRNAME_ABORTED);
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
    QFile file(dataFile);
    if (!file.open(QFile::Append)){
        error = "Could not open data file " + dataFile + " for appending data.";
        state = STATE_STOPPED;
        return false;
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);

    for (qint32 i = 0; i < etData.size(); i++){
        QVariantList line = etData.at(i).toList();
        for (qint32 j = 0; j < line.size(); j++){
            writer << line.at(j).toString() << " ";
        }
        writer << "\n";
    }

    file.close();
    etData.clear();
    return true;
}

void Experiment::keyPressEvent(QKeyEvent *event){
    Q_UNUSED(event);
    //    // FOR ALL Experiments:
    //    //   ESC = Abort
    //    //   C   = Request Calibration
    //    switch (event->key()){
    //    case Qt::Key_Escape:
    //        state = STATE_STOPPED;
    //        this->hide();
    //        emit (experimentEndend(ER_ABORTED));
    //        break;
    //    case Qt::Key_C:
    //        state = STATE_PAUSED;
    //        emit(calibrationRequest());
    //        break;
    //    }
}
