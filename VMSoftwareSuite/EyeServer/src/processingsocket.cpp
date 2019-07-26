#include "processingsocket.h"

ProcessingSocket::ProcessingSocket():SSLIDSocket(){
    sslSocket = nullptr;
    disconnectReceived = false;
}


ProcessingSocket::ProcessingSocket(QSslSocket *newSocket, const ProcessingSocketData &cdata):SSLIDSocket(newSocket, cdata.ID)
{

    configData = cdata;
    connect(&lifeTimer,&QTimer::timeout,this,&ProcessingSocket::on_lifeTimeOut);
    connect(&eyeRepGen,SIGNAL(finished(int)),this,SLOT(on_eyeRepGenFinished(qint32)));
    connect(&eyeDBMng,SIGNAL(finished(int)),this,SLOT(on_eyeDBMngFinished(qint32)));

    pstate = PS_WAIT_FINSHED_CONNECTION;
    timestamp = QDateTime::currentDateTime().toString(TIME_FORMAT_STRING);
    transactionID = timestamp + "_TID" + QString::number(configData.ID);

    // Customized log file.
    log.setLogFileLocation(QString(DIRNAME_SERVER_LOGS) + "/" + transactionID);
    log.appendStandard("Created connection id: " + transactionID + " with address " + sslSocket->peerAddress().toString());


    // Starting life timer.
    lifeTimer.setInterval(LIFE_TIMEOUT);
    lifeTimer.start();

    mtimer.measure(TIME_MEASURE_ESTABLISHED_CONNECTION);

    // This value will ensure NO changes in the client, if something goes wrong.
    numberOfRemainingEvaluations = -3;

    // Waiting for encryption finished.
    startTimeoutTimer();

}

void ProcessingSocket::on_timeout(){
    SSLIDSocket::on_timeout();
    // Can only get time out in three ocassion
    if (pstate == PS_WAIT_FINSHED_CONNECTION){
        // Connection could not be established.
        //doDisconnects();
        log.appendError("Could not established encrypted connection");
        emit(socketDone(configData.ID));
    }
    else if (pstate == PS_WAIT_INFO){
        log.appendError("Information never arrived. Waiting for disconnect from client");
    }
    else if (pstate == PS_WAIT_CLIENT_OK){
        log.appendError("Client OK never arrived. INFORMATION WON'T BE STORED. Waiting for disconnect from client");
    }
}

void ProcessingSocket::on_lifeTimeOut(){
    lifeTimer.stop();
    timer.stop();
    doDisconnects();
    log.appendError("Socket life timed out");
    emit(socketDone(configData.ID));
}

void ProcessingSocket::doDisconnects(){
    log.appendStandard("Doing slot-signals disconnect");
    SSLIDSocket::doDisconnects();
    disconnect(&lifeTimer,&QTimer::timeout,this,&ProcessingSocket::on_lifeTimeOut);

    disconnect(&eyeRepGen,SIGNAL(finished(int)),this,SLOT(on_eyeRepGenFinished(qint32)));
    disconnect(&eyeDBMng,SIGNAL(finished(int)),this,SLOT(on_eyeDBMngFinished(qint32)));
}

void ProcessingSocket::on_encryptedSuccess(){
    SSLIDSocket::on_encryptedSuccess();
    log.appendSuccess("Finalized Encrypted Connection");
    pstate = PS_WAIT_INFO;
    mtimer.measure(TIME_MEASURE_INFO_RECEIVED);
    startTimeoutTimer();
}

void ProcessingSocket::on_readyRead(){
    // Should buffer the data
    QByteArray ba = socket()->readAll();
    quint8 ans = rx.bufferByteArray(ba);
    if (ans == DataPacket::DATABUFFER_RESULT_DONE){
        timer.stop();
        if (pstate == PS_WAIT_INFO){
            mtimer.measure(TIME_MEASURE_DBMNG_CHECK);
            dbMngCheck();
            rx.clearAll();
        }
        else if (pstate == PS_WAIT_CLIENT_OK){
            pstate = PS_WAIT_DBMNG_STORE;
            mtimer.measure(TIME_MEASURE_DBMNG_STORE);
            dbMngStore();
            rx.clearAll();
        }
        else{
            log.appendError("Finshed receiving data in an unexpected state: " + stateToString());
            rx.clearAll();
            sendCodeToClient(EYESERVER_RESULT_SERVER_ERROR);
        }
    }
    else if (ans == DataPacket::DATABUFFER_RESULT_ERROR){
        timer.stop();
        log.appendError("Finshed receiving data but it was corrupted. State: " + stateToString());
        sendCodeToClient(EYESERVER_RESULT_SERVER_ERROR);
    }
}

