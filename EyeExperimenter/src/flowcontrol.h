#ifndef FLOWCONTROL_H
#define FLOWCONTROL_H

#include <QObject>
#include <QWidget>
#include <QProcess>
#include <QtMath>
#include <QFileDialog>
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>
#include <WinUser.h>
#include <iostream>

#include "../../CommonClasses/debug.h"
#include "../../CommonClasses/wait.h"

#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/RawDataContainer/viewminddatacontainer.h"

#include "../../CommonClasses/RenderServerClient/renderserverclient.h"

#include "../../CommonClasses/Experiments/binding/bindingexperiment.h"
#include "../../CommonClasses/Experiments/nbackfamiliy/nbackrtexperiment.h"
#include "../../CommonClasses/Experiments/gonogo/gonogoexperiment.h"
#include "../../CommonClasses/Experiments/gonogo_spheres/gonogosphereexperiment.h"
#include "../../CommonClasses/Experiments/passball/passballexperiment.h"

#include "../../CommonClasses/EyeTrackingInterface/HPReverb/hpomniceptinterface.h"

#include "../../CommonClasses/Calibration/calibrationmanager.h"

#include "eyexperimenter_defines.h"

class FlowControl : public QWidget
{
    Q_OBJECT

public:
    explicit FlowControl(QWidget *parent = Q_NULLPTR, ConfigurationManager *c = nullptr);
    ~FlowControl() override;
    Q_INVOKABLE void calibrateEyeTracker(bool useSlowCalibration, bool mode3D);
    Q_INVOKABLE bool startNewExperiment(QVariantMap study_config);
    Q_INVOKABLE void startStudyEvaluationPhase();
    Q_INVOKABLE void startStudyExamplePhase();

    Q_INVOKABLE bool isExperimentEndOk() const {return experimentIsOk;}
    Q_INVOKABLE void resolutionCalculations();
    Q_INVOKABLE void keyboardKeyPressed(int key);
    Q_INVOKABLE bool isVROk() const;
    Q_INVOKABLE QVariantMap getCalibrationValidationData() const;

    // Eye Tracking Control commands.
    Q_INVOKABLE bool isConnected() const;

    Q_INVOKABLE void handCalibrationControl(qint32 command, const QString &which_hand);

    // Remote render server Window Validation Control
    Q_INVOKABLE void setRenderWindowGeometry(int target_x, int target_y, int target_w, int target_h);
    Q_INVOKABLE void setRenderWindowState(bool hidden);
    Q_INVOKABLE void hideRenderWindow();
    Q_INVOKABLE void showRenderWindow();

    // Used mostly for debugging.
    Q_INVOKABLE void renderWaitScreen(const QString &message);

    // Required when closing hte application
    Q_INVOKABLE void closeApplication();

    // Required for the ID setting handshake.
    void startRenderServerAndSetWindowID(WId winID);

signals:

    // This tells QML that the experiment has finished.
    void experimentHasFinished();

    // Signals to indicate connection to eyetracker, calibration aborted or finalized.
    void calibrationDone(bool ok);

    // Required in order to continue through normal study flow whenever hand calibration is involved.
    void handCalibrationDone();

    // Singal to update the QML front end with new study string.
    void newExperimentMessages(const QVariantMap &string_value_map);

    // Puts in a request to the QML Front end to get the geometry for the render server window.
    void requestWindowGeometry();

    // Wrongfull disconnect with remote render server.
    void renderServerDisconnect();

public slots:

    // When an experiment finishes.
    void on_experimentFinished(const Experiment::ExperimentResult & er);

    // Whenever the experiment updates message to the front end.
    void onUpdatedExperimentMessages(const QVariantMap &string_value_map);

    // Whenerver a new control packet needs to be sent to the remote render server this signal is emitted by the current study.
    void onNewControlPacketAvailableFromStudy();

    // When a new packet arrives from the render server.
    void onNewPacketArrived();

    // Connection to the render server has been established.
    void onConnectionEstablished();

    // The Render Server is ready to start rendering.
    void onReadyToRender();

    // New Message from the render server client object.
    void onNewMessage(const QString &msg, const quint8 &msgType);

    // Signals that a new packet needs to be sent to the Remote Render Server.
    void onNewCalibrationRenderServerPacketAvailable();

    // When the calibration process is finished.
    void onCalibrationDone(qint32 code);

    // Receives data from the eye tracker. Corrects it. And passes it on.
    void onNewEyeDataAvailable(const EyeTrackerData &data);


private slots:

    // Slot that requests new image to draw from the OpenVR Control Object
    void onRequestUpdate();

private:

    // Render state allows to define what to send to the HMD when using the VR Solution.
    typedef enum { RENDERING_NONE, RENDERING_EXPERIMENT, RENDER_WAIT_SCREEN, RENDERING_CALIBRATION_SCREEN} RenderState;

    // Delays for a specific time.
    QTimer delayTimer;

    // The currently selected experiment
    Experiment *experiment;

    // The currently selected eyetracker
    EyeTrackerInterface *eyeTracker;

    // The Calibration Manager.
    CalibrationManager calibrationManager;

    // Remote Render Server Control. (Client) and required variables.
    RenderServerClient renderServerClient;
    QProcess renderServerProcess;
    WId mainWindowID;
    static HWND renderWindowHandle;

    // The configuration structure
    ConfigurationManager *configuration;

    // Binary status for the end of an experiment.
    bool experimentIsOk;

    // Auxiliary flag used to determine the established connection to the Remote Render Server.
    bool vrOK;

    // The country codes and the function to load them
    QStringList countryList;
    QStringList countryCodes;
    void fillCountryList();

    // Stored value of the Viewmind Logo requried for background image math rendering
    QSizeF backgroundLogoSize;

    // Only valid vlues are 5 or 9 anything else will assume 9
    static const qint32 NUMBER_OF_CALIBRATION_POINTS = 9;

    static const qint32 CALIB_PT_WAIT_TIME_NORMAL = 1000;
    static const qint32 CALIB_PT_GATHER_TIME_NORMAL = 2000;
    static const qint32 CALIB_PT_WAIT_TIME_SLOW = 1500;
    static const qint32 CALIB_PT_GATHER_TIME_SLOW = 3000;

    static const qint32 HAND_CALIB_START_H = 0;
    static const qint32 HAND_CALIB_START_V = 1;
    static const qint32 HAND_CALIB_END     = 2;

    void processCalibrationControlPacket(RenderServerPacket p);


};

#endif // FLOWCONTROL_H
