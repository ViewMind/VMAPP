#include "renderserverclient.h"

RenderServerClient::RenderServerClient(QObject *parent):QObject(parent)
{
    socket = new QTcpSocket(this);
    connect(socket,&QTcpSocket::connected,this,&RenderServerClient::onConnected);
    connect(socket,&QTcpSocket::stateChanged,this,&RenderServerClient::onStateChanged);
    connect(socket,&QTcpSocket::errorOccurred,this,&RenderServerClient::onErrorOcurred);
    connect(socket,&QTcpSocket::disconnected,this,&RenderServerClient::onDisconnected);
    connect(socket,&QTcpSocket::readyRead,this,&RenderServerClient::onReadyRead);
    portFile = "";
}

void RenderServerClient::setPathToPortFile(const QString &portFile){
   this->portFile = portFile;
}

void RenderServerClient::onReadyRead() {

    RenderServerPacket::RXState state =  this->rxPacket.rxBytes(socket->readAll());
    if (state == RenderServerPacket::RX_DONE){
        //qDebug() << "New Packet Arrived";
        emit RenderServerClient::newPacketArrived();
    }
    else if (state == RenderServerPacket::RX_ERROR){
        emit RenderServerClient::newMessage("Failed in receiving packet. Reason: " + this->rxPacket.getError(),MSG_TYPE_ERROR);
    }

}

bool RenderServerClient::isConnected() const {
    return (socket->state() == QTcpSocket::ConnectedState);
}

void RenderServerClient::connectToRenderServer() {

    // We need to get the prot file first.
    quint16 PORT = 0;

    QFile pfile(portFile);
    if (!pfile.exists()) {
        emit RenderServerClient::newMessage("Port file '" + portFile + "' does not exist",MSG_TYPE_ERROR);
        return;
    }

    if (!pfile.open(QFile::ReadOnly)){
        emit RenderServerClient::newMessage("Failed to open port file '" + portFile + "' for reading",MSG_TYPE_ERROR);
        return;
    }

    QTextStream reader(&pfile);
    QString portString = reader.readAll();
    pfile.close();

    bool ok = false;
    PORT = static_cast<quint16> (portString.toUInt(&ok));
    if (!ok){
        emit RenderServerClient::newMessage("Bad content in the por file. Invalid int: '" + portString + "'",MSG_TYPE_ERROR);
        return;
    }

    socket->connectToHost("localhost",PORT);
}

RenderServerPacket RenderServerClient::getPacket() {
    RenderServerPacket p = this->rxPacket;
    this->rxPacket.resetForRX();
    return p;
}

void RenderServerClient::sendPacket(const RenderServerPacket &packet){
    socket->write(packet.getByteArrayToSend());
}

void RenderServerClient::onConnected() {
    emit RenderServerClient::connectionEstablished();
}

void RenderServerClient::onDisconnected(){
    emit RenderServerClient::newMessage("Got Disconnected",MSG_TYPE_WARNING);
}

void RenderServerClient::onErrorOcurred(QAbstractSocket::SocketError socketError){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    QString name = metaEnum.valueToKey(socketError);
    name = "ERROR Ocurred: " +  name;
    emit RenderServerClient::newMessage(name,MSG_TYPE_ERROR);
}

void RenderServerClient::onStateChanged(QAbstractSocket::SocketState socketState){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    QString name = metaEnum.valueToKey(socketState);
    name = "Socket State Change: " +  name;
    emit RenderServerClient::newMessage(name,MSG_TYPE_WARNING);
}

