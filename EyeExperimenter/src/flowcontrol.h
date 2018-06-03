#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <QObject>
#include <QWidget>
#include <QProcess>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

#include "Experiments/readingexperiment.h"
#include "Experiments/imageexperiment.h"
#include "Experiments/fieldingexperiment.h"

#include "sslclient/sslclient.h"

#include "EyeTrackerInterface/Mouse/mouseinterface.h"
#include "EyeTrackerInterface/RED/redinterface.h"

#include "monitorscreen.h"

class FlowControl : public QWidget
{
    Q_OBJECT
public:
    explicit FlowControl(QWidget *parent = Q_NULLPTR, LogInterface *l = nullptr, ConfigurationManager *c = nullptr);
    FlowControl::~FlowControl();
    Q_INVOKABLE bool connectToEyeTracker();
    Q_INVOKABLE bool calibrateEyeTracker();
    Q_INVOKABLE bool startNewExperiment(qint32 experimentID);
    Q_INVOKABLE bool isConnected() const { return connected; }
    Q_INVOKABLE bool isExperimentEndOk() const {return experimentIsOk;}
    Q_INVOKABLE void setupSecondMonitor();
    Q_INVOKABLE void eyeTrackerChanged();
    Q_INVOKABLE void resolutionCalculations();
    Q_INVOKABLE void checkSSLAvailability() {return sslclient->sslEnabled();}
    Q_INVOKABLE void requestReportData() {sslclient->requestReport();}

signals:

    // This tells QML that the experiment has finished.
    void experimentHasFinished();

    // Transaction finished.
    void sslTransactionFinished();

    // Report generated.
    void reportGenerationFinished();

public slots:

    // When an experiment finishes.
    void on_experimentFinished(const Experiment::ExperimentResult & er);

    // A calibration is requested in the middle of an experiment.
    void requestCalibration();

    // When an SSL Transaction finishes
    void onSLLTransactionFinished(bool allOk);

private:

    // The process for the server.
    QProcess sslServer;

    // The second screen for monitoring the experiments.
    MonitorScreen *monitor;

    // The currently selected experiment
    Experiment *experiment;

    // The currently selected eyetracker
    EyeTrackerInterface *eyeTracker;

    // The Log interface
    LogInterface *logger;

    // The sslclient to send the information process request.
    SSLClient *sslclient;

    // The configuration structure
    ConfigurationManager *configuration;

    // Flags to avoid reconnecting during recalibration
    bool connected;

    // Binary status for the end of an experiment.
    bool experimentIsOk;

    // Helper function to selecte expanded binding files.
    QString getBindingExperiment(bool bc);

};

#endif // FLOWCONTROL_H
