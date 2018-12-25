#ifndef SERVERCONTROL_H
#define SERVERCONTROL_H

#include <QObject>
#include <iostream>
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "dataprocessingsslserver.h"
#include "dbcommsslserver.h"

class ServerControl : public QObject
{
    Q_OBJECT
public:
    explicit ServerControl(QObject *parent = nullptr);
    void startServer();

signals:
    void exitRequested();

private:
    DBCommSSLServer dbSSLServer;
    DataProcessingSSLServer dataProcessingSSLServer;
    LogInterface log;
    ConfigurationManager config;

    // The database interfaces
    DBInterface dbConnBase;
    DBInterface dbConnID;
    DBInterface dbConnPatData;
};

#endif // SERVERCONTROL_H
