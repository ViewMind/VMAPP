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
    sockets[socket->getID()] = socket;
    queue << socket->getID();

    // Doing the connection SIGNAL-SLOT
    connect(socket,&SSLIDSocket::sslSignal,this,&DataProcessingSSLServer::on_newSSLSignal);

    // The SSL procedure.
    sslsocket->setPrivateKey(":/certificates/server.key");
    sslsocket->setLocalCertificate(":/certificates/server.csr");
    sslsocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslsocket->startServerEncryption();

}

// Handling all signals
void DataProcessingSSLServer::on_newSSLSignal(quint64 socket, quint8 signaltype){

    switch (signaltype){
    case SSLIDSocket::SSL_SIGNAL_DISCONNECTED:
        if (sockets.contains(socket)){
            log.appendStandard("Lost connection from: " + sockets.value(socket)->socket()->peerAddress().toString());
            removeSocket(socket);
        }
        break;
    case SSLIDSocket::SSL_SIGNAL_ENCRYPTED:
        log.appendSuccess("SSL Handshake completed for address: " + sockets.value(socket)->socket()->peerAddress().toString());
        requestProcessInformation();
        break;
    case SSLIDSocket::SSL_SIGNAL_ERROR:
        socketErrorFound(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_DATA_RX_DONE:
        // Information has arrived ok. Starting the process.
        sockets.value(socket)->stopTimer();
        lauchEyeReportProcessor(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_DATA_RX_ERROR:
        sockets.value(socket)->stopTimer();
        // Data is most likely corrupted.
        if (sockets.contains(socket)){
            sockets.value(socket)->stopTimer();
            log.appendError("Data from " + sockets.value(socket)->socket()->peerAddress().toString() + " was corrupted or contained errors");
            removeSocket(socket);
        }
        break;
    case SSLIDSocket::SSL_SIGNAL_TIMEOUT:
        // This means that the data did not arrive on time.
        if (sockets.contains(socket)){
            sockets.value(socket)->stopTimer();
            log.appendError("Data from " + sockets.value(socket)->socket()->peerAddress().toString() + " did not arrive in time");
            removeSocket(socket);
        }
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
}

void DataProcessingSSLServer::sendReport(quint64 socket){

    // Opening connection to the database.
    if (!dbConn->initDB(config)){
        removeSocket(socket);
        log.appendError("Could not start SQL Connection when sending back the report:  " + dbConn->getError());
        return;
    }

    // Finding the newest rep file in the folder.
    QString reportfile = getReportFile(sockets.value(socket)->getWorkingDirectory());
    if (reportfile.isEmpty()){
        removeSocket(socket);
        return;
    }

    // Data is saved to the data base ONLY when NOT in demo mode.
    if (sockets.value(socket)->getDataPacket().getField(DataPacket::DPFI_DEMO_MODE).data.toInt() == 0){

        // Since the processing is done, now the data can be saved to the database
        ConfigurationManager toDB;
        QString dbfile = sockets.value(socket)->getWorkingDirectory() + "/" + FILE_DBDATA_FILE;
        if (toDB.loadConfiguration(dbfile,COMMON_TEXT_CODEC)){

            QStringList values;
            QStringList columns = toDB.getAllKeys();
            for (qint32 i = 0; i < columns.size(); i++){
                values << toDB.getString(columns.at(i));
            }

            // Adding the IDs and the date.
            DataPacket d = sockets.value(socket)->getDataPacket();
            columns << TEYERES_COL_STUDY_DATE << TEYERES_COL_PATIENTID << TEYERES_COL_DOCTORID;
            values << "TIMESTAMP(NOW())" << d.getField(DataPacket::DPFI_PATIENT_ID).data.toString()  << d.getField(DataPacket::DPFI_DOCTOR_ID).data.toString();

            if (!dbConn->insertDB(TABLE_EYE_RESULTS,columns,values)){
                dbConn->close();
                log.appendError("DB Error saving the results: " + dbConn->getError());
                removeSocket(socket);
                return;
            }
        }
        else{
            dbConn->close();
            log.appendError("Could not load the db info file: " + toDB.getError());
            removeSocket(socket);
            return;
        }

    }

    DataPacket tx;
    if (!tx.addFile(reportfile,DataPacket::DPFI_REPORT)){
        log.appendError("Could not add report file to data packet to send from: " + reportfile);
        removeSocket(socket);
        return;
    }

    // Adding the ACK Code of all ok. Any other code is NOT sent from this function.
    tx.addValue(RR_ALL_OK,DataPacket::DPFI_PROCESSING_ACK);

    QByteArray ba = tx.toByteArray();
    qint64 n = sockets.value(socket)->socket()->write(ba.constData(),ba.size());
    if (n != ba.size()){
        log.appendError("Could failure sending the report to host: " + sockets.value(socket)->socket()->peerAddress().toString());
        return;
    }

    // ONLY AFTER the report was successfully sent the number of evaluations is decreased, and IF this is not demo mode.
    if (sockets.value(socket)->getDataPacket().getField(DataPacket::DPFI_DEMO_MODE).data.toInt() == 0){
        qint32 UID = sockets.value(socket)->getDataPacket().getField(DataPacket::DPFI_DB_INST_UID).data.toInt();
        decreaseReportCount(UID);
    }

    // Closing the connection to the database
    dbConn->close();

    //log.appendStandard("Sending report back to host: " + sockets.value(socket)->socket()->peerAddress().toString() + ". Size: " + QString::number(ba.size()) + " bytes");


}

void DataProcessingSSLServer::lauchEyeReportProcessor(quint64 socket){    
    QString error = sockets.value(socket)->setWorkingDirectoryAndSaveAllFiles(config->getString(CONFIG_RAW_DATA_REPO));
    if (!error.isEmpty()){
        log.appendError("Could not save data files in incomming packet: " + error);
        removeSocket(socket);
        return;
    }

    // Removing previous or existing rep files
    removeAllRepFiles(sockets.value(socket)->getWorkingDirectory());

    // Opening connection to the database, to check that the doctor and patient uid in the DB are present.
    if (!dbConn->initDB(config)){
        removeSocket(socket);
        log.appendError("Could not start SQL Connection when launching the EyeReport Processor:  " + dbConn->getError());
        return;
    }

    // Getting some patient data to make sure, its in the DB.
    DataPacket d = sockets.value(socket)->getDataPacket();


    // Verifying that the information can be processed
    QString etserial = d.getField(DataPacket::DPFI_DB_ET_SERIAL).data.toString();
    qint32 UID = d.getField(DataPacket::DPFI_DB_INST_UID).data.toInt();
    quint8 code = verifyReportRequest(UID,etserial);

    //qWarning() << "VERIFY REPORT REQUEST CODE:" << code;

    if (code != RR_ALL_OK){
        // Need to return the error code.
        DataPacket tx;
        tx.addValue(code,DataPacket::DPFI_PROCESSING_ACK);
        QByteArray ba = tx.toByteArray();
        qint64 n = sockets.value(socket)->socket()->write(ba.constData(),ba.size());
        if (n != ba.size()){
            log.appendError("Could failure sending processing ack code to host: " + sockets.value(socket)->socket()->peerAddress().toString());
            return;
        }
        removeSocket(socket);
        return;
    }

    QStringList columns;
    columns << TPATREQ_KEYID  << TPATREQ_COL_FIRSTNAME << TPATREQ_COL_LASTNAME;
    QString condition = QString(TPATREQ_COL_UID) + " = '" + d.getField(DataPacket::DPFI_PATIENT_ID).data.toString() + "'";

    // Patient data
    if (!dbConn->readFromDB(TABLE_PATIENTS_REQ_DATA,columns,condition)){
        log.appendError(dbConn->getError());
    }
    else{
        DBData data = dbConn->getLastResult();
        if (data.rows.size() == 0){
            // Patient is not in the DB.
            log.appendError("No patient data found for uid:  " + d.getField(DataPacket::DPFI_PATIENT_ID).data.toString());
            return;
        }
    }

    // Doctor Data
    columns.clear();
    columns << TDOCTOR_COL_KEYID << TDOCTOR_COL_FIRSTNAME << TDOCTOR_COL_LASTNAME;
    condition = QString(TDOCTOR_COL_UID) + " = '" + d.getField(DataPacket::DPFI_DOCTOR_ID).data.toString() + "'";
    if (!dbConn->readFromDB(TABLE_DOCTORS,columns,condition)){
        log.appendError(dbConn->getError());
    }
    else{
        DBData data = dbConn->getLastResult();
        if (data.rows.size() == 0){
            // Patient is not in the DB.
            log.appendError("No doctor data found for uid:  " + d.getField(DataPacket::DPFI_DOCTOR_ID).data.toString());
            return;
        }
    }

    // All good and the files have been saved.
    QStringList arguments;
    arguments << sockets.value(socket)->getWorkingDirectory() + "/" + FILE_EYE_REP_GEN_CONFIGURATION;

    //log.appendStandard("Process information from: " + sockets.value(socket)->socket()->peerAddress().toString());

    // Closing connection to db
    dbConn->close();

    // Processing the data.
    sockets.value(socket)->processData(config->getString(CONFIG_EYEPROCESSOR_PATH),arguments);

}

void DataProcessingSSLServer::requestProcessInformation(){

    if (socketsBeingProcessed.size() < config->getInt(CONFIG_NUMBER_OF_PARALLEL_PROCESSES)){
        // Request information from the next in queue.
        if (!queue.isEmpty()){
            quint64 id = queue.first();
            queue.removeFirst();
            socketsBeingProcessed << id;
            DataPacket tx;
            tx.addValue(0,DataPacket::DPFI_SEND_INFORMATION);
            QByteArray ba = tx.toByteArray();
            qint64 num = sockets.value(id)->socket()->write(ba,ba.size());
            if (num != ba.size()){
                log.appendError("Could not send ACK packet to: " + sockets.value(id)->socket()->peerAddress().toString());
                removeSocket(id);
            }
            else {
                sockets.value(id)->startTimeoutTimer(config->getInt(CONFIG_DATA_REQUEST_TIMEOUT)*1000);
                log.appendStandard("Sent request for data to " + sockets.value(id)->socket()->peerAddress().toString());
            }
        }
    }

    log.appendStandard("Queue contains " + QString::number(queue.size()) + " pending requests");
    log.appendStandard("Processing " + QString::number(socketsBeingProcessed.size()) + " at once");

}

void DataProcessingSSLServer::removeSocket(quint64 id){
    if (sockets.contains(id)) {
        if (sockets.value(id)->isValid()){
            sockets.value(id)->socket()->disconnectFromHost();
        }
        sockets.remove(id);
    }
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
    if (!sockets.contains(id)) return;

    QAbstractSocket::SocketError error = sockets.value(id)->socket()->error();
    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    if (error != QAbstractSocket::RemoteHostClosedError)
        log.appendError(QString("Socket Error found: ") + metaEnum.valueToKey(error) + QString(" from Address: ") + addr.toString());
    //    else
    //        log.appendStandard(QString("Closed connection from Address: ") + addr.toString());

    // Eliminating it from the list.
    removeSocket(id);

}

void DataProcessingSSLServer::sslErrorsFound(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    if (!sockets.contains(id)) return;

    QList<QSslError> errorlist = sockets.value(id)->socket()->sslErrors();
    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        log.appendError("SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }

    // Eliminating it from the list.
    removeSocket(id);

}

quint8 DataProcessingSSLServer::verifyReportRequest(qint32 UID, const QString &etserial){

    // When this is called the connection to the DB is openend.

    QStringList columns;
    columns << TINST_COL_ETSERIAL << TINST_COL_EVALS;
    QString condition = QString(TINST_COL_UID) + " = '" + QString::number(UID) + "'";
    if (!dbConn->readFromDB(TABLE_INSTITUTION,columns,condition)){
        log.appendError("When querying et serial and number of evaluations: " + dbConn->getError());
        return RR_DB_ERROR;
    }

    DBData data = dbConn->getLastResult();

    if (data.rows.size() != 1){
        log.appendError("When querying et serial and number of evaluations: Number of returned rows was " + QString::number(data.rows.size()) + " instead of 1. UID: " + QString::number(UID));
        return RR_DB_ERROR;
    }

    QString serial = data.rows.first().first();
    qint32  numevals = data.rows.first().last().toInt();

    // Checking the serial
    if (serial != etserial){
        log.appendError("ETSerial |" + etserial + "| does not correspond to the serial registered for insitituion with UID " + QString::number(UID) + ": |" + serial + "|");
        return RR_WRONG_ET_SERIAL;
    }

    // Checking the number of evaluations
    if (numevals == 0 ){
        log.appendError("No evaluations remaining forinsitituion with UID " + QString::number(UID));
        return RR_OUT_OF_EVALUATIONS;
    }

    return RR_ALL_OK;
}

void DataProcessingSSLServer::decreaseReportCount(qint32 UID){

    QStringList columns;
    columns << TINST_COL_EVALS;
    QString condition = QString(TINST_COL_UID) + " = '" + QString::number(UID) + "'";
    if (!dbConn->readFromDB(TABLE_INSTITUTION,columns,condition)){
        log.appendError("Decreasing report count: " + dbConn->getError());
        return;
    }

    DBData data = dbConn->getLastResult();

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

    if (!dbConn->updateDB(TABLE_INSTITUTION,columns,values,condition)){
        log.appendError("When saving new number of evaluations " + QString(UID) + " when it is " + QString(numevals));
        return;
    }

}

void DataProcessingSSLServer::changedState(quint64 id){
    Q_UNUSED(id)
    //    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    //    // be removed once. After that, te program crashes.
    //    if (!sockets.contains(id)) return;

    //    QAbstractSocket::SocketState state = sockets.value(id)->socket()->state();
    //    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    //    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    //    log.appendStandard(addr.toString() + ": " + QString("New socket state, ") + metaEnum.valueToKey(state));
}


/*****************************************************************************************************************************
 * Rep files helper functions.
 * **************************************************************************************************************************/

void DataProcessingSSLServer::removeAllRepFiles(const QString &directory){
    QStringList filters;
    filters << "*.rep";
    QStringList files = QDir(directory).entryList(filters,QDir::Files);
    for (qint32 i = 0; i < files.size(); i++){
        QFile(directory + "/" + files.at(i)).remove();
    }
}

QString DataProcessingSSLServer::getReportFile(const QString &directory){
    QStringList filters;
    filters << "*.rep";
    QStringList files = QDir(directory).entryList(filters,QDir::Files);
    if (files.size() > 1){
        log.appendError("On directory: " + directory + " more than one report file was found.");
        return "";
    }
    else if (files.isEmpty()){
        log.appendError("No report file found on director: " + directory);
        return "";
    }
    else return directory  + "/" + files.first();
}
