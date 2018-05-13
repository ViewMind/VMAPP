#ifndef SSLIDSOCKET_H
#define SSLIDSOCKET_H

#include <QSslSocket>

/*****************************************************************
 * The objective of this class is to provide an identifying value
 * to the Sockect. This is required to properley identify the
 * QSslSocket signals that will be all connected to the same slot
 * ***************************************************************/

class SSLIDSocket: public QObject
{
    Q_OBJECT

public:

    static const quint8 SSL_SIGNAL_ENCRYPTED     = 0;
    static const quint8 SSL_SIGNAL_STATE_CHANGED = 1;
    static const quint8 SSL_SIGNAL_SSL_ERROR     = 2;
    static const quint8 SSL_SIGNAL_ERROR         = 3;
    static const quint8 SSL_SIGNAL_READY_READ    = 4;
    static const quint8 SSL_SIGNAL_DISCONNECTED  = 5;

    SSLIDSocket();
    SSLIDSocket(QSslSocket *newSocket, quint64 id);

    QSslSocket* socket() const {return sslSocket;}

    bool isValid() const { return sslSocket != nullptr; }
    quint64 getID() const {return ID;}

signals:
    void sslSignal(quint64 id, quint8 signalID);

private slots:
    void on_encryptedSuccess();
    void on_socketStateChanged(QAbstractSocket::SocketState state);
    void on_socketError(QAbstractSocket::SocketError error);
    void on_sslErrors(const QList<QSslError> &errors);
    void on_readyRead();
    void on_disconnected();

private:
    QSslSocket *sslSocket;
    quint64 ID;

};

#endif // SSLIDSOCKET_H
