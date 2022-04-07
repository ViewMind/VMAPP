#ifndef HPOMNICEPTINTERFACE_H
#define HPOMNICEPTINTERFACE_H

#include "hpomniceptprovider.h"
#include "../eyetrackerinterface.h"
#include "../calibrationleastsquares.h"

#include <QTimer>
#include <QElapsedTimer>

class HPOmniceptInterface: public EyeTrackerInterface
{
public:

    explicit HPOmniceptInterface(QObject *parent = nullptr, qreal width = 1, qreal height = 1);

    void connectToEyeTracker() override;

    void enableUpdating(bool enable) override;

    void disconnectFromEyeTracker() override;

    void calibrate(EyeTrackerCalibrationParameters params) override;

    QString getCalibrationValidationReport() const override;

    void configureCalibrationValidation(QVariantMap calibrationValidationParameters) override;

    QVariantMap getCalibrationValidationData() const override;

public slots:
    void updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l) override;

private slots:

    void onNewCalibrationImageAvailable();

    void onCalibrationFinished();

    void newEyeData(QVariantMap eyedata);

    void providedStarted();

private:

    // Object that will manage all headset data gathering.
    HPOmniceptProvider hpprovider;

    // Required to transform the raw data into proper data.
    CalibrationLeastSquares::EyeCorrectionCoeffs correctionCoefficients;

    // Calibration via least squares.
    CalibrationLeastSquares calibration;

    // Optionally, when calibration is done, coefficients can be stored in a file.
    QString coefficientsFile;

    // The transformation matrixes provided by the HMD.
    QMatrix4x4 rVRTransform;
    QMatrix4x4 lVRTransform;

    static const float SAMPLING_FREQ;

};

#endif // HPOMNICEPTINTERFACE_H
