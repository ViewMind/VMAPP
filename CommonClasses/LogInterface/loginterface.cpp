#include "loginterface.h"

LogInterface::LogInterface()
{
    // By default it creates a text file for logging.
    log = nullptr;
    // Default logFile
    logFile = LOG_FILE_LOG;
}

void LogInterface::setLogFileLocation(const QString &logfile){
    logFile = logfile;
}

void LogInterface::setLogInterface(QPlainTextEdit *pte){
    log = pte;
}

void LogInterface::appendError(const QString &msg){
    if (log != nullptr) {
        appendMessage(msg,"#aa0000",true);
        return;
    }
    // Text File version
    appendMessage(msg,LOG_ERROR);
}

void LogInterface::appendStandard(const QString &msg){
    if (log != nullptr) {
        appendMessage(msg,"#000000");
    }
    appendMessage(msg,LOG_STD);
}

void LogInterface::appendSuccess(const QString &msg){
    if (log != nullptr) {
        appendMessage(msg,"#55aa00",true);
        return;
    }
    appendMessage(msg,LOG_SUCCESS);
}

void LogInterface::appendWarning(const QString &msg){
    if (log != nullptr) {
        appendMessage(msg,"#969600",true);
        return;
    }
    appendMessage(msg,LOG_WARNING);
}

void LogInterface::appendMessage(const QString &msg, const QString &color, bool bold){
    if (log != nullptr){
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
}

void LogInterface::appendMessage(const QString &msg, MessageType type){

    QFile file(logFile);

    if (!file.open(QFile::Append)) return;

    // Prepare the message
    QString s = QDateTime::currentDateTime().toString("dd/MM/yyyy - hh:mm:ss");
    switch (type){
    case LOG_ERROR:
        s = s + ". ERROR: " + msg;
        break;
    case LOG_STD:
        s = s + ". " + msg;
        break;
    case LOG_SUCCESS:
        s = s + ". SUCCESS: " + msg;
        break;
    case LOG_WARNING:
        s = s + ". WARNING: " + msg;
        break;
    }

    // Write the message
    QTextStream writer(&file);
    writer.setCodec(COMMON_TEXT_CODEC);
    writer << s << "\n";
    file.close();
}



