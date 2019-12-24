#include "flowcontrol.h"

FlowControl::FlowControl(QWidget *parent, ConfigurationManager *c, UIConfigMap *ui) : QWidget(parent)
{

    // Intializing the eyetracker pointer
    configuration = c;
    eyeTracker = nullptr;
    connected = false;
    calibrated = false;
    experiment = nullptr;
    openvrco = nullptr;
    monitor = nullptr;
    demoTransaction = false;
    reprocessRequest = false;
    renderState = RENDERING_NONE;
    this->setVisible(false);

    uimap = ui;

    // For delay the what the UI shows before drawing a report.
    connect(&delayTimer,SIGNAL(timeout()),this,SLOT(drawReport()));

    // Creating the sslclient
    sslDataProcessingClient = new SSLDataProcessingClient(this,c);

    // Connection to the "finished" slot.
    connect(sslDataProcessingClient,SIGNAL(transactionFinished()),this,SLOT(onDisconnectionFinished()));

    // This infomration should only be updated if the report text is updated. First is the title, then the exp index and the ref index.
    reportTextDataIndexes << CONFIG_RESULTS_READ_PREDICTED_DETERIORATION << CONFIG_RESULTS_EXECUTIVE_PROCESSES << CONFIG_RESULTS_WORKING_MEMORY
                          << CONFIG_RESULTS_RETRIEVAL_MEMORY << CONFIG_RESULTS_BINDING_CONVERSION_INDEX << CONFIG_RESULTS_BEHAVIOURAL_RESPONSE;

    // Creating the OpenVR Object if so defined.
    if (c->getBool(CONFIG_VR_ENABLED)){
        openvrco = new OpenVRControlObject(this);
        connect(openvrco,SIGNAL(requestUpdate()),this,SLOT(onRequestUpdate()));

        // Intializing the OpenVR Solution.
        if (!openvrco->startRendering()){
            logger.appendError("OpenVRControl Object intilization failed. Will not be creating the wait screen");
            return;
        }

        // If rendering was started, is now stopped so as not to consume resources.
        openvrco->stopRendering();

        // Create the wait screen only once
        waitScreenBaseColor = QColor(Qt::gray);

        QImage logo(":/CommonClasses/PNGWriter/report_text/viewmind.png");
        if (logo.isNull()){
            logger.appendError("The loaded wait screen image is null");
            return;
        }
        QSize s = openvrco->getRecommendedSize();
        openvrco->setScreenColor(waitScreenBaseColor);
        logo = logo.scaled(s.width(),s.height(),Qt::KeepAspectRatio);

        waitScreen = QImage(s.width(), s.height(), QImage::Format_RGB32);
        QPainter painter(&waitScreen);
        painter.fillRect(0,0,s.width(),s.height(),QBrush(waitScreenBaseColor));
        qreal xoffset = (s.width() - logo.width())/2;
        qreal yoffset = (s.height() - logo.height())/2;

        QRectF source(0,0,logo.width(),logo.height());
        QRectF target(xoffset,yoffset,logo.width(),logo.height());
        painter.drawImage(target,logo,source);
        painter.end();
    }

    /// TEST FOR FREQ CHECK
    // configuration->addKeyValuePair(CONFIG_PATIENT_UID,"1242673082_0000_P0005");
    // doFrequencyAnalysis("reading_es_2_2019_03_22_20_59.dat");

}

////////////////////////////// AUXILIARY FUNCTIONS ///////////////////////////////////////

void FlowControl::resolutionCalculations(){
    QDesktopWidget *desktop = QApplication::desktop();
    // This line will assume that the current screen is the one where the experiments will be drawn.

    /// Depracated code.
    /// QRect screen = desktop->screenGeometry(desktop->screenNumber());
    QList<QScreen*> screens = QGuiApplication::screens();
    QRect screen = screens.at(desktop->screenNumber())->geometry();
    configuration->addKeyValuePair(CONFIG_PRIMARY_MONITOR_WIDTH,screen.width());
    configuration->addKeyValuePair(CONFIG_PRIMARY_MONITOR_HEIGHT,screen.height());

    if (openvrco != nullptr){
        // This menas the resolution for drawing should be the one recommende by OpenVR.
        QSize s = openvrco->getRecommendedSize();
        configuration->addKeyValuePair(CONFIG_VR_RECOMMENDED_WIDTH, s.width());
        configuration->addKeyValuePair(CONFIG_VR_RECOMMENDED_HEIGHT,s.height());
    }

}

