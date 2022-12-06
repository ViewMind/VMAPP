#ifndef CONTROL_H
#define CONTROL_H

#define BINDING
//#define NBACK
//#define GONOGO

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QTime>
#include <QRect>
#include <QElapsedTimer>
#include <QWindow>
#include <QProcess>
#include <Windows.h>
#include <WinUser.h>
#include <QRandomGenerator>

#include "../../../CommonClasses/RenderServerClient/RenderServerGraphics/animationmanager.h"
#include "../../../CommonClasses/LogInterface/staticthreadlogger.h"
#include "../../../CommonClasses/RenderServerClient/renderserverclient.h"
#include "../../../CommonClasses/Experiments/nbackfamiliy/nbackmanager.h"
#include "../../../CommonClasses/Experiments/binding/bindingmanager.h"
#include "../../../CommonClasses/Experiments/gonogo/gonogomanager.h"

class Control : public QObject
{

    Q_OBJECT


public:

    explicit Control(QObject *parent = nullptr);

    Q_INVOKABLE void setRenderWindowGeometry(int target_x, int target_y, int target_w, int target_h);
    Q_INVOKABLE void hideRenderWindow();
    Q_INVOKABLE void showRenderWindow();
    Q_INVOKABLE void nextStudyExplanation();
    Q_INVOKABLE void enablePacketLog(bool enable);
    Q_INVOKABLE void startRenderingStudy();
    Q_INVOKABLE void packetBurst();
    Q_INVOKABLE void appClose();
    Q_INVOKABLE bool checkRenderServerStatus();
    Q_INVOKABLE void forceConnect();

    void setWindowID(WId winID);

signals:

    void requestWindowGeometry();
    void updateEyeTracker(qreal x, qreal y);

private slots:

    void onNewPacketArrived();
    void onConnectionEstablished();
    void onTimeOut();
    void onUpdateAnimation();
    void onReachedAnimationStop(const QString &variable, qint32 animationStopIndex, qint32 animationSignalType);
    void onReadyToRender();
    void onNewMessage(const QString &msg, const quint8 &msgType);

private:

    RenderServerClient renderServer;

    QTimer baseUpdateTimer;
    QElapsedTimer mtimer;

    QProcess renderServerProcess;

    NBackManager nback;
    BindingManager binding;
    GoNoGoManager gonogo;
    qint32 expScreen;

    RenderServerScene updateImage;
    QString messageInScene;

    AnimationManager animator;
    qint32 messageID;
    qint32 movingCircleID;

    WId mainWindowID;
    static HWND renderHandle;

    void sendRemoteRenderWindowInformation();

    void setBackgroundImage(qreal w, qreal h);

    bool configureCircleAnimation();




};

#endif // CONTROL_H
