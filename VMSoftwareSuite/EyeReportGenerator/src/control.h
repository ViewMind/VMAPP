#ifndef CONTROL_H
#define CONTROL_H

#include <QDateTime>
#include "../../../CommonClasses/common.h"
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/HTMLWriter/htmlwriter.h"
#include "../../../CommonClasses/DataAnalysis/rawdataprocessor.h"

#define   EYE_REP_GEN_NAME                              "EyeRepGenerator"
#define   EYE_DIR_LOG                                   "LOGS"
#define   EYE_REP_GEN_VERSION                           "8.3.1" //8.3.1

/****************************************************************************
 * Version: 8.3.1 23/11/2020
 * -> Added the capabilities of processing go no go files.
 * -> Fixed frequency checking in reading with question 
 *****************************************************************************/

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);

    // Starts the application functionality.
    void run();

signals:
    // Singal used ot indicate that the processing is done.
    void done();

private slots:
    void onAppendMsg(const QString &msg, qint32 type);

private:
    ConfigurationManager configuration;
    LogInterface log;
    RawDataProcessor data_processor;

    // All necessary steps to properly quit
    void exitProgram();

};

#endif // CONTROL_H
