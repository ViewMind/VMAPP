#ifndef LOGINTERFACE_H
#define LOGINTERFACE_H

#include <QPlainTextEdit>

class LogInterface
{
public:
    LogInterface();
    void setLogInterface(QPlainTextEdit *pte){log = pte; enabled = true;}

    // Append messages according to type
    void appendError(const QString &msg);
    void appendStandard(const QString &msg);
    void appendSuccess(const QString &msg);
    void appendWarning(const QString &msg);

private:
    bool enabled;
    QPlainTextEdit *log;

    // The actual functions that appends the messages.
    void appendMessage(const QString &msg, const QString &color, bool bold = false);

};

#endif // LOGINTERFACE_H
