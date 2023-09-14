#ifndef MAINTENANCEMANAGER_H
#define MAINTENANCEMANAGER_H

#include <QThread>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QProcess>

#include "messagelogger.h"
#include "debugoptions.h"
#include "defines.h"
#include "langs.h"


class MaintenanceManager : public QThread
{
    Q_OBJECT
public:

    typedef enum {ACTION_UPDATE,ACTION_NONE} Action;

    MaintenanceManager();

    void setAction(Action action);

    void run();

    Action getAction() const;

    bool wasLastActionASuccess() const;

    QString getEyeExpWorkDir() const;

signals:

    void message(qint32 type, QString message);
    void progressUpdate(qreal percent, QString message);

private:

    Action currentAction;
    bool actionSuccessfull;
    QString eyeExpWorkDir;

    void error(const QString &s);
    void succes(const QString &s);
    void warning(const QString &s);
    void display(const QString &s);
    void log(const QString &s);

    void updateTheApp();

};

#endif // MAINTENANCEMANAGER_H
