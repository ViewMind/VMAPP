#include "flowcontrol.h"

FlowControl::FlowControl(QWidget *parent, ConfigurationManager *c) : QWidget(parent)
{
    // Intializing the eyetracker pointer
    configuration = c;
    eyeTracker = nullptr;
    connected = false;
    calibrated = false;
    experiment = nullptr;
    monitor = nullptr;
    demoTransaction = false;
    this->setVisible(false);

    // For delay the what the UI shows before drawing a report.
    connect(&delayTimer,SIGNAL(timeout()),this,SLOT(drawReport()));

    // Creating the sslclient
    sslDataProcessingClient = new SSLDataProcessingClient(this,c);

    // Connection to the "finished" slot.
    connect(sslDataProcessingClient,SIGNAL(transactionFinished()),this,SLOT(onDisconnectionFinished()));

    // This infomration should only be updated if the report text is updated. First is the title, then the exp index and the ref index.
    reportTextDataIndexes << CONFIG_RESULTS_ATTENTIONAL_PROCESSES << CONFIG_RESULTS_EXECUTIVE_PROCESSES << CONFIG_RESULTS_WORKING_MEMORY
                          << CONFIG_RESULTS_RETRIEVAL_MEMORY << CONFIG_RESULTS_MEMORY_ENCODING
                          << CONFIG_RESULTS_BEHAVIOURAL_RESPONSE;

}

////////////////////////////// AUXILIARY FUNCTIONS ///////////////////////////////////////

void FlowControl::resolutionCalculations(){
    QDesktopWidget *desktop = QApplication::desktop();
    // This line will assume that the current screen is the one where the experiments will be drawn.
    QRect screen = desktop->screenGeometry(desktop->screenNumber());
    configuration->addKeyValuePair(CONFIG_RESOLUTION_WIDTH,screen.width());
    configuration->addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,screen.height());
}

void FlowControl::setupSecondMonitor(){

    QDesktopWidget *desktop = QApplication::desktop();
    if (desktop->screenCount() < 2) {
        // There is nothing to do as there is no second monitor.
        if (monitor == nullptr) return;
        // In case the monitor was disconnected.
        logger.appendWarning("Attempting to restore situation in which second monitor seems to have been disconnected after it was created");
        if (monitor != nullptr){
            if (experiment != nullptr){
                disconnect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
                disconnect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
            }
            delete monitor;
            monitor = nullptr;
        }
        return;
    }

    // Nothing to do.
    if (monitor != nullptr) return;

    // Creating the second monitor.
    if (!configuration->getBool(CONFIG_DUAL_MONITOR_MODE)) return;

    // Setting up the monitor
    QRect secondScreen = desktop->screenGeometry(1);
    monitor = new MonitorScreen(Q_NULLPTR,secondScreen,configuration->getReal(CONFIG_RESOLUTION_WIDTH),configuration->getReal(CONFIG_RESOLUTION_HEIGHT));
}


////////////////////////////// REPORT REQUEST FUNCTIONS ///////////////////////////////////////

void FlowControl::prepareForReportListIteration(const QString &patientDirectory){
    selectedReport = -1;
    reportsForPatient.setDirectory(patientDirectory);
    reportsForPatient.prepareIteration();
}

QVariantMap FlowControl::nextReportInList(){
    return reportsForPatient.nextReportInfo();
}

void FlowControl::startDemoTransaction(){
    demoTransaction = true;
    onFileSetEmitted(QStringList());
}

void FlowControl::saveReport(){
    delayTimer.setInterval(200);
    delayTimer.start();
}

void FlowControl::drawReport(){
    delayTimer.stop();
    ImageReportDrawer drawer;
    if (!drawer.drawReport(reportsForPatient.getRepData(selectedReport),configuration)){
        logger.appendError("Could not save the requested report file to: " + configuration->getString(CONFIG_IMAGE_REPORT_PATH));
    }
    emit(reportGenerationDone());
}

void FlowControl::saveReportAs(const QString &title){
    QString newFileName = QFileDialog::getSaveFileName(nullptr,title,"","*.png");
    if (newFileName.isEmpty()) return;
    configuration->addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,newFileName);
    emit(reportGenerationRequested());
}

void FlowControl::requestReportData(bool fromEndOfSimulation){
    sslTransactionAllOk = false;
    if (fromEndOfSimulation){
        //currentRunFiles.clear();
        //currentRunFiles << "reading_2_2019_03_09_18_58.dat";
        emit(requestFileSet(currentRunFiles));
    }
    else{
        emit(requestFileSet(QStringList()));
    }
}

