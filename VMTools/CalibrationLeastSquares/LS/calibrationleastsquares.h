#ifndef CALIBRATIONLEASTSQUARES_H
#define CALIBRATIONLEASTSQUARES_H

#include <QList>
#include <QPoint>

class CalibrationLeastSquares
{
public:

    CalibrationLeastSquares();

    struct CalibrationData {
        QList<qreal> xr;
        QList<qreal> yr;
        QList<qreal> xl;
        QList<qreal> yl;
        void clear(){
            xr.clear(); yr.clear(); xl.clear(); yl.clear();
        }
        bool isValid(){
            bool ans = (xr.size() >= 2);
            ans = ans && (yr.size() >= 2);
            ans = ans && (xl.size() >= 2);
            ans = ans && (yl.size() >= 2);
            ans = ans && (xr.size() == yr.size()) && (yr.size() == xl.size()) && (xl.size() == yl.size());
            return ans;
        }
        QString toString() const{
            QString ans = "[";
            for (qint32 i = 0; i < xr.size(); i++){
                ans = ans + QString::number(xr.at(i)) + " " + QString::number(yr.at(i)) + " " + QString::number(xl.at(i)) + " " + QString::number(yl.at(i)) + ";\n";
            }
            ans.remove(ans.length()-1,1);
            ans= ans + "];";
            return ans;
        }
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
