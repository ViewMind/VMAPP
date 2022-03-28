#ifndef MOUSEINTERFACE_H
#define MOUSEINTERFACE_H

#include <QTimer>
#include <QDebug>
#include <QCursor>
#include "../eyetrackerinterface.h"
#include "calibrationarea.h"

// This class is prepared to simulate the behaviour of the eyetracker without the actual eyetracker.
// Enabling the development of the application without the device

class MouseInterface: public EyeTrackerInterface
{
public:
    MouseInterface();

    void connectToEyeTracker() override;

    void enableUpdating(bool enable) override;

    void disconnectFromEyeTracker() override;

    void calibrate(EyeTrackerCalibrationParameters params) override;

    QString getCalibrationValidationReport() const override;

    void mouseSetCalibrationToTrue();

    void overrideCalibration();

    ~MouseInterface() override;

private slots:
    void on_pollTimerUp();
    void on_calibrationCancelled();

private:
    QTimer pollTimer;

    // 8.3 Milliseconds is approximately 1/120, for 120 Hz sampling rate.
    const qint32 TIMEOUT = 8;

    EyeTrackerData dataToSend;

    bool isCalibrated;
    bool isBeingCalibrated;
    bool sendData;
    bool overrideCalibrationFlag;

    CalibrationArea *calibrationScreen;

};

#endif // MOUSEINTERFACE_H
