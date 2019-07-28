#ifndef CONTROL_H
#define CONTROL_H

#include <QSslSocket>
#include <QObject>
#include <QTimer>
#include <QMetaEnum>
#include <iostream>

#define VIEWMIND_DATA_REPO          "viewmind-raw-eye-data"
#define RAW_DATA_SERVER_PASSWORD    "hashpass"
#define RAW_DATA_SERVER_DELWORKDIR  "delworkdir"
#define FILE_RAW_DATA_SERVER_CONF   "conf"

#include "../../../CommonClasses/SSLIDSocketMap/sslidsocketmap.h"
#include "ssllistener.h"
#include "rawdataserversocket.h"

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);
    void startServer();

private slots:
    void on_removeSocket(quint64);
    void on_newConnection();

signals:
    void exitRequested();

private:

    ConfigurationManager config;
    LogInterface logger;

    // Listening from new connections.
    SSLListener *listener;
    SSLIDSocketMap sockets;

    int idGen;


};

#endif // CONTROL_H
