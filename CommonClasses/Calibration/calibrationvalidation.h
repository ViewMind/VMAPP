#ifndef CALIBRATIONVALIDATION_H
#define CALIBRATIONVALIDATION_H

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
#include "../LogInterface/staticthreadlogger.h"

/*
 * When used in 2D Mode it will generate the render packets to show the 2D Calibration on the screen,
 * gather the data and compute the calibration report and the calibration structure that needs to be saved in an study.
 * When used in 3D Mode. It will merely do the computations once in receives the data from the RemoteRenderServer.
 * It will also prepare the coefficients and send them back.
 **/

class CalibrationValidation
{

public:

    CalibrationValidation();

    // Configure all necessary paramters to generate the calibration report.
    void configureValidation(const QVariantMap &calibrationValidationParameters,
                             const QList<QVector3D> &nonNormalizedTargetVecs,
                             qreal validationRadious,
                             const QVariantList &calibrationTargetCornersFor2D,
                             qreal calibrationDiameterFor2D);

    // Used the created data to generate the calibration report values. Returns whether the calibration was successfull or not.
    bool generateCalibrationReport(const EyeCorrectionCoefficients & coeffs);

    // Prints some stats per target to see how well the calibration was. For the Log, mostly.
    QString getValidationReport() const;

    // Used for saving validation data in each specific study.
    QVariantMap getCalibrationValidationData() const;

    // This will return the recommended eye to use for evaluation/processing of the study.
    QString getRecommendedEye() const;

    // FOR DEBUGGING.
    void setDataFromString(const QString &json);

private:

    // The recommnded eye to use. reset in start calibration sequence.
    QString recommendedEye;

    // Depending on the parameters passed when configuration was called we can tell if ti's 3D validation or not.
    bool configuredFor3D;

    // Where the validation report is stored.
    QString validationReport;
    QVariantMap calibrationValidationData;

    qreal validationApproveThreshold;
    qreal validationPointHitTolerance;
    qint32 validationPointsToPassForAcceptedValidation;

    qint32 calibration_wait_time;
    qint32 calibration_gather_time;

    QVariantList fitted3DVectorsTo2DValidationGraphApproximation(const EyeCorrectionCoefficients &coeffs);


};

#endif // CALIBRATIONVALIDATION_H
