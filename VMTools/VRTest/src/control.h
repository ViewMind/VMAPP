#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include "../../../CommonClasses/OpenVRControlObject/openvrcontrolobject.h"
#include "targettest.h"
#include "viveeyepoller.h"

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);

    Q_INVOKABLE void startTest();
    Q_INVOKABLE void stopTest();

signals:

public slots:
    void onCalibrationTimerTimeout();

private:
    OpenVRControlObject *openvrco;
    TargetTest tt;

    static const qint32 CALIBRATION_WAIT = 1000; // Waits 1 second for transition
    static const qint32 CALIBRATION_SHOW = 3000; // Gathers data for 3 seconds.

    QTimer calibrationTimer;
    QList<QPoint> calibrationPoints;
    qint32 calibrationPointIndex;
    bool isWaiting;

    VIVEEyePoller eyetracker;

};

#endif // CONTROL_H
