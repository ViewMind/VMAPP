#ifndef HTCVIVEEYEPROEYETRACKINGINTERFACE_H
#define HTCVIVEEYEPROEYETRACKINGINTERFACE_H

#include "../eyetrackerinterface.h"
#include "../calibrationleastsquares.h"
#include "viveeyepoller.h"

class HTCViveEyeProEyeTrackingInterface: public EyeTrackerInterface
{
    Q_OBJECT
public:
    HTCViveEyeProEyeTrackingInterface(QObject *parent = nullptr, qreal width = 1, qreal height = 1);

    void connectToEyeTracker() override;

    void enableUpdating(bool enable) override;

    void disconnectFromEyeTracker() override;

    void calibrate(EyeTrackerCalibrationParameters params) override;

    QString getCalibrationValidationReport() const override;

public slots:
    void updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l) override;

private slots:
    void onNewCalibrationImageAvailable();

    void onCalibrationFinished();

    void newEyeData(QVariantMap eyedata);

private:

    VIVEEyePoller eyetracker;

    // Required to transform the raw data into proper data.
    CalibrationLeastSquares::EyeCorrectionCoeffs correctionCoefficients;

    // Calibration via least squares.
    CalibrationLeastSquares calibration;

    // Optionally, when calibration is done, coefficients can be stored in a file.
    QString coefficientsFile;

    // The transformation matrixes provided by the HMD.
    QMatrix4x4 rVRTransform;
    QMatrix4x4 lVRTransform;

};

#endif // HTCVIVEEYEPROEYETRACKINGINTERFACE_H
