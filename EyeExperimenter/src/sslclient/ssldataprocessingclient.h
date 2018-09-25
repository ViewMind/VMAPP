#ifndef SLLDATAPROCESSINGCLIENT_H
#define SLLDATAPROCESSINGCLIENT_H

/****************************************************************************************************
 * This class contains all the methods for communicating the withe processing part of the EyeServer,
 * sending the data for processing and receiving its resutls
 * **************************************************************************************************/

#include "sslclient.h"

class SSLDataProcessingClient: public SSLClient
{
    Q_OBJECT

public:

    SSLDataProcessingClient(QObject *parent = 0, ConfigurationManager *c = nullptr);
    ~SSLDataProcessingClient();

    // Main interface function. The only one required. Everything after this point is predefined.
    void requestReport(bool saveData);

private slots:

    // SSL and TCP Related slots
    void on_encryptedSuccess();
    void on_socketError(QAbstractSocket::SocketError error);
    void on_readyRead();

    // Timeout function.
    void on_timeOut();

private:

    // Flag to indicate that the information has been sent
    bool informationSent;

    // State of the communication process.
    typedef enum {CS_CONNECTING, CS_WAIT_FOR_ACK, CS_WAIT_FOR_REPORT} ClientState;
    ClientState clientState;

    // Starts the whole process
    void connectToServer(bool saveData);

    // Flag that indicates if the transaction finished signal was setn
    bool sentTransactionFinishedSignal;

    // Two steps to send the signal so a function is used.
    void sendFinishedSignal(bool okvalue);

};

#endif // SLLDATAPROCESSINGCLIENT_H