void FlowControl::setupSecondMonitor(){

    /// Depracated code
    /// QDesktopWidget *desktop = QApplication::desktop();
    QList<QScreen*> screens = QGuiApplication::screens();

    if (screens.size() < 2) {
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

    // Setting up the monitor. Assuming 1 is the SECONDARY MONITOR.
    QRect secondScreen = screens.at(1)->geometry();
    monitor = new MonitorScreen(Q_NULLPTR,secondScreen,configuration->getReal(CONFIG_RESOLUTION_WIDTH),configuration->getReal(CONFIG_RESOLUTION_HEIGHT));
}

void FlowControl::keyboardKeyPressed(int key){
    if (experiment != nullptr){
        experiment->keyboardKeyPressed(key);
    }
}

void FlowControl::stopRenderingVR(){
    //qDebug() << "On FlowControl::stopRenderingVR()";
    if (openvrco != nullptr){
        openvrco->stopRendering();
    }
    if (eyeTracker != nullptr){
        eyeTracker->enableUpdating(false); // This is specially important to stop the VIVE EYE Poller
    }
}

////////////////////////////// REPORT REQUEST FUNCTIONS ///////////////////////////////////////

void FlowControl::prepareForReportListIteration(const QString &patientDirectory){
    selectedReport = -1;
    RepFileInfo::AlgorithmVersions algver;
    algver.bindingAlg = configuration->getInt(CONFIG_BINDING_ALG_VERSION);
    algver.readingAlg = configuration->getInt(CONFIG_READING_ALG_VERSION);
    reportsForPatient.setDirectory(patientDirectory,algver);
    reportsForPatient.prepareIteration();
}

QVariantMap FlowControl::nextReportInList(){
    return reportsForPatient.nextReportInfo();
}

void FlowControl::startDemoTransaction(){
    demoTransaction = true;
    onFileSetEmitted(QStringList(),"");
}

void FlowControl::saveReport(){
    delayTimer.setInterval(200);
    delayTimer.start();
}

void FlowControl::drawReport(){
    delayTimer.stop();
    ImageReportDrawer drawer;
    QVariantMap repmap = reportsForPatient.getRepData(selectedReport);
    repmap[CONFIG_DOCTOR_NAME] = configuration->getString(CONFIG_DOCTOR_NAME);
    repmap[CONFIG_PATIENT_NAME] = configuration->getString(CONFIG_PATIENT_NAME);
    if (!drawer.drawReport(repmap,configuration,uimap->getIndexBehavioral())){
        logger.appendError("Could not save the requested report file to: " + configuration->getString(CONFIG_IMAGE_REPORT_PATH));
    }
    emit(reportGenerationDone());
}

void FlowControl::saveReportAs(const QString &title){
    QString newFileName = QFileDialog::getSaveFileName(nullptr,title,"","*.png");
    //QString newFileName = "test.png";
    if (newFileName.isEmpty()) return;
    configuration->addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,newFileName);
    emit(reportGenerationRequested());
}

void FlowControl::requestReportData(){
    sslTransactionAllOk = false;
    reprocessRequest = false;
    emit(requestFileSet());
}

void FlowControl::requestDataReprocessing(const QString &reportName, const QString &fileList, const QString &evaluationID){
    sslTransactionAllOk = false;
    reprocessRequest = true;
    QStringList fileSet;
    fileSet << reportName << fileList.split("|",QString::SkipEmptyParts);
    onFileSetEmitted(fileSet,evaluationID);
}

