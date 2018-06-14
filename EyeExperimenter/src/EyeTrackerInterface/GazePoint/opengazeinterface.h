#ifndef OPENGAZEINTERFACE_H
#define OPENGAZEINTERFACE_H

#include <QTcpSocket>
#include <QMetaEnum>

#include "../eyetrackerinterface.h"
#include "opengazecommand.h"
#include "eventdetection.h"

#define  USE_LEFT_EYE          0
#define  USE_RIGHT_EYE         1
#define  USE_BOTH_EYES         2
#define  CALIBRATION_THRESHOLD 0.7

class OpenGazeInterface: public EyeTrackerInterface
{
    Q_OBJECT

public:
    OpenGazeInterface(QObject *parent = 0, qreal width = 1, qreal height = 1);
    ~OpenGazeInterface();
    void connectToEyeTracker();
    void enableUpdating(bool enable);
    void disconnectFromEyeTracker();
    void calibrate(EyeTrackerCalibrationParameters params);

private slots:

    void on_connected();
    void on_socketError(QAbstractSocket::SocketError error);
    void on_readyRead();
    void on_disconnected();
    void on_calibrationAborted();

private:

    // Assuming fixed parameters for both the address and the port where the ET Server is.
    const quint16 TCP_SERVER_PORT = 4242;
    const QString TCP_SERVER_ADDRESS = "127.0.0.1";

    // Detect events during calibration
    EventDetection *eventDetecter;

    // To provent wrongly sending the disconnect signal, as it is an error signal
    bool shouldDisconnect;

    // Flag to determine if in the middle of a connection process.
    bool isConnecting;

    // Connection to the ET Server
    QTcpSocket *socket;

    // Buffers the current command in order to process it.
    QByteArray buffer;

    // Eye data to use
    qint32 eyeDataToUse;

    // Function that takes command depending on the received command.
    void processReceivedCommand(const OpenGazeCommand &cmd);

    // Shorcut functions for clarity.
    bool canUseLeft() const {return (eyeDataToUse == USE_LEFT_EYE) || (eyeDataToUse == USE_BOTH_EYES);}
    bool canUseRight() const {return (eyeDataToUse == USE_LEFT_EYE) || (eyeDataToUse == USE_BOTH_EYES);}

};

#endif // OPENGAZEINTERFACE_H
