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
#include "../../CommonClasses/RawDataContainer/default_configurations.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/RawDataContainer/viewminddatacontainer.h"
#include "../../CommonClasses/RenderServerClient/RenderServerPackets/RenderServerPacketNames.h"
#include "../../CommonClasses/RenderServerClient/renderserverclient.h"
#include "../../CommonClasses/StudyControl/studycontrol.h"
#include "../../CommonClasses/Calibration/calibrationmanager.h"
#include "../../CommonClasses/Calibration/calibrationhistory.h"
#include "../../CommonClasses/HWRecog/processrecognizer.h"
#include "studyendoperations.h"
#include "loaderflowcomm.h"
#include "eyexperimenter_defines.h"

class FlowControl : public QObject
{
    Q_OBJECT

public:
    explicit FlowControl(QObject *parent = Q_NULLPTR, LoaderFlowComm *c = nullptr);
    ~FlowControl() override;
    Q_INVOKABLE void calibrateEyeTracker(bool useSlowCalibration, bool mode3D);
    Q_INVOKABLE bool startTask(qint32 taskIndexInCurrentList);
    Q_INVOKABLE void startStudyEvaluationPhase();
    Q_INVOKABLE void startStudyExamplePhase();
    Q_INVOKABLE void requestStudyData();    
    Q_INVOKABLE void newEvaluation(const QString &evaluationID, const QString &clinician, const QString &selectedProtocol);
    Q_INVOKABLE QVariantList setupCurrentTaskList();

    Q_INVOKABLE bool isExperimentEndOk() const;
    Q_INVOKABLE bool hasLastTaskPassedQCI() const;
    Q_INVOKABLE void keyboardKeyPressed(int key);
    Q_INVOKABLE bool isRenderServerWorking() const;
    Q_INVOKABLE QVariantMap getCalibrationValidationData() const;
    Q_INVOKABLE void storeCalibrationHistoryAsFailedCalibration();

    Q_INVOKABLE bool doesTaskRequireHandCalibration(const QString &task_code) const;
    Q_INVOKABLE bool isTask3D(const QString &task_code) const;

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

    // For debugging.
    Q_INVOKABLE void printLastCalibrationPacketReceived();

    // For checking the debug option in the front end. (QML Code). This will only return true if the vmdebug file exists with the option set to true.
    Q_INVOKABLE bool isSkipETCheckEnabled();

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

    // Signal for more than one instance of the RRS running.
    void checkOnRRSFailed();

    // Once study operations are finalized this signal is emitted. The final operations make take time. So the study end signaled so that the front end might put up
    // a wait screen. And then the study end processing is carried out on a background thread.
    void studyEndProcessingDone();

    // Used to notify loader of information that might come from the server.
    void notifyLoader(QVariantMap information);

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

    // Whenever calibration needs to send a new packet
    void onNewCalibrationPacketAvailable();

private:

    // Delays for a specific time.
    QTimer delayTimer;

    // This identifies the eyetracker.
    QString HMDKey;

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
    LoaderFlowComm *comm;

    // The Study End Processor.
    StudyEndOperations studyEndProcessor;

    // The current task list.
    QVariantList currentEvaluationTaskList;

    // Only valid vlues are 5 or 9 anything else will assume 9
    static const qint32 NUMBER_OF_CALIBRATION_POINTS = 9;

    static const qint32 CALIB_PT_WAIT_TIME_NORMAL    = 1000;
    static const qint32 CALIB_PT_GATHER_TIME_NORMAL  = 2000;
    static const qint32 CALIB_PT_WAIT_TIME_SLOW      = 1500;
    static const qint32 CALIB_PT_GATHER_TIME_SLOW    = 3000;

    static const qint32 HAND_CALIB_START_H           = 0;
    static const qint32 HAND_CALIB_START_V           = 1;
    static const qint32 HAND_CALIB_END               = 2;

    // Minimum number of points to conform a fixation.
    static const qint32 MINIMUM_NUMBER_OF_DATAPOINTS_IN_FIXATION = 4;

    bool initDataContainerForNewTask(const QVariantMap &evaluationDefintion, ViewMindDataContainer *rdc);

};

#endif // FLOWCONTROL_H
