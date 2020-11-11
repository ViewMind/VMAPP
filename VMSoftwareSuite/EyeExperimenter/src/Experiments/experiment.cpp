#include "experiment.h"

Experiment::Experiment(QWidget *parent) : QWidget(parent)
{

    // The Experiment is created in the stopped state.
    state = STATE_STOPPED;

    // Hiding the window
    this->hide();

    // Bye default data needs to be saved.
    debugMode = false;

    // By default VR is disabled
    vrEnabled = false;
}

void Experiment::setupView(){
    // Making this window frameless and making sure it stays on top.
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint|Qt::Window);

    if (vrEnabled){
        config->addKeyValuePair(CONFIG_RESOLUTION_WIDTH,config->getReal(CONFIG_VR_RECOMMENDED_WIDTH));
        config->addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,config->getReal(CONFIG_VR_RECOMMENDED_HEIGHT));
    }
    else{
        config->addKeyValuePair(CONFIG_RESOLUTION_WIDTH,config->getReal(CONFIG_PRIMARY_MONITOR_WIDTH));
        config->addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,config->getReal(CONFIG_PRIMARY_MONITOR_HEIGHT));
    }

    // Finding the current desktop resolution
    this->setGeometry(0,0,
                      static_cast<qint32>(config->getReal(CONFIG_RESOLUTION_WIDTH)),
                      static_cast<qint32>(config->getReal(CONFIG_RESOLUTION_HEIGHT)));

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
    vrEnabled = (c->getString(CONFIG_EYETRACKER_CONFIGURED) == CONFIG_P_ET_HTCVIVEEYEPRO) && (!c->getBool(CONFIG_USE_MOUSE));

    // Loading the experiment configuration file.
    //qWarning() << "Reading the configuration file" << config->getString(CONFIG_EXP_CONFIG_FILE);
    QFile expfile(config->getString(CONFIG_EXP_CONFIG_FILE));
    if (!expfile.open(QFile::ReadOnly)){
        error = "Could not open experiment configuration file: " + expfile.fileName();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    bool saveData = !(config->getBool(CONFIG_DEMO_MODE) || config->getBool(CONFIG_USE_MOUSE));
    /// FOR DEBUGGING ONLY
    //saveData = true;
    //qDebug() << "Experiment::startExperiment: REMOVE FORCE saveData = true LINE";
    ///

    QTextStream reader(&expfile);
    reader.setCodec(COMMON_TEXT_CODEC);
    QString contents = reader.readAll();
    expfile.close();

    // Initializing the graphical interface and passing on the configuration.
    setupView();  /// WARNING: Setup view needs to come BEFORE manager intialization as it contians the resolution intialization.
    manager->init(c);
    this->gview->setScene(manager->getCanvas());

    // Configuring the experimet
    if (!manager->parseExpConfiguration(contents)){
        error = "ERROR parsing the configuration file " + expfile.fileName() + ": " + manager->getError();
        emit (experimentEndend(ER_FAILURE));
        return false;
    }

    multiPartIdentifier = "";

    // This if needs to be reproduced for all possible multi part experiments.
    if (c->containsKeyword(CONFIG_PERCEPTION_MP_CURRENT_IDENTIFIER)){
        multiPartIdentifier = c->getString(CONFIG_PERCEPTION_MP_CURRENT_IDENTIFIER);
        dataFile = workingDirectory + "/" + c->getString(CONFIG_PERCEPTION_MP_CURRENT_STUDY_FILE);
    }

    // The file is created ONLY when it's not part of a Multi Part File

    if (multiPartIdentifier.isEmpty()){
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
    }

    return true;

}

