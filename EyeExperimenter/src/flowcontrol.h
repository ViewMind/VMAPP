#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <QObject>
#include <QWidget>
#include <QProcess>
#include <QtMath>
#include <QFileDialog>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/PNGWriter/imagereportdrawer.h"

#include "Experiments/readingexperiment.h"
#include "Experiments/imageexperiment.h"
#include "Experiments/fieldingexperiment.h"

#include "sslclient/ssldataprocessingclient.h"

#include "EyeTrackerInterface/Mouse/mouseinterface.h"
#include "EyeTrackerInterface/RED/redinterface.h"
#include "EyeTrackerInterface/GazePoint/opengazeinterface.h"

#include "monitorscreen.h"


class FlowControl : public QWidget
{
    Q_OBJECT
public:
    explicit FlowControl(QWidget *parent = Q_NULLPTR, ConfigurationManager *c = nullptr);
    FlowControl::~FlowControl();
    Q_INVOKABLE void connectToEyeTracker();
    Q_INVOKABLE void calibrateEyeTracker();
    Q_INVOKABLE bool startNewExperiment(qint32 experimentID);
    Q_INVOKABLE bool isConnected() const { return connected; }
    Q_INVOKABLE bool isCalibrated() const { return calibrated; }
    Q_INVOKABLE bool isExperimentEndOk() const {return experimentIsOk;}
    Q_INVOKABLE void setupSecondMonitor();
    Q_INVOKABLE void eyeTrackerChanged();
    Q_INVOKABLE void resolutionCalculations();
    Q_INVOKABLE bool checkSSLAvailability() {return sslDataProcessingClient->sslEnabled();}
    Q_INVOKABLE void requestReportData();
    Q_INVOKABLE bool isSSLTransactionOK() const {return sslTransactionAllOk;}
    Q_INVOKABLE QString getReportDataField(const QString &key);
    Q_INVOKABLE int getReportResultBarPosition(const QString &key);
    Q_INVOKABLE void saveReport();
    Q_INVOKABLE void saveReportAs(const QString &title);
    Q_INVOKABLE void setUse3BindingTargets(bool use3) {use3BindingTargetsEnable = use3;}
    Q_INVOKABLE bool getUse3BindingTargets() { return use3BindingTargetsEnable; }

signals:

    // This tells QML that the experiment has finished.
    void experimentHasFinished();

    // Transaction finished.
    void sslTransactionFinished();

    // Signals to indicate all is good.
    void connectedToEyeTracker(bool ok);
    void calibrationDone(bool ok);

public slots:

    // When an experiment finishes.
    void on_experimentFinished(const Experiment::ExperimentResult & er);

    // A calibration is requested in the middle of an experiment.
    void requestCalibration();

    // When an SSL Transaction finishes
    void onSLLTransactionFinished(bool allOk);

    // Eye tracker control changes
    void onEyeTrackerControl(quint8 code);

    // FOR Debugging.
    void onStateChanged(QProcess::ProcessState newState);
    void onErrorOccurred(QProcess::ProcessError error);

private:

    // The process for the server.
    QProcess sslServer;

    // Delays saving the report until the wait dialog can be shown.
    QTimer delayTimer;

    // The second screen for monitoring the experiments.
    MonitorScreen *monitor;

    // The currently selected experiment
    Experiment *experiment;

    // The currently selected eyetracker
    EyeTrackerInterface *eyeTracker;

    // The Log interface
    LogInterface logger;

    // Flag to indicate the use of 3 Binding Targets instead of 2.
    bool use3BindingTargetsEnable;

    // The sslclient to send the information process request.
    SSLDataProcessingClient *sslDataProcessingClient;

    // The configuration structure
    ConfigurationManager *configuration;

    // The structure where the report data is saved.
    ConfigurationManager reportData;

    // Flags to avoid reconnecting during recalibration
    bool connected;

    // Flag to check if the eyetracker is calibrated or not
    bool calibrated;

    // Binary status for the end of an experiment.
    bool experimentIsOk;

    // Flag to check on transaction status
    bool sslTransactionAllOk;

    // Helper function to selecte expanded binding files.
    QString getBindingExperiment(bool bc);

    // Calculates the segment of the result bar based on boundaries.
    int calculateSegment(qreal value, QList<qreal> segments, bool largerBetter);

    // The country codes and the function to load them
    QStringList countryList;
    QStringList countryCodes;
    void fillCountryList();


};

#endif // FLOWCONTROL_H