void FlowControl::onFileSetEmitted(const QStringList &fileSetAndName){

    //qWarning() << "Processing File Set: " << fileSetAndName;

    if (fileSetAndName.isEmpty() && !demoTransaction){
        //qWarning() << "File name set is empty";
        emit(sslTransactionFinished());
        return;
    }


    // The first value is the expected report name.
    QStringList fileSet = fileSetAndName;
    QString expectedFileName;
    if (!demoTransaction){
        //logger.appendStandard("Expected Report is: " + fileSetAndName.first());
        expectedFileName = fileSet.first();
        fileSet.removeFirst();
    }
    else{
        fileSet.clear();
        expectedFileName = "report_r_b2l_2010_06_03_09_00.rep";
        fileSet << "binding_bc_2010_06_03.dat" << "binding_uc_2010_06_03.dat" << "reading_2010_06_03.dat";
    }

    //qWarning() << "File set" << fileSet;

    // Generating the configuration file required to send to the server.
    QString expgenfile = configuration->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_EYE_REP_GEN_CONFIGURATION;
    // Making sure previous version of the file are not present so as to ensure that no garbage data is used in this current configuration.
    QFile(expgenfile).remove();

    // Setting the expected file name
    configuration->addKeyValuePair(CONFIG_REPORT_FILENAME,expectedFileName);

    QString error;
    QStringList toSave;

    toSave << CONFIG_DOCTOR_NAME << CONFIG_PATIENT_AGE << CONFIG_PATIENT_NAME << CONFIG_MOVING_WINDOW_DISP
           << CONFIG_MIN_FIXATION_LENGTH << CONFIG_SAMPLE_FREQUENCY << CONFIG_DISTANCE_2_MONITOR << CONFIG_XPX_2_MM << CONFIG_YPX_2_MM
           << CONFIG_LATENCY_ESCAPE_RAD << CONFIG_MARGIN_TARGET_HIT << CONFIG_REPORT_FILENAME;

    for (qint32 i = 0; i < toSave.size(); i++){
        error = configuration->saveValueToFile(expgenfile,COMMON_TEXT_CODEC,toSave.at(i));
        if (!error.isEmpty()){
            logger.appendError("WRITING EYE REP GEN FILE: " + error);
            sslTransactionAllOk = false;
            emit(sslTransactionFinished());
            return;
        }
    }

    //qWarning() << "Saved expgen";

    qint32 validEye;
    if (!demoTransaction) validEye = DatFileInfoInDir::getValidEyeForDatList(fileSet);
    else validEye = EYE_BOTH;

    error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_VALID_EYE,QString::number(validEye));
    if (!error.isEmpty()){
        logger.appendError("WRITING EYE REP GEN FILE: " + error);
        sslTransactionAllOk = false;
        emit(sslTransactionFinished());
        return;
    }

    error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_DAT_TIME_FILTER_THRESHOLD,"0");
    if (!error.isEmpty()){
        logger.appendError("WRITING EYE REP GEN FILE: " + error);
        sslTransactionAllOk = false;
        emit(sslTransactionFinished());
        return;
    }

    for (qint32 i = 0; i < fileSet.size(); i++){
        if (fileSet.at(i).startsWith(FILE_OUTPUT_READING)){
            error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_FILE_READING,fileSet.at(i));
            if (!error.isEmpty()){
                logger.appendError("WRITING EYE REP GEN FILE: " + error);
                sslTransactionAllOk = false;
                emit(sslTransactionFinished());
                return;
            }
        }
        else if (fileSet.at(i).startsWith(FILE_OUTPUT_BINDING_BC)){
            error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_FILE_BIDING_BC,fileSet.at(i));
            if (!error.isEmpty()){
                logger.appendError("WRITING EYE REP GEN FILE: " + error);
                sslTransactionAllOk = false;
                emit(sslTransactionFinished());
                return;
            }
        }
        else if (fileSet.at(i).startsWith(FILE_OUTPUT_BINDING_UC)){
            error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_FILE_BIDING_UC,fileSet.at(i));
            if (!error.isEmpty()){
                logger.appendError("WRITING EYE REP GEN FILE: " + error);
                sslTransactionAllOk = false;
                emit(sslTransactionFinished());
                return;
            }
        }
    }

    if (!demoTransaction) fileSetSentToProcess = fileSet;
    //qWarning() << "Requesting report for file set" << fileSetSentToProcess;
    sslDataProcessingClient->requestReport(!demoTransaction);

}

