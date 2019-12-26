#ifndef CALIBRATIONLEASTSQUARES_H
#define CALIBRATIONLEASTSQUARES_H

#include <QList>
#include <QPoint>
#include <QtGlobal>
#include <QDebug>
#include <QDataStream>
#include <QFile>

class CalibrationLeastSquares
{
public:

    CalibrationLeastSquares();

    struct EyeInputData{
        qreal xr;
        qreal yr;
        qreal xl;
        qreal yl;
    };

    struct CalibrationData {
        QList<EyeInputData> eyeData;
        void clear();
        bool isValid();
        QString toString() const;
    };

    struct LinearCoeffs{
        qreal m;
        qreal b;
        bool valid;
        QVariant toVariant();
        void fromVariant(const QVariant &v);
    };

    struct EyeCorrectionCoeffs{
        LinearCoeffs xr;
        LinearCoeffs yr;
        LinearCoeffs xl;
        LinearCoeffs yl;
        EyeInputData computeCorrections(EyeInputData input);
        void saveCalibrationCoefficients(const QString &file);
        void loadCalibrationCoefficients(const QString &file);
    };

    bool computeCalibrationCoeffs(QList<CalibrationData> calibrationData);
    bool isRightEyeCalibrated();
    bool isLeftEyeCalibrated();
    EyeCorrectionCoeffs getCalculatedCoeficients() const;

private:

    struct LeastSquaresData{
        QList<qreal> input;
        QList<qreal> target;
        LinearCoeffs computeLinearCoeffs();
    };

    EyeCorrectionCoeffs coeffs;


};

#endif // CALIBRATIONLEASTSQUARES_H
