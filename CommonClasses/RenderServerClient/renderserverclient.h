#ifndef RENDERSERVERCLIENT_H
#define RENDERSERVERCLIENT_H

#include <QTcpSocket>
#include <QDebug>
#include <QMetaEnum>
#include <QFile>
#include <QTextStream>

#include "RenderServerPackets/renderserverpacket.h"

class RenderServerClient : public QObject
{

    Q_OBJECT

public:

    static const quint8 MSG_TYPE_INFO    = 0;
    static const quint8 MSG_TYPE_ERROR   = 1;
    static const quint8 MSG_TYPE_WARNING = 2;

    explicit RenderServerClient(QObject *parent = nullptr);

    void setPathToPortFile(const QString &portFile);
    void connectToRenderServer();

    void sendPacket(const RenderServerPacket &packet);

    bool isConnected() const;

    RenderServerPacket getPacket();

signals:

    void newPacketArrived();
    void connectionEstablished();
    void newMessage(const QString &msg, const quint8 &msgType);

private slots:

    void onConnected();
    void onDisconnected();
    void onErrorOcurred(QAbstractSocket::SocketError socketError);
    void onStateChanged(QAbstractSocket::SocketState socketState);
    void onReadyRead();

private:
    QTcpSocket *socket;
    QString portFile;

    RenderServerPacket rxPacket;

};

#endif // RENDERSERVERCLIENT_H