void FlowControl::onDisconnectionFinished(){
    sslTransactionAllOk = sslDataProcessingClient->getTransactionStatus();

    //qWarning() << "Transaction finished" << demoTransaction << sslTransactionAllOk;

    // If this is a demo transaction there is nothing else to do.
    if (demoTransaction){
        demoTransaction = false;
        emit(sslTransactionFinished());
        return;
    }

    if (sslTransactionAllOk) {
        moveProcessedFilesToProcessedFolder(fileSetSentToProcess);
    }
    emit(sslTransactionFinished());
}

////////////////////////////// FLOW CONTROL FUNCTIONS ///////////////////////////////////////

void FlowControl::eyeTrackerChanged(){
    if (eyeTracker != nullptr){
        if (experiment != nullptr){
            disconnect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
        }
        delete eyeTracker;
        eyeTracker = nullptr;
    }
    connected = false;
}

void FlowControl::connectToEyeTracker(){

    // Creating the connection with the EyeTracker
    if (eyeTracker != nullptr) delete eyeTracker;

    // The new et is NOT calibrated.
    calibrated = false;

    QString eyeTrackerSelected = configuration->getString(CONFIG_SELECTED_ET);

    //qWarning() << "CONNECTING TO ET. Selected" << eyeTrackerSelected;

    if (eyeTrackerSelected == CONFIG_P_ET_MOUSE){
        eyeTracker = new MouseInterface();
    }
#ifdef USE_IVIEW
    else if (eyeTrackerSelected == CONFIG_P_ET_REDM){
        eyeTracker = new REDInterface();
    }
#endif
    else if (eyeTrackerSelected == CONFIG_P_ET_GP3HD){
        //qWarning() << "Creating the Open Gaze ET";
        eyeTracker = new OpenGazeInterface(this,configuration->getReal(CONFIG_RESOLUTION_WIDTH),configuration->getReal(CONFIG_RESOLUTION_HEIGHT));
    }
    else{
        logger.appendError("Selected unknown EyeTracker: " + eyeTrackerSelected);
        return;
    }

    connect(eyeTracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
    eyeTracker->connectToEyeTracker();
}

void FlowControl::calibrateEyeTracker(){
    EyeTrackerCalibrationParameters calibrationParams;
    calibrationParams.forceCalibration = true;
    calibrationParams.name = "";
    calibrated = false;
    //qWarning() << "CALIBRATION STARTED";

    // Making sure the right eye is used, in both the calibration and the experiment.
    eyeTracker->setEyeToTransmit(configuration->getInt(CONFIG_VALID_EYE));

    eyeTracker->calibrate(calibrationParams);
}

void FlowControl::onEyeTrackerControl(quint8 code){
    //qWarning() << "ON EYETRACKER CONTROL" << code;
    switch(code){
    case EyeTrackerInterface::ET_CODE_CALIBRATION_ABORTED:
        calibrated = false;
        emit(calibrationDone(false));
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_DONE:
        calibrated = true;
        emit(calibrationDone(true));
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_FAILED:
        calibrated = false;
        emit(calibrationDone(false));
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_FAIL:
        connected = false;
        emit(connectedToEyeTracker(false));
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_SUCCESS:
        connected = true;
        emit(connectedToEyeTracker(true));
        break;
    case EyeTrackerInterface::ET_CODE_DISCONNECTED_FROM_ET:
        logger.appendError("Disconnected from EyeTracker");
        connected = false;
        break;
    }
}

bool FlowControl::startNewExperiment(qint32 experimentID){

    QBrush background;
    experimentIsOk = true;

    // Making sure no old reports are stored.
    configuration->addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,"");

    //logger.appendStandard("Starting experiment with ID " + QString::number(experimentID));
    currentRunFiles.clear();

    // Using the polimorphism, the experiment object is created according to the selected index.
    QString readingQuestions;
    switch (experimentID){
    case EXP_READING:
        readingQuestions = ":/experiment_data/Reading_" + configuration->getString(CONFIG_READING_EXP_LANG) + ".dat";
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,readingQuestions);
        experiment = new ReadingExperiment();
        background = QBrush(Qt::gray);
        break;
    case EXP_BINDING_BC:
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,getBindingExperiment(true));
        experiment = new ImageExperiment(true);
        background = QBrush(Qt::gray);
        break;
    case EXP_BINDING_UC:
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,getBindingExperiment(false));
        experiment = new ImageExperiment(false);
        background = QBrush(Qt::gray);
        break;
    case EXP_FIELDNG:
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,":/experiment_data/fielding.dat");
        experiment = new FieldingExperiment();
        background = QBrush(Qt::black);
        break;
    default:
        logger.appendError("Unknown experiment was selected " + QString::number(experimentID));
        return false;
    }

    // Hiding cursor UNLESS the ET is in mouse mode.
    if (configuration->getString(CONFIG_SELECTED_ET) != CONFIG_P_ET_MOUSE){
        experiment->hideCursor();
    }

    // Making sure that that both experiment signals are connected.
    // Eyetracker should be connected by this point.
    connect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    //connect(experiment,&Experiment::calibrationRequest,this,&FlowControl::requestCalibration);
    connect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);

    if (monitor != nullptr){
        connect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
        connect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
        monitor->setBackgroundBrush(background);
    }

    // Setting the debug mode according to the current configuration.
    experiment->setDebugMode(configuration->getBool(CONFIG_DUAL_MONITOR_MODE));

    // Making sure that the eyetracker is sending data.
    eyeTracker->enableUpdating(true);

    // Start the experiment.
    experiment->startExperiment(configuration);

    if (monitor != nullptr){
        if (configuration->getBool(CONFIG_DUAL_MONITOR_MODE)) {
            monitor->show();
        }
        else monitor->hide();
    }

    return true;

}

