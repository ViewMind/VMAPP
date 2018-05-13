#include "sslclientwindow.h"
#include "ui_sslclientwindow.h"

SSLClient::SSLClient(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SSLClient)
{
    ui->setupUi(this);

    if (!QSslSocket::supportsSsl()){
        ui->pteLog->appendPlainText("SSL NOT SUPPORTED");
        ui->pbConnect->setEnabled(false);
    }

    socket = new QSslSocket(this);

}

SSLClient::~SSLClient()
{
    delete ui;
}

void SSLClient::on_pbConnect_clicked()
{
    connect(socket,SIGNAL(encrypted()),this,SLOT(on_EncryptedSuccess()));
    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
}

void SSLClient::on_sslErrors(const QList<QSslError> &errors){
    for (qint32 i = 0; i < errors.size(); i++){
        ui->pteLog->appendPlainText("SSL ERROR: " + errors.at(i).errorString());
    }
}

void SSLClient::on_encryptedSuccess(){
    ui->pteLog->appendPlainText("ENCRYPTED CONNECTION ESTABLISHED");
}

void SSLClient::on_socketStateChanged(QAbstractSocket::SocketState state){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    ui->pteLog->appendPlainText("SOCKET STATE: " + metaEnum.valueToKey(state));
}

void SSLClient::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    ui->pteLog->appendPlainText("SOCKET ERROR: " + metaEnum.valueToKey(error));
}
