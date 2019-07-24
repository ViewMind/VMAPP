#include "sslidsocket.h"

SSLIDSocket::SSLIDSocket():QObject(){
    sslSocket = nullptr;
    disconnectReceived = false;
}


SSLIDSocket::SSLIDSocket(QSslSocket *newSocket, quint64 id):QObject()
{

    ID = id;
    sslSocket = nullptr;
    if (newSocket == nullptr) return;
    sslSocket = newSocket;
    disconnectReceived = false;

    // Creating all the connections from signals and slots.
    connect(sslSocket,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(sslSocket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    connect(sslSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(sslSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(sslSocket,&QSslSocket::readyRead,this,&SSLIDSocket::on_readyRead);
    connect(sslSocket,&QSslSocket::disconnected,this,&SSLIDSocket::on_disconnected);

    connect(&timer,&QTimer::timeout,this,&SSLIDSocket::on_timeout);

}

void SSLIDSocket::on_timeout(){
    timer.stop();
}


void SSLIDSocket::doDisconnects(){
    disconnect(sslSocket,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    disconnect(sslSocket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    disconnect(sslSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    disconnect(sslSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    disconnect(sslSocket,&QSslSocket::readyRead,this,&SSLIDSocket::on_readyRead);
    disconnect(sslSocket,&QSslSocket::disconnected,this,&SSLIDSocket::on_disconnected);

    disconnect(&timer,&QTimer::timeout,this,&SSLIDSocket::on_timeout);
}

void SSLIDSocket::on_encryptedSuccess(){
    timer.stop();
}

void SSLIDSocket::on_readyRead(){
    // Should be completely be handled in the reimplementation.
}

void SSLIDSocket::on_disconnected(){
    disconnectReceived = true;
    timer.stop();
}


void SSLIDSocket::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    log.appendError(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
}

void SSLIDSocket::on_socketStateChanged(QAbstractSocket::SocketState state){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    log.appendStandard(QString("SOCKET STATE: ") + metaEnum.valueToKey(state));
}

void SSLIDSocket::on_sslErrors(const QList<QSslError> &errorlist){
    QHostAddress addr = sslSocket->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        log.appendWarning("SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }
}

SSLIDSocket::~SSLIDSocket(){
    timer.stop();
    if (sslSocket != nullptr) delete sslSocket;
}
