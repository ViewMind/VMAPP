//QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
//ui->pteLog->appendPlainText(QString("SOCKET STATE: ") + metaEnum.valueToKey(state));
#include "sslmanager.h"

SSLManager::SSLManager(QObject *parent):QObject(parent)
{
    // Creating the the listner.
    listener = new SSLListener(this);

    // Start generator from zero.
    idGen = 0;

    // Listen for new connections.
    connect(listener,&SSLListener::newConnection,this,&SSLManager::on_newConnection);
}

void SSLManager::startServer(quint16 port){
    if (!listener->listen(QHostAddress::Any,port)){
        addMessage("ERROR","Could not start SSL Server: " + listener->errorString());
        return;
    }
}

void SSLManager::on_newConnection(){

    // New connection is available.
    QSslSocket *sslsocket = dynamic_cast<QSslSocket*>(listener->nextPendingConnection());
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
    //connect(&socket,&SSLIDSocket::sslSignal,this,&SSLManager::on_newSSLSignal);

    // The SSL procedure.
    sslsocket->setPrivateKey(":/certificates/server.key");
    sslsocket->setLocalCertificate(":/certificates/server.csr");
    sslsocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslsocket->startServerEncryption();
}

// Handling all signals
void SSLManager::on_newSSLSignal(quint64 socket, quint8 signaltype){

    QByteArray ba;

    switch (signaltype){
    case SSLIDSocket::SSL_SIGNAL_DISCONNECTED:
        addMessage("LOG","Lost connection from: " + sockets.value(socket)->socket()->peerAddress().toString());
        removeSocket(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_ENCRYPTED:
        addMessage("SUCCESS","SSL Handshake completed for address: " + sockets.value(socket)->socket()->peerAddress().toString());
        break;
    case SSLIDSocket::SSL_SIGNAL_ERROR:
        socketErrorFound(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_READY_READ:
        ba = sockets.value(socket)->socket()->readAll();
        addMessage("LOG","Received " + QString::number(ba.size()) + " bytes");
        break;
    case SSLIDSocket::SSL_SIGNAL_SSL_ERROR:
        sslErrorsFound(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_STATE_CHANGED:
        changedState(socket);
        break;
    }
}

void SSLManager::socketErrorFound(quint64 id){

    QAbstractSocket::SocketError error = sockets.value(id)->socket()->error();
    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    addMessage("ERROR",QString("Socket Error found: ") + metaEnum.valueToKey(error) + QString(" from Address: ") + addr.toString());

    // Eliminating it from the list.
    removeSocket(id);

}

void SSLManager::sslErrorsFound(quint64 id){
    QList<QSslError> errorlist = sockets.value(id)->socket()->sslErrors();
    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        addMessage("ERROR","SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }

    // Eliminating it from the list.
    removeSocket(id);

}

void SSLManager::changedState(quint64 id){
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

void SSLManager::removeSocket(quint64 id){
    if (sockets.contains(id)) sockets.remove(id);
    qint32 index = queue.indexOf(id);
    if (index != -1) queue.removeAt(index);
}
