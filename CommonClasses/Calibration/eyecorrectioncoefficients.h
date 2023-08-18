#ifndef EYECORRECTIONCOEFFICIENTS_H
#define EYECORRECTIONCOEFFICIENTS_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>
#include <QPointF>
#include <QVector3D>
#include "../EyeTrackingInterface/eyetrackerdata.h"
#include "../LinearLeastSquares/linearleastsquaresfit.h"
#include "../LinearLeastSquares/ordinaryleastsquares.h"
#include "../eyetracker_defines.h"

class EyeCorrectionCoefficients
{
public:

    struct RCoefficients {
        qreal xr;
        qreal yr;
        qreal xl;
        qreal yl;
        qreal zl;
        qreal zr;
        void clear(){
            xr = 0; yr = 0; xl = 0; yl = 0; zl = 0; zr = 0;
        }
    };

    EyeCorrectionCoefficients();

    // Reset all internal variables to their status as they were just created.
    void clear();

    // 2D Calibration Setup Configures For 2D Calibration Process.
    void configureFor2DCoefficientComputation(const QList<QPointF> &target);

    // 3D Calibration Setup. Configures For 3D Calibration Process
    //void configureForCoefficientComputationOf3DVectors(qint32 N_targets);
    void configureFor3DCoefficientComputation(const QList<QVector3D> &targetVectors, qreal validationRadiousValue);

    // Common functions.
    void addPointForCoefficientComputation(const EyeTrackerData &etdata, qint32 calibrationTargetIndex);
    void setStartPointForValidCalibrationRawData(qint32 calibrationTargetIndex);
    bool computeCoefficients();

    EyeTrackerData computeCorrections(const EyeTrackerData &input) const;

    QList< QList<EyeTrackerData> > getFittedData() const;
    QList<QList<qreal> > getHitPercentInTarget(qreal dimension, qreal tolerance, bool forLeftEye) const;

    QList<qreal> getCalibrationPointsXCordinates() const;
    QList<qreal> getCalibrationPointsYCordinates() const;

    QList<QVector3D> getNonNormalizedTargetVector() const;

    bool isRightEyeCalibrated();
    bool isLeftEyeCalibrated();
    void saveCalibrationCoefficients(const QString &file_name) const;
    bool loadCalibrationCoefficients(const QString &file_name);
    QString toString() const;

    QString getLastError() const;

    RCoefficients getRCoefficients() const;

    bool getResultOfLastComputation() const;

    QString getCalibrationPointsWithNoDataAsAString() const;
    QVariantList getCalibrationPointsWithNoData() const;
    QVariantList getCutoffIndexesListAsVariantList() const;

    QVariantMap getCalibrationControlPacketCompatibleMap(bool left_eye) const;

    // For debugging
    bool loadFromTextMatrix (const QString &text);

private:

    LinearCoefficients xr;
    LinearCoefficients yr;
    LinearCoefficients xl;
    LinearCoefficients yl;
    LinearCoefficients zr;
    LinearCoefficients zl;

    RCoefficients R2;

    QList<qreal> xtarget;
    QList<qreal> ytarget;
    QList<qreal> ztarget;

    QString error_msg;

    QList<QVector3D> nonNormalizedTargetVectors;
    qreal validationRadious;

    bool resultOfLastComputation;

    QList< QList<EyeTrackerData> > calibrationData;

    QList< QList<EyeTrackerData> > fittedEyeDataPoints;

    QList<qint32> cuttoffForCalibrationDataForCumputation;

    QList<qint32> effectiveNumberOfDataPointsUsedPerCalibrationPoint;

    bool mode3D; // Flag to indicate whether we are doing a 3D calibration or 2D calibration.

    bool isPointInside(qreal x, qreal y, qreal upperLeftX, qreal upperLeftY, qreal side, qreal tolerance) const;
    bool isVectorCloseEnough(QVector3D tv, qreal x, qreal y, qreal z) const;

    bool computeCoefficients2D();
    bool computeCoefficients3D();

    void configureForCoefficientComputation(const QList<QVector3D> &targetVectors);

};

#endif // EYECORRECTIONCOEFFICIENTS_H
