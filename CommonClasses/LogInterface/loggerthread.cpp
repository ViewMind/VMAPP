#include "loggerthread.h"

LoggerThread::LoggerThread(){

    threadShouldRun = true;

}


void LoggerThread::run(){

    threadShouldRun = true;

    while (threadShouldRun){

        if (messages.count() > 0){

            //qDebug() << "THERE ARE MESSAGE TO LOG";

            QString msg = popMessage();
            qint32 max_retrys = 10;

            QFile file(filename);

            bool fileOpen = false;
            while (!fileOpen){
                fileOpen = file.open(QFile::Append);
                if (!fileOpen){
                    msleep(10);
                    max_retrys--;
                    if (max_retrys == 0) break;
                }
            }

            if (!fileOpen) continue;

            QTextStream writer(&file);
            writer << msg + "\n";
            file.close();

        }

    }

}

void LoggerThread::setLogFile(const QString &logfile){
    filename = logfile;
}

QString LoggerThread::getFullLogFilePath() const {
    QFileInfo info(filename);
    return info.absoluteFilePath();
}

void LoggerThread::stopLoggerThread(){
    threadShouldRun = false;
}

void LoggerThread::addMessage(const QString &message){
    mutex.lock();
    messages << message;
    mutex.unlock();
}

QString LoggerThread::popMessage(){

    if (messages.isEmpty()) return "";

    QString ans;
    mutex.lock();
    ans = messages.first();
    messages.removeFirst();
    mutex.unlock();

    return ans;
}
