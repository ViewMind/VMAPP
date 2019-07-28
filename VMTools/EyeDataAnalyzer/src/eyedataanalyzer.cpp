#include "eyedataanalyzer.h"
#include "ui_eyedataanalyzer.h"

EyeDataAnalyzer::EyeDataAnalyzer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::EyeDataAnalyzer)
{
    ui->setupUi(this);
    logForProcessing.setGraphicalLogInterface();
    logForDB.setGraphicalLogInterface();
    connect(&logForProcessing,SIGNAL(newUiMessage(QString)),this,SLOT(on_newUIMessage(QString)));
    connect(&logForDB,SIGNAL(newUiMessage(QString)),this,SLOT(on_newUIMessage_for_DB(QString)));
    this->setWindowTitle(QString(PROGRAM_NAME) + " - " + QString(PROGRAM_VERSION));

    appViews.append(ui->groupBoxDataBaseView);
    appViews.append(ui->groupBoxProcessingView);

    // Default values
    defaultReportCompletionParameters.addKeyValuePair(CONFIG_DOCTOR_NAME,"No Doctor");
    defaultReportCompletionParameters.addKeyValuePair(CONFIG_PATIENT_AGE,"0");
    defaultReportCompletionParameters.addKeyValuePair(CONFIG_PATIENT_NAME,"Unnamed");

    // Default processing parameters
    defaultValues.addKeyValuePair(CONFIG_MOVING_WINDOW_DISP,100);
    defaultValues.addKeyValuePair(CONFIG_MIN_FIXATION_LENGTH,50);
    defaultValues.addKeyValuePair(CONFIG_SAMPLE_FREQUENCY,150);
    defaultValues.addKeyValuePair(CONFIG_DISTANCE_2_MONITOR,60);
    defaultValues.addKeyValuePair(CONFIG_XPX_2_MM,0.25);
    defaultValues.addKeyValuePair(CONFIG_YPX_2_MM,0.25);
    defaultValues.addKeyValuePair(CONFIG_LATENCY_ESCAPE_RAD,80);
    defaultValues.addKeyValuePair(CONFIG_MARGIN_TARGET_HIT,10);
    defaultValues.addKeyValuePair(CONFIG_VALID_EYE,2);
    defaultValues.addKeyValuePair(CONFIG_DAT_TIME_FILTER_THRESHOLD,0);

    // Frequency analysis paramters
    defaultValues.addKeyValuePair(CONFIG_TOL_MAX_PERIOD_TOL,8);
    defaultValues.addKeyValuePair(CONFIG_TOL_MIN_PERIOD_TOL,4);
    defaultValues.addKeyValuePair(CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL,10);
    defaultValues.addKeyValuePair(CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES,3);
    defaultValues.addKeyValuePair(CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL,50);
    defaultValues.addKeyValuePair(CONFIG_TOL_NUM_ALLOWED_FAILED_DATA_SETS,0);

    // The existing patient name db
    patNameMng.loadPatNameDB();
    //patNameMng.printMap();

    //switchViews(VIEW_1_PROCESSING_VIEW);
    switchViews(VIEW_0_DATABASE_VIEW);
    currentDirectory = WORK_DIR;
    processDirectory();

    ui->pbGetData->setEnabled(false);
    waitDiag = new WaitDialog(this);

    connect(&batchProcessor,&QThread::finished,this,&EyeDataAnalyzer::on_batchProcessing_Done);
    connect(&batchProcessor,&BatchCSVProcessing::updateAdvance,waitDiag,&WaitDialog::setProgressBarValue);
    batchProcessor.setDBConnect(&patNameMng);

    if (!QSslSocket::supportsSsl()){
        QMessageBox::critical(this,"SSL Error","There is no SSL support. The application will be unable to download any information",QMessageBox::Ok);
        return;
    }

    // Setting the curren  date.
    ui->deEndDate->setDate(QDate::currentDate());
    ui->deStartDate->setDate(QDate::currentDate());

    // Creating the socket and making the connections.
    serverConn = new QSslSocket(this);
    connect(serverConn,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(serverConn,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(serverConn,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(serverConn,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    connect(serverConn,SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    connect(&timer,&QTimer::timeout,this,&EyeDataAnalyzer::on_timeOut);
    connectionState = CS_CONNECTING_FOR_NAME_LIST;
    serverConn->connectToHostEncrypted(SERVER_IP,TCP_PORT_RAW_DATA_SERVER);
    timer.start(10000);
    waitDiag->setMessage("Contacting the server and getting the institution list. ");
    waitDiag->setProgressBarVisibility(false);
    waitDiag->open();

#ifndef COMPILE_FOR_PRODUCTION
    ui->lePasswordField->setText("c1bt!fpkbQ");
#endif

}

//*******************************************  SSL AND TCP RELATED SLOTS *****************************************************
void EyeDataAnalyzer::on_encryptedSuccess(){
    timer.stop();
    logForDB.appendSuccess("Established connection with the server. Connecting state: " + connectionStateToString());
    if (connectionState == CS_CONNECTING_FOR_NAME_LIST){
        connectionState = CS_GETTING_NAMELIST;
        waitDiag->setMessage("Getting the institution name list from the server");
        DataPacket tx;
        tx.addString("--",DataPacket::DPFI_INST_NAMES);
        QByteArray ba = tx.toByteArray();
        qint64 num = serverConn->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logForDB.appendError("Could not send namelist request.");
            waitDiag->close();
            return;
        }
        timer.start(30000);
    }
    else if (connectionState == CS_CONNECTING_FOR_DATA){
        connectionState = CS_GETTING_DATA;
        waitDiag->setMessage("Getting the requested information from the server\nThe progress will start only when the information transfer starts (it might take a few minutes)");
        waitDiag->setProgressBarVisibility(true);
        waitDiag->setProgressBarValue(0);

        QString startDate = ui->deStartDate->date().toString("yyyy-MM-dd");
        QString endDate = ui->deEndDate->date().toString("yyyy-MM-dd");
        institution = ui->cbInstitutions->currentData().toString();
        QString password = ui->lePasswordField->text();

        DataPacket tx;
        tx.addString(password,DataPacket::DPFI_DB_INST_PASSWORD);
        tx.addString(institution,DataPacket::DPFI_DB_INST_UID);
        tx.addString(startDate,DataPacket::DPFI_DATE_START);
        tx.addString(endDate,DataPacket::DPFI_DATE_END);
        QByteArray ba = tx.toByteArray();
        rx.clearAll();
        qint64 num = serverConn->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logForDB.appendError("Could not send data request.");
            waitDiag->close();
            return;
        }
        logForDB.appendStandard("Searching for " +ui->cbInstitutions->currentText() + " (" + ui->cbInstitutions->currentData().toString() + "). From " + startDate + " to " + endDate);
    }
    else if (connectionState == CS_CONNECTING_VMID_TABLE){
        connectionState = CS_GETTING_VMID_TABLE;
        waitDiag->setMessage("Getting the ViewMind ID information for " + ui->cbInstitutions->currentText());
        waitDiag->setProgressBarVisibility(false);
        waitDiag->setProgressBarValue(0);

        DataPacket tx;
        QString password = ui->lePasswordField->text();
        tx.addString(ui->cbInstitutions->currentData().toString(),DataPacket::DPFI_DB_INST_UID);
        tx.addString(password,DataPacket::DPFI_DB_INST_PASSWORD);
        tx.addString("--",DataPacket::DPFI_VMID_TABLE);

        rx.clearAll();
        QByteArray ba = tx.toByteArray();
        qint64 num = serverConn->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logForDB.appendError("Could not send local DB request.");
            waitDiag->close();
            return;
        }
        timer.start(30000);
    }
    else if (connectionState == CS_CONNECTING_FOR_DB_BKP){
        connectionState = CS_GETTING_DB_BKP;
        waitDiag->setMessage("Getting, if it exists the local DB BKP");
        waitDiag->setProgressBarVisibility(false);
        waitDiag->setProgressBarValue(0);

        DataPacket tx;
        QString password = ui->lePasswordField->text();
        tx.addString(ui->cbInstitutions->currentData().toString(),DataPacket::DPFI_LOCAL_DB_BKP);
        tx.addString(password,DataPacket::DPFI_DB_INST_PASSWORD);

        rx.clearAll();
        QByteArray ba = tx.toByteArray();
        qint64 num = serverConn->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logForDB.appendError("Could not send local DB request.");
            waitDiag->close();
            return;
        }
        timer.start(30000);
    }

}

void EyeDataAnalyzer::on_socketStateChanged(QAbstractSocket::SocketState state){
    if (!ui->actionShow_Full_Log->isChecked()) return;
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    logForDB.appendStandard(QString("SOCKET STATE: ") + metaEnum.valueToKey(state));
}

void EyeDataAnalyzer::on_socketError(QAbstractSocket::SocketError error){
    if (!ui->actionShow_Full_Log->isChecked()) return;
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    logForDB.appendError(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
    serverConn->disconnectFromHost();
}

void EyeDataAnalyzer::on_sslErrors(const QList<QSslError> &errors){
    if (ui->actionShow_Full_Log->isChecked()){
        for (qint32 i = 0; i < errors.size(); i++){
            logForDB.appendWarning("SSL MESSAGE: " + errors.at(i).errorString());
        }
    }
    serverConn->ignoreSslErrors();
}

void EyeDataAnalyzer::on_readyRead(){
    quint8 errcode = rx.bufferByteArray(serverConn->readAll());
    if (connectionState == CS_GETTING_DATA){
        waitDiag->setProgressBarValue(rx.getPercentArrived());
    }    
    if (errcode == DataPacket::DATABUFFER_RESULT_DONE){
        timer.stop();
        waitDiag->close();

        logForDB.appendStandard("Finished receiving information. Connecting state: " + connectionStateToString());

        if (connectionState == CS_GETTING_DATA){
            timer.stop();
            if (rx.hasInformationField(DataPacket::DPFI_DB_ERROR)){
                logForDB.appendError("Getting data from the server returned the following error message: " + rx.getField(DataPacket::DPFI_DB_ERROR).data.toString());
                serverConn->disconnectFromHost();
                return;
            }
            QStringList fileNames = rx.getField(DataPacket::DPFI_RAW_DATA_NAMES).data.toString().split(DB_LIST_IN_COL_SEP);
            QStringList fileContents = rx.getField(DataPacket::DPFI_RAW_DATA_CONTENT).data.toString().split(DB_LIST_IN_COL_SEP);

            if (fileNames.size() != fileContents.size()){
                logForDB.appendError("The number of file names is different than the number of file contents");
                return;
            }

            for (qint32 i = 0; i < fileNames.size(); i++){
                //logForDB.appendSuccess("Downloaded File: " + fileNames.at(i));
                QString path = fileNames.at(i);
                path = path.replace(WORK_DIR,institution);
                QStringList pathparts = path.split("/");
                if (pathparts.size() > 3){
                    pathparts.removeLast(); // The file name.
                    QString dir = pathparts.join("/");
                    QDir(WORK_DIR).mkpath(dir);
                    QString finalDirForFile = QString(WORK_DIR) + "/" + dir;
                    QString finalFileName   = QString(WORK_DIR) + "/" + path;
                    if (!QDir(finalDirForFile).exists()){
                        logForDB.appendError("Could not create residing directory " + finalDirForFile + " for file: " + fileNames.at(i));
                        continue;
                    }
                    QFile file(finalFileName);
                    if (!file.open(QFile::WriteOnly)){
                        logForDB.appendError("Could not opent file: " + finalFileName + " for writing");
                        continue;
                    }
                    QTextStream writer(&file);
                    writer << fileContents.at(i);
                    file.close();
                    logForDB.appendSuccess("Downloaded file: " + finalFileName);
                }
                else{
                    logForDB.appendError("Unrecognized file format: " + path);
                }
            }

        }
        else if (connectionState == CS_GETTING_DB_BKP){
            timer.stop();
            if (rx.hasInformationField(DataPacket::DPFI_DB_ERROR)){
                logForDB.appendError("Getting data from the server returned the following error message: " + rx.getField(DataPacket::DPFI_DB_ERROR).data.toString());
                serverConn->disconnectFromHost();
                return;
            }
            QString serializedDB = rx.getField(DataPacket::DPFI_SERIALIZED_DB).data.toString();
            QString serialziedIDMap = rx.getField(DataPacket::DPFI_PUID_LIST).data.toString();

            //qWarning() << serializedDB;
            //qWarning() << "SERIALIZED ID MAP";
            //qWarning() << serialziedIDMap;

            QString error = patNameMng.addSerializedIDMap(serialziedIDMap);
            if (!error.isEmpty()) logForDB.appendError("Interpreting the Serialized IDMap: " + error);
            error = patNameMng.fromSerializedMapData(serializedDB);
            if (!error.isEmpty()) logForDB.appendError("Interpreting the Serialized Map DB Data: " + error);
        }
        else if (connectionState == CS_GETTING_VMID_TABLE){
            timer.stop();
            if (rx.hasInformationField(DataPacket::DPFI_DB_ERROR)){
                logForDB.appendError("Getting VM ID Table returned the following error message: " + rx.getField(DataPacket::DPFI_DB_ERROR).data.toString());
                serverConn->disconnectFromHost();
                return;
            }

            QString table = rx.getField(DataPacket::DPFI_VMID_TABLE).data.toString();
            QString err = patNameMng.addVMIDTableData(table,ui->cbInstitutions->currentData().toString());
            if (!err.isEmpty()){
                logForDB.appendError("Error while getting the VMID table: " + err);
            }

        }
        else {
            timer.stop();
            QString nameAndUids = rx.getField(DataPacket::DPFI_INST_NAMES).data.toString();
            //qWarning() << nameAndUids;
            QStringList parts = nameAndUids.split(DB_LIST_IN_COL_SEP,QString::SkipEmptyParts);
            QStringList instNames;
            QStringList instIDs;
            for (qint32 i = 0; i < parts.size(); i++){
                if ((i % 3) == 2){
                    // This is the NAME and the value before it, is its UID.
                    if (parts.at(i-2) == "1") {
                        instNames << parts.at(i);
                        instIDs << parts.at(i-1);
                        ui->cbInstitutions->addItem(parts.at(i) + " - " + parts.at(i-1),parts.at(i-1));
                    }

                }
            }
            if (parts.size() > 0) ui->pbGetData->setEnabled(true);
            if (instIDs.size() == instNames.size()) patNameMng.setInstitutions(instIDs,instNames);
        }


        serverConn->disconnectFromHost();
    }

}

void EyeDataAnalyzer::on_timeOut(){
    timer.stop();
    switch(connectionState){
    case CS_CONNECTING_FOR_DATA:
        logForDB.appendError("Time out while waiting for the connection to get data");
        break;
    case CS_CONNECTING_FOR_NAME_LIST:
        logForDB.appendError("Timeout while waiting for server connection");
        break;
    case CS_GETTING_DATA:
        logForDB.appendError("Time out while getting the data");
        break;
    case CS_GETTING_NAMELIST:
        logForDB.appendError("Timeout while waiting for server connection");
        break;
    case CS_CONNECTING_FOR_DB_BKP:
        logForDB.appendError("Timeout while waiting for server connection");
        break;
    case CS_GETTING_DB_BKP:
        logForDB.appendError("Timeout while getting DB Bkp");
        break;
    }
    waitDiag->close();
}

void EyeDataAnalyzer::on_pbGetData_clicked()
{
    connectionState = CS_CONNECTING_FOR_DATA;
    serverConn->connectToHostEncrypted(SERVER_IP,TCP_PORT_RAW_DATA_SERVER);
    timer.start(10000);
    waitDiag->setMessage("Contacting the server for getting the requested data");
    waitDiag->setProgressBarVisibility(false);
    waitDiag->open();

}

void EyeDataAnalyzer::on_pbVMIDTableInfo_clicked()
{
    connectionState = CS_CONNECTING_VMID_TABLE;
    serverConn->connectToHostEncrypted(SERVER_IP,TCP_PORT_RAW_DATA_SERVER);
    timer.start(10000);
    waitDiag->setMessage("Contacting the server for getting the ViewMind ID Table information");
    waitDiag->setProgressBarVisibility(false);
    waitDiag->open();
}


void EyeDataAnalyzer::on_pbLocalDB_clicked(){
    connectionState = CS_CONNECTING_FOR_DB_BKP;
    serverConn->connectToHostEncrypted(SERVER_IP,TCP_PORT_RAW_DATA_SERVER);
    timer.start(10000);
    waitDiag->setMessage("Contacting the server for getting local backup data");
    waitDiag->setProgressBarVisibility(false);
    waitDiag->open();
}

//****************************************************************************************************************************

void EyeDataAnalyzer::processDirectory(){

    QStringList filters;
    QDir currentDir(currentDirectory);
    QStringList directories = currentDir.entryList(filters,QDir::Dirs|QDir::NoDotAndDotDot);
    filters.clear(); filters << "*.dat" << "*.datf";
    QStringList datFiles = currentDir.entryList(filters,QDir::Files);
    filters.clear(); filters << "*.rep";
    QStringList repFiles = currentDir.entryList(filters,QDir::Files);


    // Check if eye_rep_gen_conf exists
    filters.clear(); filters << "eye_rep_gen*";
    QStringList eyeRepGen = currentDir.entryList(filters,QDir::Files);

    if (eyeRepGen.size() > 0){
        QString filename = currentDirectory + "/" + eyeRepGen.first();
        QFile file(filename);
        if (!file.open(QFile::ReadOnly)){
            logForProcessing.appendError("Could not open " + eyeRepGen.first() + " for reading");
        }
        else{
            QTextStream reader(&file);
            reader.setCodec(COMMON_TEXT_CODEC);
            ui->pteConfig->setPlainText(reader.readAll());
            file.close();
        }
    }

    // Loading the default settings that are not presesnt
    if (!QFile(FILE_DEFAULT_VALUES).exists()){
        //logForProcessing.appendStandard("Antes");
        overWriteCurrentConfigurationWith(defaultValues,true);
        //logForProcessing.appendStandard("MIDDLE");
        overWriteCurrentConfigurationWith(defaultReportCompletionParameters,true);
        //logForProcessing.appendStandard("Despues");
    }
    else{
        ConfigurationManager config;
        if (!config.loadConfiguration(FILE_DEFAULT_VALUES,COMMON_TEXT_CODEC)){
            logForProcessing.appendError("Error loading default settings file: " + config.getError());
            return;
        }
        overWriteCurrentConfigurationWith(config,true);
    }

    overWriteCurrentConfigurationWith(patNameMng.getPatientNameFromDirname(currentDirectory),false);

    ui->lwDirs->clear();
    ui->lwDatFiles->clear();
    ui->lwRepFiles->clear();

    for (qint32 i = 0; i < directories.size(); i++){
        QListWidgetItem *item = new QListWidgetItem(directories.at(i));
        item->setData(ROLE_DATA,currentDirectory + "/" + directories.at(i));
        ui->lwDirs->addItem(item);
    }

    for (qint32 i = 0; i < datFiles.size(); i++){
        QListWidgetItem *item = new QListWidgetItem(datFiles.at(i));
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        //item->setCheckState(Qt::Unchecked);
        item->setData(ROLE_DATA,datFiles.at(i));
        ui->lwDatFiles->addItem(item);
    }

    for (qint32 i = 0; i < repFiles.size(); i++){
        QListWidgetItem *item = new QListWidgetItem(repFiles.at(i));
        item->setData(ROLE_DATA,repFiles.at(i));
        ui->lwRepFiles->addItem(item);
    }

}

void EyeDataAnalyzer::on_newUIMessage(const QString &html){
    ui->pteProcessingOutput->appendHtml(html);
}

void EyeDataAnalyzer::on_newUIMessage_for_DB(const QString &html){
    ui->pteDBOutputLog->appendHtml(html);
}


void EyeDataAnalyzer::onProcessorMessage(const QString &msg, qint32 type){
    if (type == MSG_TYPE_STD) htmlWriter.appendStandard(msg);
    else if (type == MSG_TYPE_SUCC) htmlWriter.appendSuccess(msg);
    else if (type == MSG_TYPE_ERR) htmlWriter.appendError(msg);
    else htmlWriter.appendWarning(msg);
}

void EyeDataAnalyzer::switchViews(qint32 view){
    for (qint32 i = 0; i < appViews.size(); i++){
        appViews[i]->setVisible(i == view);
    }
}

EyeDataAnalyzer::~EyeDataAnalyzer()
{
    delete ui;
}

void EyeDataAnalyzer::on_actionDataBase_Connection_triggered()
{
    switchViews(VIEW_0_DATABASE_VIEW);
}

void EyeDataAnalyzer::on_actionProcessing_View_triggered()
{
    switchViews(VIEW_1_PROCESSING_VIEW);
}

void EyeDataAnalyzer::on_lwDirs_itemDoubleClicked(QListWidgetItem *item)
{
    currentDirectory = item->data(ROLE_DATA).toString();
    processDirectory();
}

void EyeDataAnalyzer::on_pbUp_clicked()
{
    if (currentDirectory.contains("/")){
        QStringList parts = currentDirectory.split("/");
        parts.removeLast();
        currentDirectory = parts.join("/");
        processDirectory();
    }
    else{
        currentDirectory = WORK_DIR;
        processDirectory();
    }
}

ConfigurationManager EyeDataAnalyzer::createProcessingConfiguration(bool *ok){
    ConfigurationManager processingParameters;
    *ok = true;
    if (!ui->pteConfig->toPlainText().isEmpty()){
        if (!processingParameters.loadConfiguration("",COMMON_TEXT_CODEC,ui->pteConfig->toPlainText())){
            logForProcessing.appendError("ERROR in parameters file: " +  processingParameters.getError());
            *ok = false;
            return processingParameters;
        }
    }
    processingParameters.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,currentDirectory);
    return processingParameters;
}

void EyeDataAnalyzer::overWriteCurrentConfigurationWith(const ConfigurationManager &mng, bool addOnlyNonExistant){
    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    if (addOnlyNonExistant){
        QStringList keys = mng.getAllKeys();
        for (qint32 i = 0; i < keys.size(); i++){
            if (!processingParameters.containsKeyword(keys.at(i))){
                processingParameters.addKeyValuePair(keys.at(i),mng.getVariant(keys.at(i)));
            }
        }
    }
    else processingParameters.merge(mng);

    // Whatever the case the patient and doctor name are overwritten forcefully. In the patient name ONLY if it does not exist.
    if (!processingParameters.containsKeyword(CONFIG_PATIENT_NAME)){
       processingParameters.addKeyValuePair(CONFIG_PATIENT_NAME,defaultReportCompletionParameters.getString(CONFIG_PATIENT_NAME));
    }
    processingParameters.addKeyValuePair(CONFIG_DOCTOR_NAME,defaultReportCompletionParameters.getString(CONFIG_DOCTOR_NAME));

    QStringList keys = processingParameters.getAllKeys();
    QString newText = "";
    for (qint32 i = 0; i < keys.size(); i++){
        newText = newText + keys.at(i) + " = " + processingParameters.getString(keys.at(i)) + ";\n";
    }
    ui->pteConfig->setPlainText(newText);
}

void EyeDataAnalyzer::on_pbAnalyzeData_clicked()
{

    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);

    // Making sure the working directory has a full path
    QDir wdir(processingParameters.getString(CONFIG_PATIENT_DIRECTORY));
    processingParameters.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,wdir.absolutePath());

    if (!ok) return;
    // Third step, processing the data.
    RawDataProcessor processor(this);
    htmlWriter.reset();
    connect(&processor,SIGNAL(appendMessage(QString,qint32)),this,SLOT(onProcessorMessage(QString,qint32)));
    processor.initialize(&processingParameters);
    logForProcessing.appendStandard("Processing started...");
    processor.run();
    QString outputFile = currentDirectory + "/output.html";
    htmlWriter.write(outputFile,"Data Analyizer - " + QString(PROGRAM_VERSION));
    logForProcessing.appendSuccess("Processing done: Output at: " + outputFile);
    logForProcessing.appendSuccess("Current Report is: : "  + processor.getReportFileOutput());

