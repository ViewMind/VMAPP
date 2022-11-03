#include "staticthreadlogger.h"

LoggerThread StaticThreadLogger::thread;
QString StaticThreadLogger::app_designation  = "";


StaticThreadLogger::StaticThreadLogger()
{

}

void StaticThreadLogger::StartLogger(const QString &name, const QString &location){
    app_designation = name;
    thread.setLogFile(location);
    if (!thread.isRunning()) thread.start();
}


void StaticThreadLogger::writeLine(const QString trace, const QString &message, const QString &msg_type){
    QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString final_message = "[" + app_designation + "][" + msg_type + "][" + ts + "][" + trace + "] " + message;
    thread.addMessage(final_message);
}

void StaticThreadLogger::log(const QString trace, const QString message){
    StaticThreadLogger::writeLine(trace,message,"INFO");
}

void StaticThreadLogger::error(const QString trace, const QString message){
    StaticThreadLogger::writeLine(trace,message,"ERROR");
}

void StaticThreadLogger::warning(const QString trace, const QString message){
    StaticThreadLogger::writeLine(trace,message,"WARNING");
}

QString StaticThreadLogger::getFullLogFilePath() {
    return thread.getFullLogFilePath();
}
