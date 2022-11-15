#ifndef CONTROL_H
#define CONTROL_H

#define  BINDING
//#define  NBACK
//#define GONOGO

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QRect>
#include <QElapsedTimer>
#include <QWindow>
#include <QProcess>
#include <Windows.h>
#include <WinUser.h>

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

    void setWindowID(WId winID);

signals:

    void requestWindowGeometry();

private slots:

    void onNewPacketArrived();
    void onConnectionEstablished();
    void onTimeOut();
    void onFastTimer();
    void onReadyToRender();
    void onNewMessage(const QString &msg, const quint8 &msgType);

private:

    RenderServerClient renderServer;

    QTimer baseUpdateTimer;
    QTimer fastTimer;

    QProcess renderServerProcess;

    NBackManager nback;
    BindingManager binding;
    GoNoGoManager gonogo;
    qint32 expScreen;

    WId mainWindowID;
    static HWND renderHandle;

    void sendRemoteRenderWindowInformation();


};

#endif // CONTROL_H
