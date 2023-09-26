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
                             qreal calibrationDiameterFor2D, bool is3DMode);

    // Used the created data to generate the calibration report values. Returns whether the calibration was successfull or not.
    bool generateCalibrationReport(const EyeCorrectionCoefficients & coeffs);

    // When the data from the RRS comes too screwed up we still need to generate a validation report so that system can show the error message safely.
    // This function is used to generate exactly that.
    void generateAFailedCalibrationReport();

    // Prints some stats per target to see how well the calibration was. For the Log, mostly.
    QString getValidationReport() const;

    // Used for saving validation data in each specific study.
    QVariantMap getCalibrationAttemptData() const;

    // Used for getting the calibration configuration to be stored in the study output file.
    QVariantMap getCalibrationConfigurationData() const;

    // This will return the recommended eye to use for evaluation/processing of the study.
    QString getRecommendedEye() const;

    // FOR DEBUGGING.
    void setDataFromString(const QString &json);
    void saveToJSONFile(const QString &filename);

private:

    // The recommnded eye to use. reset in start calibration sequence.
    QString recommendedEye;

    // Depending on the parameters passed when configuration was called we can tell if ti's 3D validation or not.
    bool configuredFor3D;

    // Where the validation report is stored.
    QString validationReport;
    QVariantMap calibrationAttempt;
    QVariantMap calibrationConfiguration;

    qreal validationApproveThreshold;
    qreal validationPointHitTolerance;
    qint32 validationPointsToPassForAcceptedValidation;


    QVariantList fitted3DVectorsTo2DValidationGraphApproximation(const EyeCorrectionCoefficients &coeffs);


};

#endif // CALIBRATIONVALIDATION_H
