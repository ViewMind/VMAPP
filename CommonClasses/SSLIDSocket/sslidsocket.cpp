#include "sslidsocket.h"

SSLIDSocket::SSLIDSocket():QObject(){
    sslSocket = nullptr;
    ID = 0;
}

SSLIDSocket::SSLIDSocket(QSslSocket *newSocket, quint64 id):QObject()
{

    sslSocket = nullptr;
    ID = id;
    if (newSocket == nullptr) return;

    sslSocket = newSocket;

    // Creating all the connections from signals and slots.
    connect(sslSocket,&QSslSocket::encrypted,this,&SSLIDSocket::on_encryptedSuccess);
    connect(sslSocket,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(sslSocket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    connect(sslSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(sslSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(sslSocket,&QSslSocket::readyRead,this,&SSLIDSocket::on_readyRead);
    connect(sslSocket,&QSslSocket::disconnected,this,&SSLIDSocket::on_disconnected);
}


void SSLIDSocket::on_disconnected(){
    emit(sslSignal(ID,SSL_SIGNAL_DISCONNECTED));
}

void SSLIDSocket::on_encryptedSuccess(){
    emit(sslSignal(ID,SSL_SIGNAL_ENCRYPTED));
}

void SSLIDSocket::on_readyRead(){
    emit(sslSignal(ID,SSL_SIGNAL_READY_READ));
}

void SSLIDSocket::on_socketError(QAbstractSocket::SocketError error){
    Q_UNUSED(error);
    emit(sslSignal(ID,SSL_SIGNAL_ERROR));
}

void SSLIDSocket::on_socketStateChanged(QAbstractSocket::SocketState state){
    Q_UNUSED(state);
    emit(sslSignal(ID,SSL_SIGNAL_STATE_CHANGED));
}

void SSLIDSocket::on_sslErrors(const QList<QSslError> &errors){
    Q_UNUSED(errors);
    emit(sslSignal(ID,SSL_SIGNAL_SSL_ERROR));
}
