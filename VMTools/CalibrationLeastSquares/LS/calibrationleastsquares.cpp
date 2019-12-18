#include "calibrationleastsquares.h"

CalibrationLeastSquares::CalibrationLeastSquares()
{

}

void CalibrationLeastSquares::CalibrationData::clear(){
    xr.clear(); yr.clear(); xl.clear(); yl.clear();
}
bool CalibrationLeastSquares::CalibrationData::isValid(){
    bool ans = (xr.size() >= 2);
    ans = ans && (yr.size() >= 2);
    ans = ans && (xl.size() >= 2);
    ans = ans && (yl.size() >= 2);
    ans = ans && (xr.size() == yr.size()) && (yr.size() == xl.size()) && (xl.size() == yl.size());
    return ans;
}
QString CalibrationLeastSquares::CalibrationData::toString() const{
    QString ans = "[";
    for (qint32 i = 0; i < xr.size(); i++){
        ans = ans + QString::number(xr.at(i)) + " " + QString::number(yr.at(i)) + " " + QString::number(xl.at(i)) + " " + QString::number(yl.at(i)) + ";\n";
    }
    ans.remove(ans.length()-1,1);
    ans= ans + "];";
    return ans;
}

bool CalibrationLeastSquares::computeCalibrationCoeffs(QList<CalibrationData> calibrationData){

    QList<QPoint> calibrationPoints;

    // Getting the calibration points
    LeastSquaresData xr;
    LeastSquaresData yr;
    LeastSquaresData xl;
    LeastSquaresData yl;

    for (int i = 0; i < calibrationData.size(); i++){
        CalibrationData d = calibrationData.at(i);
        if (!d.isValid()) return false;

        // Getting the calibraton point.
        //QPoint point(static_cast<qint32>(d.xl.first()),static_cast<qint32>(d.yl.first()));
        qreal targetX = d.xl.first();
        qreal targetY = d.yl.first();

        // Removing the point from list
        d.xl.removeAt(0);
        d.yl.removeAt(0);
        d.xr.removeAt(0);
        d.yr.removeAt(0);

        // Appending the list to the correspodning Least Squeres Struc
        xr.input << d.xr;
        xl.input << d.xl;
        yr.input << d.yr;
        yl.input << d.yl;

        // Adding the corresponding value to the target vectors
        for (int j = 0; j < d.xr.size(); j++){
            xr.target << targetX;
            xl.target << targetX;
        }

        for (int j = 0; j < d.yr.size(); j++){
            yr.target << targetY;
            yl.target << targetY;
        }
    }

    // Doing the least squres computation. Starting by computing the matrix oefficients.
    coeffs.xl = xl.computeLinearCoeffs();
    coeffs.xr = xr.computeLinearCoeffs();
    coeffs.yl = yl.computeLinearCoeffs();
    coeffs.yr = yr.computeLinearCoeffs();

    return true;
}

CalibrationLeastSquares::EyeCorrectionCoeffs CalibrationLeastSquares::getCalculatedCoeficients() const{
    return coeffs;
}

CalibrationLeastSquares::LinearCoeffs CalibrationLeastSquares::LeastSquaresData::computeLinearCoeffs(){
    LinearCoeffs lc;

    // Computing the matrix coefficients
    qreal m11 = 0;
    qreal m12 = 0;
    qreal m21 = 0;
    qreal m22 = 0;

    // Target value vector
    qreal t1 =0;
    qreal t2 = 0;
    for (int i = 0; i < input.size(); i++){
        if (qIsNaN(input.at(i))){ // in case I get NaN values.
            continue;
        }
        m12 = m12 + input.at(i);
        m22 = m22 + input.at(i)*input.at(i);
        t1 = t1 + target.at(i);
        t2 = t2 + target.at(i)*input.at(i);
        m11++;
    }
    m21 = m12;

    // Calculating the inverse
    qreal det = m22*m11 - m12*m21;
    if (det == 0.0) {
        lc.valid = false;
        return  lc;
    }
    else lc.valid = true;
    qreal im11 = m22/det;
    qreal im12 = -m12/det;
    qreal im21 = -m21/det;
    qreal im22 = m11/det;

    // Target value vector
    lc.b = im11*t1 + im12*t2;
    lc.m = im21*t1 + im22*t2;

    return lc;
}

CalibrationLeastSquares::EyeInputData CalibrationLeastSquares::EyeCorrectionCoeffs::computeCorrections(EyeInputData input){
    input.xl = xl.m*input.xl + xl.b;
    input.xr = xr.m*input.xr + xr.b;
    input.yl = yl.m*input.yl + yl.b;
    input.yr = yr.m*input.yr + yr.b;
    return input;
}
