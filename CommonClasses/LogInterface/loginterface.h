#ifndef LOGINTERFACE_H
#define LOGINTERFACE_H

#include <QDateTime>
#include <QTextStream>
#include <QDebug>
#include <QFile>

// The log file name
#define   LOG_FILE_LOG                                  "logfile.log"

// Defining text codec for all text files.
#define   COMMON_TEXT_CODEC                             "UTF-8"

class LogInterface: public QObject
{
    Q_OBJECT

public:

    LogInterface();
    void setGraphicalLogInterface() {graphicalLogInterface = true;}
    void setLogFileLocation(const QString &logfile);

    // Append messages according to type
    void appendError(const QString &msg);
    void appendStandard(const QString &msg);
    void appendSuccess(const QString &msg);
    void appendWarning(const QString &msg);

signals:
    void newUiMessage(const QString &html);

private:
    bool graphicalLogInterface;
    QString logFile;
    typedef enum {LOG_STD, LOG_ERROR, LOG_WARNING, LOG_SUCCESS} MessageType;

    // The actual functions that appends the messages.
    void appendMessage(const QString &msg, const QString &color, bool bold = false);
    void appendMessage(const QString &msg, MessageType type);

};

#endif // LOGINTERFACE_H
