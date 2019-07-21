#ifndef DBCOMMMANAGER_H
#define DBCOMMMANAGER_H

#include <QObject>
#include <QMetaEnum>
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/SSLIDSocketMap/sslidsocketmap.h"
#include "../../../CommonClasses/server_defines.h"
#include "ssllistener.h"

#define  DIRNAME_UPDATE_DIR                "etdir"
#define  DIRNAME_UPDATE_DIR_LOG_SUBIDR     "logs"
#define  DIRNAME_UPDATE_DIR_FLOGS_SUBIDR   "flogs"
#define  FILENAME_EYE_LAUNCHER             "etdir/launcher.exe"
#define  FILENAME_CONFIGURATION            "configuration"
#define  FILENAME_CHANGELOG                "changelog"
#define  FILENAME_EYE_EXPERIMENTER         "EyeExperimenter.exe"

class UpdateServer : public QObject
{
    Q_OBJECT
public:
    explicit UpdateServer(QObject *parent = nullptr);
    bool startServer(ConfigurationManager *c);

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

    // Listens for new connections and stores them in the sockets.
    SSLListener *listener;
    SSLIDSocketMap sockets;

    // Generates ever increasing values for unique socket ids.
    quint64 idGen;

    // Auxiliary functions for error handling and state changing
    void changedState(quint64 id);
    void socketErrorFound(quint64 id);
    void sslErrorsFound(quint64 id);

    // Update the EyeExperimenter
    void processUpdateRequest(quint64 socket);

    // Sends the checksum of one of the exes
    void sendUpdateAns(DataPacket tx, quint64 socket, const QString &ans);

};

#endif // DBCOMMMANAGER_H
