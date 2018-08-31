#ifndef CONTROL_H
#define CONTROL_H

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/HTMLWriter/htmlwriter.h"
#include "../../CommonClasses/DataAnalysis/rawdataprocessor.h"

#define   EYE_REP_GEN_NAME                              "EyeRepGenerator"
#define   EYE_REP_GEN_VERSION                           "3.2.1"

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
    HTMLWriter log;
    RawDataProcessor data_processor;

    // All necessary steps to properly quit
    void exitProgram();

    // Load arguments from the command line if present.
    void loadArguments();

    // Adds to the configuration based on command line arguments.
    void addToConfigFromCmdLine(const QString &field, const QString &value);
};

#endif // CONTROL_H