//    // Generating the graphs.
//    QString graphFile = currentDirectory + "/graphs.html";
//    BarGrapher bgrapher;
//    for (qint32 i = 0; i < processor.getBarGraphOptions().size(); i++){
//        bgrapher.addGraphToDo(processor.getBarGraphOptions().at(i));
//    }
//    if (!bgrapher.createBarGraphHTML(graphFile)){
//        logForProcessing.appendError("When creating fix graphs: " + bgrapher.getLastError());
//    }
}

void EyeDataAnalyzer::on_pbDrawFixations_clicked()
{

    if (ui->lwDatFiles->count() == 0){
        logForProcessing.appendError("No files to process");
        return;
    }

    if (ui->lwDatFiles->currentRow() < 0){
        logForProcessing.appendError("No file is selected");
        return;
    }

    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    QString fileToProcess = ui->lwDatFiles->currentItem()->text();
    QString fixationHashName = "";
    if (fileToProcess.isEmpty()){
        logForProcessing.appendError("No seleted file: " + fileToProcess);
        return;
    }
    if (fileToProcess.startsWith(FILE_OUTPUT_READING))    {
        processingParameters.addKeyValuePair(CONFIG_FILE_READING,fileToProcess);
        fixationHashName = CONFIG_P_EXP_READING;
    }
    else if (fileToProcess.startsWith(FILE_OUTPUT_BINDING_BC)){
        processingParameters.addKeyValuePair(CONFIG_FILE_BIDING_BC,fileToProcess);
        fixationHashName = CONFIG_P_EXP_BIDING_BC;
    }
    else if (fileToProcess.startsWith(FILE_OUTPUT_BINDING_UC)){
        processingParameters.addKeyValuePair(CONFIG_FILE_BIDING_UC,fileToProcess);
        fixationHashName = CONFIG_P_EXP_BIDING_UC;
    }
    else{
        logForProcessing.appendError("Unrecognized file type: " + fileToProcess);
        return;
    }

    RawDataProcessor processor(this);
    htmlWriter.reset();
    connect(&processor,SIGNAL(appendMessage(QString,qint32)),this,SLOT(onProcessorMessage(QString,qint32)));
    processor.initialize(&processingParameters);
    logForProcessing.appendStandard("Processing started...");
    processor.run();
    FixationList fix = processor.getFixations().value(fixationHashName);

    // Draw fixations.
    FixationDrawer fdrawer;

    QFileInfo info(fileToProcess);
    QDir(currentDirectory).mkdir(info.baseName());
    QString outputDir =  currentDirectory + "/" + info.baseName();
    if (!QDir(outputDir).exists()){
        logForProcessing.appendError("Could not create output directory: " + outputDir);
        return;
    }

    if (!fdrawer.prepareToDrawFixations(fixationHashName,&processingParameters,fix.experimentDescription,outputDir)){
        logForProcessing.appendError("Error drawing fixation for experiment " + fixationHashName + ". Error is: " + fdrawer.getError());
        return;
    }
    if (!fdrawer.drawFixations(fix)){
        logForProcessing.appendError("Error drawing fixation for experiment " + fixationHashName + ". Error is: " + fdrawer.getError());
        return;
    }

    logForProcessing.appendSuccess("Fixations were draw in directory: " + outputDir);

}

