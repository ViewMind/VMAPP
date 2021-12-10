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

#include "../../../CommonClasses/Experiments/reading/readingexperiment.h"
#include "../../../CommonClasses/Experiments/binding/imageexperiment.h"
#include "../../../CommonClasses/Experiments/nbackfamiliy/fieldingexperiment.h"
#include "../../../CommonClasses/Experiments/nbackfamiliy/nbackrtexperiment.h"
#include "../../../CommonClasses/Experiments/parkinson/parkinsonexperiment.h"
#include "../../../CommonClasses/Experiments/gonogo/gonogoexperiment.h"
#include "../../../CommonClasses/Experiments/perception/perceptionexperiment.h"
#include "../../../CommonClasses/Experiments/monitorscreen.h"

#include "../../../CommonClasses/EyeTrackingInterface/Mouse/mouseinterface.h"
#include "../../../CommonClasses/EyeTrackingInterface/GazePoint/opengazeinterface.h"
#include "../../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptinterface.h"
#include "../../../CommonClasses/EyeTrackingInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.h"

#include "eyexperimenter_defines.h"


class FlowControl : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY newImageAvailable)

public:
    explicit FlowControl(QWidget *parent = Q_NULLPTR, ConfigurationManager *c = nullptr);
    ~FlowControl() override;
    Q_INVOKABLE void connectToEyeTracker();
    Q_INVOKABLE void calibrateEyeTracker(const QString &eye_to_use);
    Q_INVOKABLE bool startNewExperiment(QVariantMap study_config);
    Q_INVOKABLE void startStudy();
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
    Q_INVOKABLE bool isVROk() const;

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

    // Singal to update the QML front end with new study string.
    void newExperimentMessages(const QVariantMap &string_value_map);

public slots:

    // When an experiment finishes.
    void on_experimentFinished(const Experiment::ExperimentResult & er);

    // Eye tracker control changes
    void onEyeTrackerControl(quint8 code);

    // Whenever the experiment updates message to the front end.
    void onUpdatedExperimentMessages(const QVariantMap &string_value_map);


private slots:

    // Slot that requests new image to draw from the OpenVR Control Object
    void onRequestUpdate();

private:

    // Render state allows to define what to send to the HMD when using the VR Solution.
    typedef enum { RENDERING_NONE, RENDERING_EXPERIMENT, RENDER_WAIT_SCREEN, RENDERING_CALIBRATION_SCREEN} RenderState;

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

    // Flag if the open vr object could be properly initialized.
    bool vrOK;

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

    // The country codes and the function to load them
    QStringList countryList;
    QStringList countryCodes;
    void fillCountryList();

    // Only valid vlues are 5 or 9 anything else will assume 9
    static const qint32 NUMBER_OF_CALIBRATION_POINTS = 9;

};

#endif // FLOWCONTROL_H
