#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QSslSocket>
#include <QTimer>
#include <QCryptographicHash>
#include <QProcess>
#include <QCoreApplication>
#include <QMetaEnum>
#include "../../../CommonClasses/DataPacket/datapacket.h"
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/common.h"

#define  SERVER_IP                      "192.168.1.10"    // Local server
//#define  SERVER_IP                      "18.220.30.34"  // Production server
#define  FILE_EYEEXP_SETTINGS           "../settings"
#define  FILE_EYEEXP_CONFIGURATION      "../configuration"
#define  FILE_EYEEXP_EXE                "../EyeExperimenter.exe"
#define  FILE_EYEEXP_OLDEXE             "../previous.exe"
#define  FILE_EYEEXP_CHANGELOG          "changelog"

#define  TIMEOUT_TO_CONNECT              60*1000       // In miliseconds
#define  TIMEOUT_TO_GET_UPDATE_CHECK     80*1000       // In miliseconds
#define  TIMEOUT_TO_GET_EXE              300*1000      // In miliseconds
#define  TIMEOUT_CONN_READY_POLLING      1*1000        // In miliseconds

class Loader : public QObject
{
    Q_OBJECT
public:

    typedef enum {CS_CONNECTING_FOR_UPDATE,
                  CS_CONNECTING_FOR_CHECK,
                  CS_CHECK_UPDATE_SENT,
                  CS_WAIT_FOR_CONNECTION_READY,
                  CS_GET_UPDATE_SENT,
                  CS_STARTING_EYEEXP,
                  CS_NONE} CheckState;

    explicit Loader(QObject *parent = nullptr);
    Q_INVOKABLE QString getStringForKey(const QString &key);
    Q_INVOKABLE void checkForUpdates();

    void requestExeUpdate();
    void startEyeExperimenter();

signals:
    void changeMessage(QString msg);

public slots:

    // SSL and TCP Related slots
    void on_encryptedSuccess();
    void on_socketError(QAbstractSocket::SocketError error);
    void on_readyRead();
    void on_sslErrors(const QList<QSslError> &errors);
    void on_socketStateChanged(QAbstractSocket::SocketState state);

    // Timer
    void on_timeOut();

private:
    LogInterface logger;
    bool loadingError;
    ConfigurationManager language;
    QSslSocket *serverConn;
    QTimer timeoutTimer;
    CheckState connectionState;
    QString updateMessage;
    QString hashLocalExe;
    DataPacket rx;
    QProcess eyeExpProcess;
    QString selectedLanguage;
    QString changeLogFile;
    bool isConnReady;

};

#endif // LOADER_H
