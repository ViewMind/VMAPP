#ifndef SSLIDSOCKET_H
#define SSLIDSOCKET_H

#include <QSslSocket>
#include <QTimer>
#include <QHostAddress>
#include <QMetaEnum>
#include "../../CommonClasses/DataPacket/datapacket.h"
#include "../../CommonClasses/LogInterface/loginterface.h"

/*****************************************************************
 * Base class that contains an SSL Socke and an ID;
 * ***************************************************************/

class SSLIDSocket: public QObject
{
    Q_OBJECT

public:

    SSLIDSocket();
    SSLIDSocket(QSslSocket *newSocket, quint64 id);
    ~SSLIDSocket();

    QSslSocket* socket() const {return sslSocket;}
    quint64 getID() const { return ID; }
    bool isValid() const { return sslSocket != nullptr; }

signals:
    void socketDone(quint64 id);

protected slots:
    virtual void on_encryptedSuccess();
    virtual void on_socketStateChanged(QAbstractSocket::SocketState state);
    virtual void on_socketError(QAbstractSocket::SocketError error);
    virtual void on_sslErrors(const QList<QSslError> &errors);
    virtual void on_readyRead();
    virtual void on_disconnected();
    virtual void on_timeout();
    virtual void doDisconnects();

protected:

    LogInterface log;
    QSslSocket *sslSocket;
    DataPacket rx;
    QTimer timer;
    quint64 ID;
    bool disconnectReceived;

};

#endif // SSLIDSOCKET_H
