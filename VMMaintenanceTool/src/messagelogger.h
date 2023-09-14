#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include <QDateTime>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QDir>
#include <QTextStream>

#include "defines.h"
#include "debugoptions.h"

class MessageLogger
{
public:
    MessageLogger(QListWidget *lister);

    static const qint32 MSG_LOG = 0;
    static const qint32 MSG_ERROR = 1;
    static const qint32 MSG_WARNING = 2;
    static const qint32 MSG_DISP = 3;
    static const qint32 MSG_SUCCESS = 4;

    void log(const QString msg);
    void warning(const QString msg);
    void error(const QString msg);
    void success(const QString msg);
    void display(const QString msg);

private:

    QBrush colorWarning;
    QBrush colorError;
    QBrush colorSuccess;
    QBrush colorDisplay;
    QBrush colorText;
    QFont  font;

    QListWidget *uilist;
    QString logFile;

    void addToLog(const QString &type,  const QString &message);

};

#endif // MESSAGELOGGER_H
