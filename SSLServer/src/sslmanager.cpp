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

    // TODO This should be moved to where the socke is acknowledged to start transmitting.
    // receivingData.clearBufferedData();

}

// Handling all signals
void SSLManager::on_newSSLSignal(quint64 socket, quint8 signaltype){

    QByteArray ba;

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
    case SSLIDSocket::SSL_SIGNAL_SSL_ERROR:
        sslErrorsFound(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_STATE_CHANGED:
        changedState(socket);
        break;
    }
}

void SSLManager::requestProcessInformation(){

    if (socketsBeingProcessed.size() < maxNumberOfParalellProcesses){
        // Request information from the next in queue.
        if (!queue.isEmpty()){
            quint64 id = queue.removeFirst();
            socketsBeingProcessed << id;
            DataPacket tx;
            tx.addValue(0,DPFI_SEND_INFORMATION);
            QByteArray ba = tx.toByteArray();
            qint64 num = sockets.value(id)->socket()->write(ba,ba.size());
            if (num != ba.size()){
                addMessage("ERROR","Could not send ACK packet to: " + sockets.value(id)->socket()->peerAddress().toString());
                removeSocket(id);
            }
            else{
                sockets.value(id)->startTimeoutTimer(config->getInt(CONFIG_WAIT_DATA_TIMEOUT));
            }
        }
    }

}

void SSLManager::removeSocket(quint64 id){
    if (sockets.contains(id)) {
        sockets.value(id)->socket()->disconnectFromHost();
        sockets.remove(id);
    }
    if (receivingData.contains(id)) receivingData.remove(id);
    qint32 index = queue.indexOf(id);
    if (index != -1) queue.removeAt(index);
    if (socketsBeingProcessed.remove(id)){
        requestProcessInformation();
    }
}

void SSLManager::processDataPacket(){
    QString outputDirectory = "/home/ariela/Workspace/ET/test_output";

    for (qint32 i = 0; i < receivingData.fieldList().size(); i++){
        if (receivingData.fieldList().at(i).fieldType == DPFT_FILE){
            QString resultingFile = receivingData.saveFile(outputDirectory,i);
            if (resultingFile.isEmpty()){
                addMessage("ERROR", "Could not save file field " + QString::number(i));
            }
            else addMessage("LOG", "Saved file field " + QString::number(i));
        }
        else if (receivingData.fieldList().at(i).fieldType == DPFT_REAL_VALUE){
            addMessage("LOG","Received value : " + QString::number(receivingData.fieldList().at(i).data.toDouble()));
        }
        else if (receivingData.fieldList().at(i).fieldType == DPFT_STRING) {
            addMessage("LOG","Received string : " + receivingData.fieldList().at(i).data.toString());
        }
        else{
            addMessage("ERROR","Received unknown field type");
        }
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
    addMessage("ERROR",QString("Socket Error found: ") + metaEnum.valueToKey(error) + QString(" from Address: ") + addr.toString());

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


