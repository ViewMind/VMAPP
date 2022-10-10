#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QImage>
#include <QTimer>
#include <QElapsedTimer>

#include "../../../CommonClasses/LogInterface/loginterface.h"
#include "../../../CommonClasses/RenderServerClient/renderserverclient.h"

class Control : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QImage image READ image NOTIFY newImageAvailable)

public:

    explicit Control(QObject *parent = nullptr);

    Q_INVOKABLE void connectToRenderServer();
    Q_INVOKABLE void setTargetInDisplayResolution(qint32 width, qint32 height);

    QImage image() const;

signals:

    // Signal to update QML Image on screen.
    void newImageAvailable();

private slots:
    void onNewPacketArrived();
    void onConnectionEstablished();
    void onTimeOut();
    void onNewMessage(const QString &msg, const quint8 &msgType);

private:

    RenderServerClient renderServer;
    QImage displayImage;
    qint32 displayWidth;
    qint32 displayHeight;
    qint32 debugDataFrameCounter;
    QTimer baseUpdateTimer;
    LogInterface logger;

    void LoadImageDataToImage(const QByteArray &imgData, qint32 displayWidth, qint32 displayHeight);
};

#endif // CONTROL_H
