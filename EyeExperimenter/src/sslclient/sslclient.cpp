#include "sslclient.h"

SSLClient::SSLClient(QObject *parent, ConfigurationManager *c) :
    QObject(parent)
{

    clientConfigured = false;

    if (!QSslSocket::supportsSsl()){
        log.appendError("There is no support for SSL. Please request help from ViewMind to solve this issue.");
        return;
    }

    // Creating the socket and making the connections.
    socket = new QSslSocket(this);
    connect(socket,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    connect(socket,SIGNAL(readyRead()),this,SLOT(on_readyRead()));

    // Timer connection
    connect(&timer,&QTimer::timeout,this,&SSLClient::on_timeOut);

    // Setting up the configuation and checking that everything is there.
    config = c;

    if (config == nullptr){
        log.appendError("BAD SSLCLIENT INITIALIZATION");
        return;
    }

    if (!c->containsKeyword(CONFIG_SERVER_ADDRESS)){
        log.appendError("Server address was not set in the configuration file");
        return;
    }

    if (!c->containsKeyword(CONFIG_CONNECTION_TIMEOUT)){
        log.appendError("Connection timeout was not configured");
        return;
    }

}

void SSLClient::on_encryptedSuccess(){
}

void SSLClient::on_readyRead(){

}

//************************************* Socket and SSL Errors, Socket state changes ****************************************

void SSLClient::on_sslErrors(const QList<QSslError> &errors){
    for (qint32 i = 0; i < errors.size(); i++){
        log.appendWarning("SSL ERROR: " + errors.at(i).errorString());
    }
    socket->ignoreSslErrors();
}

void SSLClient::on_socketStateChanged(QAbstractSocket::SocketState state){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    log.appendStandard(QString("Log: Socket state - ") + metaEnum.valueToKey(state));
    if (state == QAbstractSocket::UnconnectedState){
        emit(diconnectionFinished());
    }
}

void SSLClient::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    log.appendWarning(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
    // If there is an error then the timer should be stopped.
    if (timer.isActive()) timer.stop();
    socket->disconnectFromHost();
}

//*************************************************************************************************************************

void SSLClient::on_timeOut(){
}

void SSLClient::startTimeoutTimer(qint32 ms){
    timer.setInterval(ms);
    timer.start();
}

SSLClient::~SSLClient()
{

}