void FlowControl::onFileSetEmitted(const QStringList &fileSetAndName, const QString &evaluationID){

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
        expectedFileName = "report_REN_B2L2_2010_06_03_10_15.rep";
        fileSet << "binding_bc_2_l_2_2010_06_03_10_00.dat" << "binding_uc_2_l_2_2010_03_06_10_03.dat" << "reading_2_2010_06_03_10_15.dat";
    }

    //qWarning() << "File set" << fileSet;

    // Generating the configuration file required to send to the server.
    QString expgenfile = configuration->getString(CONFIG_PATIENT_DIRECTORY) + "/" + FILE_EYE_REP_GEN_CONFIGURATION;
    // Making sure previous version of the file are not present so as to ensure that no garbage data is used in this current configuration.
    QFile(expgenfile).remove();

    // Setting the expected file name
    configuration->addKeyValuePair(CONFIG_REPORT_FILENAME,expectedFileName);

    QString error;
    QString oldRepFile = "";
    QStringList toSave;

    // Setting the protocol name

    toSave << CONFIG_PATIENT_AGE
              // Processing Parameters
           << CONFIG_MOVING_WINDOW_DISP << CONFIG_MIN_FIXATION_LENGTH << CONFIG_SAMPLE_FREQUENCY
           << CONFIG_DISTANCE_2_MONITOR << CONFIG_XPX_2_MM << CONFIG_YPX_2_MM
           << CONFIG_LATENCY_ESCAPE_RAD << CONFIG_MARGIN_TARGET_HIT << CONFIG_FIELDING_XPX_2_MM << CONFIG_FIELDING_YPX_2_MM
           << CONFIG_REPORT_FILENAME << CONFIG_PROTOCOL_NAME << CONFIG_VR_ENABLED
              // Frequency check parameters
           << CONFIG_TOL_MAX_PERIOD_TOL << CONFIG_TOL_MIN_PERIOD_TOL
           << CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL << CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL
           << CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES << CONFIG_TOL_NUM_ALLOWED_FAILED_DATA_SETS
           << CONFIG_TOL_NUM_MIN_PTS_IN_FIELDING_TRIAL
              // Record keeping parameters.
           << CONFIG_INST_ETSERIAL;

    if (reprocessRequest){
        // This means that the report file name exists, so it needs to be loaded.
        ConfigurationManager existingRepFile;

        // This will be sent to the server.
        oldRepFile = expectedFileName;

        if (!existingRepFile.loadConfiguration(configuration->getString(CONFIG_PATIENT_DIRECTORY) + "/" + expectedFileName,COMMON_TEXT_CODEC)){
            logger.appendError("While attempting to reprocess file: " + configuration->getString(CONFIG_PATIENT_DIRECTORY) + "/" + expectedFileName
                               + " got error on loading: " + existingRepFile.getError());
            sslTransactionAllOk = false;
            reprocessRequest = false;
            emit(sslTransactionFinished());
            return;
        }
        ConfigurationManager eyerepgen;
        for (qint32 i = 0; i < toSave.size(); i++){
            if (existingRepFile.containsKeyword(toSave.at(i))) eyerepgen.addKeyValuePair(toSave.at(i),existingRepFile.getString(toSave.at(i)));
            else eyerepgen.addKeyValuePair(toSave.at(i),configuration->getString(toSave.at(i)));
        }

        eyerepgen.addKeyValuePair(CONFIG_REPROCESS_REQUEST,true);
        eyerepgen.addKeyValuePair(CONFIG_DEMO_MODE,false);

        if (!eyerepgen.saveToFile(expgenfile,COMMON_TEXT_CODEC)){
            logger.appendError("WRITING EYE REP GEN FILE in reprocessing: " + expgenfile + ", could not open file for writing");
            sslTransactionAllOk = false;
            reprocessRequest = false;
            emit(sslTransactionFinished());
            return;
        }
    }
    else{
        for (qint32 i = 0; i < toSave.size(); i++){
            error = configuration->saveValueToFile(expgenfile,COMMON_TEXT_CODEC,toSave.at(i));
            if (!error.isEmpty()){
                logger.appendError("WRITING EYE REP GEN FILE: " + error);
                sslTransactionAllOk = false;
                emit(sslTransactionFinished());
                return;
            }
        }

        QString demoStr;
        if (demoTransaction) demoStr = "true";
        else demoStr = "false";

        error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_DEMO_MODE,demoStr);
        if (!error.isEmpty()){
            logger.appendError("WRITING EYE REP GEN FILE: " + error);
            sslTransactionAllOk = false;
            emit(sslTransactionFinished());
            return;
        }

    }

    //qWarning() << "Saved expgen";
    //qWarning() << "Evaluation ID: " << evaluationID;

    error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_RESULT_ENTRY_ID,evaluationID);
    if (!error.isEmpty()){
        logger.appendError("WRITING EYE REP GEN FILE: " + error);
        sslTransactionAllOk = false;
        emit(sslTransactionFinished());
        return;
    }

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
        if (fileSet.at(i).startsWith(FILE_OUTPUT_FIELDING)){
            error = ConfigurationManager::setValue(expgenfile,COMMON_TEXT_CODEC,CONFIG_FILE_FIELDING,fileSet.at(i));
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
    sslDataProcessingClient->requestReport(demoTransaction,oldRepFile);

}

