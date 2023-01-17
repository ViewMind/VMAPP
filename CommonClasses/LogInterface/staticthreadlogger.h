#ifndef STATICTHREADLOGGER_H
#define STATICTHREADLOGGER_H

#include <QDateTime>
#include "loggerthread.h"

class StaticThreadLogger
{
public:
    StaticThreadLogger();

    static void StartLogger(const QString &name, const QString &location);

    static void log(const QString trace, const QString message);
    static void error(const QString trace, const QString message);
    static void warning(const QString trace, const QString message);
    static void kill();

    static QString getFullLogFilePath();

private:
    static LoggerThread thread;
    static QString app_designation;
    static void writeLine(const QString trace, const QString &message ,const QString &msg_type);
};



#endif // STATICTHREADLOGGER_H
