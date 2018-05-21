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
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/ImageExplorer/imageexplorer.h"
#include "../../CommonClasses/DataPacket/datapacket.h"

#define   DEFAULT_TIMEOUT_CONNECTION                    60000
#define   DEFAULT_TIMEOUT_WAIT_ACK                      60000

namespace Ui {
class SSLClient;
}

class SSLClient : public QDialog
{
    Q_OBJECT

public:
    explicit SSLClient(QWidget *parent = 0, ConfigurationManager *c = nullptr);
    ~SSLClient();

private slots:

    // SSL Related slots
    void on_encryptedSuccess();
    void on_socketStateChanged(QAbstractSocket::SocketState state);
    void on_socketError(QAbstractSocket::SocketError error);
    void on_sslErrors(const QList<QSslError> &errors);
    void on_readyRead();

    // Timer
    void on_timeOut();

    // GUI Slots
    void on_pbSearch_clicked();
    void on_pbRequestReport_clicked();

private:
    Ui::SSLClient *ui;

    // All configuration variables.
    ConfigurationManager *config;

    // Ui Message writer
    LogInterface log;

    // To show the report once received.
    ImageExplorer reportShower;

    // The socket for the actual connection
    QSslSocket *socket;

    // Buffers for sending and receiving data
    DataPacket txDP;
    DataPacket rxDP;

    // Flag to indicate that the information has been sent
    bool informationSent;

    // Connection time out and a variable to determine in which state was the timeout
    QTimer timer;
    typedef enum {CS_CONNECTING, CS_WAIT_FOR_ACK, CS_WAIT_FOR_REPORT} ClientState;
    ClientState clientState;

    // Configures and starts the timer and uses a default value if one is not present
    void startTimeoutTimer();
};

#endif // SSLCLIENT_H
