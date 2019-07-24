#ifndef PROCESSINGSOCKET_H
#define PROCESSINGSOCKET_H

#include <QProcess>
#include <QDateTime>
#include "../../../CommonClasses/SSLIDSocketMap/sslidsocket.h"
#include "../../../CommonClasses/TimeMeasurer/timemeasurer.h"
#include "../../../CommonClasses/DataPacket/datapacket.h"
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"

#define  TIME_FORMAT_STRING  "yyyy_MM_dd_hh_mm_ss"

#define  LIFE_TIMEOUT        5*60*1000         // A socket cannot live more than this.

#define  TIME_MEASURE_ESTABLISHED_CONNECTION   "00_Established Connection"
#define  TIME_MEASURE_INFO_RECEIVED            "01_Information Received"
#define  TIME_MEASURE_DBMNG_CHECK              "02_DB MNG Check Done"
#define  TIME_MEASURE_PROCESSING_DONE          "03_Processing done"
#define  TIME_MEASURE_CLIENT_OK                "04_Client OK"
#define  TIME_MEASURE_DBMNG_STORE              "05_DB MNG Store Done"
#define  TIME_MEASURE_DISCONNECT               "06_To Disconnect"

/*****************************************************************
 * Self contained processing control and client communication class
 * ***************************************************************/

class ProcessingSocket: public SSLIDSocket
{
    Q_OBJECT

public:

    struct ProcessingSocketData {
        QString eyeRepGenPath;
        QString eyeDBMngPath;
        quint64 ID;
        qint32 timeOutInMs;
    };

    ProcessingSocket();
    ProcessingSocket(QSslSocket *newSocket, const ProcessingSocketData &cdata);
    ~ProcessingSocket();

    QString getTimeStampID() const { return transactionID; }

private slots:
    void on_encryptedSuccess();
    void on_socketStateChanged(QAbstractSocket::SocketState state);
    void on_socketError(QAbstractSocket::SocketError error);
    void on_sslErrors(const QList<QSslError> &errors);
    void on_readyRead();
    void on_disconnected();
    void on_timeout();
    void doDisconnects();

    void on_lifeTimeOut();

    void on_eyeRepGenFinished(qint32 status);
    void on_eyeDBMngFinished(qint32 status);

private:

    typedef enum {PS_WAIT_FINSHED_CONNECTION, PS_WAIT_INFO, PS_WAIT_DBMNG_CHECK, PS_WAIT_EYEPROC, PS_WAIT_CLIENT_OK, PS_WAIT_DBMNG_STORE,PS_WAIT_DISCONNECT} ProcessState;

    // Common strings.
    QString timestamp;
    QString transactionID;
    QString workingDir;
    QString etSerial;
    QString instID;
    QString patientHashedID;

    ProcessingSocketData configData;
    ProcessState pstate;
    QTimer lifeTimer;
    QProcess eyeRepGen;
    QProcess eyeDBMng;
    TimeMeasurer mtimer;    

    // Helper functions
    void startTimeoutTimer();    
    QString stateToString();
    void sendCodeToClient(qint32 code, const QString repFile = "");
    void dbMngCheck();
    void dbMngStore();

};

#endif // PROCESSINGSOCKET_H
