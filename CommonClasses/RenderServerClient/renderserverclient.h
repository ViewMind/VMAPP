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
#include "../debug.h"

class RenderServerClient : public QObject
{

    Q_OBJECT

public:

    static const quint8 MSG_TYPE_INFO    = 0;
    static const quint8 MSG_TYPE_ERROR   = 1;
    static const quint8 MSG_TYPE_WARNING = 2;

    explicit RenderServerClient(QObject *parent = nullptr);

    void startRenderServer(const QString &fullPath, WId mainWinID, bool notaskkill);

    void sendPacket(RenderServerPacket packet);

    // Two conditions must be met for render server to be working: The process must be running AND the client must be connected to the server.
    bool isRenderServerWorking() const;

    bool isReadyToRender() const;

    RenderServerPacket getPacket();

    // Stores the window positiona and size. Then calls show.
    void resizeRenderWindow(qint32 x, qint32 y, qint32 w, qint32 h);

    // Hide Render Window. It does not CHANGE it's hidden state.
    void hideRenderWindow();

    // Shows only if the hidden flag is false.
    void showRenderWindow();

    void setRenderWindowHiddenFlag(bool flag);

    void closeRenderServer();

signals:

    void newPacketArrived();
    void connectionEstablished();
    void readyToRender();
    void renderWindowHandleReady();
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
    void onCoolDownTimerTimeout();

private:
    // The socket for the client connection.
    QTcpSocket *socket;

    // The directory where the server resides.
    QString renderServerDirectory;

    // Used for receiving information.
    RenderServerPacket rxPacket;

    // The render server process.
    QProcess renderServerProcess;

    int frequency_update_packet_counter = 0;

    // The ID of the main display of the application.
    WId mainWindowID;

    // Timer used to get the window handle
    QTimer waitTimer;

    // Requires wait time between sending packets.
    QTimer cooldownTimer;
    QElapsedTimer mtimer;

    // Packet counter. For debugging and messaging.
    qint32 packetCounter;

    // Used to see if there are any packets to send.
    QList<RenderServerPacket> sendPacketQueue;
    bool bytesAreBeingSent;
    bool sentResolutionRequest;

    // Render handle ready emitted flag.
    bool emittedRenderHandleReady;
    bool isConnectedToRRS;

    // The Handle to render Window.
    static HWND renderHandle;

    // The dimension and position of the render window.
    QRect renderWindowGeometry;
    bool renderWindowHidden;

    // The callback that allows to get the handle for the render window.
    static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam);

    const QString PORT_FILE = "selected_port";
    const qint32 POLL_INTERVAL_TO_GET_WINDOW_HANDLE = 600;
    const qint32 POLL_INTERVAL_FOR_SEND_PACKET_CHECK = 10;
    const qint32 COOLDOWN_BETWEEN_PACKETS = 3; // Wait time between two consecutive packages.
    const qint32 PRINT_EVERY_N_FUPDATES = 5;   // We can't print all of frequency update packets or the log will become too large. So we set an interval.

//    // Resolution constants and the field of view.
//    qint32 screenResolutionWidth;
//    qint32 screenResolutionHeight;
//    qreal hFOV;
//    qreal vFOV;

    // Variable to know when we are closing.
    bool onClosing;

    /// PRIVATE FUNCTIONS
    void connectToRenderServer();

};

#endif // RENDERSERVERCLIENT_H
