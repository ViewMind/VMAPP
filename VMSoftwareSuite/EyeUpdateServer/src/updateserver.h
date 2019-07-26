#ifndef DBCOMMMANAGER_H
#define DBCOMMMANAGER_H

#include <QObject>
#include <QMetaEnum>
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/SSLIDSocketMap/sslidsocketmap.h"
#include "../../../CommonClasses/server_defines.h"
#include "ssllistener.h"
#include "updatesocket.h"

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
    void on_removeSocket(quint64 socket);

private:

    // Logging to file.
    LogInterface log;

    // Pointer to the configuration file
    ConfigurationManager *config;

    // Listens for new connections and stores them in the sockets.
    SSLListener *listener;
    SSLIDSocketMap sockets;

    // Generates ever increasing values for unique socket ids.
    quint64 idGen;

};

#endif // DBCOMMMANAGER_H