void ProcessingSocket::dbMngCheck(){

    etSerial = "";
    instID = "";
    patientHashedID = "";

    if (rx.hasInformationField(DataPacket::DPFI_PATIENT_ID)){
        patientHashedID = rx.getField(DataPacket::DPFI_PATIENT_ID).data.toString();
    }

    if (rx.hasInformationField(DataPacket::DPFI_DB_INST_UID)){
        instID = rx.getField(DataPacket::DPFI_DB_INST_UID).data.toString();
    }

    if (rx.hasInformationField(DataPacket::DPFI_DB_ET_SERIAL)){
        etSerial = rx.getField(DataPacket::DPFI_DB_ET_SERIAL).data.toString();
    }

    if (etSerial.isEmpty() || instID.isEmpty() || patientHashedID.isEmpty()){
        log.appendError("Missing information. ETSerial: " + etSerial + ". INST ID: " + instID + ". PAT HASH ID: " + patientHashedID);
        sendCodeToClient(EYESERVER_RESULT_SERVER_ERROR);
    }
    log.appendStandard("INSTITUTION: " + instID);
    log.appendStandard("ET SERIAL: " + etSerial);
    log.appendStandard("PAT HASHED ID: " + patientHashedID);

    // Creating the work directory
    workingDir = QString(DIRNAME_SERVER_WORKDIR) + "/" + patientHashedID + "/" + timestamp;

    // It is safe to assume that the work dir exists.
    QDir(QString(DIRNAME_SERVER_WORKDIR)).mkdir(patientHashedID);
    QDir(QString(DIRNAME_SERVER_WORKDIR) + "/" + patientHashedID).mkdir(timestamp);

    if (!QDir(workingDir).exists()){
        log.appendError("Working directory: " + workingDir + " does not exists or could not be created");
        sendCodeToClient(EYESERVER_RESULT_SERVER_ERROR);
        return;
    }

    if (!rx.saveFiles(workingDir)){
        log.appendError("Could not save files in data packet.");
        sendCodeToClient(EYESERVER_RESULT_SERVER_ERROR);
        return;
    }

    // All files saved sucessfully Creating the dbmng comm file.
    ConfigurationManager dbmngComm;
    dbmngComm.addKeyValuePair(CONFIG_INST_ETSERIAL,etSerial);
    dbmngComm.addKeyValuePair(CONFIG_INST_UID,instID);
    dbmngComm.addKeyValuePair(CONFIG_TRANSACTION_ID,transactionID);
    dbmngComm.addKeyValuePair(CONFIG_TIMESTAMP,timestamp);
    dbmngComm.addKeyValuePair(CONFIG_DBMNG_ACTION,CONFIG_P_DMBNG_ACTION_CHECK);
    if (!dbmngComm.saveToFile(workingDir + "/" + QString(FILE_DBMNG_COMM_FILE),COMMON_TEXT_CODEC)){
        log.appendError("Could not create DB Comm file for check procedure: " + dbmngComm.getError());
        sendCodeToClient(EYESERVER_RESULT_SERVER_ERROR);
        return;
    }

    //eyeDBMng.setWorkingDirectory(workingDir);
    pstate = PS_WAIT_DBMNG_CHECK;
    QStringList arguments;
    arguments << workingDir;
    eyeDBMng.start(configData.eyeDBMngPath,arguments);
}

