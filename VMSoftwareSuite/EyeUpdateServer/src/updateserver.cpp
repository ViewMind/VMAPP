#include "updateserver.h"

UpdateServer::UpdateServer(QObject *parent) : QObject(parent)
{

    // Creating the the listner.
    listener = new SSLListener(this);

    // Listen for new connections.
    connect(listener,&SSLListener::newConnection,this,&UpdateServer::on_newConnection);
    connect(listener,&SSLListener::lostConnection,this,&UpdateServer::on_lostConnection);

    idGen = 0;

}


void UpdateServer::on_lostConnection(){
    log.appendError("Lost connection to SSLListener");
}

bool UpdateServer::startServer(ConfigurationManager *c){


    // Checking the existance of both directories.
    if (!QDir(DIRNAME_UPDATE_DIR).exists()){
        log.appendError("Could not find etdir");
        return false;
    }

    QDir(".").mkdir(DIRNAME_SERVER_LOGS);
    if (!QDir(DIRNAME_SERVER_LOGS).exists()){
        log.appendError("Could not find or create the log directory");
        return false;
    }

    config = c;
    if (!listener->listen(QHostAddress::Any,TCP_PORT_UPDATE_SERVER)){
        log.appendError("ERROR : Could not start SQL SSL Server: " + listener->errorString());
        return false;
    }

    return true;

}

void UpdateServer::on_newConnection(){

    // New connection is available.
    QSslSocket *sslsocket = (QSslSocket*)(listener->nextPendingConnection());
    UpdateSocket *socket = new UpdateSocket(sslsocket,idGen);

    //log.appendStandard("New connection with id " + QString::number(idGen));

    idGen++;

    if (!socket->isValid()) {
        log.appendError("ERROR: Could not cast incomming socket connection");
        return;
    }

    // Saving the socket.
    sockets.addSocket(socket);

    // Doing the connection SIGNAL-SLOT
    connect(socket,&SSLIDSocket::socketDone,this,&UpdateServer::on_removeSocket);

    log.appendStandard("New update request from " + sslsocket->peerAddress().toString());

    // The SSL procedure.
    sslsocket->setPrivateKey(":/certificates/server.key");
    sslsocket->setLocalCertificate(":/certificates/server.csr");
    sslsocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslsocket->startServerEncryption();

}

void UpdateServer::on_removeSocket(quint64 socket){

    QString where = "onRemoveSocket";
    UpdateSocket *sslsocket = (UpdateSocket*)sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(socket,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        return;
    }

    log.appendStandard("Removing socket: " + QString::number(sslsocket->getID()));

    sockets.releaseSocket(socket,where);
    sockets.deleteSocket(socket,where);

}


