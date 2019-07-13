#include "dataprocessingsslserver.h"

DataProcessingSSLServer::DataProcessingSSLServer(QObject *parent):QObject(parent)
{
    // Creating the the listner.
    listener = new SSLListener(this);

    // Listen for new connections.
    connect(listener,&SSLListener::newConnection,this,&DataProcessingSSLServer::on_newConnection);
    connect(listener,&SSLListener::lostConnection,this,&DataProcessingSSLServer::on_lostConnection);

    // Start generator from zero.
    idGen = 0;
}

void DataProcessingSSLServer::startServer(ConfigurationManager *c){

    config = c;

    // Checking that the output repo exists.
    QDir d(c->getString(CONFIG_RAW_DATA_REPO));
    if (!d.exists()){
        log.appendError("Output repo directory does not exist: " + c->getString(CONFIG_RAW_DATA_REPO));
        return;
    }

    // Checking that the processor exists.
    QFile file(c->getString(CONFIG_EYEPROCESSOR_PATH));
    if (!file.exists()){
        log.appendError("EyeReportGenerator could not be found at: " + c->getString(CONFIG_EYEPROCESSOR_PATH));
        return;
    }

    log.appendStandard("PARALLEL PROCESSES: "  + c->getString(CONFIG_NUMBER_OF_PARALLEL_PROCESSES));

    if (!listener->listen(QHostAddress::Any,TCP_PORT_DATA_PROCESSING)){
        log.appendError("Could not start SSL Server: " + listener->errorString());
        return;
    }

}

void DataProcessingSSLServer::on_lostConnection(){
    log.appendWarning("Lost connection from the SSL Listener!!!");
}

void DataProcessingSSLServer::on_newConnection(){

    // New connection is available.
    QSslSocket *sslsocket = (QSslSocket*)(listener->nextPendingConnection());
    SSLIDSocket *socket = new SSLIDSocket(sslsocket,idGen,config->getString(CONFIG_S3_ADDRESS));

    if (!socket->isValid()) {
        log.appendError("Could not cast incomming socket connection");
        return;
    }

    idGen++;

    // Saving the socket.
    sockets.addSocket(socket);
    queue << socket->getID();

    // Doing the connection SIGNAL-SLOT
    connect(socket,&SSLIDSocket::sslSignal,this,&DataProcessingSSLServer::on_newSSLSignal);

    // The SSL procedure.
    sslsocket->setPrivateKey(":/certificates/server.key");
    sslsocket->setLocalCertificate(":/certificates/server.csr");
    sslsocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslsocket->startServerEncryption();

}

