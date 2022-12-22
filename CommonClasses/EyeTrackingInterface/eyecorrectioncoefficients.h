#ifndef EYECORRECTIONCOEFFICIENTS_H
#define EYECORRECTIONCOEFFICIENTS_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>
#include <QPointF>
#include <QVector3D>
#include "eyerealdata.h"
#include "eyetrackerdata.h"
#include "../LinearLeastSquares/linearleastsquaresfit.h"
#include "../LinearLeastSquares/ordinaryleastsquares.h"

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
    };

    EyeCorrectionCoefficients();

    void setMode(bool is3D);
    bool isIn3DMode() const;

    // 2D Calibration Functions.
    void configureForCoefficientComputation(const QList<QPointF> &target);    

    // 3D Calibration functions.
    void configureForCoefficientComputationOf3DVectors(qint32 N_targets);
    void set3DTargetVectors(const QList<QVector3D> &targetVectors);


    // Common functions.
    void addPointForCoefficientComputation(const EyeRealData &etdata, qint32 calibrationTargetIndex);
    bool computeCoefficients();

    EyeTrackerData computeCorrections(const EyeRealData &input) const;

    QList< QList<EyeTrackerData> > getFittedData() const;
    QList< qint32 > getHitsInTarget(qreal dimension, qreal tolerance, bool forLeftEye);

    QList<qreal> getCalibrationPointsXCordinates() const;
    QList<qreal> getCalibrationPointsYCordinates() const;

    bool isRightEyeCalibrated();
    bool isLeftEyeCalibrated();
    void saveCalibrationCoefficients(const QString &file_name);
    bool loadCalibrationCoefficients(const QString &file_name);
    QString toString() const;

    RCoefficients getRCoefficients() const;

    QString getCalibrationPointsWithNoDataAsAString() const;
    QVariantList getCalibrationPointsWithNoData() const;


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

    QList< QList<EyeRealData> > calibrationData;

    QList< QList<EyeTrackerData> > fittedEyeDataPoints;

    bool mode3D; // Flag to indicate whether we are doing a 3D calibration or 2D calibration.

    bool isPointInside(qreal x, qreal y, qreal upperLeftX, qreal upperLeftY, qreal side, qreal tolerance);
    bool isVectorCloseEnough(qreal xt, qreal yt, qreal zt, qreal x, qreal y, qreal z, qreal tolerance);

    bool computeCoefficients2D();
    bool computeCoefficients3D();

};

#endif // EYECORRECTIONCOEFFICIENTS_H