void FlowControl::on_experimentFinished(const Experiment::ExperimentResult &er){

    // Make the experiment windown invisible.
    experiment->hide();

    // Hide monitor if showing
    if (monitor != nullptr) monitor->hide();
    QFileInfo info(experiment->getDataFileLocation());

    switch (er){
    case Experiment::ER_ABORTED:
        logger.appendStandard("Experiment aborted");
        experimentIsOk = false;
        break;
    case Experiment::ER_FAILURE:
        logger.appendError("EXPERIMENT FAILURE: " + experiment->getError());
        experimentIsOk = false;
        break;
    case Experiment::ER_NORMAL:
        currentRunFiles << info.baseName() + ".dat";
        break;
    case Experiment::ER_WARNING:
        logger.appendWarning("EXPERIMENT WARNING: " + experiment->getError());
        currentRunFiles << info.baseName() + ".dat";
        break;
    }


    // Destroying the experiment and reactivating the start experiment.
    disconnect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    //disconnect(experiment,&Experiment::calibrationRequest,this,&FlowControl::requestCalibration);
    disconnect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
    if (monitor != nullptr){
        disconnect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
        disconnect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
    }
    delete experiment;
    experiment = nullptr;

    // Notifying the QML.
    emit(experimentHasFinished());

}

QString FlowControl::getBindingExperiment(bool bc){
    if (configuration->getInt(CONFIG_BINDING_NUMBER_OF_TARGETS) == 2){
        if (bc) return ":/experiment_data/bc.dat";
        else return ":/experiment_data/uc.dat";
    }
    else{
        if (bc) return ":/experiment_data/bc_3.dat";
        else return ":/experiment_data/uc_3.dat";
    }
}

////////////////////////////// MOVE PROCESSED FILES TO PROCESSED DIRECTORY ///////////////////////////////////////
void FlowControl::moveProcessedFilesToProcessedFolder(const QStringList &fileSet){
    QString patdir = DIRNAME_RAWDATA;
    patdir = patdir + "/" + configuration->getString(CONFIG_DOCTOR_UID)
            + "/" + configuration->getString(CONFIG_PATIENT_UID);

    if (!QDir(patdir + "/" + QString(DIRNAME_PROCESSED_DATA)).exists()){
        QDir dir(patdir);
        dir.mkdir(DIRNAME_PROCESSED_DATA);
    }

    QString pdatafolder = patdir + "/" + QString(DIRNAME_PROCESSED_DATA);
    if (!QDir(pdatafolder).exists()){
        logger.appendError("Could not create the processed data folder: " + pdatafolder);
        return;
    }

    // Copying each of the files to the processed folder and then deleting them.
    for (qint32 i = 0; i < fileSet.size(); i++){
        QString source = patdir + "/" + fileSet.at(i);
        QString destination = pdatafolder + "/" + fileSet.at(i);
        if (QFile::copy(source,destination)){
            if (!QFile(source).remove()){
                logger.appendError("REMOVING PROCESSED DATA. Could not delete " + source);
            }
        }
        else{
            logger.appendError("COPYING PROCESSED DATA. Could not copy " + source  + " to " + destination);
        }
    }
}