void DataProcessingSSLServer::on_newSSLSignal(quint64 socket, quint8 signaltype){

    //qWarning() << "DATA PROCESSING SSL SIGNAL" << signalType;
    QString where = "on_newSSLSignal: " + SSLIDSocket::SSLSignalToString(signaltype);
    SSLIDSocket *sslsocket = sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(socket,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        return;
    }

    switch (signaltype){
    case SSLIDSocket::SSL_SIGNAL_DISCONNECTED:
        log.appendStandard("Terminated connection: " + sslsocket->socket()->peerAddress().toString());
        removeSocket(socket,where);
        break;
    case SSLIDSocket::SSL_SIGNAL_ENCRYPTED:
        log.appendSuccess("SSL Handshake completed for address: " + sslsocket->socket()->peerAddress().toString());
        requestProcessInformation();
        break;
    case SSLIDSocket::SSL_SIGNAL_SOCKET_ERROR:
        socketErrorFound(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_DATA_RX_DONE:
        // Information has arrived ok. Starting the process.
        sslsocket->stopTimer();
        lauchEyeReportProcessor(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_DATA_RX_ERROR:
        sslsocket->stopTimer();
        // Data is most likely corrupted.
        log.appendError("Data from " + sslsocket->socket()->peerAddress().toString() + " was corrupted or contained errors");
        removeSocket(socket,where);
        break;
    case SSLIDSocket::SSL_SIGNAL_TIMEOUT:
        // This means that the data did not arrive on time.
        sslsocket->stopTimer();
        log.appendError("Data from " + sslsocket->socket()->peerAddress().toString() + " did not arrive in time");
        removeSocket(socket,where);
        break;
    case SSLIDSocket::SSL_SIGNAL_PROCESS_DONE:
        sendReport(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_SSL_ERROR:
        sslErrorsFound(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_STATE_CHANGED:
        changedState(socket);
        break;
    }

    sockets.releaseSocket(socket,where);

}

void DataProcessingSSLServer::sendReport(quint64 socket){

    // Opening connection to the database.
    if (!initAllDBS()) return;

    QString where = "sendReport";
    SSLIDSocket *sslsocket = sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        sockets.releaseSocket(socket,where);
        return;
    }

    // Finding the newest rep file in the folder.
    DataProcessingSSLServer::EyeRepGenGeneratedFiles ergf = getNonStandardFileNamesFromEyeRepGen(sslsocket->getWorkingDirectory());
    if (!ergf.allOk){
        sockets.releaseSocket(socket,where);
        removeSocket(socket,where);
        return;
    }

    // Data is saved to the data base ONLY when NOT in demo mode.
    QString logid;
    if (sslsocket->getDataPacket().getField(DataPacket::DPFI_DEMO_MODE).data.toInt() == 0){

        // Since the processing is done, now the data can be saved to the database
        ConfigurationManager toDB;
        qint32 teyereskeyid;

        if (toDB.loadConfiguration(ergf.dbfFile,COMMON_TEXT_CODEC)){

            QStringList values;
            QStringList columns = toDB.getAllKeys();
            for (qint32 i = 0; i < columns.size(); i++){
                values << toDB.getString(columns.at(i));
            }

            // Adding the IDs and the date.
            DataPacket d = sslsocket->getDataPacket();
            QString puid = getPatientID(d.getField(DataPacket::DPFI_PATIENT_ID).data.toString());
            if (puid == -1){
                log.appendError("Unknown patient id from uid: " + d.getField(DataPacket::DPFI_PATIENT_ID).data.toString() + " when getting puid from report. This should not be possible.");
                dbConnBase->close();
                dbConnID->close();
                dbConnPatData->close();
                sockets.releaseSocket(socket,where);
                removeSocket(socket,where);
                return;
            }

            QString doctorid = d.getField(DataPacket::DPFI_DOCTOR_ID).data.toString();
            columns << TEYERES_COL_STUDY_DATE << TEYERES_COL_PUID << TEYERES_COL_DOCTORID;
            values << "TIMESTAMP(NOW())" << puid  << doctorid;

            logid = doctorid + " for " + puid;

            if (!dbConnBase->insertDB(TABLE_EYE_RESULTS,columns,values,logid)){
                dbConnBase->close();
                dbConnID->close();
                dbConnPatData->close();
                log.appendError("DB Error saving the results: " + dbConnBase->getError());
                sockets.releaseSocket(socket,where);
                removeSocket(socket,where);
                return;
            }

            teyereskeyid = dbConnBase->getNewestKeyid(TEYERES_COL_KEYID,TABLE_EYE_RESULTS);
        }
        else{
            dbConnBase->close();
            dbConnID->close();
            dbConnPatData->close();
            log.appendError("Could not load the db info file: " + toDB.getError());
            sockets.releaseSocket(socket,where);
            removeSocket(socket,where);
            return;
        }

        // If the teyereskeyid is -1 the result entry is already saved in the table. It is better to save the current values with a keyid of -1 than not save it at all.
        if (teyereskeyid == -1){
            log.appendError("Failed to get the max keyid of the Eye Result Table for files: " + ergf.fdbFiles.join(","));
        }

        // Saving the FDB Files
        for (qint32 i = 0; i < ergf.fdbFiles.size(); i++){
            toDB.clear();
            if (!toDB.loadConfiguration(ergf.fdbFiles.at(i),COMMON_TEXT_CODEC)){
                log.appendError("Could not load FDB File: " + ergf.fdbFiles.at(i));
                continue;
            }
            QStringList values;
            QStringList columns;
            columns = toDB.getAllKeys();
            for (qint32 i = 0; i < columns.size(); i++){
                values << toDB.getString(columns.at(i));
            }
            columns << TFDATA_COL_TEYERESULT;
            values << QString::number(teyereskeyid);
            if (!dbConnBase->insertDB(TABLE_FDATA,columns,values,logid)){
                log.appendError("DB Error saving the FDB FILE " + ergf.fdbFiles.at(i) + ": " + dbConnBase->getError());
            }
        }

        // Sending the email if the mail body exists.
        sendFreqErrorEmail(sslsocket->getDataPacket().getField(DataPacket::DPFI_DB_INST_UID).data.toInt(),ergf.mailFile,ergf.graphFile);
    }

    DataPacket tx;
    if (!tx.addFile(ergf.repFile,DataPacket::DPFI_REPORT)){
        log.appendError("Could not add report file to data packet to send from: " + ergf.repFile);
        sockets.releaseSocket(socket,where);
        removeSocket(socket,where);
        return;
    }

    // Adding the ACK Code of all ok. Any other code is NOT sent from this function.
    tx.addValue(RR_ALL_OK,DataPacket::DPFI_PROCESSING_ACK);

    // Adding the previous version of report sent. (Will be empty if it does not exist).
    QString previous_report_version = "";
    if (sslsocket->getDataPacket().hasInformationField(DataPacket::DPFI_OLD_REP_FILE)){
        previous_report_version = sslsocket->getDataPacket().getField(DataPacket::DPFI_OLD_REP_FILE).data.toString();
    }
    tx.addString(previous_report_version,DataPacket::DPFI_OLD_REP_FILE);

    QByteArray ba = tx.toByteArray();
    qint64 n = sslsocket->socket()->write(ba.constData(),ba.size());
    if (n != ba.size()){
        log.appendError("Could failure sending the report to host: " + sslsocket->socket()->peerAddress().toString());
        sockets.releaseSocket(socket,where);
        return;
    }

    qint32 UID = sslsocket->getDataPacket().getField(DataPacket::DPFI_DB_INST_UID).data.toInt();
    if (!previous_report_version.isEmpty()){
        log.appendStandard("Institution " + QString::number(UID) + " requested reprocessing of file " + previous_report_version + " will not decrease evaluation count");
    }

    // ONLY AFTER the report was successfully sent the number of evaluations is decreased, and IF this is not demo mode OR a reprocessed file.
    if ((sslsocket->getDataPacket().getField(DataPacket::DPFI_DEMO_MODE).data.toInt() == 0) && (previous_report_version.isEmpty())){
        decreaseReportCount(UID,logid);
    }

    // Closing the connection to the database
    dbConnBase->close();
    dbConnID->close();
    dbConnPatData->close();
    sockets.releaseSocket(socket,where);

    //log.appendStandard("Sending report back to host: " + sslsocket->socket()->peerAddress().toString() + ". Size: " + QString::number(ba.size()) + " bytes");

}

void DataProcessingSSLServer::lauchEyeReportProcessor(quint64 socket){    

    QString where = "launchEyeReportProcessor";
    SSLIDSocket *sslsocket = sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        sockets.releaseSocket(socket,where);
        return;
    }

    QString error = sslsocket->setWorkingDirectoryAndSaveAllFiles(config->getString(CONFIG_RAW_DATA_REPO));
    if (!error.isEmpty()){
        log.appendError("Could not save data files in incomming packet: " + error);
        sockets.releaseSocket(socket,where);
        removeSocket(socket,where);
        return;
    }

    // Removing previous or existing rep files
    // removeAllRepFiles(sslsocket->getWorkingDirectory());

    // Opening connection to the database, to check that the doctor and patient uid in the DB are present.
    if (!initAllDBS()) return;

    // Getting some patient data to make sure, its in the DB.
    DataPacket d = sslsocket->getDataPacket();

    // Verifying that the information can be processed
    QString etserial = d.getField(DataPacket::DPFI_DB_ET_SERIAL).data.toString();
    qint32 inst_uid = d.getField(DataPacket::DPFI_DB_INST_UID).data.toInt();
    quint8 code = verifyReportRequest(inst_uid,etserial);

    if (code != RR_ALL_OK){
        // Need to return the error code.
        DataPacket tx;
        tx.addValue(code,DataPacket::DPFI_PROCESSING_ACK);
        QByteArray ba = tx.toByteArray();
        qint64 n = sslsocket->socket()->write(ba.constData(),ba.size());
        if (n != ba.size()){
            log.appendError("Could failure sending processing ack code to host: " + sslsocket->socket()->peerAddress().toString());
            return;
        }
        sockets.releaseSocket(socket,where);
        removeSocket(socket,where);
        return;
    }


    QString puid = getPatientID(d.getField(DataPacket::DPFI_PATIENT_ID).data.toString());
    if (puid == -1){
        // Patient is not in the DB.
        log.appendError("No patient id found for uid:  " + d.getField(DataPacket::DPFI_PATIENT_ID).data.toString());
        return;
    }

    QStringList columns;
    columns << TPATDATA_COL_KEYID;
    QString condition = QString(TPATDATA_COL_PUID) + " = '" + puid + "'";

    // Patient data
    if (!dbConnPatData->readFromDB(TABLE_PATDATA,columns,condition)){
        log.appendError(dbConnPatData->getError());
        return;
    }
    else{
        DBData data = dbConnPatData->getLastResult();
        if (data.rows.size() == 0){
            // Patient is not in the DB.
            log.appendError("No patient data found for uid:  " + d.getField(DataPacket::DPFI_PATIENT_ID).data.toString());
            sockets.releaseSocket(socket,where);
            return;
        }
    }

    // Doctor Data
    columns.clear();
    columns << TDOCTOR_COL_KEYID;
    condition = QString(TDOCTOR_COL_UID) + " = '" + d.getField(DataPacket::DPFI_DOCTOR_ID).data.toString() + "'";
    if (!dbConnBase->readFromDB(TABLE_DOCTORS,columns,condition)){
        log.appendError(dbConnBase->getError());
        return;
    }
    else{
        DBData data = dbConnBase->getLastResult();
        if (data.rows.size() == 0){
            // Doctor is not in the DB.
            log.appendError("No doctor data found for uid:  " + d.getField(DataPacket::DPFI_DOCTOR_ID).data.toString());
            sockets.releaseSocket(socket,where);
            return;
        }
    }

    // All good and the files have been saved.
    QStringList arguments;
    arguments << sslsocket->getWorkingDirectory() + "/" + FILE_EYE_REP_GEN_CONFIGURATION;

    //log.appendStandard("Process information from: " + sslsocket->socket()->peerAddress().toString());

    // Closing connection to db
    dbConnBase->close();
    dbConnPatData->close();
    dbConnID->close();

    // Processing the data.
    sslsocket->processData(config->getString(CONFIG_EYEPROCESSOR_PATH),arguments);
    sockets.releaseSocket(socket,where);

}

void DataProcessingSSLServer::requestProcessInformation(){

    QString where = "requestProcessInformation";

    if (socketsBeingProcessed.size() < config->getInt(CONFIG_NUMBER_OF_PARALLEL_PROCESSES)){
        // Request information from the next in queue.
        if (!queue.isEmpty()){
            quint64 id = queue.first();
            queue.removeFirst();

            SSLIDSocket *sslsocket = sockets.getSocketLock(id,where);
            if (sslsocket == nullptr){
                log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(id));
                sockets.releaseSocket(id,where);
                return;
            }

            socketsBeingProcessed << id;
            DataPacket tx;
            tx.addValue(0,DataPacket::DPFI_SEND_INFORMATION);
            QByteArray ba = tx.toByteArray();
            qint64 num = sslsocket->socket()->write(ba,ba.size());
            if (num != ba.size()){
                log.appendError("Could not send ACK packet to: " + sslsocket->socket()->peerAddress().toString());
                sockets.releaseSocket(id,where);
                removeSocket(id,where);
            }
            else {
                sslsocket->startTimeoutTimer(config->getInt(CONFIG_DATA_REQUEST_TIMEOUT)*1000);
                log.appendStandard("Sent request for data to " + sslsocket->socket()->peerAddress().toString());
            }
            sockets.releaseSocket(id,where);
        }
    }

    log.appendStandard("Queue contains " + QString::number(queue.size()) + " pending requests");
    log.appendStandard("Processing " + QString::number(socketsBeingProcessed.size()) + " at once");

}

void DataProcessingSSLServer::removeSocket(quint64 id, const QString &where){

    sockets.deleteSocket(id,where);

    qint32 index = queue.indexOf(id);
    if (index != -1) queue.removeAt(index);
    if (socketsBeingProcessed.remove(id)){
        log.appendStandard("Remaining processing requests: " + QString::number(socketsBeingProcessed.size()));
        requestProcessInformation();
    }

}


/*****************************************************************************************************************************
 * Message managging functions
 * **************************************************************************************************************************/

void DataProcessingSSLServer::socketErrorFound(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    QString where = "socketErrorFound";
    SSLIDSocket *sslsocket = sockets.getSocketLock(id,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(id,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(id));
        return;
    }

    QAbstractSocket::SocketError error = sslsocket->socket()->error();
    QHostAddress addr = sslsocket->socket()->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    if (error != QAbstractSocket::RemoteHostClosedError){
        log.appendError(QString("Socket Error found: ") + metaEnum.valueToKey(error) + QString(" from Address: ") + addr.toString());
        // Eliminating it from the list.
        sockets.releaseSocket(id,where);
        removeSocket(id,where);
    }

    sockets.releaseSocket(id,where);

}

void DataProcessingSSLServer::sslErrorsFound(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    QString where = "sslErrorsFound";
    SSLIDSocket *sslsocket = sockets.getSocketLock(id,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(id,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(id));
        return;
    }

    QList<QSslError> errorlist = sslsocket->socket()->sslErrors();
    QHostAddress addr = sslsocket->socket()->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        log.appendError("SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }

    // Eliminating it from the list.
    sockets.releaseSocket(id,where);
    removeSocket(id,where);

}

void DataProcessingSSLServer::changedState(quint64 id){
    Q_UNUSED(id)
    //    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    //    // be removed once. After that, te program crashes.
    //    if (!sockets.contains(id)) return;

    //    QAbstractSocket::SocketState state = sslsocket->socket()->state();
    //    QHostAddress addr = sslsocket->socket()->peerAddress();
    //    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    //    log.appendStandard(addr.toString() + ": " + QString("New socket state, ") + metaEnum.valueToKey(state));
}


/*****************************************************************************************************************************
 * Database transaction helper functions
 * **************************************************************************************************************************/

quint8 DataProcessingSSLServer::verifyReportRequest(qint32 UID, const QString &etserial){

    // When this is called the connection to the DB is openend.

    QStringList columns;
    columns << TINST_COL_UID << TINST_COL_EVALS;
    QString condition = QString(TINST_COL_UID) + " = '" + QString::number(UID) + "'";
    if (!dbConnBase->readFromDB(TABLE_INSTITUTION,columns,condition)){
        log.appendError("When querying institution and number of evaluations: " + dbConnBase->getError());
        return RR_DB_ERROR;
    }

    DBData data = dbConnBase->getLastResult();

    if (data.rows.size() != 1){
        log.appendError("When querying the institution and number of evaluations: Number of returned rows was " + QString::number(data.rows.size()) + " instead of 1. UID: " + QString::number(UID));
        return RR_DB_ERROR;
    }

    // Checking the number of evaluations
    if (data.rows.first().size() != 2){
        log.appendError("When querying the institution and number of evaluations: Number of returned columns was " + QString::number(data.rows.first().size()) + " instead of 2. INST UID: " + QString::number(UID));
        return RR_DB_ERROR;

    }

    qint32  numevals = data.rows.first().last().toInt();
    if (numevals == 0 ){
        log.appendError("No evaluations remaining forinsitituion with UID " + QString::number(UID));
        return RR_OUT_OF_EVALUATIONS;
    }

    // Checking the serial number of the ET
    columns.clear();
    columns << TPLACED_PROD_COL_ETSERIAL;
    condition = "(" + QString(TPLACED_PROD_COL_ETSERIAL) + " = '" + etserial + "') AND (" + QString(TPLACED_PROD_COL_INSTITUTION) + " = '" + QString::number(UID) + "')";
    if (!dbConnBase->readFromDB(TABLE_PLACEDPRODUCTS,columns,condition)){
        log.appendError("When querying serial number for institituion: " + dbConnBase->getError());
        return RR_DB_ERROR;
    }

    //qWarning() << "Number of Evaluations: " << numevals << "Sent serial" << etserial;
    data = dbConnBase->getLastResult();

    // Checking the serial
    if (data.rows.size() < 1){
        log.appendError("ETSerial |" + etserial + "| does not correspond to the serial registered for insitituion with UID " + QString::number(UID));
        return RR_WRONG_ET_SERIAL;

    }
    if (data.rows.first().size() != 1){
        log.appendError("ETSerial |" + etserial + "| does not correspond to the serial registered for insitituion with UID " + QString::number(UID));
        return RR_WRONG_ET_SERIAL;
    }

    return RR_ALL_OK;
}

void DataProcessingSSLServer::decreaseReportCount(qint32 UID, const QString &logid){

    QStringList columns;
    columns << TINST_COL_EVALS;
    QString condition = QString(TINST_COL_UID) + " = '" + QString::number(UID) + "'";
    if (!dbConnBase->readFromDB(TABLE_INSTITUTION,columns,condition)){
        log.appendError("Decreasing report count: " + dbConnBase->getError());
        return;
    }

    DBData data = dbConnBase->getLastResult();

    if (data.rows.size() != 1){
        log.appendError("Decreasing report count: Number of returned rows was " + QString::number(data.rows.size()) + " instead of 1, for UID: " + QString::number(UID) );
        return;
    }
    qint32  numevals = data.rows.first().first().toInt();

    if (numevals == -1) return;

    // Checking the number of evaluations
    if ((numevals <= 0)){
        log.appendError("Asked to decrease number of evaluations for " + QString(UID) + " when it is " + QString(numevals));
        return;
    }

    // Decreasing and saving.
    numevals--;
    QStringList values;
    values << QString::number(numevals);

    if (!dbConnBase->updateDB(TABLE_INSTITUTION,columns,values,condition,logid)){
        log.appendError("When saving new number of evaluations " + QString(UID) + " when it is " + QString(numevals) + ": " + dbConnBase->getError());
        return;
    }

}

bool DataProcessingSSLServer::initAllDBS(){
    if (!dbConnBase->open()){
        log.appendError("ERROR : Could not start SQL Connection on Data Processing Server to Base DB : " + dbConnBase->getError());
        return false;
    }
    if (!dbConnID->open()){
        log.appendError("ERROR : Could not start SQL Connection on Data Processing Server to Patient ID DB: " + dbConnID->getError());
        return false;
    }
    if (!dbConnPatData->open()){
        log.appendError("ERROR : Could not start SQL Connection on Data Processing Server to the Patient Data DB: " + dbConnPatData->getError());
        return false;
    }
    return true;
}

QString DataProcessingSSLServer::getPatientID(const QString &pat_uid){

    QStringList columns;
    columns << TPATID_COL_KEYID;

    QString condition = QString(TPATID_COL_UID) + " = '" + pat_uid + "'";
    if (!dbConnID->readFromDB(TABLE_PATIENTD_IDS,columns,condition)){
        log.appendError("When querying for the database id of patient: " + pat_uid + ". Got the error: " +  dbConnID->getError());
        return "-1";
    }

    DBData data = dbConnID->getLastResult();
    if (data.rows.size() != 1){
        log.appendError("When querying for the database id of patient: " + pat_uid + ". Got the number of rows: " +  QString::number(data.rows.size()));
        return "-1";
    }

    if (data.rows.first().size() != 1){
        log.appendError("When querying for the database id of patient: " + pat_uid + ". Got the number of columns: " +  QString::number(data.rows.first().size()));
        return "-1";
    }

    return data.rows.first().first();

}

/*****************************************************************************************************************************
 * Rep files helper functions.
 * **************************************************************************************************************************/

//void DataProcessingSSLServer::removeAllRepFiles(const QString &directory){
//    QStringList filters;
//    filters << "*.rep";
//    QStringList files = QDir(directory).entryList(filters,QDir::Files);
//    for (qint32 i = 0; i < files.size(); i++){
//        QFile(directory + "/" + files.at(i)).remove();
//    }
//}

DataProcessingSSLServer::EyeRepGenGeneratedFiles DataProcessingSSLServer::getNonStandardFileNamesFromEyeRepGen(const QString &directory){
    EyeRepGenGeneratedFiles ans;
    ans.allOk = true;

    QStringList filters;
    filters << "*.rep";
    QDir wdir(directory);

    QStringList files = wdir.entryList(filters,QDir::Files);
    if (files.size() > 1){
        log.appendError("On directory: " + directory + " more than one report file was found.");
        ans.allOk = false;
        return ans;
    }
    else if (files.isEmpty()){
        log.appendError("No report file found on directory: " + directory);
        ans.allOk = false;
        return ans;
    }
    else ans.repFile = directory  + "/" + files.first();

    filters.clear();
    files.clear();
    filters << "*.fdb";
    files = wdir.entryList(filters,QDir::Files);
    if (files.isEmpty()){
        log.appendError("No fdb files found in: " + directory);
        ans.allOk = false;
        return ans;
    }
    for (qint32 i = 0; i < files.size(); i++){
        ans.fdbFiles << directory + "/" + files.at(i);
    }

    ans.dbfFile = directory + "/" + QString(FILE_DBDATA_FILE);
    ans.graphFile = directory + "/" + QString(FILE_GRAPHS_FILE);

    if (!QFile(ans.dbfFile).exists()){
        log.appendError("No dbf file found on directory: " + directory);
        ans.allOk = false;
        return ans;
    }

    if (!QFile(ans.graphFile).exists()){
        log.appendError("No graphs file found on directory: " + directory);
        ans.allOk = false;
        return ans;
    }

    ans.mailFile = directory + "/" + QString(FILE_MAIL_ERROR_LOG);
    if (!QFile(ans.mailFile).exists()){
        ans.mailFile = "";
    }
    return ans;
}

void DataProcessingSSLServer::sendFreqErrorEmail(qint32 instUID, const QString &emailBodyFileName, const QString &graphFile){

    if (emailBodyFileName.isEmpty()) return;

    QStringList columns;
    columns << TINST_COL_NAME;
    QString condition = QString(TINST_COL_UID) + " = '" + QString::number(instUID) + "'";
    QString instName = QString::number(instUID);
    if (!dbConnBase->readFromDB(TABLE_INSTITUTION,columns,condition)){
        log.appendError("Getting inst name for " + instName + ": " + dbConnBase->getError());
    }
    else{
        DBData data = dbConnBase->getLastResult();
        if (data.rows.size() != 1){
            log.appendError("Getting inst name for email, rows size " + QString::number(data.rows.size()) + " instead of 1, for UID: " + QString::number(instUID) );
        }
        else {
            if (data.rows.first().size() < 1){
                log.appendError("Getting inst name for email, col size is zero for UID: " + QString::number(instUID) );
            }
        }
        instName = data.rows.first().first();
    }

    QFile mailBody(emailBodyFileName);
    if (!mailBody.open(QFile::ReadOnly)){
        log.appendError("Could not open mail body file: " + emailBodyFileName);
        return;
    }
    QTextStream reader(&mailBody);
    reader.setCodec(COMMON_TEXT_CODEC);
    QString body = reader.readAll();
    mailBody.close();

    // Path of the output PHP file
    QFileInfo info(emailBodyFileName);
    QString outPHPFilename = info.path() + "/" + QString(FILE_PHP_MAIL);
    QFile phpFile(outPHPFilename);
    if (!phpFile.open(QFile::WriteOnly)){
        log.appendError("Could not open php mail file for writing " + outPHPFilename);
        return;
    }
    QTextStream writer(&phpFile);
    writer.setCodec(COMMON_TEXT_CODEC);

    writer << "<?php\n";
#ifdef SERVER_PRODUCTION
    writer << "require '/home/ec2-user/composer/vendor/autoload.php';\n";
    writer << "require '/home/ec2-user/composer/vendor/phpmailer/phpmailer/PHPMailerAutoload.php';\n";
#else
    writer << "use PHPMailer\\PHPMailer\\PHPMailer;\n";
    writer << "require '/home/ariela/repos/viewmind_projects/Scripts/php/vendor/autoload.php';\n";
#endif

    writer << "$mail = new PHPMailer;\n";
    writer << "$mail->isSMTP();\n";
    writer << "$mail->setFrom('check.viewmind@gmail.com', 'ViewMind Administration');\n";
    writer << "$mail->Username = 'AKIARDLQA5AHRSTLPCYS';\n";
    writer << "$mail->Password = 'BHGWozyNwZoHjvUAhL8d7H9FC/H4RBNfh564MnKZRKj/';\n";
    writer << "$mail->Host = 'email-smtp.us-east-1.amazonaws.com';\n";
    writer << "$mail->Subject = 'ViewMind Frequency Check Alert From: " + instName + "';\n";
    writer << "$mail->addAddress('ariel.arelovich@viewmind.com.ar', 'Ariel Arelovich');\n";

#ifdef SERVER_PRODUCTION
    writer << "$mail->addAddress('matias.shulz@viewmind.com.ar', 'Matias Shulz');\n";
    writer << "$mail->addAddress('gerardofernandez480@gmail.com ', 'Gerardo Fernandez');\n";
#endif

    // The HTML-formatted body of the email
    writer << "$mail->Body = '<h3>Frequency problems detected from Institituion: " + instName + "</h3>\n<h3>Details</h3>" + body + "';\n";
    writer << "$mail->addAttachment('" + graphFile  + "');\n";
    writer << "$mail->SMTPAuth = true;\n";
    writer << "$mail->SMTPSecure = 'tls';\n";
    writer << "$mail->Port = 587;\n";
    writer << "$mail->isHTML(true);\n";
    writer << "if(!$mail->send()) {\n";
    writer << "    echo 'Email error is: ' , $mail->ErrorInfo , PHP_EOL;\n";
    writer << "}\n";
    writer << "?>\n";

    // Actually sending the email.
    phpFile.close();
    QProcess p;
    p.setProgram("php");
    QStringList args; args << outPHPFilename;
    p.setArguments(args);
    p.start();
    p.waitForFinished();
    QString output(p.readAllStandardOutput());
    if (!output.trimmed().isEmpty()){
        log.appendError("EMAIL: " + output.trimmed());
    }

}
