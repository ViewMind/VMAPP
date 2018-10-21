#ifndef DBCOMMMANAGER_H
#define DBCOMMMANAGER_H

#include <QObject>
#include <QMetaEnum>
#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "../../CommonClasses/SQLConn/dbinterface.h"
#include "sslidsocket.h"
#include "ssllistener.h"

class DBCommSSLServer : public QObject
{
    Q_OBJECT
public:
    explicit DBCommSSLServer(QObject *parent = nullptr);
    bool startServer(ConfigurationManager *c);
    DBInterface * getDBInterface() { return &dbConnection; }

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

    // Manages the SQL Connection.
    DBInterface dbConnection;

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

    // Check the password for creating/updating doctor information.
    quint8 checkInstitutionPassword(qint32 UID, const QString &password);

};

#endif // DBCOMMMANAGER_H
