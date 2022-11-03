#ifndef RENDERSERVERCLIENT_H
#define RENDERSERVERCLIENT_H

#include <QTcpSocket>
#include <QDebug>
#include <QMetaEnum>
#include <QFile>
#include <QTextStream>
#include <QTimer>
#include <QFileInfo>
#include <QDir>
#include <QRect>
#include <QElapsedTimer>
#include <QWindow>
#include <QProcess>
#include <Windows.h>
#include <WinUser.h>

#include "RenderServerPackets/renderserverpacket.h"

class RenderServerClient : public QObject
{

    Q_OBJECT

public:

    static const quint8 MSG_TYPE_INFO    = 0;
    static const quint8 MSG_TYPE_ERROR   = 1;
    static const quint8 MSG_TYPE_WARNING = 2;

    explicit RenderServerClient(QObject *parent = nullptr);

    void startRenderServer(const QString &fullPath, WId mainWinID);

    void sendPacket(const RenderServerPacket &packet);

    // Two conditions must be met for render server to be working: The process must be running AND the client must be connected to the server.
    bool isRenderServerWorking() const;

    RenderServerPacket getPacket();

    void resizeRenderWindow(qint32 x, qint32 y, qint32 w, qint32 h);

signals:

    void newPacketArrived();
    void connectionEstablished();
    void newMessage(const QString &msg, const quint8 &msgType);

private slots:

    // Socket Slots
    void onConnected();
    void onDisconnected();
    void onErrorOcurred(QAbstractSocket::SocketError socketError);
    void onStateChanged(QAbstractSocket::SocketState socketState);
    void onReadyRead();

    // Process Slots
    void onProcessErrorOcurred(QProcess::ProcessError error);
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessStarted();
    void onProcessStateChanged(QProcess::ProcessState newState);

    // Timer slots
    void onWaitTimerTimeout();

private:
    // The socket for the client connection.
    QTcpSocket *socket;

    // The directory where the server resides.
    QString renderServerDirectory;

    // Used for receiving information.
    RenderServerPacket rxPacket;

    // The render server process.
    QProcess renderServerProcess;

    // The ID of the main display of the application.
    WId mainWindowID;

    // Timer used to get the window handle
    QTimer waitTimer;

    // The Handle to render Window.
    static HWND renderHandle;

    // The callback that allows to get the handle for the render window.
    static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);

    const QString PORT_FILE = "selected_port";
    const qint32 POLL_INTERVAL_TO_GET_WINDOW_HANDLE = 600;

    /// PRIVATE FUNCTIONS
    void connectToRenderServer();

};

#endif // RENDERSERVERCLIENT_H
