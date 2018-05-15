#ifndef PROCESSORTHREAD_H
#define PROCESSORTHREAD_H

#include <QThread>
#include <QProcess>
#include "../../CommonClasses/common.h"

class ProcessorThread : public QThread
{
    Q_OBJECT
public:
    ProcessorThread();
    void run();
    void setConfiguration(ConfigurationManager *c);
    bool didItStart() const { return finishCode != -2; }
    bool didItCrash() const { return finishCode == -1;}    
    QString getError(){return error;}
    bool isProcessingEnabled() const;

private:
    int finishCode;
    QString error;
    ConfigurationManager *config;
    void writeIntoConfiguration(const QString &confFile, const QString &configName2Copy, QStringList *errors);

};

#endif // PROCESSORTHREAD_H