void FlowControl::sendMedicalRecordsToServer(const QString &patid){
    sslTransactionAllOk = false;
    reprocessRequest = false;

    // Creating the conf file with the minimal information required by the server.
    QString expgenfile = QString(DIRNAME_RAWDATA) + "/" + patid + "/" + FILE_EYE_REP_GEN_CONFIGURATION;
    QFile(expgenfile).remove();

    ConfigurationManager eyerepgen;
    eyerepgen.addKeyValuePair(CONFIG_REPROCESS_REQUEST,false);
    eyerepgen.addKeyValuePair(CONFIG_DEMO_MODE,false);

    if (!eyerepgen.saveToFile(expgenfile,COMMON_TEXT_CODEC)){
        logger.appendError("WRITING EYE REP GEN FILE for medical records: " + expgenfile + ", could not open file for writing");
        sslTransactionAllOk = false;
        reprocessRequest = false;
        emit(sslTransactionFinished());
        return;
    }
    sslDataProcessingClient->sendMedicalRecordData(patid);

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

    if ((sslTransactionAllOk) && (!reprocessRequest)) {
        moveProcessedFilesToProcessedFolder(fileSetSentToProcess);
    }

    // Ensuring that the preocessing is done.
    reprocessRequest = false;

    emit(sslTransactionFinished());
}

////////////////////////////// FLOW CONTROL FUNCTIONS ///////////////////////////////////////

void FlowControl::onRequestUpdate(){
    if (renderState == RENDERING_EXPERIMENT){
        if (eyeTracker == nullptr) {
            logger.appendWarning("VR Update Request: Rendering experiment with a non existant eyeTracker object");
            return;
        }
        if (experiment == nullptr) {
            logger.appendWarning("VR Update Request: Rendering experiment with a non existant experiment object");
            return;
        }
        EyeTrackerData data = eyeTracker->getLastData();
        QImage image = experiment->getVRDisplayImage();
        displayImage = image;
        QPainter painter(&displayImage);
        qreal r = 0.01*displayImage.width();
        painter.setBrush(QBrush(QColor(0,255,0,100)));
        painter.drawEllipse(static_cast<qint32>(data.xLeft-r),static_cast<qint32>(data.yLeft-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r));
        painter.setBrush(QBrush(QColor(0,0,255,100)));
        painter.drawEllipse(static_cast<qint32>(data.xRight-r),static_cast<qint32>(data.yRight-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r));
        openvrco->setImage(&image);
        emit(newImageAvailable());
    }
    if (renderState == RENDER_WAIT_SCREEN){
        if (displayImage != waitScreen){
            displayImage = waitScreen;
            openvrco->setScreenColor(waitScreenBaseColor);
            openvrco->setImage(&displayImage);
            emit(newImageAvailable());
        }
    }
}

