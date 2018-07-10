#include "sslmanager.h"

SSLManager::SSLManager(QObject *parent):QObject(parent)
{
    // Creating the the listner.
    listener = new SSLListener(this);

    // Listen for new connections.
    connect(listener,&SSLListener::newConnection,this,&SSLManager::on_newConnection);
    connect(listener,&SSLListener::lostConnection,this,&SSLManager::on_lostConnection);

    // Start generator from zero.
    idGen = 0;
}

void SSLManager::startServer(ConfigurationManager *c){

    config = c;

    // Checking that the output repo exists.
    QDir d(c->getString(CONFIG_RAW_DATA_REPO));
    if (!d.exists()){
        addMessage("ERROR","Output repo directory does not exist: " + c->getString(CONFIG_RAW_DATA_REPO));
        return;
    }

    // Checking that the processor exists.
    QFile file(c->getString(CONFIG_EYEPROCESSOR_PATH));
    if (!file.exists()){
        addMessage("ERROR","EyeReportGenerator could not be found at: " + c->getString(CONFIG_EYEPROCESSOR_PATH));
        return;
    }

    addMessage("PARALLEL PROCESSES: ",c->getString(CONFIG_NUMBER_OF_PARALLEL_PROCESSES));

    if (!listener->listen(QHostAddress::Any,(quint16)config->getInt(CONFIG_TCP_PORT))){
        addMessage("ERROR","Could not start SSL Server: " + listener->errorString());
        return;
    }

}

void SSLManager::on_lostConnection(){
    addMessage("WARNING","Lost connection from the SSL Listener!!!");
}

void SSLManager::on_newConnection(){

    // New connection is available.
    QSslSocket *sslsocket = (QSslSocket*)(listener->nextPendingConnection());
    SSLIDSocket *socket = new SSLIDSocket(sslsocket,idGen);

    if (!socket->isValid()) {
        addMessage("ERROR","Could not cast incomming socket connection");
        return;
    }

    idGen++;

    // Saving the socket.
    sockets[socket->getID()] = socket;
    queue << socket->getID();

    // Doing the connection SIGNAL-SLOT
    connect(socket,&SSLIDSocket::sslSignal,this,&SSLManager::on_newSSLSignal);

    // The SSL procedure.
    sslsocket->setPrivateKey(":/certificates/server.key");
    sslsocket->setLocalCertificate(":/certificates/server.csr");
    sslsocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslsocket->startServerEncryption();

}

// Handling all signals
void SSLManager::on_newSSLSignal(quint64 socket, quint8 signaltype){

    switch (signaltype){
    case SSLIDSocket::SSL_SIGNAL_DISCONNECTED:
        if (sockets.contains(socket)){
            addMessage("LOG","Lost connection from: " + sockets.value(socket)->socket()->peerAddress().toString());
            removeSocket(socket);
        }
        break;
    case SSLIDSocket::SSL_SIGNAL_ENCRYPTED:
        addMessage("SUCCESS","SSL Handshake completed for address: " + sockets.value(socket)->socket()->peerAddress().toString());
        // Requests process information.
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
            addMessage("ERROR","Data from " + sockets.value(socket)->socket()->peerAddress().toString() + " did not arrive in time");
            removeSocket(socket);
        }
        break;
    case SSLIDSocket::SSL_SIGNAL_TIMEOUT:
        // This means that the data did not arrive on time.
        if (sockets.contains(socket)){
            sockets.value(socket)->stopTimer();
            addMessage("ERROR","Data from " + sockets.value(socket)->socket()->peerAddress().toString() + " did not arrive in time");
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

void SSLManager::sendReport(quint64 socket){

    // Finding the newest rep file in the folder.
    QString reportfile = getNewestFile(sockets.value(socket)->getWorkingDirectory(),FILE_REPORT_NAME,"rep");

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
            addMessage("ERROR","DB Error saving the results: " + dbConn->getError());
        }
    }
    else{
        addMessage("ERROR","Could not load the db info file: " + toDB.getError());
    }

    if (!QFile(reportfile).exists()){
        addMessage("ERROR","Could not generate report file for working folder: " + sockets.value(socket)->getWorkingDirectory());
        removeSocket(socket);
        return;
    }
    addMessage("LOG","Report file found : " + reportfile);

    DataPacket tx;
    if (!tx.addFile(reportfile,DataPacket::DPFI_REPORT)){
        addMessage("ERROR","Could not add report file to data packet to send from: " + reportfile);
        removeSocket(socket);
        return;
    }

    QByteArray ba = tx.toByteArray();
    qint64 n = sockets.value(socket)->socket()->write(ba.constData(),ba.size());
    if (n != ba.size()){
        addMessage("ERROR","Could failure sending the report to host: " + sockets.value(socket)->socket()->peerAddress().toString());
    }

    addMessage("LOG","Sending report back to host: " + sockets.value(socket)->socket()->peerAddress().toString() + ". Size: " + QString::number(ba.size()) + " bytes");

}

