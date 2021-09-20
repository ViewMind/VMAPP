#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QSslSocket>
#include <QTimer>
#include <QCryptographicHash>
#include <QProcess>
#include <QCoreApplication>
#include <QMetaEnum>
#include <QDataStream>
#include "../../../../CommonClasses/DataPacket/datapacket.h"
#include "../../../../CommonClasses/SQLConn/dbdescription.h"
#include "../../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../../CommonClasses/common.h"

#define  LAUNCHER_VERSION               "3.0.0"  // Previous was 2.1.2. Change is to signal X64 Version.

#define  FILE_EYEEXP_SETTINGS           "../settings"
#define  FILE_EYEEXP_CONFIGURATION      "../configuration"
#define  FILE_EYEEXP_CONFIGURATION_BKP  "../configuration.old"
#define  FILE_EYEEXP_EXE                "../EyeExperimenter.exe"
#define  FILE_EYEEXP_LOG                "../logfile.log"
#define  FILE_EYEEXP_OLDEXE             "../previous.exe"
#define  FILE_EYEEXP_LAUNCHER           "EyeLauncherClient.exe"
#define  FILE_EYEEXP_CHANGELOG          "changelog"

#define  TIMEOUT_CONN_READY_POLLING      1*1000        // In miliseconds

class Loader : public QObject
{
    Q_OBJECT
public:

    explicit Loader(QObject *parent = nullptr);
    Q_INVOKABLE QString getStringForKey(const QString &key);
    Q_INVOKABLE void checkForUpdates();
    Q_INVOKABLE QString getTitleString();

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

    typedef enum {CS_CONNECTING,CS_GETTING_INFO,CS_POLLING} ConnectionState;

    LogInterface logger;

    // Variables obtained from the configuration and settings of the EyeExperimenter.
    QString instUID;
    QString eyeexpID;
    QString serverIP;
    bool    bkpLocalDB;
    QString selectedLanguage;
    QString eyeLauncherHash;
    QString eyeConfigurationHash;
    QString eyeExperimenterHash;
    qint32  timeoutConnection;
    qint32  timeoutGetUpdateCheck;

    // Local language configuration
    ConfigurationManager language;

    // The connection to the server, timer and receive package.
    QSslSocket *serverConn;
    DataPacket rx;
    QTimer timeoutTimer;
    ConnectionState connectionState;

    QString updateMessage;

    // The process for starting the eye experimenter.
    QProcess eyeExpProcess;
    QString changeLogFile;

    // Function that gets all frequency log files.
    void getAllFreqLogFilesNotSent(const QSet<QString> &sentAllReady);

};

#endif // LOADER_H