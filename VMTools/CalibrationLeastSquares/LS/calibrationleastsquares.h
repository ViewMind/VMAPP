#ifndef CALIBRATIONLEASTSQUARES_H
#define CALIBRATIONLEASTSQUARES_H

#include <QList>
#include <QPoint>
#include <QtGlobal>
#include <QDebug>

class CalibrationLeastSquares
{
public:

    CalibrationLeastSquares();

    struct CalibrationData {
        QList<qreal> xr;
        QList<qreal> yr;
        QList<qreal> xl;
        QList<qreal> yl;
        void clear();
        bool isValid();
        QString toString() const;
    };

    struct LinearCoeffs{
        qreal m;
        qreal b;
        bool valid;        
    };

    struct EyeInputData{
        qreal xr;
        qreal yr;
        qreal xl;
        qreal yl;
    };

    struct EyeCorrectionCoeffs{
        LinearCoeffs xr;
        LinearCoeffs yr;
        LinearCoeffs xl;
        LinearCoeffs yl;
        EyeInputData computeCorrections(EyeInputData input);
    };

    bool computeCalibrationCoeffs(QList<CalibrationData> calibrationData);
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
