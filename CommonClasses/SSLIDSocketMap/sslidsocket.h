#ifndef SSLIDSOCKET_H
#define SSLIDSOCKET_H

#include <QSslSocket>
#include <QTimer>
#include <QProcess>
#include <QDateTime>
#include <QHostAddress>
#include <QMetaEnum>
#include <QElapsedTimer>
#include "../../CommonClasses/DataPacket/datapacket.h"
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

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

class SSLIDSocket: public QObject
{
    Q_OBJECT

public:

    struct SSLIDSocketData {
        QString eyeRepGenPath;
        QString eyeDBMngPath;
        quint64 ID;
        qint32 timeOutInMs;
    };

    SSLIDSocket();
    SSLIDSocket(QSslSocket *newSocket, const SSLIDSocketData &cdata);
    ~SSLIDSocket();

    QSslSocket* socket() const {return sslSocket;}
    quint64 getID() const { return configData.ID; }
    QString getTimeStampID() const { return transactionID; }
    bool isValid() const { return sslSocket != nullptr; }

signals:
    //void sslSignal(quint64 id, quint8 signalID);
    void removeSocket(quint64 id);

private slots:
    void on_encryptedSuccess();
    void on_socketStateChanged(QAbstractSocket::SocketState state);
    void on_socketError(QAbstractSocket::SocketError error);
    void on_sslErrors(const QList<QSslError> &errors);
    void on_readyRead();
    void on_disconnected();
    void on_timeout();

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

    // Flags for sy
    bool disconectedReceived;

    SSLIDSocketData configData;
    ProcessState pstate;
    LogInterface log;
    QSslSocket *sslSocket;
    DataPacket rx;
    QElapsedTimer timeMeasurer;
    QTimer timer;
    QTimer lifeTimer;
    QProcess eyeRepGen;
    QProcess eyeDBMng;
    QHash<QString,quint64> timeMeasures;

    // Helper functions
    void startTimeoutTimer();    
    QString stateToString();
    QString timeMeasuresToString();
    void sendCodeToClient(qint32 code, const QString repFile = "");
    void dbMngCheck();
    void dbMngStore();
    void doDisconnects();

};

#endif // SSLIDSOCKET_H
