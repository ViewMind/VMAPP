#include "eyecorrectioncoefficients.h"

EyeCorrectionCoefficients::EyeCorrectionCoefficients()
{
}

void EyeCorrectionCoefficients::configureForCoefficientComputation(const QList<QPointF> &target){

    xtarget.clear();
    ytarget.clear();
    calibrationData.clear();
    fittedEyeDataPoints.clear();

    for (qint32 i = 0; i < target.size(); i++){
        xtarget << target.at(i).x();
        ytarget << target.at(i).y();
        calibrationData << QList<EyeRealData>();
    }
}

void EyeCorrectionCoefficients::addPointForCoefficientComputation(const EyeRealData &etdata, qint32 calibrationTargetIndex){
    if ((calibrationTargetIndex >= 0) && (calibrationTargetIndex < calibrationData.size()) ){
        // qDebug() << "Calibration Point" << calibrationTargetIndex << ":" << etdata.toString();
        calibrationData[calibrationTargetIndex].append(etdata);
    }
}

QString EyeCorrectionCoefficients::calibrationPointsWithNoData() const {
    QStringList ans;
    for (qint32 i = 0; i < calibrationData.size(); i++){
        if (calibrationData.at(i).size() < 2) ans << QString::number(i);
    }
    if (ans.empty()) return "";
    return ans.join(",");
}

bool EyeCorrectionCoefficients::computeCoefficients(){

    LinearLeastSquaresFit fitterXL;
    LinearLeastSquaresFit fitterXR;
    LinearLeastSquaresFit fitterYL;
    LinearLeastSquaresFit fitterYR;

    for (qint32 i = 0; i < calibrationData.size(); i++){

        qreal xref = xtarget.at(i);
        qreal yref = ytarget.at(i);

        if (calibrationData.at(i).size() < 2) return false;

        for (qint32 j = 0; j < calibrationData.at(i).size(); j++){
            EyeRealData erd = calibrationData.at(i).at(j);
            fitterXL.addInputAndTarget(erd.xLeft,xref);
            fitterYL.addInputAndTarget(erd.yLeft,yref);

            fitterXR.addInputAndTarget(erd.xRight,xref);
            fitterYR.addInputAndTarget(erd.yRight,yref);
        }
    }

    //qDebug() << "LINEAR FITTING XL";
    xl = fitterXL.linearFit();
    if (!xl.isValid()) return false;

    //qDebug() << "LINEAR FITTING XR";
    xr = fitterXR.linearFit();
    if (!xr.isValid()) return false;

    //qDebug() << "LINEAR FITTING YL";
    yl = fitterYL.linearFit();
    if (!yl.isValid()) return false;

    //qDebug() << "LINEAR FITTING YR";
    yr = fitterYR.linearFit();
    if (!yr.isValid()) return false;

    // We now compute the the fitted data points, for each target.
    fittedEyeDataPoints.clear();

    for (qint32 i = 0; i < calibrationData.size(); i++){

        QList<EyeTrackerData> f;

        for (qint32 j = 0; j < calibrationData.at(i).size(); j++){
            EyeRealData erd = calibrationData.at(i).at(j);
            EyeTrackerData etd;
            etd.xLeft  = static_cast<qint32>(xl.predict(erd.xLeft));
            etd.xRight = static_cast<qint32>(xr.predict(erd.xRight));
            etd.yLeft  = static_cast<qint32>(yl.predict(erd.yLeft));
            etd.yRight = static_cast<qint32>(yr.predict(erd.yRight));
            f << etd;
        }

        fittedEyeDataPoints << f;
    }



    R2.xr = fitterXR.R2();
    R2.yr = fitterYR.R2();

    R2.xl = fitterXL.R2();
    R2.yl = fitterYL.R2();

    // qDebug() << "R: EyeR " << R2.xr << R2.yr << " EyeL " << R2.xl << R2.yl;


    return true;

}

QList< QList<EyeTrackerData> > EyeCorrectionCoefficients::getFittedData() const {
    return fittedEyeDataPoints;
}

QList<qreal> EyeCorrectionCoefficients::getCalibrationPointsXCordinates() const {
    return xtarget;
}

QList<qreal> EyeCorrectionCoefficients::getCalibrationPointsYCordinates() const {
    return ytarget;
}

