#include "ssllistener.h"

SSLListener::SSLListener(QObject *parent):QTcpServer(parent)
{
}

void SSLListener::incomingConnection(qintptr socketDescriptor){

    // Creating the SSL Socket.
    QSslSocket *socket = new QSslSocket();
    if (socket->setSocketDescriptor(socketDescriptor)){
        addPendingConnection(socket);
    }
    else{
        delete socket;
        emit (lostConnection());
    }

}


