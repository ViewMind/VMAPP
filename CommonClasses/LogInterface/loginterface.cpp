#include "loginterface.h"

LogInterface::LogInterface()
{
    enabled = false;
}

void LogInterface::appendError(const QString &msg){
    appendMessage(msg,"#aa0000",true);
}

void LogInterface::appendStandard(const QString &msg){
    appendMessage(msg,"#000000");
}

void LogInterface::appendSuccess(const QString &msg){
    appendMessage(msg,"#55aa00",true);
}

void LogInterface::appendWarning(const QString &msg){
    appendMessage(msg,"#969600",true);
}

void LogInterface::appendMessage(const QString &msg, const QString &color, bool bold){
    if (!enabled) return;
    QString html = "<p><span style=\"font-family: 'Courier New'; color: " + color;
    if (bold){
        html = html + "; font-weight: bold\">";
    }
    else{
        html = html + "; font-weight: normal\">";
    }
    html = html + msg + "</span></p>";
    log->appendHtml(html);
}