void EyeDataAnalyzer::on_pgFrequencyAnalsis_clicked()
{
    if (ui->lwDatFiles->count() == 0){
        logForProcessing.appendError("No files to process");
        return;
    }

    if (ui->lwDatFiles->currentRow() < 0){
        logForProcessing.appendError("No file is selected");
        return;
    }

    QString fileToProcess = currentDirectory + "/" + ui->lwDatFiles->currentItem()->text();

    FreqAnalysis freqChecker;
    FreqAnalysis::FreqAnalysisResult fres;
    fres = freqChecker.analyzeFile(fileToProcess);

    QString freqReport;

    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    if (!fres.errorList.isEmpty()){
        freqReport = "FREQ ANALYSIS ERROR: \n   " + fres.errorList.join("\n   ");
    }
    else {

        FreqAnalysis::FreqCheckParameters fcp;
        fcp.fexpected                        = processingParameters.getReal(CONFIG_SAMPLE_FREQUENCY);
        fcp.periodMax                        = processingParameters.getReal(CONFIG_TOL_MAX_PERIOD_TOL);
        fcp.periodMin                        = processingParameters.getReal(CONFIG_TOL_MIN_PERIOD_TOL);
        fcp.maxAllowedFreqGlitchesPerTrial   = processingParameters.getReal(CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL);
        fcp.maxAllowedPercentOfInvalidValues = processingParameters.getReal(CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES);
        fcp.minNumberOfDataItems             = processingParameters.getReal(CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL);
        fcp.maxAllowedFailedTrials           = processingParameters.getReal(CONFIG_TOL_NUM_ALLOWED_FAILED_DATA_SETS);

        fres.analysisValid(fcp);


        freqReport = "FREQ ANALYSIS REPORT: Avg Frequency: " + QString::number(fres.averageFrequency) + "\n   ";
        freqReport = freqReport + "Will Result in Frequency Error: ";
        if (fres.errorList.isEmpty()) freqReport = freqReport + " NO\n   ";
        else freqReport = freqReport + " YES\n   ";
        freqReport = freqReport + fres.errorList.join("\n   ");
        freqReport = freqReport  + "\n   Individual Freq Errors:\n   " + fres.individualErrorList.join("\n   ");

    }

    // Saving the frequency log.
    QString freqLog = fileToProcess + ".flog";
    QFile file(freqLog);
    if (!file.open(QFile::WriteOnly)){
        logForProcessing.appendError("Could not open output file " + freqLog + " for writing");
        return;
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);
    writer << freqReport;
    file.close();

    logForProcessing.appendSuccess("Generated frequency log file at: " + freqLog);

}

