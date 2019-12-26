#include "calibrationleastsquares.h"

CalibrationLeastSquares::CalibrationLeastSquares()
{

}

void CalibrationLeastSquares::CalibrationData::clear(){
    eyeData.clear();
}
bool CalibrationLeastSquares::CalibrationData::isValid(){
    return (eyeData.size() >= 2 );
}
QString CalibrationLeastSquares::CalibrationData::toString() const{
    QString ans = "[";
    for (qint32 i = 0; i < eyeData.size(); i++){
        ans = ans + QString::number(eyeData.at(i).xr) + " "
                + QString::number(eyeData.at(i).yr) + " "
                + QString::number(eyeData.at(i).xl) + " "
                + QString::number(eyeData.at(i).yl) + ";\n";
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
        if (!d.isValid()) {
            ///qDebug() << "Calibration failed because a calibration data contains less that two values. Which means that NO DATA was gathered for a given calibration point";
            return false;
        }

        // Getting the calibraton point.
        //QPoint point(static_cast<qint32>(d.xl.first()),static_cast<qint32>(d.yl.first()));
        qreal targetX = d.eyeData.first().xl;
        qreal targetY = d.eyeData.first().yl;

        // Removing the point from list
        d.eyeData.removeAt(0);

        // Appending the list to the correspodning Least Squeres Struc
        for (qint32 i = 0; i < d.eyeData.size(); i++){
            xr.input << d.eyeData.at(i).xr;
            xl.input << d.eyeData.at(i).xl;
            yr.input << d.eyeData.at(i).yr;
            yl.input << d.eyeData.at(i).yl;
        }

        // Adding the corresponding value to the target vectors
        for (int j = 0; j < d.eyeData.size(); j++){
            xr.target << targetX;
            xl.target << targetX;
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


bool CalibrationLeastSquares::EyeCorrectionCoeffs::isRightEyeCalibrated(){
   return xr.valid && yr.valid;
}

bool CalibrationLeastSquares::EyeCorrectionCoeffs::isLeftEyeCalibrated(){
   return xl.valid && yl.valid;
}

CalibrationLeastSquares::EyeCorrectionCoeffs CalibrationLeastSquares::getCalculatedCoeficients() const{
    return coeffs;
}

CalibrationLeastSquares::LinearCoeffs CalibrationLeastSquares::LeastSquaresData::computeLinearCoeffs(){
    LinearCoeffs lc;
    lc.m = 0;
    lc.b = 0;

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

    // Making doubly sure No NaN appears in the results.
    if (qIsNaN(lc.m) || qIsNaN(lc.b)){
        lc.valid = false;
        lc.m = 0;
        lc.b = 0;
    }

    return lc;
}

CalibrationLeastSquares::EyeInputData CalibrationLeastSquares::EyeCorrectionCoeffs::computeCorrections(EyeInputData input){
    input.xl = xl.m*input.xl + xl.b;
    input.xr = xr.m*input.xr + xr.b;
    input.yl = yl.m*input.yl + yl.b;
    input.yr = yr.m*input.yr + yr.b;

    // Making triple sure there are non NaN nubmers.
    if (qIsNaN(input.xl)) input.xl = 0;
    if (qIsNaN(input.xr)) input.xr = 0;
    if (qIsNaN(input.yl)) input.yl = 0;
    if (qIsNaN(input.yr)) input.yr = 0;


    return input;
}

QVariant CalibrationLeastSquares::LinearCoeffs::toVariant(){
    QVariantList list;
    list << m << b << valid;
    return list;
}

void CalibrationLeastSquares::LinearCoeffs::fromVariant(const QVariant &v){
    QVariantList list = v.toList();
    if (list.size() != 3) return;
    m = list.at(0).toDouble();
    b = list.at(1).toDouble();
    valid = list.at(2).toBool();
}

void CalibrationLeastSquares::EyeCorrectionCoeffs::loadCalibrationCoefficients(const QString &file){
    QFile f(file);
    if (!f.open(QFile::ReadOnly)) return;
    QDataStream reader(&f);
    QVariantList coeffs;
    reader >> coeffs;
    f.close();
    //qDebug() << "Loaded" << coeffs;
    //QVariantList coeffs = cdata.toList();
    if (coeffs.size() != 4) return;
    xl.fromVariant(coeffs.at(0));
    xr.fromVariant(coeffs.at(1));
    yl.fromVariant(coeffs.at(2));
    yr.fromVariant(coeffs.at(3));
}

void CalibrationLeastSquares::EyeCorrectionCoeffs::saveCalibrationCoefficients(const QString &file){
    QFile f(file);
    if (!f.open(QFile::WriteOnly)) return;
    QDataStream writer(&f);
    QVariantList coeffs;
    coeffs << xl.toVariant() << xr.toVariant() << yl.toVariant() << yr.toVariant();
    //qDebug() << "SAVING "  << coeffs ;
    writer << coeffs;
    f.close();
}
