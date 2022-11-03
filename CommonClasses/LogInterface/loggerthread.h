#ifndef LOGGERTHREAD_H
#define LOGGERTHREAD_H

#include <QThread>
#include <QStringList>
#include <QTextStream>
#include <QMutex>
#include <QFile>
#include <QFileInfo>
#include <QElapsedTimer>
#include <QDebug>

class LoggerThread : public QThread
{
public:
    LoggerThread();

    void run() override;

    void setLogFile(const QString &logfile);
    void addMessage(const QString &message);
    void stopLoggerThread();
    QString getFullLogFilePath() const;

private:

    QStringList messages;
    QMutex mutex;
    bool threadShouldRun;
    QString filename;
    QString popMessage();

};

#endif // LOGGERTHREAD_H