void ProcessingSocket::on_eyeDBMngFinished(qint32 status){

    // Loading the configuration to make sure all is ok.
    ConfigurationManager dbmngComm;
    if (!dbmngComm.loadConfiguration(workingDir + "/" + QString(FILE_DBMNG_COMM_FILE),COMMON_TEXT_CODEC)){
        log.appendError("Could not load dbmng comm file after the DBMNG finished");
        sendCodeToClient(EYESERVER_RESULT_SERVER_ERROR);
        return;
    }

    log.appendStandard("EYE DB MNG Finished with status: " + QString::number(status) + " on state: " + stateToString());

    qint32 dbmngAns = -1;
    if (dbmngComm.containsKeyword(CONFIG_DBMNG_RESULT)){
        dbmngAns = dbmngComm.getInt(CONFIG_DBMNG_RESULT);
    }

    if (pstate == PS_WAIT_DBMNG_CHECK){

        if (dbmngAns != EYEDBMNG_ANS_OK){
            log.appendError("EYEDBMNG returned NOT OK result code: " + QString::number(dbmngAns));
            qint32 code;
            switch (dbmngAns){
            case EYEDBMNG_ANS_DB_ERROR:
                code = EYESERVER_RESULT_SERVER_ERROR;
                break;
            case EYEDBMNG_ANS_FILE_ERROR:
                code = EYESERVER_RESULT_SERVER_ERROR;
                break;
            case EYEDBMNG_ANS_NOEVALS:
                code = EYESERVER_RESULT_NOEVALS;
                break;
            case EYEDBMNG_ANS_PARAM_ERROR:
                code = EYESERVER_RESULT_SERVER_ERROR;
                break;
            case EYEDBMNG_ANS_WRONG_SERIAL:
                code = EYESERVER_RESULT_WRONG_SERIAL;
                break;
            default:
                code = EYESERVER_RESULT_SERVER_ERROR;
                break;
            }
            sendCodeToClient(code);
            return;
        }
        else{
            if (dbmngComm.containsKeyword(CONFIG_REMAINING_EVALUATIONS)){
               numberOfRemainingEvaluations = dbmngComm.getInt(CONFIG_REMAINING_EVALUATIONS);
               if (numberOfRemainingEvaluations > 0) numberOfRemainingEvaluations = numberOfRemainingEvaluations - 1;
               log.appendStandard("Number of remaining evaluations (After this one): " + QString::number(numberOfRemainingEvaluations));
            }
        }

        // All is good. The processing is done now.

        // Adding the transaction ID to the eye rep gent conf
        QString eyeRepGenConf  = workingDir + "/"  + QString(FILE_EYE_REP_GEN_CONFIGURATION);
        ConfigurationManager::setValue(eyeRepGenConf,COMMON_TEXT_CODEC,CONFIG_TRANSACTION_ID,transactionID);

        pstate = PS_WAIT_EYEPROC;
        QStringList args;
        QFileInfo info(configData.eyeRepGenPath);
        QFileInfo confInfo(eyeRepGenConf);
        args << confInfo.absolutePath() + "/" + QString(FILE_EYE_REP_GEN_CONFIGURATION);
        eyeRepGen.setWorkingDirectory(info.absolutePath());
        eyeRepGen.start(configData.eyeRepGenPath,args);

        mtimer.measure(TIME_MEASURE_PROCESSING_DONE);
    }
    else {

        // The store procedure
        mtimer.measure(TIME_MEASURE_DISCONNECT);
        log.appendStandard("EYE DB MNG Store procedure finished with code: " + QString::number(dbmngAns));
        if (disconnectReceived) {
            doDisconnects();
            mtimer.end();
            log.appendStandard("TIMES: " + mtimer.getTimeSummary());
            emit(socketDone(configData.ID));
        }
    }

}

void ProcessingSocket::on_eyeRepGenFinished(qint32 status){

    mtimer.measure(TIME_MEASURE_CLIENT_OK);
    log.appendStandard("EYE REP GEN Finished with status: " + QString::number(status));

    // If all went according to plan, then one and only one .rep file must exist in the directory.
    QStringList filters; filters << "*.rep";
    QStringList repFiles = QDir(workingDir).entryList(filters,QDir::Files);
    if (repFiles.size() != 1){
        log.appendError("Number of report files found in the processing directory is different than one. They are: " + repFiles.join(","));
        sendCodeToClient(EYESERVER_RESULT_SERVER_ERROR);
        return;
    }

    QString repFile = repFiles.first();
    // Since the file exists. It is sent with an OK code to the Client.
    sendCodeToClient(EYESERVER_RESULT_OK,workingDir + "/" + repFile);

}

