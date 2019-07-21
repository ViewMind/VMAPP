#ifndef SSLMANAGER_H
#define SSLMANAGER_H

#include <QMetaEnum>
#include <QHash>
#include <QtMath>

#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/SSLIDSocketMap/sslidsocketmap.h"
#include "../../../CommonClasses/server_defines.h"
#include "ssllistener.h"

class DataProcessingSSLServer: public QObject
{
    Q_OBJECT
public:
    DataProcessingSSLServer(QObject *parent = Q_NULLPTR);

    // Start the server
    bool startServer(ConfigurationManager *c);

private slots:
    void on_newConnection();
    void on_removeSocket(quint64 socket);

private:

    // Logging to file.
    LogInterface log;

    // The configuration
    ConfigurationManager *config;

    // Listens for incoming connections and enables de SSL capabilities by using a QSSLSocket
    SSLListener *listener;

    // The queue of information that needs to be processed
    SSLIDSocketMap sockets;

    // Generates ever increasing values for unique socket ids.
    quint64 idGen;

};

#endif // SSLMANAGER_H
