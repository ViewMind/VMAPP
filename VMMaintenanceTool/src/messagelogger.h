#ifndef MESSAGELOGGER_H
#define MESSAGELOGGER_H

#include <QDateTime>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFile>
#include <QDir>
#include <QTextStream>

#include "defines.h"


class MessageLogger
{
public:
    MessageLogger(QListWidget *lister);

    void log(const QString msg);
    void warning(const QString msg);
    void error(const QString msg);
    void success(const QString msg);
    void display(const QString msg);

private:

    const char* MSG_DISPLAY = "DISPLAY";
    const char* MSG_WARNING = "WARNING";
    const char* MSG_SUCCESS = "SUCCESS";
    const char* MSG_ERROR   = "ERROR";
    const char* MSG_LOG     = "LOG";

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
