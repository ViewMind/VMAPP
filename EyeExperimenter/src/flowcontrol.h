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

#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/RawDataContainer/viewminddatacontainer.h"

#include "../../CommonClasses/RenderServerClient/renderserverclient.h"

#include "../../CommonClasses/StudyControl/studycontrol.h"

#include "../../CommonClasses/Calibration/calibrationmanager.h"
#include "../../CommonClasses/Calibration/calibrationhistory.h"

#include "studyendoperations.h"

#include "eyexperimenter_defines.h"

class FlowControl : public QObject
{
    Q_OBJECT

public:
    explicit FlowControl(QObject *parent = Q_NULLPTR, ConfigurationManager *c = nullptr);
    ~FlowControl() override;
    Q_INVOKABLE void calibrateEyeTracker(bool useSlowCalibration, bool mode3D);
    Q_INVOKABLE bool startNewExperiment(QVariantMap study_config);
    Q_INVOKABLE void startStudyEvaluationPhase();
    Q_INVOKABLE void startStudyExamplePhase();
    Q_INVOKABLE void finalizeStudyOperations();

    Q_INVOKABLE bool isExperimentEndOk() const;
    Q_INVOKABLE void keyboardKeyPressed(int key);
    Q_INVOKABLE bool isRenderServerWorking() const;
    Q_INVOKABLE QVariantMap getCalibrationValidationData() const;
    Q_INVOKABLE void storeCalibrationHistoryAsFailedCalibration();

    Q_INVOKABLE void handCalibrationControl(qint32 command, const QString &which_hand);

    // Remote render server Window Validation Control
    Q_INVOKABLE void setRenderWindowGeometry(int target_x, int target_y, int target_w, int target_h);
    Q_INVOKABLE void setRenderWindowState(bool hidden);
    Q_INVOKABLE void hideRenderWindow();
    Q_INVOKABLE void showRenderWindow();

    Q_INVOKABLE void resetCalibrationHistory();

    // Used mostly for debugging.
    Q_INVOKABLE void renderWaitScreen(const QString &message, bool renderAsCornerTargets = false);

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

    // Once study operations are finalized this signal is emitted. The final operations make take time. So the study end signaled so that the front end might put up
    // a wait screen. And then the study end processing is carried out on a background thread.
    void studyEndProcessingDone();

public slots:

    // When an experiment finishes.
    void onStudyEnd();

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

    // When the calibration process is finished.
    void onCalibrationDone(qint32 code);

    // The sstudy end process is triggered and this funciton is called. For storing data in the local DB and notifying the front end.
    void onStudyEndProcessFinished();


private:

    // Delays for a specific time.
    QTimer delayTimer;

    // Controlling the study class
    StudyControl studyControl;

    // The Calibration Manager.
    CalibrationManager calibrationManager;

    // Structure where the the calibration attempts are stored.
    CalibrationHistory calibrationHistory;

    // Remote Render Server Control. (Client) and required variables.
    RenderServerClient renderServerClient;
    QProcess renderServerProcess;
    WId mainWindowID;
    static HWND renderWindowHandle;

    // The configuration structure
    ConfigurationManager *configuration;

    // The Study End Processor.
    StudyEndOperations studyEndProcessor;

    // Only valid vlues are 5 or 9 anything else will assume 9
    static const qint32 NUMBER_OF_CALIBRATION_POINTS = 9;

    static const qint32 CALIB_PT_WAIT_TIME_NORMAL    = 1000;
    static const qint32 CALIB_PT_GATHER_TIME_NORMAL  = 2000;
    static const qint32 CALIB_PT_WAIT_TIME_SLOW      = 1500;
    static const qint32 CALIB_PT_GATHER_TIME_SLOW    = 3000;

    static const qint32 HAND_CALIB_START_H           = 0;
    static const qint32 HAND_CALIB_START_V           = 1;
    static const qint32 HAND_CALIB_END               = 2;

};

#endif // FLOWCONTROL_H
