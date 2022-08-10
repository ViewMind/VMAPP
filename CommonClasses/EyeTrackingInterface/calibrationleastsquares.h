#ifndef CALIBRATIONLEASTSQUARES_H
#define CALIBRATIONLEASTSQUARES_H

#include <QList>
#include <QPoint>
#include <QtGlobal>
#include <QDebug>
#include <QDataStream>
#include <QFile>
#include <QObject>
#include <QTimer>
#include "calibrationtargets.h"
#include "eyecorrectioncoefficients.h"
#include "../RawDataContainer/VMDC.h"
#include "../LinearLeastSquares/linearleastsquaresfit.h"


class CalibrationLeastSquares: public QObject
{
    Q_OBJECT
public:

    CalibrationLeastSquares();

    // This sets the calibration target class, initializes all variables and starts the calibration timer.
    // npoints can be 5 or 9 if anything other than 9 is used, 5 is assumed.
    void startCalibrationSequence(qint32 width, qint32 height, qint32 npoints, qint32 ms_gather_time_for_calib_pt, qint32 ms_wait_time_calib_pt);

    // Adding the data point for calibration.
    void addDataPointForCalibration(float xl, float yl, float xr, float yr);

    // Getting the coefficients once computation is done
    EyeCorrectionCoefficients getCalculatedCoeficients() const;

    // Gets the current Calibration image as generated by the CalibrationTargets class.
    QImage getCurrentCalibrationImage() const;

    // Returns true if the calibration process was started but has not ended.
    bool isCalibrating() const;

    // Returns true if the calibation process is in it's validation phase.
    bool isValidating() const;

    // Prints some stats per target to see how well the calibration was. For the Log, mostly.
    QString getValidationReport() const;

    // Configure everything related to calibration validation.
    void configureValidation(const QVariantMap &calibrationValidationParameters);

    // Used for saving validation data in each specific study.
    QVariantMap getCalibrationValidationData() const;

    // This will return the recommended eye to use for evaluation/processing of the study.
    QString getRecommendedEye() const;

signals:
    void newCalibrationImageAvailable();
    void calibrationDone();

private slots:
    void calibrationTimeInTargetUp();

private:

    // This will take care of gathering calibration and actually performing the calibration itself.
    EyeCorrectionCoefficients coeffs;

    // The recommnded eye to use. reset in start calibration sequence.
    QString recommendedEye;

    // This is the image that will be shown.
    CalibrationTargets calibrationTargets;

    // Holds the currently generated calibration image.
    QImage currentCalibrationImage;

    // Index of the list of collectedCalibrationDataPoints.
    qint32 currentCalibrationPointIndex;

    // The timer that indicates when to move the next target.
    QTimer calibrationTimer;

    // Flag that indicates if we are in the wait time of calibration or the gather time.
    bool isDataGatheringEnabled;

    // Flag to indicate the duration of the calibration process.
    bool isCalibratingFlag;

    // Number of calibration points
    qint32 numberOfCalibrationPoints;

    // Where the validation report is stored.
    QString validationReport;
    QVariantMap calibrationValidationData;
    qint32 calibrationPointsUsed;

    bool successfullyComputedCoefficients;
    qreal validationApproveThreshold;
    qreal validationPointHitTolerance;
    qint32 validationPointsToPassForAcceptedValidation;

    // How long to wait to gather data for each target.
    //static const qint32 CALIBRATION_WAIT_TIME = 1000;

    // How data is gathered for each target.
    // static const qint32 CALIBRATION_GATHER_TIME = 2000;

    qint32 calibration_wait_time;
    qint32 calibration_gather_time;

    // Number of validations points is always nine.
    static const qint32 VALIDATION_NPOINTS = 9;

    // This will compute the calibration coefficients once data gathering is done.
    void generateCalibrationReport();

};

#endif // CALIBRATIONLEASTSQUARES_H
