#ifndef CONTROL_H
#define CONTROL_H

#include <QDateTime>
#include "../../../CommonClasses/common.h"
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/HTMLWriter/htmlwriter.h"
#include "../../../CommonClasses/DataAnalysis/rawdataprocessor.h"

#define   EYE_REP_GEN_NAME                              "EyeRepGenerator"
#define   EYE_DIR_LOG                                   "LOGS"
#define   EYE_REP_GEN_VERSION                           "8.3.0" //8.2.0

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
