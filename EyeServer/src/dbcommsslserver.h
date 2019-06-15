#ifndef DBCOMMMANAGER_H
#define DBCOMMMANAGER_H

#include <QObject>
#include <QMetaEnum>
#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "../../CommonClasses/SQLConn/dbinterface.h"
#include "../../CommonClasses/SSLIDSocketMap/sslidsocketmap.h"
#include "../../CommonClasses/server_defines.h"
#include "ssllistener.h"

#define  DIRNAME_UPDATE_DIR                "etdir"
#define  DIRNAME_UPDATE_DIR_LOG_SUBIDR     "logs"
#define  DIRNAME_UPDATE_DIR_FLOGS_SUBIDR   "flogs"
#define  FILENAME_EYE_LAUNCHER             "etdir/launcher.exe"
#define  FILENAME_CONFIGURATION            "configuration"
#define  FILENAME_CHANGELOG                "changelog"
#define  FILENAME_EYE_EXPERIMENTER         "EyeExperimenter.exe"

class DBCommSSLServer : public QObject
{
    Q_OBJECT
public:
    explicit DBCommSSLServer(QObject *parent = nullptr);
    bool startServer(ConfigurationManager *c);
    void setDBConnections(DBInterface *base, DBInterface *id, DBInterface *patdata){ dbConnBase = base; dbConnID = id; dbConnPatData = patdata; }

signals:

private slots:
    void on_newConnection();
    void on_lostConnection();
    void on_newSSLSignal(quint64 socket, quint8 signaltype);

private:

    static const qint32 PASS_HASH_SIZE = 64;

    // Logging to file.
    LogInterface log;

    // Pointer to the configuration file
    ConfigurationManager *config;

    // The SQL Connections to the different databases
    DBInterface *dbConnBase;
    DBInterface *dbConnID;
    DBInterface *dbConnPatData;

    // Listens for new connections and stores them in the sockets.
    SSLListener *listener;
    SSLIDSocketMap sockets;

    // Generates ever increasing values for unique socket ids.
    quint64 idGen;

    // Auxiliary functions for error handling and state changing
    void changedState(quint64 id);
    void socketErrorFound(quint64 id);
    void sslErrorsFound(quint64 id);

    // Do the actual DB stuff
    void processSQLRequest(quint64 socket);

    // Update the EyeExperimenter
    void processUpdateRequest(quint64 socket);

    // Auxiliary function to handel the multiple DBInterfaces
    bool initAllDBS();
    DBInterface *getDBIFFromTable(const QString &tableName);

    struct VerifyDBRetStruct {
        quint8 errorCode;
        qint32 puid;
        qint32 indexOfPatUid;
        QString logid;
    };

    // Verify to insert
    VerifyDBRetStruct verifyDoctor(const QStringList &columns, const QStringList &values);
    VerifyDBRetStruct verifyPatient(const QStringList &columns, const QStringList &values);

    // Sends the checksum of one of the exes
    void sendUpdateAns(DataPacket tx, quint64 socket, const QString &ans);

};

#endif // DBCOMMMANAGER_H
