#ifndef EYECORRECTIONCOEFFICIENTS_H
#define EYECORRECTIONCOEFFICIENTS_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QHash>
#include <QPointF>
#include "eyerealdata.h"
#include "eyetrackerdata.h"
#include "../LinearLeastSquares/linearleastsquaresfit.h"

class EyeCorrectionCoefficients
{
public:

    struct RCoefficients {
        qreal xr;
        qreal yr;
        qreal xl;
        qreal yl;
    };

    EyeCorrectionCoefficients();

    void configureForCoefficientComputation(const QList<QPointF> &target);
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

    QString calibrationPointsWithNoData() const;


private:
    LinearCoefficients xr;
    LinearCoefficients yr;
    LinearCoefficients xl;
    LinearCoefficients yl;

    RCoefficients R2;

    QList<qreal> xtarget;
    QList<qreal> ytarget;
    QList< QList<EyeRealData> > calibrationData;

    QList< QList<EyeTrackerData> > fittedEyeDataPoints;

    bool isPointInside(qreal x, qreal y, qreal upperLeftX, qreal upperLeftY, qreal side, qreal tolerance);

};

#endif // EYECORRECTIONCOEFFICIENTS_H