void ProcessingSocket::dbMngStore(){

    ConfigurationManager dbmngComm;

    QHash<QString,QString> codes;
    codes[TIME_MEASURE_CLIENT_OK]              = "COK";
    codes[TIME_MEASURE_DBMNG_CHECK]            = "CHK";
    codes[TIME_MEASURE_DBMNG_STORE]            = "STO";
    codes[TIME_MEASURE_DISCONNECT]             = "DIS";
    codes[TIME_MEASURE_ESTABLISHED_CONNECTION] = "ECN";
    codes[TIME_MEASURE_INFO_RECEIVED]          = "IRC";
    codes[TIME_MEASURE_PROCESSING_DONE]        = "PDN";

    dbmngComm.addKeyValuePair(CONFIG_INST_ETSERIAL,etSerial);
    dbmngComm.addKeyValuePair(CONFIG_INST_UID,instID);
    dbmngComm.addKeyValuePair(CONFIG_TRANSACTION_ID,transactionID);
    dbmngComm.addKeyValuePair(CONFIG_TIMESTAMP,timestamp);
    dbmngComm.addKeyValuePair(CONFIG_TIMESTRING,mtimer.getTimeString(codes));
    dbmngComm.addKeyValuePair(CONFIG_DBMNG_ACTION,CONFIG_P_DMBNG_ACTION_STORE);
    if (!dbmngComm.saveToFile(workingDir + "/" + QString(FILE_DBMNG_COMM_FILE),COMMON_TEXT_CODEC)){
        // At this point client will disconnect or has allready disconnected.
        log.appendError("Could not create DB Comm file for store procedure: " + dbmngComm.getError());
        pstate = PS_WAIT_DISCONNECT;
        if (disconnectReceived) {
            doDisconnects();
            emit(socketDone(configData.ID));
        }
        return;
    }

    QStringList arguments;
    arguments << workingDir;
    eyeDBMng.start(configData.eyeDBMngPath,arguments);
}

void ProcessingSocket::startTimeoutTimer(){
    timer.setInterval(configData.timeOutInMs);
    timer.start();
}

void ProcessingSocket::on_disconnected(){
    SSLIDSocket::on_disconnected();
    log.appendStandard("Disconnected from client");
    lifeTimer.stop();
    if (pstate != PS_WAIT_DBMNG_STORE) {
        // Only in this case the data timeout is computed and the data shown. Otherwise we need to wait until the last part of the processing is finished
        mtimer.end();
        log.appendStandard("TIMES: " + mtimer.getTimeSummary());
        emit(socketDone(configData.ID));
    }
    else{
        log.appendStandard("Not removing socket. Waiting the DBMNG Store to finish");
    }
}


void ProcessingSocket::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    log.appendError(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
}

void ProcessingSocket::on_socketStateChanged(QAbstractSocket::SocketState state){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    log.appendStandard(QString("SOCKET STATE: ") + metaEnum.valueToKey(state));
}

void ProcessingSocket::on_sslErrors(const QList<QSslError> &errorlist){
    QHostAddress addr = sslSocket->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        log.appendWarning("SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }
}

ProcessingSocket::~ProcessingSocket(){
    timer.stop();
    if (sslSocket != nullptr) delete sslSocket;
}

QString ProcessingSocket::stateToString(){
    switch (pstate){
    case PS_WAIT_CLIENT_OK: return "WAIT CLIENT OK";
    case PS_WAIT_DBMNG_CHECK: return "WAIT DBMNG CHECK";
    case PS_WAIT_DBMNG_STORE: return "WAIT DBMNG STORE";
    case PS_WAIT_EYEPROC: return "WAIT EYEPROC";
    case PS_WAIT_FINSHED_CONNECTION: return "WAIT FINISHED CONNECTION";
    case PS_WAIT_INFO: return "WAIT INFO";
    case PS_WAIT_DISCONNECT: return "WAIT DISCONNECT";
    }
    return "UNKNOWN STATE";
}

void ProcessingSocket::sendCodeToClient(qint32 code, const QString repFile){
    DataPacket tx;
    tx.addValue(code,DataPacket::DPFI_PROCESSING_ACK);

    if (!repFile.isEmpty()){
        if (!tx.addFile(repFile,DataPacket::DPFI_REPORT)){
            log.appendError("Could not add repFile: " + repFile);
            code = EYESERVER_RESULT_SERVER_ERROR;
        }
        else{
            tx.addValue(numberOfRemainingEvaluations,DataPacket::DPFI_NUM_EVALS);
            pstate = PS_WAIT_CLIENT_OK;
            startTimeoutTimer();
        }
    }
    QByteArray ba;
    ba = tx.toByteArray();
    qint64 numBytesWritten = sslSocket->write(ba);
    if (numBytesWritten != ba.size()){
        timer.stop();
        log.appendError("While sending answer, number of bytes sent: " + QString::number(numBytesWritten) + " but required " + QString::number(ba.size()));
    }
}

