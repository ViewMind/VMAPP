#ifndef HTCVIVEEYEPROEYETRACKINGINTERFACE_H
#define HTCVIVEEYEPROEYETRACKINGINTERFACE_H

#include "../eyetrackerinterface.h"
#include "viveeyepoller.h"
#include "calibrationtargets.h"

class HTCViveEyeProEyeTrackingInterface: public EyeTrackerInterface
{
    Q_OBJECT
public:
    HTCViveEyeProEyeTrackingInterface(QObject *parent = nullptr, qreal width = 1, qreal height = 1);

    void connectToEyeTracker();

    void enableUpdating(bool enable);

    void disconnectFromEyeTracker();

    void calibrate(EyeTrackerCalibrationParameters params);

public slots:
    void updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l);    

private slots:
    void onCalibrationTimerTimeout();
    void onNewData(EyeTrackerData etd);

private:
    static const qint32 CALIBRATION_WAIT = 1000; // Waits 1 second for transition
    static const qint32 CALIBRATION_SHOW = 2000; // Gathers data for 3 seconds.

    VIVEEyePoller eyetracker;

    // Calibration related variables.
    CalibrationTargets calibrationTargets;
    QTimer calibrationTimer;
    QList<QPoint> calibrationPoints;
    qint32 calibrationPointIndex;
    bool isWaiting;
    bool calibrationPassed;

};

#endif // HTCVIVEEYEPROEYETRACKINGINTERFACE_H
