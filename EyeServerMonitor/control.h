#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QProcess>
#include <QMetaEnum>
#include <QFile>
#include <QTextStream>
#include <iostream>
#include "../../CommonClasses/server_defines.h"
#include "../../CommonClasses/LogInterface/loginterface.h"

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);
    void launchServer();

private slots:
    void onFinished(int exitCode);
    void onErrorOcurred(QProcess::ProcessError error);
    void onReadyReadStandardError();
    void onReadyReadStandardOutput();

signals:
    void exitRequested();

private:
    QProcess eyeServer;
    LogInterface logger;
    bool sendingMail;
    void sendMail(const QString &err);
};

#endif // CONTROL_H
