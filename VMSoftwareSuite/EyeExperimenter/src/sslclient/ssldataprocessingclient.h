#ifndef SLLDATAPROCESSINGCLIENT_H
#define SLLDATAPROCESSINGCLIENT_H

/****************************************************************************************************
 * This class contains all the methods for communicating the withe processing part of the EyeServer,
 * sending the data for processing and receiving its resutls
 * **************************************************************************************************/

#include "sslclient.h"
#include <QDateTime>

class SSLDataProcessingClient: public SSLClient
{
    Q_OBJECT

public:

    SSLDataProcessingClient(QObject *parent = 0, ConfigurationManager *c = nullptr);
    ~SSLDataProcessingClient();

    // Main interface function. The only one required. Everything after this point is predefined.
    void requestReport(bool saveData, const QString &oldRepFile);

    // Obtaining code allows for more precise messages.
    quint8 getProcessingCode() {return processingACKCode;}

private slots:

    // SSL and TCP Related slots
    void on_encryptedSuccess();
    void on_socketError(QAbstractSocket::SocketError error);
    void on_readyRead();

    // Timeout function.
    void on_timeOut();

private:

    // State of the communication process.
    typedef enum {CS_CONNECTING, CS_WAIT_FOR_REPORT} ClientState;
    ClientState clientState;

    // The last processing code.
    quint8 processingACKCode;

    // Flag to indicate reprocessing.
    bool reprocessRequest;

    // Starts the whole process
    void connectToServer(bool saveData, const QString &oldRepFile);

};

#endif // SLLDATAPROCESSINGCLIENT_H
