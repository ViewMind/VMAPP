#ifndef DBCOMMMANAGER_H
#define DBCOMMMANAGER_H

#include <QObject>
#include <QMetaEnum>
#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "../../CommonClasses/SQLConn/dbinterface.h"
#include "sslidsocket.h"
#include "ssllistener.h"
#include "server_defines.h"

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
    QHash<quint64,SSLIDSocket*> sockets;

    // Generates ever increasing values for unique socket ids.
    quint64 idGen;

    // Function that cleanly removes a socket
    void removeSocket(quint64 id);

    // Auxiliary functions for error handling and state changing
    void changedState(quint64 id);
    void socketErrorFound(quint64 id);
    void sslErrorsFound(quint64 id);

    // Do the actual DB stuff
    void processSQLRequest(quint64 socket);

    // Auxiliary function to handel the multiple DBInterfaces
    bool initAllDBS();
    DBInterface *getDBIFFromTable(const QString &tableName);


    struct VerifyPatientRetStruct {
        quint8 errorCode;
        qint32 puid;
        qint32 indexOfPatUid;
    };

    // Verify to insert
    quint8 verifyDoctor(const QStringList &columns, const QStringList &values);
    VerifyPatientRetStruct verifyPatient(const QStringList &columns, const QStringList &values);

};

#endif // DBCOMMMANAGER_H
