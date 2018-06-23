#ifndef HTMLWRITER_H
#define HTMLWRITER_H

#include <QFile>
#include <QTextStream>

class HTMLWriter
{
public:
    HTMLWriter();    

    // Append messages according to type
    void appendError(const QString &msg) { appendMessage(msg,"#aa0000",true); }
    void appendStandard(const QString &msg) { appendMessage(msg,"#000000"); }
    void appendSuccess(const QString &msg) { appendMessage(msg,"#55aa00",true); }
    void appendWarning(const QString &msg) { appendMessage(msg,"#969600",true); }

    void write(const QString &filename, const QString &title);

private:
    QString htmlData;
    void appendMessage(const QString &msg, const QString &color, bool bold = false, bool plainHTML = false);
};

#endif // HTMLWRITER_H
