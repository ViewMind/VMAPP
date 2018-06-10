#ifndef SSLCLIENT_H
#define SSLCLIENT_H

#include <QDialog>
#include <QSslSocket>
#include <QFileDialog>
#include <QDir>
#include <QTimer>
#include <QMetaEnum>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/DataPacket/datapacket.h"

#define   DEFAULT_TIMEOUT_CONNECTION                    60000
#define   DEFAULT_TIMEOUT_WAIT_ACK                      60000

namespace Ui {
class SSLClient;
}

class SSLClient : public QObject
{
    Q_OBJECT

public:
    explicit SSLClient(QObject *parent = 0, ConfigurationManager *c = nullptr);
    bool sslEnabled() const {return QSslSocket::supportsSsl();}
    ~SSLClient();
    void requestReport();

signals:

    // Notifyies that the attempt has finished. The parameter indicates that there was an error;
    void transactionFinished(bool allOk);

private slots:

    // SSL Related slots
    void on_encryptedSuccess();
    void on_socketStateChanged(QAbstractSocket::SocketState state);
    void on_socketError(QAbstractSocket::SocketError error);
    void on_sslErrors(const QList<QSslError> &errors);
    void on_readyRead();

    // Timer
    void on_timeOut();

private:
    // All configuration variables.
    ConfigurationManager *config;

    // Ui Message writer
    LogInterface log;

    // The socket for the actual connection
    QSslSocket *socket;

    // Buffers for sending and receiving data
    DataPacket txDP;
    DataPacket rxDP;

    // Flag to indicate that the information has been sent
    bool informationSent;

    // Flag that indicates if the transaction finished signal was setn
    bool sentTransactionFinishedSignal;

    // Connection time out and a variable to determine in which state was the timeout
    QTimer timer;
    typedef enum {CS_CONNECTING, CS_WAIT_FOR_ACK, CS_WAIT_FOR_REPORT} ClientState;
    ClientState clientState;

    // Configures and starts the timer and uses a default value if one is not present
    void startTimeoutTimer();

    // Starts the whole process
    void connectToServer();

    // Two steps to send the signal so a fucntion is used.
    void sendFinishedSignal(bool okvalue);

};

#endif // SSLCLIENT_H
