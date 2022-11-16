#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <QObject>
#include <QWidget>
#include <QProcess>
#include <QtMath>
#include <QFileDialog>
#include <QScreen>
#include <Windows.h>
#include <WinUser.h>
#include <iostream>

#include "../../CommonClasses/debug.h"

#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/RawDataContainer/viewminddatacontainer.h"

#include "../../CommonClasses/RenderServerClient/renderserverclient.h"

#include "../../CommonClasses/Experiments/binding/bindingexperiment.h"
#include "../../CommonClasses/Experiments/nbackfamiliy/nbackrtexperiment.h"
#include "../../CommonClasses/Experiments/gonogo/gonogoexperiment.h"

#include "../../CommonClasses/EyeTrackingInterface/Mouse/mouseinterface.h"
#include "../../CommonClasses/EyeTrackingInterface/GazePoint/opengazeinterface.h"
#include "../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptinterface.h"
#include "../../CommonClasses/EyeTrackingInterface/HTCVIVEEyePro/htcviveeyeproeyetrackinginterface.h"

#include "eyexperimenter_defines.h"

class FlowControl : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY newImageAvailable)

public:
    explicit FlowControl(QWidget *parent = Q_NULLPTR, ConfigurationManager *c = nullptr);
    ~FlowControl() override;
    Q_INVOKABLE void connectToEyeTracker();
    Q_INVOKABLE void calibrateEyeTracker(bool useSlowCalibration);
    Q_INVOKABLE bool startNewExperiment(QVariantMap study_config);
    Q_INVOKABLE void startStudyEvaluationPhase();
    Q_INVOKABLE void startStudyExamplePhase();
    Q_INVOKABLE bool isConnected() const { return connected; }
    Q_INVOKABLE bool isCalibrated() const;
    Q_INVOKABLE bool isRightEyeCalibrated() const;
    Q_INVOKABLE bool isLeftEyeCalibrated() const;
    Q_INVOKABLE bool isExperimentEndOk() const {return experimentIsOk;}
    Q_INVOKABLE void eyeTrackerChanged();
    Q_INVOKABLE void resolutionCalculations();
    Q_INVOKABLE void keyboardKeyPressed(int key);
    Q_INVOKABLE void stopRenderingVR();
    Q_INVOKABLE void generateWaitScreen(const QString &message);
    Q_INVOKABLE bool isVROk() const;
    Q_INVOKABLE QVariantMap getCalibrationValidationData() const;
    // This is a debugging funciton which will only return true when a coefficient file is loaded or the mouse is selected.
    Q_INVOKABLE bool autoValidateCalibration() const;

    // Remote render server Window Validation Control
    Q_INVOKABLE void setRenderWindowGeometry(int target_x, int target_y, int target_w, int target_h);
    Q_INVOKABLE void hideRenderWindow();
    Q_INVOKABLE void showRenderWindow();

    // The image to be shown.
    QImage image() const;

    // Required for the ID setting handshake.
    void startRenderServerAndSetWindowID(WId winID);

signals:

    // This tells QML that the experiment has finished.
    void experimentHasFinished();

    // Signals to indicate connection to eyetracker, calibration aborted or finalized.
    void connectedToEyeTracker(bool ok);
    void calibrationDone(bool ok);

    // Signal to update QML Image on screen.
    void newImageAvailable();

    // Singal to update the QML front end with new study string.
    void newExperimentMessages(const QVariantMap &string_value_map);

    // Puts in a request to the QML Front end to get the geometry for the render server window.
    void requestWindowGeometry();

public slots:

    // When an experiment finishes.
    void on_experimentFinished(const Experiment::ExperimentResult & er);

    // Eye tracker control changes
    void onEyeTrackerControl(quint8 code);

    // Whenever the experiment updates message to the front end.
    void onUpdatedExperimentMessages(const QVariantMap &string_value_map);

    // When a new packet arrives from the render server.
    void onNewPacketArrived();

    // Connection to the render server has been established.
    void onConnectionEstablished();

    // The Render Server is ready to start rendering.
    void onReadyToRender();

    // New Message from the render server client object.
    void onNewMessage(const QString &msg, const quint8 &msgType);

private slots:

    // Slot that requests new image to draw from the OpenVR Control Object
    void onRequestUpdate();

private:

    // Render state allows to define what to send to the HMD when using the VR Solution.
    typedef enum { RENDERING_NONE, RENDERING_EXPERIMENT, RENDER_WAIT_SCREEN, RENDERING_CALIBRATION_SCREEN} RenderState;

    // Delays saving the report until the wait dialog can be shown.
    QTimer delayTimer;

    // The currently selected experiment
    Experiment *experiment;

    // The currently selected eyetracker
    EyeTrackerInterface *eyeTracker;

    // Remote Render Server Control. (Client) and required variables.
    RenderServerClient renderServerClient;
    QProcess renderServerProcess;
    WId mainWindowID;
    static HWND renderWindowHandle;

    // Flag if the open vr object could be properly initialized.
    bool vrOK;

    // Flag to indicate if VR is in use.
    bool usingVR;

    // Definining the render state for VR
    RenderState renderState;

    // Display image used when using VR Solution. And the Wait Screen.
//    QImage displayImage;
    RenderServerScene *displayImage;
//    QImage waitScreen;
//    QImage logo;  // the viewmind logo.
//    QFont waitFont;
//    QColor waitScreenBaseColor;
    RenderServerScene *waitScreen;

    // The Log interface
    // LogInterface logger;

    // The configuration structure
    ConfigurationManager *configuration;

    // The eye as defined by the calibration.
    QString selected_eye_to_use;

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

    static const qint32 CALIB_PT_WAIT_TIME_NORMAL = 1000;
    static const qint32 CALIB_PT_GATHER_TIME_NORMAL = 2000;
    static const qint32 CALIB_PT_WAIT_TIME_SLOW = 1500;
    static const qint32 CALIB_PT_GATHER_TIME_SLOW = 3000;


};

#endif // FLOWCONTROL_H
