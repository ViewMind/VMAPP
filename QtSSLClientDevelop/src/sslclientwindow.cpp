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

    connect(socket,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));

}

SSLClient::~SSLClient()
{
    delete ui;
}

void SSLClient::on_pbConnect_clicked()
{
    socket->connectToHostEncrypted("192.168.1.10",54912);
}

void SSLClient::on_sslErrors(const QList<QSslError> &errors){
    for (qint32 i = 0; i < errors.size(); i++){
        ui->pteLog->appendPlainText("SSL ERROR: " + errors.at(i).errorString());
    }
    socket->ignoreSslErrors();
}

void SSLClient::on_encryptedSuccess(){
    ui->pteLog->appendPlainText("ENCRYPTED CONNECTION ESTABLISHED");

    // Transmitting the data.
    DataPacket dp;
    if (!dp.addFile("C:/Users/Viewmind/Documents/ExperimentOutputs/00_test/binding_bc_2_x_0000.dat")){
        ui->pteLog->appendPlainText("Could not append the file");
        return;
    }

    dp.addValue(568);
    dp.addString("Este es mi primer paquete");
    dp.addValue(-6988.7);

    // Sending the raw data.
    ui->pteLog->appendPlainText("Sending the raw data: " + QString::number(dp.toByteArray().size()));

    qint64 num = socket->write(dp.toByteArray().constData());

    ui->pteLog->appendPlainText("Number of bytes actually sent: " + QString::number(num));


}

void SSLClient::on_socketStateChanged(QAbstractSocket::SocketState state){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    ui->pteLog->appendPlainText(QString("SOCKET STATE: ") + metaEnum.valueToKey(state));
}

void SSLClient::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    ui->pteLog->appendPlainText(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
}
