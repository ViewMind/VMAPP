#ifndef MOUSEINTERFACE_H
#define MOUSEINTERFACE_H

#include <QTimer>
#include <QDebug>
#include <QCursor>
#include "../eyetrackerinterface.h"
#include "../calibrationleastsquares.h"
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

    void overrideCalibration();

    void setCalibrationAreaDimensions(qreal w, qreal h);

    ~MouseInterface() override;

private slots:
    void on_pollTimerUp();
    void on_calibrationCancelled();
    void onNewCalibrationImageAvailable();
    void onCalibrationFinished();

private:
    QTimer pollTimer;

    // 8.3 Milliseconds is approximately 1/120, for 120 Hz sampling rate.
    const qint32 TIMEOUT = 8;

    EyeTrackerData dataToSend;

    // Required to transform the raw data into proper data.
    CalibrationLeastSquares::EyeCorrectionCoeffs correctionCoefficients;

    // Calibration via least squares.
    CalibrationLeastSquares calibration;

    bool isCalibrated;
    bool isBeingCalibrated;
    bool sendData;
    bool overrideCalibrationFlag;

    CalibrationArea calibrationArea;

};

#endif // MOUSEINTERFACE_H