void EyeDataAnalyzer::on_pteDefConfig_clicked()
{

    if (!QFile(FILE_DEFAULT_VALUES).exists()){
        overWriteCurrentConfigurationWith(defaultValues,false);
        overWriteCurrentConfigurationWith(defaultReportCompletionParameters,true);
    }
    else{
        ConfigurationManager config;
        if (!config.loadConfiguration(FILE_DEFAULT_VALUES,COMMON_TEXT_CODEC)){
            logForProcessing.appendError("Error loading default settings file: " + config.getError());
            return;
        }
        overWriteCurrentConfigurationWith(config,false);
    }
}

void EyeDataAnalyzer::on_pbSaveDefConf_clicked()
{
    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    QString toSave;
    QStringList keys = defaultValues.getAllKeys();
    for (qint32 i = 0; i < keys.size(); i++){
        toSave = toSave  + keys.at(i) + " = ";
        if (processingParameters.containsKeyword(keys.at(i))) toSave = toSave + processingParameters.getString(keys.at(i)) + ";\n";
        else toSave = toSave + defaultValues.getString(keys.at(i)) + ";\n";
    }

    QFile file(FILE_DEFAULT_VALUES);
    if (!file.open(QFile::WriteOnly)){
        logForProcessing.appendError("Could not open settings file for saving");
        return;
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);
    writer << toSave;
    file.close();

    logForProcessing.appendSuccess("Saved new default configuration");

}

