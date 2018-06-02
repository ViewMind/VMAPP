#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <QObject>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

#include "Experiments/readingexperiment.h"
#include "Experiments/imageexperiment.h"
#include "Experiments/fieldingexperiment.h"

#include "EyeTrackerInterface/Mouse/mouseinterface.h"
//#include "EyeTrackerInterface/RED/redinterface.h"

#include "monitorscreen.h"

class FlowControl : public QObject
{
    Q_OBJECT
public:
    explicit FlowControl(QObject *parent = nullptr, LogInterface *l = nullptr, ConfigurationManager *c = nullptr);
    Q_INVOKABLE bool connectToEyeTracker();
    Q_INVOKABLE bool calibrateEyeTracker();
    Q_INVOKABLE bool startNewExperiment(qint32 experimentID);
    Q_INVOKABLE bool isConnected() const { return connected; }
    Q_INVOKABLE bool isExperimentEndOk() const {return experimentIsOk;}

signals:

    // This tells QML that the experiment has finished.
    void experimentHasFinished();

public slots:

    // When an experiment finishes.
    void on_experimentFinished(const Experiment::ExperimentResult & er);

    // A calibration is requested in the middle of an experiment.
    void requestCalibration();

private:

    // The second screen for monitoring the experiments.
    MonitorScreen *monitor;

    // The currently selected experiment
    Experiment *experiment;

    // The currently selected eyetracker
    EyeTrackerInterface *eyeTracker;

    // The Log interface
    LogInterface *logger;

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