EyeCorrectionCoefficients::RCoefficients EyeCorrectionCoefficients::getRCoefficients() const {
    return R2;
}

EyeTrackerData EyeCorrectionCoefficients::computeCorrections(const EyeRealData &input) const {

    qreal nxl = xl.predict(input.xLeft);
    qreal nxr = xr.predict(input.xRight);
    qreal nyl = yl.predict(input.yLeft);
    qreal nyr = yr.predict(input.yRight);

    // Making triple sure there are non NaN nubmers.
    if (qIsNaN(nxl)) nxl = 0;
    if (qIsNaN(nxr)) nxr = 0;
    if (qIsNaN(nyl)) nyl = 0;
    if (qIsNaN(nyr)) nyr = 0;

    EyeTrackerData etd;
    etd.xLeft  = static_cast<qint32>(nxl);
    etd.xRight = static_cast<qint32>(nxr);
    etd.yLeft  = static_cast<qint32>(nyl);
    etd.yRight = static_cast<qint32>(nyr);

    return etd;
}

QString EyeCorrectionCoefficients::toString() const{
    QString ans;
    ans = ans + "XR: " + xr.toString() + "\n";
    ans = ans + "YR: " + yr.toString() + "\n";
    ans = ans + "XL: " + xl.toString() + "\n";
    ans = ans + "YL: " + yl.toString() + "\n";
    return ans;
}

bool EyeCorrectionCoefficients::loadCalibrationCoefficients(const QString &file_name){

    QJsonParseError json_error;

    QFile file(file_name);
    if (!file.open(QFile::ReadOnly)) return false;
    QString val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8(),&json_error);
    if (doc.isNull()) return  false;

    QVariantMap data = doc.object().toVariantMap();
    xl.fromMap(data.value("xl").toMap());
    xr.fromMap(data.value("xr").toMap());
    yl.fromMap(data.value("yl").toMap());
    yr.fromMap(data.value("yr").toMap());
    return true;


}

void EyeCorrectionCoefficients::saveCalibrationCoefficients(const QString &file_name){
    QVariantMap map;
    map.insert("xl",xl.toMap());
    map.insert("xr",xr.toMap());
    map.insert("yl",yl.toMap());
    map.insert("yr",yr.toMap());
    QJsonDocument doc = QJsonDocument::fromVariant(map);

    QFile file(file_name);
    if (!file.open(QFile::WriteOnly)) return;

    QTextStream writer(&file);
    writer << doc.toJson(QJsonDocument::Indented);
    file.close();
}

bool EyeCorrectionCoefficients::isRightEyeCalibrated(){
    return xr.isValid() && yr.isValid();
}

bool EyeCorrectionCoefficients::isLeftEyeCalibrated(){
    return xl.isValid() && yl.isValid();
}

QList< qint32 > EyeCorrectionCoefficients::getHitsInTarget(qreal dimension, qreal tolerance, bool forLeftEye){
    QList<qint32> hits;

    for (qint32 i = 0; i < fittedEyeDataPoints.size(); i++){

        qreal xref = xtarget.at(i);
        qreal yref = ytarget.at(i);

        qint32 counter = 0;

        for (qint32 j = 0; j < fittedEyeDataPoints.at(i).size(); j++){
            EyeTrackerData etd = fittedEyeDataPoints.at(i).at(j);
            qreal x, y;
            if (forLeftEye){
                x = etd.xLeft;
                y = etd.yLeft;
            }
            else {
                x = etd.xRight;
                y = etd.yRight;
            }

            if (isPointInside(x,y,xref,yref,dimension,tolerance)){
                counter++;
            }

        }

        hits << counter;

    }

    return hits;

}

bool EyeCorrectionCoefficients::isPointInside(qreal x, qreal y, qreal x_target, qreal y_target, qreal dimension, qreal tolerance){

    // The target x and y are the center of the circle. So we compute the upper left corner.
    qreal tol = tolerance*dimension;
    qreal R = dimension/2;
    qreal tol_offset = tol/2;

    qreal xmin = x_target - R - tol_offset;
    qreal xmax = x_target + R + tol_offset;
    qreal ymin = y_target - R - tol_offset;
    qreal ymax = y_target + R + tol_offset;

    if ((x >= xmin) && (x <= xmax)){
        if ((y >= ymin) && (y <= ymax)){
            return true;
        }
    }
    return false;


}