void EyeDataAnalyzer::on_pbDATFiles_clicked()
{
    ConfigurationManager config;
    for (qint32 i = 0; i < ui->lwDatFiles->count(); i++){
        QString fileToProcess = ui->lwDatFiles->item(i)->text();
        if (fileToProcess.startsWith(FILE_OUTPUT_READING))    {
            config.addKeyValuePair(CONFIG_FILE_READING,fileToProcess);
        }
        else if (fileToProcess.startsWith(FILE_OUTPUT_BINDING_BC)){
            config.addKeyValuePair(CONFIG_FILE_BIDING_BC,fileToProcess);
        }
        else if (fileToProcess.startsWith(FILE_OUTPUT_BINDING_UC)){
            config.addKeyValuePair(CONFIG_FILE_BIDING_UC,fileToProcess);
        }
    }

    overWriteCurrentConfigurationWith(config,false);
}

void EyeDataAnalyzer::on_pbGenerateReport_clicked()
{

    if (ui->lwRepFiles->count() == 0){
        logForProcessing.appendError("No reports generated");
        return;
    }

    if (ui->lwRepFiles->currentRow() < 0){
        logForProcessing.appendError("No report selected");
        return;
    }

    logForProcessing.appendStandard("Generating PNG Report ...");

    bool ok;
    ConfigurationManager processingParameters = createProcessingConfiguration(&ok);
    if (!ok) return;

    QString selectedReport = ui->lwRepFiles->currentItem()->text();

    RepFileInfo repInfoOnDir;

    // It's useless here, but required by the function.
    RepFileInfo::AlgorithmVersions algver;
    algver.bindingAlg = 1;
    algver.readingAlg = 1;

    repInfoOnDir.setDirectory(currentDirectory,algver);
    QVariantMap dataSet = repInfoOnDir.getRepData(selectedReport);
    if (dataSet.isEmpty()){
        logForProcessing.appendError("Could not load data on report file: " + selectedReport);
        return;
    }

    // Setting the image report name
    QFileInfo info(selectedReport);
    QString outputPath = currentDirectory + "/" + info.baseName() + ".png";
    processingParameters.addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,outputPath);

    ImageReportDrawer reportDrawer;
    reportDrawer.drawReport(dataSet,&processingParameters,"B");

    if (QFile(outputPath).exists()){
        logForProcessing.appendSuccess("Generated image report at: " + outputPath);
    }
    else{
        logForProcessing.appendError("Could not generate PNG report");
    }

}