QImage FlowControl::image() const{
    return displayImage;
}

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

    logger.appendStandard("Creating new EyeTracker Object");

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
    else if (eyeTrackerSelected == CONFIG_P_ET_HTCVIVEEYEPRO){
        QSize s = openvrco->getRecommendedSize();
        eyeTracker = new HTCViveEyeProEyeTrackingInterface(this,s.width(),s.height());
    }
    else{
        logger.appendError("Selected unknown EyeTracker: " + eyeTrackerSelected);
        return;
    }

    logger.appendStandard("Connecting to EyeTracker: " + eyeTrackerSelected + "....");
    connect(eyeTracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
    connect(openvrco,SIGNAL(newProjectionMatrixes(QMatrix4x4,QMatrix4x4)),eyeTracker,SLOT(updateProjectionMatrices(QMatrix4x4,QMatrix4x4)));
    eyeTracker->connectToEyeTracker();
}

void FlowControl::calibrateEyeTracker(){

    //qDebug() << "In FlowControl::calibrateEyeTracker";

    EyeTrackerCalibrationParameters calibrationParams;
    calibrationParams.forceCalibration = true;
    calibrationParams.name = "";
    calibrated = false;

    // Making sure the right eye is used, in both the calibration and the experiment.
    eyeTracker->setEyeToTransmit(static_cast<quint8>(configuration->getInt(CONFIG_VALID_EYE)));

    // Required during calibration.
    renderState = RENDERING_NONE;

    logger.appendStandard("Calibrating the eyetracker....");
    if (openvrco != nullptr) {
        if (!openvrco->startRendering()){
            qDebug() << "Failed to start rendering upon calibration";
        }
        else{
            openvrco->setScreenColor(QColor(Qt::gray));
        }
    }

    //qDebug() << "In FlowwControl::calibrateEyeTracker, is VR ENABLED?"  <<configuration->getBool(CONFIG_VR_ENABLED);

    if (configuration->getBool(CONFIG_VR_ENABLED)) eyeTracker->enableUpdating(true); // To ensure that eyetracking data is gathered.
    eyeTracker->calibrate(calibrationParams);
}