void SSLManager::lauchEyeReportProcessor(quint64 socket){
    QString error = sockets.value(socket)->setWorkingDirectoryAndSaveAllFiles(config->getString(CONFIG_RAW_DATA_REPO));
    if (!error.isEmpty()){
        addMessage("ERROR","Could not save data files in incomming packet: " + error);
        removeSocket(socket);
        return;
    }

    DataPacket d = sockets.value(socket)->getDataPacket();

    // The patient birth date, name, lastname and the doctor's name and last name should be in the DB.
    QString patientName, patientAge, doctorName;
    QStringList columns;

    // Math to get the birthdate from the date
    columns << "TIMESTAMPDIFF(YEAR,"  + QString(TPATREQ_COL_BIRTHDATE) +  ",CURDATE())" << TPATREQ_COL_FIRSTNAME << TPATREQ_COL_LASTNAME;
    QString condition = QString(TPATREQ_COL_UID) + " = '" + d.getField(DataPacket::DPFI_PATIENT_ID).data.toString() + "'";

    // Patient data
    if (!dbConn->readFromDB(TABLE_PATIENTS_REQ_DATA,columns,condition)){
        addMessage("ERROR: ",dbConn->getError());
        patientAge = "0";
        patientName = d.getField(DataPacket::DPFI_PATIENT_ID).data.toString();
    }
    else{
        DBInterface::DBData data = dbConn->getLastResult();
        if (data.rows.size() > 0){
            patientName = data.rows.first().at(1) + " " + data.rows.first().at(2);
            patientAge =  data.rows.first().at(0);
        }
    }

    // Doctor Data
    columns.clear();
    columns << TDOCTOR_COL_FIRSTNAME << TDOCTOR_COL_LASTNAME;
    condition = QString(TDOCTOR_COL_UID) + " = '" + d.getField(DataPacket::DPFI_DOCTOR_ID).data.toString() + "'";
    if (!dbConn->readFromDB(TABLE_DOCTORS,columns,condition)){
        addMessage("ERROR: ",dbConn->getError());
        doctorName = d.getField(DataPacket::DPFI_DOCTOR_ID).data.toString();
    }
    else{
        DBInterface::DBData data = dbConn->getLastResult();
        if (data.rows.size() > 0){
            doctorName = data.rows.first().at(0) + " " + data.rows.first().at(1);
        }
    }

    // All good and the files have been saved.
    QStringList arguments;
    QString dash = "-";
    arguments << dash + CONFIG_PATIENT_DIRECTORY << sockets.value(socket)->getWorkingDirectory();
    arguments << dash + CONFIG_DOCTOR_NAME << doctorName;
    arguments << dash + CONFIG_PATIENT_NAME << patientName;
    arguments << dash + CONFIG_PATIENT_AGE << patientAge;
    arguments << dash + CONFIG_VALID_EYE << d.getField(DataPacket::DPFI_VALID_EYE).data.toString();

    addMessage("LOG","Process information from: " + sockets.value(socket)->socket()->peerAddress().toString());
    sockets.value(socket)->processData(config->getString(CONFIG_EYEPROCESSOR_PATH),arguments);

}

void SSLManager::requestProcessInformation(){

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
                addMessage("ERROR","Could not send ACK packet to: " + sockets.value(id)->socket()->peerAddress().toString());
                removeSocket(id);
            }
            else{
                sockets.value(id)->startTimeoutTimer(config->getInt(CONFIG_WAIT_DATA_TIMEOUT));
                addMessage("LOG","Sent request for data to " + sockets.value(id)->socket()->peerAddress().toString());
            }
        }
    }

    addMessage("LOG","Queue contains " + QString::number(queue.size()) + " pending requests");
    addMessage("LOG","Processing " + QString::number(socketsBeingProcessed.size()) + " at once");

}

void SSLManager::removeSocket(quint64 id){
    if (sockets.contains(id)) {
        if (sockets.value(id)->isValid()){
            sockets.value(id)->socket()->disconnectFromHost();
        }
        sockets.remove(id);
    }
    qint32 index = queue.indexOf(id);
    if (index != -1) queue.removeAt(index);
    if (socketsBeingProcessed.remove(id)){
        addMessage("LOG","Remaining processing requests: " + QString::number(socketsBeingProcessed.size()));
        requestProcessInformation();
    }

}


/*****************************************************************************************************************************
 * Message managging functions
 * **************************************************************************************************************************/

void SSLManager::socketErrorFound(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    if (!sockets.contains(id)) return;

    QAbstractSocket::SocketError error = sockets.value(id)->socket()->error();
    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    if (error != QAbstractSocket::RemoteHostClosedError)
        addMessage("ERROR",QString("Socket Error found: ") + metaEnum.valueToKey(error) + QString(" from Address: ") + addr.toString());
    else
        addMessage("LOG",QString("Closed connection from Address: ") + addr.toString());

    // Eliminating it from the list.
    removeSocket(id);

}

void SSLManager::sslErrorsFound(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    if (!sockets.contains(id)) return;

    QList<QSslError> errorlist = sockets.value(id)->socket()->sslErrors();
    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        addMessage("ERROR","SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }

    // Eliminating it from the list.
    removeSocket(id);

}

void SSLManager::changedState(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    if (!sockets.contains(id)) return;

    QAbstractSocket::SocketState state = sockets.value(id)->socket()->state();
    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    addMessage(addr.toString(),QString("New socket state, ") + metaEnum.valueToKey(state));

}

void SSLManager::addMessage(const QString &type, const QString &msg){
    bool sendSignal = messages.isEmpty();
    messages << type + ": " + msg;
    if (sendSignal) emit(newMessages());
}

QStringList SSLManager::getMessages(){
    QStringList ans = messages;
    messages.clear();
    return ans;
}