void EyeDataAnalyzer::on_pbClearLog_clicked()
{
    ui->pteDBOutputLog->setPlainText("");
}



void EyeDataAnalyzer::on_pbUnifiedCSV_clicked()
{
    QString selectedDir;

    SelectUnifiedCSVFolderDiag diag(this);
    if (diag.exec() == QDialog::Rejected) return;
    selectedDir = diag.getDirectory();
    if (selectedDir.isEmpty())  return;

//    selectedDir = "C:/Users/Viewmind/Documents/viewmind_projects/EyeDataAnalyzer/exe/work/1369462188";
    waitDiag->setProgressBarVisibility(true);
    waitDiag->setProgressBarValue(0);
    waitDiag->setMessage("Processing directory " + selectedDir);
    waitDiag->open();
    batchProcessor.setWorkingDir(selectedDir,diag.getDisplayIDCode());
    batchProcessor.start();

}

void EyeDataAnalyzer::on_batchProcessing_Done(){
    logForProcessing.appendSuccess("Finished batch processing");

    for (qint32 i = 0; i < batchProcessor.getErrors().size(); i++){
        logForProcessing.appendError(batchProcessor.getErrors().at(i));
    }

    for (qint32 i = 0; i < batchProcessor.getMessage().size(); i++){
        logForProcessing.appendSuccess(batchProcessor.getMessage().at(i));
    }

    waitDiag->close();

}