void FlowControl::onEyeTrackerControl(quint8 code){
    //qDebug() << "ON FlowControl::EYETRACKER CONTROL" << code;

    if (eyeTracker == nullptr){
        logger.appendError("Possible crash alert: Entering onEyeTrackerControl with a null pointer eyeTracker object");
    }

    switch(code){
    case EyeTrackerInterface::ET_CODE_CALIBRATION_ABORTED:
        logger.appendWarning("EyeTracker Control: Calibration aborted");
        calibrated = false;
        if (configuration->getBool(CONFIG_VR_ENABLED)) eyeTracker->enableUpdating(false);
        renderState = RENDER_WAIT_SCREEN;
        emit(calibrationDone(false));
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_DONE:
        logger.appendStandard("EyeTracker Control: Calibration done successfully");
        calibrated = true;
        if (configuration->getBool(CONFIG_VR_ENABLED)) eyeTracker->enableUpdating(false);
        renderState = RENDER_WAIT_SCREEN;
        emit(calibrationDone(true));
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_FAILED:
        logger.appendError("EyeTracker Control: Calibration Failed");
        calibrated = false;
        if (configuration->getBool(CONFIG_VR_ENABLED)) eyeTracker->enableUpdating(false);
        renderState = RENDER_WAIT_SCREEN;
        emit(calibrationDone(false));
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_FAIL:
        logger.appendError("EyeTracker Control: Connection to EyeTracker Failed");
        connected = false;
        emit(connectedToEyeTracker(false));
        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_SUCCESS:
        logger.appendStandard("EyeTracker Control: Connection to EyeTracker Established");
        connected = true;
        emit(connectedToEyeTracker(true));
        break;
    case EyeTrackerInterface::ET_CODE_DISCONNECTED_FROM_ET:
        logger.appendError("EyeTracker Control: Disconnected from EyeTracker");
        connected = false;
        break;
    case EyeTrackerInterface::ET_CODE_NEW_CALIBRATION_IMAGE_AVAILABLE:
        displayImage = eyeTracker->getCalibrationImage();
        openvrco->setImage(&displayImage);
        emit(newImageAvailable());
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
        logger.appendStandard("STARTING READING EXPERIMENT");
        readingQuestions = ":/experiment_data/Reading_" + configuration->getString(CONFIG_READING_EXP_LANG) + ".dat";
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,readingQuestions);
        experiment = new ReadingExperiment();
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray).darker(110));
        break;
    case EXP_BINDING_BC:
        logger.appendStandard("STARTING BINDING BC EXPERÏMENT");
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,getBindingExperiment(true));
        experiment = new ImageExperiment(true);
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray));
        break;
    case EXP_BINDING_UC:
        logger.appendStandard("STARTING BINDING UC EXPERÏMENT");
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,getBindingExperiment(false));
        experiment = new ImageExperiment(false);
        background = QBrush(Qt::gray);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::gray));
        break;
    case EXP_FIELDNG:
        logger.appendStandard("STARTING FIELDING (N BACK TRACE)");
        configuration->addKeyValuePair(CONFIG_EXP_CONFIG_FILE,":/experiment_data/fielding.dat");
        experiment = new FieldingExperiment();
        background = QBrush(Qt::black);
        if (openvrco != nullptr) openvrco->setScreenColor(QColor(Qt::black));
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
    connect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
    connect(experiment,&Experiment::updateVRDisplay,this,&FlowControl::onRequestUpdate);

    if ( (monitor != nullptr) && (!configuration->getBool(CONFIG_VR_ENABLED)) ){
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

    if (openvrco != nullptr){
        if (!openvrco->startRendering()){
            logger.appendError("Could not start rendering upon starritn experiment");
            return false;
        }
        renderState = RENDERING_EXPERIMENT;
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
        logger.appendStandard("EXPERIMENT aborted");
        experimentIsOk = false;
        break;
    case Experiment::ER_FAILURE:
        logger.appendError("EXPERIMENT FAILURE: " + experiment->getError());
        experimentIsOk = false;
        break;
    case Experiment::ER_NORMAL:
        logger.appendSuccess("EXPERIMENT Finshed Successfully");
        currentRunFiles << info.baseName() + ".dat";
        break;
    case Experiment::ER_WARNING:
        logger.appendWarning("EXPERIMENT WARNING: " + experiment->getError());
        currentRunFiles << info.baseName() + ".dat";
        break;
    }

    // Doing the frequency check.
    frequencyErrorsPresent = false;
    if (experimentIsOk){
        if (!configuration->getBool(CONFIG_DEMO_MODE) && !configuration->getBool(CONFIG_USE_MOUSE)){
            //            qDebug() << "IN FLOW CONTROL. ON EXPERIMENT FINISHED. FREQUENCY ERROR CHECK HAS BEEN DISABLED";
            if (!experiment->doFrequencyCheck()){
                logger.appendError(experiment->getError());
                experimentIsOk = false;
                frequencyErrorsPresent = true;
            }
        }
    }

    // Destroying the experiment and reactivating the start experiment.
    disconnect(experiment,&Experiment::experimentEndend,this,&FlowControl::on_experimentFinished);
    disconnect(eyeTracker,&EyeTrackerInterface::newDataAvailable,experiment,&Experiment::newEyeDataAvailable);
    disconnect(experiment,&Experiment::updateVRDisplay,this,&FlowControl::onRequestUpdate);

    if (monitor != nullptr){
        disconnect(experiment,&Experiment::updateBackground,monitor,&MonitorScreen::updateBackground);
        disconnect(experiment,&Experiment::updateEyePositions,monitor,&MonitorScreen::updateEyePositions);
    }
    delete experiment;
    experiment = nullptr;

    // This should make the user of the HMD see the wait screen until the next experiment.
    renderState = RENDER_WAIT_SCREEN;

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
    patdir = patdir + "/" + configuration->getString(CONFIG_PATIENT_UID);

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

void FlowControl::moveFileToArchivedFileFolder(const QString &filename){
    QString patdir = DIRNAME_RAWDATA;
    patdir = patdir + "/" + configuration->getString(CONFIG_PATIENT_UID);

    if (!QDir(patdir + "/" + QString(DIRNAME_ARCHIVE)).exists()){
        QDir dir(patdir);
        dir.mkdir(DIRNAME_ARCHIVE);
    }

    QString pdatafolder = patdir + "/" + QString(DIRNAME_ARCHIVE);
    if (!QDir(pdatafolder).exists()){
        logger.appendError("Could not create the archive data folder: " + pdatafolder);
        return;
    }

    // Copying each of the files to the archive folder and then deleting them. Also adding the time stamp.
    QString timestamp = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm");

    QString source = patdir + "/" + filename;
    QString destination = pdatafolder + "/" + filename + "." + timestamp;
    if (QFile::copy(source,destination)){
        if (!QFile(source).remove()){
            logger.appendError("REMOVING ARCHIVED DATA. Could not delete " + source);
        }
    }
    else{
        logger.appendError("COPYING ARCHIVED DATA. Could not copy " + source  + " to " + destination);
    }

}

////////////////////////////// REPORT GENERATION FUNCTIONS ///////////////////////////////////////

void FlowControl::prepareSelectedReportIteration(){
    reportItems.clear();

    /// DEBUG CODE
    ///reportsForPatient.setDirectory("C:/Users/Viewmind/Documents/viewmind_projects/VMSoftwareSuite/EyeExperimenter/exe32/viewmind_etdata/0_0000_P0000",RepFileInfo::AlgorithmVersions());
    ///selectedReport = 0;
    /// END DEBUG CODE

    QVariantMap report = reportsForPatient.getRepData(selectedReport);

    ConfigurationManager text = ImageReportDrawer::loadReportText(configuration->getString(CONFIG_REPORT_LANGUAGE));
    QStringList titles = text.getStringList(DR_CONFG_RESULTS_NAME);
    QStringList explanations = text.getStringList(DR_CONFG_RES_CLARIFICATION);
    QStringList references = text.getStringList(DR_CONFG_RESULT_RANGES);

    diagnosisClassText.clear();
    diagnosisClassText << ImageReportDrawer::cleanNewLines(text.getString(DR_CONFG_DISCLAIMER));
    resultBarSummary.reset();

    if (report.contains(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION)){
        QString ans = report.value(CONFIG_RESULTS_ATTENTIONAL_PROCESSES).toString();
        if ((ans != "nan") && (ans != "0")){
            // Adding all the reading items.
            QStringList reading;
            reading << CONFIG_RESULTS_READ_PREDICTED_DETERIORATION << CONFIG_RESULTS_EXECUTIVE_PROCESSES
                    << CONFIG_RESULTS_WORKING_MEMORY << CONFIG_RESULTS_RETRIEVAL_MEMORY;
            addToReportItems(reading,report,titles,explanations,references);
        }

    }

    if (report.contains(CONFIG_RESULTS_BINDING_CONVERSION_INDEX)){
        QString ans = report.value(CONFIG_RESULTS_BINDING_CONVERSION_INDEX).toString();
        if ((ans != "nan") && (ans != "0")){
            QStringList binding;
            if (uimap->getIndexBehavioral() == "I") binding << CONFIG_RESULTS_BINDING_CONVERSION_INDEX;
            else if (uimap->getIndexBehavioral() == "B") binding << CONFIG_RESULTS_BEHAVIOURAL_RESPONSE;
            addToReportItems(binding,report,titles,explanations,references);
        }
    }

    selectedReportItemIterator = 0;

    // Getting the diagnosis class.
    QString diag_class_key = DR_CONFG_DIAG_CLASS;
    diag_class_key = diag_class_key + resultBarSummary.getDiagnosisClass();
    //qDebug() << "DIAG CLASS SUMMARY:";
    //qDebug().noquote() << "   " + resultBarSummary.toString("\n   ");
    //qDebug() << "RESULT" << diag_class_key;
    //diag_class_key = "class_2";
    diagnosisClassText <<  ImageReportDrawer::cleanNewLines(text.getString(diag_class_key)) << text.getString(DR_CONFG_DIAGNOSIS_TITLE);


}

void FlowControl::addToReportItems(const QStringList &items, const QVariantMap &report, const QStringList &titles,
                                   const QStringList &explanations, const QStringList &references){


    for (qint32 i = 0; i < items.size(); i++){
        QVariantMap map;
        qint32 index = reportTextDataIndexes.indexOf(items.at(i));
        ResultBar bar;

        bar.setResultType(items.at(i));
        bar.setValue(report.value(items.at(i)));

        // Adding the bar segment to determine the diagnosis class.
        resultBarSummary.setIDX(bar);

        qint32 indicator = bar.getSegmentBarIndex();

        map["vmTitleText"] = titles.at(index);
        if (index == 0) map ["vmExpText"] = "";
        else{
            QString exp = explanations.at(index-1);
            exp = exp.replace("/n","<br>");
            map["vmExpText"] = exp;
        }
        map["vmRefText"] = references.at(index);
        map["vmResValue"] = bar.getValue();
        map["vmResBarIndicator"] = QString::number(indicator);
        map["vmHasTwoSections"] = bar.hasTwoSections();
        reportItems << map;
    }
}

QStringList FlowControl::getDiagnosticClass(){
    return diagnosisClassText;
}


QStringList FlowControl::getSelectedReportInfo(){
    QVariantMap report = reportsForPatient.getRepData(selectedReport);
    QStringList ans;
    ans << configuration->getString(CONFIG_DOCTOR_NAME);
    if (uimap->getStructure() == "S") ans << configuration->getString(CONFIG_PATIENT_DISPLAYID);
    else if (uimap->getStructure() == "P") ans << configuration->getString(CONFIG_PATIENT_NAME);

    /// PATCH: When the age is not present the age is the year.
    qint32 patient_age = report.value(CONFIG_PATIENT_AGE).toInt();
    if ((patient_age > 120) || (patient_age < 10)){
        ans << "N/A";
    }
    else ans << QString::number(patient_age);

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

void FlowControl::doFrequencyAnalysis(const QString &filename){

    FreqAnalysis freqChecker;
    FreqAnalysis::FreqAnalysisResult fres;

    // Froming path
    QString pathToFile = QString(DIRNAME_RAWDATA) + "/" + configuration->getString(CONFIG_PATIENT_UID) + "/" + filename;

    fres = freqChecker.analyzeFile(pathToFile);
    QString outputFile = pathToFile + ".gflog";

    QString freqReport;

    if (!fres.errorList.isEmpty()){
        freqReport = "FREQ ANALYSIS ERROR: \n   " + fres.errorList.join("\n   ");
        logger.appendStandard(freqReport);
        return;
    }
    else {

        FreqAnalysis::FreqCheckParameters fcp;
        fcp.fexpected                             = configuration->getReal(CONFIG_SAMPLE_FREQUENCY);
        fcp.periodMax                             = configuration->getReal(CONFIG_TOL_MAX_PERIOD_TOL);
        fcp.periodMin                             = configuration->getReal(CONFIG_TOL_MIN_PERIOD_TOL);
        fcp.maxAllowedFreqGlitchesPerTrial        = configuration->getReal(CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL);
        fcp.maxAllowedPercentOfInvalidValues      = configuration->getReal(CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES);
        fcp.maxAllowedFailedTrials                = configuration->getInt(CONFIG_TOL_NUM_ALLOWED_FAILED_DATA_SETS);
        if (!filename.startsWith(FILE_OUTPUT_FIELDING)){
            fcp.minNumberOfDataItems                  = configuration->getInt(CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL);
        }
        else{
            fcp.minNumberOfDataItems                = configuration->getInt(CONFIG_TOL_NUM_MIN_PTS_IN_FIELDING_TRIAL);
        }
        fres.analysisValid(fcp);
        freqReport = "FREQ ANALYSIS REPORT: Avg Frequency: " + QString::number(fres.averageFrequency) + "\n   ";
        freqReport = freqReport + fres.errorList.join("\n   ");
        freqReport = freqReport  + "\n   Individual Freq Errors:\n   " + fres.individualErrorList.join("\n   ");
    }

    QFile file(outputFile);
    if (!file.open(QFile::WriteOnly)){
        logger.appendError("Could not write requested frequency analysis output to write output to: " + outputFile);
        logger.appendStandard(freqReport);
    }
    else{
        QTextStream writer(&file);
        writer << freqReport;
        file.close();
    }

}

///////////////////////////////////////////////////////////////////
FlowControl::~FlowControl(){

}