bool Experiment::doFrequencyCheck(){

    FreqAnalysis::FreqAnalysisResult fres;
    fres = FreqAnalysis::doFrequencyAnalysis(config,dataFile);

    QString freqReport;
    bool ans = true;

    if (!fres.fileError.isEmpty()){
        freqReport = "FREQ ANALYSIS ERROR: \n   " + fres.errorList.join("\n   ");
        ans = false;
    }
    else if (!fres.errorList.isEmpty()){
        freqReport = "FREQ ANALYSIS REPORT: Avg Frequency: " + QString::number(fres.averageFrequency) + "\n   ";
        freqReport = freqReport + fres.errorList.join("\n   ");
        freqReport = freqReport  + "\n   Individual Freq Errors:\n   " + fres.individualErrorList.join("\n   ");
        ans = false;
    }

    // If there was aproblem with the frequency check, the frequency report is saved to its own log. This way the common
    // log is NOT polluted.
    if (!ans){

        //QString movedFile = moveDataFileToAborted();
        QString renamedFile = changeFileExtensionToDatF();
        if (renamedFile.isEmpty()){
            error = "ERROR while changing the file extension in frequency check: " + error;
            error = error + "\n" +  freqReport;
        }
        else{
            LogInterface freqerrlogger;
            error = "Frequency Check Failed: Log can be seen at: " + renamedFile + ".flog";
            freqerrlogger.setLogFileLocation(renamedFile + ".flog");
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

QString Experiment::changeFileExtensionToDatF(){
    QFileInfo info(dataFile);
    QString destination = workingDirectory + "/" + info.baseName() + ".datf";
    if (!QFile::copy(dataFile,destination)) {
        error = "Could not copy data file " + dataFile + " to its new name: " + destination;
        return "";
    }
    if (!QFile(dataFile).remove()){
        error = "Could not delete orginal data file " + dataFile + " after changing its extension";
        return "";
    }
    else return destination;
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
    if (vrEnabled){
        emit(updateVRDisplay());
    }
    else if (debugMode){
        emit(updateBackground(manager->getImage()));
    }
}

void Experiment::keyPressHandler(int keyPressed){
    Q_UNUSED(keyPressed)
}


////////////////////// Static Functions for Handling Multi Part Studies.

QVariantMap Experiment::checkForMultiPartIdentifiers(const QString &patientDirectory, QStringList *errorList){

    QVariantMap ans;

    QHash<QString,QString> possibleExperimentsThatAreMultiPart;
    QHash<QString,qint32> totalNumberOfPartsPerStudy;

    // These should a list of all possible studies that are multi part as well as their expected total parts.
    possibleExperimentsThatAreMultiPart[FILE_OUTPUT_PERCEPTION] = QString::number(LIST_INDEX_PERCEPTION);
    totalNumberOfPartsPerStudy[FILE_OUTPUT_PERCEPTION] = QString(PERCEPTION_STUDY_LAST_MULTI_PART_IDENTIFIER).toInt() + 1;

    // Until they are finalized, multi part experiment will all end with .mp (after the .dat).
    QStringList name_filters;
    name_filters << QString("*.") + EXTRA_EXTENSION_FOR_MULTI_PART_FILES;

    QStringList mp_files = QDir(patientDirectory).entryList(name_filters,QDir::Files);

    for (qint32 i = 0; i < mp_files.size(); i++){

        QString filename = mp_files.at(i);
        QStringList filename_parts = filename.split("_");

        // If the first part of the study name is one of the possible experiments that are multi part ..
        if (possibleExperimentsThatAreMultiPart.contains(filename_parts.first())){

            QFile actualFile(patientDirectory + "/" + filename);
            if (!actualFile.open(QFile::ReadOnly)) {
                if (errorList != nullptr){
                    errorList->append("Could not open " + actualFile.fileName() + " for reading");
                }
                continue;
            }

            // Then all content is read.
            QTextStream reader(&actualFile);
            QStringList all_lines = reader.readAll().split("\n");
            actualFile.close();

            // And we search for the last occurrence of the MULTI_PART_FILE_STUDY_IDENTIFIER and save it's value to the map.
            // In a multi part file this should be the very first NON empty line starting from the end of the file.
            for (qint32 i = all_lines.size()-1; i >= 0; i--){

                if (all_lines.at(i).isEmpty()) continue;

                if (all_lines.at(i).startsWith(MULTI_PART_FILE_STUDY_IDENTIFIER)){

                    // Part identifier should ALL be the same, a line witht the format: MULTI_PART_FILE_STUDY_IDENTIFIER some_string_with_no_spaces

                    QStringList part_identifier = all_lines.at(i).split(" ",QString::SkipEmptyParts);
                    //qDebug() << "SPLITTING Last line " << all_lines.at(i) << " INTO " << part_identifier;
                    QVariantList data;

                    // Saving the name is necessary in order to actually know to which file to save the data for the next study.
                    data << filename << part_identifier.last() << totalNumberOfPartsPerStudy.value(filename_parts.first());
                    QString exp_id = possibleExperimentsThatAreMultiPart.value(filename_parts.first());
                    //ans[filename_parts.first()] = data;
                    ans[exp_id] = data;

                }
                else{
                    if (errorList != nullptr){
                        errorList->append("For file " + actualFile.fileName() + " the last non empty line of the file did NOT contain the multi part identifier. Skipping");
                    }
                }

                // After the first non empty line the break is mandatory. Either the last line was MP indentifier or the file has the wrong format.
                break;
            }

        }

    }

    return ans;
}

QString Experiment::finalizeMultiPartStudyFile(const QString &studyFileName){

    if (!QFile(studyFileName).exists()){
        return "File " + studyFileName + " does not exist";
    }

    // Creating the new name
    QStringList fileName_with_dot_separated_parts = studyFileName.split(".");
    if (fileName_with_dot_separated_parts.last() == EXTRA_EXTENSION_FOR_MULTI_PART_FILES){
        fileName_with_dot_separated_parts.removeLast();
        QString newFileName = fileName_with_dot_separated_parts.join(".");
        if (!QFile::rename(studyFileName,newFileName)){
            return "File renaming for " + studyFileName + "to" + newFileName  + " has failed";
        }
    }
    else{
        return "File " + studyFileName + " does not appear to be a multi part study file";
    }

    return "";

}

QString Experiment::makeStudyMultiPart(QString *studyFileName){

    if (!QFile(*studyFileName).exists()){
        return "File " + *studyFileName + " does not exist";
    }

    QString newFileName = *studyFileName + "." + QString(EXTRA_EXTENSION_FOR_MULTI_PART_FILES);

    if (!QFile::rename(*studyFileName,newFileName)){
        return "File renaming for " + *studyFileName + "to" + newFileName  + " has failed";
    }

    *studyFileName = newFileName;

    return "";
}