void EyeDataAnalyzer::on_pbIDTable_clicked()
{
    IDTableDiag tableDiag;
    tableDiag.setInstitutions(patNameMng.getInstHash());
    if (tableDiag.exec() != QDialog::Accepted) return;

    IDTableDiag::Columns cols = tableDiag.getTableColumns();

    QStringList dbpuids = patNameMng.getDBPUIDForInst(cols.dir);
    if (dbpuids.isEmpty()){
        logForProcessing.appendError("Could not find any DBPUIDs for Institution: " + cols.dir + ". Please update the VM ID Database");
        return;
    }

    QStringList csv;
    QStringList header;

    if (cols.DBPUID)  header << "DBPUID";
    if (cols.DID)     header << "DID";
    if (cols.PUID)    header << "PUID";
    if (cols.HPUID)   header << "HPUID";

    if (header.isEmpty()){
        logForProcessing.appendError("No columns in the table were selected");
        return;
    }

    csv << header.join(",");

    for (qint32 i = 0; i < dbpuids.size(); i++){;
        ConfigurationManager t =  patNameMng.getPatientIDInfoFromDBPUID(dbpuids.at(i));
        QStringList row;
        if(cols.DBPUID){
            if (t.containsKeyword(ID_DBUID)) row << t.getString(ID_DBUID);
            else row << "N/A";
        }
        if (cols.DID){
            if (t.containsKeyword(ID_DID)) row << t.getString(ID_DID);
            else row << "N/A";
        }
        if (cols.PUID){
            if (t.containsKeyword(ID_PUID)) row << t.getString(ID_PUID);
            else row << "N/A";
        }
        if (cols.HPUID){
            if (t.containsKeyword(ID_HPUID)) row << t.getString(ID_HPUID);
            else row << "N/A";
        }
        csv << row.join(",");
    }

    QFile file(QString(WORK_DIR) + "/"  +  cols.dir + "_ID.csv");
    if (!file.open(QFile::WriteOnly)){
        logForProcessing.appendError("Could not open file " + file.fileName() + " for writing");
        return;
    }

    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);
    writer << csv.join("\n");
    file.close();

    logForProcessing.appendSuccess("Finished. File at: " + file.fileName());

}


