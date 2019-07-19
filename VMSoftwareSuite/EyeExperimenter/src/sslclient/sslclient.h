#ifndef SSLCLIENT_H
#define SSLCLIENT_H

#include <QSslSocket>
#include <QDir>
#include <QTimer>
#include <QMetaEnum>

#include "../../../CommonClasses/SQLConn/dbdescription.h"
#include "../../../CommonClasses/common.h"
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/DataPacket/datapacket.h"

#define   DEFAULT_TIMEOUT_CONNECTION                    60000
#define   DEFAULT_TIMEOUT_WAIT_ACK                      60000

class SSLClient : public QObject
{
    Q_OBJECT

public:
    explicit SSLClient(QObject *parent = 0, ConfigurationManager *c = nullptr);
    ~SSLClient();

    bool sslEnabled() const {return QSslSocket::supportsSsl();}
    bool getTransactionStatus() const {return transactionIsOk;}

signals:

    // The transaction finished will be sent on disconnect only.
    void transactionFinished();

protected slots:

    // SSL and TCP Related slots
    virtual void on_encryptedSuccess();
    virtual void on_socketError(QAbstractSocket::SocketError error);
    virtual void on_readyRead();
    void on_sslErrors(const QList<QSslError> &errors);
    void on_socketStateChanged(QAbstractSocket::SocketState state);

    // Timer
    virtual void on_timeOut();

protected:
    // All configuration variables.
    ConfigurationManager *config;

    // Ui Message writer
    LogInterface log;

    // The socket for the actual connection
    QSslSocket *socket;

    // Buffers for sending and receiving data
    DataPacket txDP;
    DataPacket rxDP;

    // Connection time out and a variable to determine in which state was the timeout
    QTimer timer;

    // For checking if there were errors
    bool transactionIsOk;

    // Simple fucntion to start the timer with a given value.
    void startTimeoutTimer(qint32 ms);

};

#endif // SSLCLIENT_H
