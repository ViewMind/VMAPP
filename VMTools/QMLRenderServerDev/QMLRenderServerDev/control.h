#ifndef CONTROL_H
#define CONTROL_H

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

class Control : public QObject
{

    Q_OBJECT


public:

    explicit Control(QObject *parent = nullptr);

    Q_INVOKABLE void setRenderWindowGeometry(int target_x, int target_y, int target_w, int target_h);
    Q_INVOKABLE void hideRenderWindow();
    Q_INVOKABLE void showRenderWindow();

    void setWindowID(WId winID);

signals:

    void requestWindowGeometry();

private slots:

    void onNewPacketArrived();
    void onConnectionEstablished();
    void onTimeOut();
    void onNewMessage(const QString &msg, const quint8 &msgType);

private:

    RenderServerClient renderServer;

    QTimer baseUpdateTimer;

    QProcess renderServerProcess;

    WId mainWindowID;
    static HWND renderHandle;

    void sendRemoteRenderWindowInformation();


};

#endif // CONTROL_H
