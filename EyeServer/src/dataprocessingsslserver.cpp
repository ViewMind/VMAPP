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

void DataProcessingSSLServer::on_newSSLSignal(quint64 socket, quint8 signaltype){

    //qWarning() << "DATA PROCESSING SSL SIGNAL" << signalType;

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
    if (!initAllDBS()) return;

    // Finding the newest rep file in the folder.
    QString reportfile = getReportFile(sockets.value(socket)->getWorkingDirectory());
    if (reportfile.isEmpty()){
        removeSocket(socket);
        return;
    }

    // Data is saved to the data base ONLY when NOT in demo mode.
    QString logid;
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
            QString puid = getPatientID(d.getField(DataPacket::DPFI_PATIENT_ID).data.toString());
            if (puid == -1){
                log.appendError("Unknown patient id from uid: " + d.getField(DataPacket::DPFI_PATIENT_ID).data.toString() + " when getting puid from report. This should not be possible.");
                dbConnBase->close();
                dbConnID->close();
                dbConnPatData->close();
                removeSocket(socket);
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
                removeSocket(socket);
                return;
            }
        }
        else{
            dbConnBase->close();
            dbConnID->close();
            dbConnPatData->close();
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
        decreaseReportCount(UID,logid);
    }

    // Closing the connection to the database
    dbConnBase->close();
    dbConnID->close();
    dbConnPatData->close();

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
    if (!initAllDBS()) return;

    // Getting some patient data to make sure, its in the DB.
    DataPacket d = sockets.value(socket)->getDataPacket();

    // Verifying that the information can be processed
    QString etserial = d.getField(DataPacket::DPFI_DB_ET_SERIAL).data.toString();
    qint32 inst_uid = d.getField(DataPacket::DPFI_DB_INST_UID).data.toInt();
    quint8 code = verifyReportRequest(inst_uid,etserial);

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
            return;
        }
    }

    // All good and the files have been saved.
    QStringList arguments;
    arguments << sockets.value(socket)->getWorkingDirectory() + "/" + FILE_EYE_REP_GEN_CONFIGURATION;

    //log.appendStandard("Process information from: " + sockets.value(socket)->socket()->peerAddress().toString());

    // Closing connection to db
    dbConnBase->close();
    dbConnPatData->close();
    dbConnID->close();

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
