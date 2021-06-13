#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <QObject>
#include <QWidget>
#include <QProcess>
#include <QtMath>
#include <QFileDialog>
#include <QScreen>

#include <iostream>

#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h"
#include "../../../CommonClasses/RawDataContainer/viewminddatacontainer.h"

#include "Experiments/readingexperiment.h"
#include "Experiments/imageexperiment.h"
#include "Experiments/fieldingexperiment.h"
#include "Experiments/nbackrtexperiment.h"
#include "Experiments/parkinsonexperiment.h"
#include "Experiments/gonogoexperiment.h"
#include "Experiments/perceptionexperiment.h"

#include "EyeTrackerInterface/Mouse/mouseinterface.h"
#include "EyeTrackerInterface/GazePoint/opengazeinterface.h"
#include "EyeTrackerInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.h"

#include "monitorscreen.h"
#include "eyexperimenter_defines.h"


class FlowControl : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY newImageAvailable)

public:
    explicit FlowControl(QWidget *parent = Q_NULLPTR, ConfigurationManager *c = nullptr);
    ~FlowControl();
    Q_INVOKABLE void connectToEyeTracker();
    Q_INVOKABLE void calibrateEyeTracker(quint8 eye_to_use);
    Q_INVOKABLE bool startNewExperiment(QVariantMap study_config);
    Q_INVOKABLE bool isConnected() const { return connected; }
    Q_INVOKABLE bool isCalibrated() const;
    Q_INVOKABLE bool isRightEyeCalibrated() const;
    Q_INVOKABLE bool isLeftEyeCalibrated() const;
    Q_INVOKABLE bool isExperimentEndOk() const {return experimentIsOk;}
    Q_INVOKABLE void setupSecondMonitor();
    Q_INVOKABLE void eyeTrackerChanged();
    Q_INVOKABLE void resolutionCalculations();
    Q_INVOKABLE void keyboardKeyPressed(int key);
    Q_INVOKABLE void stopRenderingVR();
    Q_INVOKABLE void generateWaitScreen(const QString &message);

    // The image to be shown.
    QImage image() const;

signals:

    // This tells QML that the experiment has finished.
    void experimentHasFinished();

    // Signals to indicate all is good.
    void connectedToEyeTracker(bool ok);
    void calibrationDone(bool ok);

    // Signal to update QML Image on screen.
    void newImageAvailable();

public slots:

    // When an experiment finishes.
    void on_experimentFinished(const Experiment::ExperimentResult & er);

    // Eye tracker control changes
    void onEyeTrackerControl(quint8 code);


private slots:

    // Slot that requests new image to draw from the OpenVR Control Object
    void onRequestUpdate();

private:

    // Render state allows to define what to send to the HMD when using the VR Solution.
    typedef enum { RENDERING_NONE, RENDERING_EXPERIMENT, RENDER_WAIT_SCREEN} RenderState;

    // Delays saving the report until the wait dialog can be shown.
    QTimer delayTimer;

    // The second screen for monitoring the experiments.
    MonitorScreen *monitor;

    // Selected eye for doing an study
    QString selected_eye_to_use;

    // The currently selected experiment
    Experiment *experiment;

    // The currently selected eyetracker
    EyeTrackerInterface *eyeTracker;

    // Open VR Control object if VR is ENABLED.
    OpenVRControlObject *openvrco;

    // Definining the render state for VR
    RenderState renderState;

    // Display image used when using VR Solution. And the Wait Screen.
    QImage displayImage;
    QImage waitScreen;
    QImage logo;  // the viewmind logo.
    QFont waitFont;
    QColor waitScreenBaseColor;

    // The Log interface
    LogInterface logger;

    // The configuration structure
    ConfigurationManager *configuration;

    // Flags to avoid reconnecting during recalibration
    bool connected;

    // Flag to check if the eyetracker is calibrated or not. And if not which eye failed.
    bool calibrated;
    bool rightEyeCalibrated;
    bool leftEyeCalibrated;

    // Binary status for the end of an experiment.
    bool experimentIsOk;

    // Checks that all mandatory data parameters have been set.
    bool checkAllProcessingParameters();

    // The country codes and the function to load them
    QStringList countryList;
    QStringList countryCodes;
    void fillCountryList();


};

#endif // FLOWCONTROL_H
