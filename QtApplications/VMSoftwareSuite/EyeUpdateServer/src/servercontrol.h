#ifndef SERVERCONTROL_H
#define SERVERCONTROL_H

#include <QObject>
#include <iostream>
#include "../../../CommonClasses/LogInterface/loginterface.h"
#include "updateserver.h"

class ServerControl : public QObject
{
    Q_OBJECT
public:
    explicit ServerControl(QObject *parent = nullptr);
    void startServer();

signals:
    void exitRequested();

private:
    UpdateServer updateServer;
    LogInterface log;
    ConfigurationManager config;
};

#endif // SERVERCONTROL_H