////////////////////////////// REPORT GENERATION FUNCTIONS ///////////////////////////////////////

void FlowControl::prepareSelectedReportIteration(){
    reportItems.clear();
    QVariantMap report = reportsForPatient.getRepData(selectedReport);

    ConfigurationManager text = ImageReportDrawer::loadReportText(configuration->getString(CONFIG_REPORT_LANGUAGE));
    QStringList titles = text.getStringList(DR_CONFG_RESULTS_NAME);
    QStringList explanations = text.getStringList(DR_CONFG_RES_CLARIFICATION);
    QStringList references = text.getStringList(DR_CONFG_RESULT_RANGES);

    if (report.contains(CONFIG_RESULTS_ATTENTIONAL_PROCESSES)){
        QString ans = report.value(CONFIG_RESULTS_ATTENTIONAL_PROCESSES).toString();
        if ((ans != "nan") && (ans != "0")){
            // Adding all the reading items.
            QStringList reading; reading << CONFIG_RESULTS_ATTENTIONAL_PROCESSES << CONFIG_RESULTS_EXECUTIVE_PROCESSES
                                         << CONFIG_RESULTS_WORKING_MEMORY << CONFIG_RESULTS_RETRIEVAL_MEMORY;
            addToReportItems(reading,report,titles,explanations,references);
        }

    }

    if (report.contains(CONFIG_RESULTS_MEMORY_ENCODING)){
        QString ans = report.value(CONFIG_RESULTS_MEMORY_ENCODING).toString();
        if ((ans != "nan") && (ans != "0")){
            QStringList binding; binding << CONFIG_RESULTS_MEMORY_ENCODING << CONFIG_RESULTS_BEHAVIOURAL_RESPONSE;
            addToReportItems(binding,report,titles,explanations,references);
        }
    }

    selectedReportItemIterator = 0;

}

void FlowControl::addToReportItems(const QStringList &items, const QVariantMap &report, const QStringList &titles,
                                   const QStringList &explanations, const QStringList &references){

    for (qint32 i = 0; i < items.size(); i++){
        QVariantMap map;
        qint32 index = reportTextDataIndexes.indexOf(items.at(i));
        ResultBar bar;
        bar.setResultType(items.at(i));
        bar.setValue(report.value(items.at(i)));
        qreal value = bar.getValue();
        qint32 indicator = bar.getSegmentBarIndex();

        map["vmTitleText"] = titles.at(index);
        if (index == 0) map ["vmExpText"] = "";
        else{
            QString exp = explanations.at(index-1);
            exp = exp.replace("\n","<br>");
            map["vmExpText"] = exp;
        }
        map["vmRefText"] = references.at(index);
        map["vmResValue"] = QString::number(value);
        map["vmResBarIndicator"] = QString::number(indicator);
        map["vmHasTwoSections"] = bar.hasTwoSections();
        reportItems << map;
    }
}


QStringList FlowControl::getSelectedReportInfo(){
    QVariantMap report = reportsForPatient.getRepData(selectedReport);
    QStringList ans;
    ans << report.value(CONFIG_DOCTOR_NAME).toString();
    ans << report.value(CONFIG_PATIENT_NAME).toString();
    ans << report.value(CONFIG_PATIENT_AGE).toString();
    ans << report.value(CONFIG_REPORT_DATE).toString();
    ans << report.value(CONFIG_RESULTS_FREQ_ERRORS_PRESENT).toString();
    return ans;
}

QVariantMap FlowControl::nextSelectedReportItem(){
    if (selectedReportItemIterator < reportItems.size()){
        selectedReportItemIterator++;
        return reportItems.at(selectedReportItemIterator-1);
    }
    else return QVariantMap();
}

void FlowControl::archiveSelectedFile(const QString &selectedFile){
    QStringList fileList;
    fileList << selectedFile;
    moveProcessedFilesToProcessedFolder(fileList);
}

///////////////////////////////////////////////////////////////////
FlowControl::~FlowControl(){

}
