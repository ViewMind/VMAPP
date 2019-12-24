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

    SSLDataProcessingClient(QObject *parent = nullptr, ConfigurationManager *c = nullptr);
    ~SSLDataProcessingClient();

    // Main interface function. The only one required. Everything after this point is predefined.
    void requestReport(bool isDemoMode, const QString &oldRepFile);

    // Request for medical record storage.
    void sendMedicalRecordData(const QString &patuid);

    // Obtaining code allows for more precise messages.
    qint32 getProcessingCode() {return processingACKCode;}
    qint32 getNumberOfEvaluations() {return numberOfEvals;}

private slots:

    // SSL and TCP Related slots
    void on_encryptedSuccess();
    void on_socketError(QAbstractSocket::SocketError error);
    void on_readyRead();

    // Timeout function.
    void on_timeOut();

private:

    // State of the communication process.
    typedef enum {CS_CONNECTING, CS_WAIT_FOR_REPORT, CS_CONNECTING_FOR_MEDREPORT, CS_WAIT_FOR_MEDREP_OK} ClientState;
    ClientState clientState;

    // The last processing code.
    qint32 processingACKCode;

    // The number of evaluations returned.
    qint32 numberOfEvals;

    // Flag to indicate reprocessing.
    QString previousReportFile;

    // Starts the whole process of report requesting.
    void connectToServer(bool isDemoMode, const QString &oldRepFile);    

};

#endif // SLLDATAPROCESSINGCLIENT_H
