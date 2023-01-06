#include "eyecorrectioncoefficients.h"

EyeCorrectionCoefficients::EyeCorrectionCoefficients(){
    mode3D = false;
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

void EyeCorrectionCoefficients::set3DMode(bool is3D){
    mode3D = is3D;
}

bool EyeCorrectionCoefficients::isIn3DMode() const {
    return mode3D;
}

void EyeCorrectionCoefficients::configureForCoefficientComputationOf3DVectors(qint32 N_targets){

    xtarget.clear();
    ytarget.clear();
    ztarget.clear();
    calibrationData.clear();
    fittedEyeDataPoints.clear();

    for (qint32 i = 0; i < N_targets; i++){
        calibrationData << QList<EyeRealData>();
    }

}

void EyeCorrectionCoefficients::addPointForCoefficientComputation(const EyeRealData &etdata, qint32 calibrationTargetIndex){    
    if ((calibrationTargetIndex >= 0) && (calibrationTargetIndex < calibrationData.size()) ){
        // qDebug() << "Calibration Point" << calibrationTargetIndex << ":" << etdata.toString();
        calibrationData[calibrationTargetIndex].append(etdata);
    }
}

void EyeCorrectionCoefficients::set3DTargetVectors(const QList<QVector3D> &targetVectors, qreal validationRadiousValue){
    //qDebug() << "Setting target vectors with " << targetVectors.size() << "vectors";

    // The vectors need to be normalized. So the original values must be saved.
    nonNormalizedTargetVectors = targetVectors;
    validationRadious = validationRadiousValue;

    for (qint32 i = 0; i < targetVectors.size(); i++){
        QVector3D ntv = targetVectors.at(i).normalized();
        xtarget << static_cast<qreal>(ntv.x());
        ytarget << static_cast<qreal>(ntv.y());
        ztarget << static_cast<qreal>(ntv.z());
    }
}

QString EyeCorrectionCoefficients::getCalibrationPointsWithNoDataAsAString() const {
    QStringList ans;
    for (qint32 i = 0; i < calibrationData.size(); i++){
        if (calibrationData.at(i).size() < 2) ans << QString::number(i);
    }
    if (ans.empty()) return "";
    return ans.join(",");
}

QVariantList EyeCorrectionCoefficients::getCalibrationPointsWithNoData() const {
    QVariantList ans;
    for (qint32 i = 0; i < calibrationData.size(); i++){
        if (calibrationData.at(i).size() < 2) ans << i;
    }
    return ans;
}

bool EyeCorrectionCoefficients::computeCoefficients(){
    if (mode3D) return computeCoefficients3D();
    else return computeCoefficients2D();
}

bool EyeCorrectionCoefficients::computeCoefficients2D(){

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

            etd.setXR(xr.predict(erd.xRight));
            etd.setYR(yr.predict(erd.yRight));

            etd.setXL(xl.predict(erd.xLeft));
            etd.setYL(yl.predict(erd.yLeft));

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

bool EyeCorrectionCoefficients::computeCoefficients3D(){

    OrdinaryLeastSquares fitterXL;
    OrdinaryLeastSquares fitterXR;
    OrdinaryLeastSquares fitterYL;
    OrdinaryLeastSquares fitterYR;
    OrdinaryLeastSquares fitterZR;
    OrdinaryLeastSquares fitterZL;

    QStringList textdata;

    for (qint32 i = 0; i < calibrationData.size(); i++){

        //qDebug() << "Accessing target" << i << "when sizes are" << xtarget.size() << ytarget.size() << ztarget.size() << " Calibration Data Points" << calibrationData.at(i).size();

        qreal xref = xtarget.at(i);
        qreal yref = ytarget.at(i);
        qreal zref = ztarget.at(i);

        textdata << QString::number(xref) + ", " + QString::number(yref) + ", " + QString::number(zref);

        if (calibrationData.at(i).size() < 2) return false;

        for (qint32 j = 0; j < calibrationData.at(i).size(); j++){
            EyeRealData erd = calibrationData.at(i).at(j);

            textdata << erd.toString(true);

            QVector<qreal> lsample = erd.leftVector();
            QVector<qreal> rsample = erd.rightVector();
            lsample << 1;
            rsample << 1;

            fitterXL.addSample(lsample,xref);
            fitterYL.addSample(lsample,yref);
            fitterZL.addSample(lsample,zref);

            fitterXR.addSample(rsample,xref);
            fitterYR.addSample(rsample,yref);
            fitterZR.addSample(rsample,zref);

        }
    }


    //qDebug() << "LINEAR FITTING XL";
    if (!fitterXL.computeCoefficients()) return false;
    xl.fromSimpleMatrix(fitterXL.getCoefficients());
    if (!xl.isValid()) return false;

    //qDebug() << "LINEAR FITTING XR";
    if (!fitterXR.computeCoefficients()) return false;
    xr.fromSimpleMatrix(fitterXR.getCoefficients());
    if (!xr.isValid()) return false;

    //qDebug() << "LINEAR FITTING YL";
    if (!fitterYL.computeCoefficients()) return false;
    yl.fromSimpleMatrix(fitterYL.getCoefficients());
    if (!yl.isValid()) return false;

    //qDebug() << "LINEAR FITTING YR";
    if (!fitterYR.computeCoefficients()) return false;
    yr.fromSimpleMatrix(fitterYR.getCoefficients());
    if (!yr.isValid()) return false;

    if (!fitterZR.computeCoefficients()) return false;
    zr.fromSimpleMatrix(fitterZR.getCoefficients());
    if (!zr.isValid()) return false;

    if (!fitterZL.computeCoefficients()) return false;
    zl.fromSimpleMatrix(fitterZL.getCoefficients());
    if (!zl.isValid()) return false;

    // We now compute the the fitted data points, for each target.
    fittedEyeDataPoints.clear();

    for (qint32 i = 0; i < calibrationData.size(); i++){

        QList<EyeTrackerData> f;

        for (qint32 j = 0; j < calibrationData.at(i).size(); j++){
            EyeRealData erd = calibrationData.at(i).at(j);
            EyeTrackerData etd;

            etd.setXR(xr.predict(erd.xRight,erd.yRight,erd.zRight));
            etd.setYR(yr.predict(erd.xRight,erd.yRight,erd.zRight));
            etd.setZR(zr.predict(erd.xRight,erd.yRight,erd.zRight));

            etd.setXL(xl.predict(erd.xLeft,erd.yLeft,erd.zLeft));
            etd.setYL(yl.predict(erd.xLeft,erd.yLeft,erd.zLeft));
            etd.setZL(zl.predict(erd.xLeft,erd.yLeft,erd.zLeft));

            //qDebug() << "Raw Data Point" << erd.toString(true) << " Fitted to " << etd.toString(true);

            f << etd;
        }

        fittedEyeDataPoints << f;
    }



    R2.xr = fitterXR.getRSquared();
    R2.yr = fitterYR.getRSquared();

    R2.xl = fitterXL.getRSquared();
    R2.yl = fitterYL.getRSquared();

    R2.zr = fitterZR.getRSquared();
    R2.zl = fitterZL.getRSquared();

    // qDebug() << "R: EyeR " << R2.xr << R2.yr << " EyeL " << R2.xl << R2.yl;

    QFile file("calibration_data.txt");
    file.open(QFile::WriteOnly);
    QTextStream writer (&file);
    writer << textdata.join("\n");
    file.close();

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

    qreal nxl,nxr,nyl,nyr,nzl,nzr;

    if (mode3D){

        nxl = xl.predict(input.xLeft,input.yLeft,input.zLeft);
        nyl = yl.predict(input.xLeft,input.yLeft,input.zLeft);
        nzl = zl.predict(input.xLeft,input.yLeft,input.zLeft);

        nxr = xr.predict(input.xRight,input.yRight,input.zRight);
        nyr = yr.predict(input.yRight,input.yRight,input.zRight);
        nzr = zr.predict(input.yRight,input.yRight,input.zRight);

    }
    else {
        nxl = xl.predict(input.xLeft);
        nxr = xr.predict(input.xRight);
        nyl = yl.predict(input.yLeft);
        nyr = yr.predict(input.yRight);
        nzl = 0;
        nzr = 0;

    }

    // Making triple sure there are non NaN nubmers.
    if (qIsNaN(nxl)) nxl = 0;
    if (qIsNaN(nxr)) nxr = 0;
    if (qIsNaN(nyl)) nyl = 0;
    if (qIsNaN(nyr)) nyr = 0;
    if (qIsNaN(nzl)) nzl = 0;
    if (qIsNaN(nzr)) nzr = 0;

    EyeTrackerData etd;
    etd.setXL(nxl);
    etd.setXR(nxr);
    etd.setYR(nyr);
    etd.setYL(nyl);
    etd.setZL(nzl);
    etd.setZR(nzr);

    return etd;
}

QString EyeCorrectionCoefficients::toString() const{
    QString ans;
    ans = ans + "XR: " + xr.toString() + "\n";
    ans = ans + "YR: " + yr.toString() + "\n";
    ans = ans + "XL: " + xl.toString() + "\n";
    ans = ans + "YL: " + yl.toString() + "\n";
    if (mode3D){
        ans = ans + "ZL: " + zl.toString() + "\n";
        ans = ans + "ZR: " + zr.toString() + "\n";
    }
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
    yl.fromMap(data.value("zl").toMap());
    yr.fromMap(data.value("zr").toMap());
    return true;


}

void EyeCorrectionCoefficients::saveCalibrationCoefficients(const QString &file_name){
    QVariantMap map;
    map.insert("xl",xl.toMap());
    map.insert("xr",xr.toMap());
    map.insert("yl",yl.toMap());
    map.insert("yr",yr.toMap());
    map.insert("zl",zl.toMap());
    map.insert("zr",zr.toMap());
    QJsonDocument doc = QJsonDocument::fromVariant(map);

    QFile file(file_name);
    if (!file.open(QFile::WriteOnly)) return;

    QTextStream writer(&file);
    writer << doc.toJson(QJsonDocument::Indented);
    file.close();
}

bool EyeCorrectionCoefficients::isRightEyeCalibrated(){
    if (mode3D) return (xr.isValid() && yr.isValid() && zr.isValid());
    else return xr.isValid() && yr.isValid();
}

bool EyeCorrectionCoefficients::isLeftEyeCalibrated(){
    if (mode3D) return (xl.isValid() && yl.isValid() && zl.isValid());
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
            qreal x, y, z;
            if (forLeftEye){
                x = etd.xl();
                y = etd.yl();
                z = etd.zl();
            }
            else {
                x = etd.xr();
                y = etd.yr();
                z = etd.zr();
            }

            if (mode3D){
                //qDebug() << "Fitted raw data point of" << x << y << z;
                QVector3D nonNormalizedTargetVector = nonNormalizedTargetVectors.at(i);
                if (isVectorCloseEnough(nonNormalizedTargetVector,x,y,z)){
                    counter++;
                }
            }
            else {
                if (isPointInside(x,y,xref,yref,dimension,tolerance)){
                    counter++;
                }
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

bool EyeCorrectionCoefficients::isVectorCloseEnough(QVector3D tv, qreal x, qreal y, qreal z){

    qreal module = static_cast<qreal>(tv.length());
    x = x*module;
    y = y*module;
    z = z*module;

    qDebug() << "TARGET VECTOR" << tv << " Predicted Point" << x << y << z << "After using module" << module;

    qreal xt = static_cast<qreal>(tv.x());
    qreal yt = static_cast<qreal>(tv.y());
    qreal zt = static_cast<qreal>(tv.z());

    qreal sqrt_arg = (xt-x)*(xt-x) + (yt-y)*(yt-y) + (zt -z)*(zt -z);

    qreal dis = qSqrt(sqrt_arg);

    return (dis <= validationRadious);
}
