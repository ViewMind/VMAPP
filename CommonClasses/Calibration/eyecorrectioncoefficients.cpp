#include "eyecorrectioncoefficients.h"

EyeCorrectionCoefficients::EyeCorrectionCoefficients(){
    mode3D = false;
}


void EyeCorrectionCoefficients::addPointForCoefficientComputation(const EyeTrackerData &etdata, qint32 calibrationTargetIndex){
    if ((calibrationTargetIndex >= 0) && (calibrationTargetIndex < calibrationData.size()) ){
        // qDebug() << "Calibration Point" << calibrationTargetIndex << ":" << etdata.toString();
        calibrationData[calibrationTargetIndex].append(etdata);
    }
}

void EyeCorrectionCoefficients::setStartPointForValidCalibrationRawData(qint32 calibrationTargetIndex){
    if ((calibrationTargetIndex >= 0) && (calibrationTargetIndex < calibrationData.size()) ){
        qint32 index = calibrationData.at(calibrationTargetIndex).size();
        cuttoffForCalibrationDataForCumputation[calibrationTargetIndex] = index;
    }
}

void EyeCorrectionCoefficients::configureFor2DCoefficientComputation(const QList<QPointF> &target){
    QList<QVector3D> vectors;
    for (qint32 i = 0; i < target.size(); i++){
        QVector3D v(static_cast<float>(target.at(i).x()),static_cast<float>(target.at(i).y()),0);
        vectors << v;
    }
    mode3D = false;
    configureForCoefficientComputation(vectors);
}


void EyeCorrectionCoefficients::configureFor3DCoefficientComputation(const QList<QVector3D> &targetVectors, qreal validationRadiousValue){
    //qDebug() << "Setting target vectors with " << targetVectors.size() << "vectors";

    // The vectors need to be normalized. So the original values must be saved.
    nonNormalizedTargetVectors = targetVectors;
    validationRadious = validationRadiousValue;
    QList<QVector3D> normalizedVectors;

    for (qint32 i = 0; i < targetVectors.size(); i++){
        QVector3D ntv = targetVectors.at(i).normalized();
        normalizedVectors << ntv;
    }

    mode3D = true;

    configureForCoefficientComputation(normalizedVectors);
}

void EyeCorrectionCoefficients::configureForCoefficientComputation(const QList<QVector3D> &targetVectors){
    //qDebug() << "Setting target vectors with " << targetVectors.size() << "vectors";

    xtarget.clear();
    ytarget.clear();
    ztarget.clear();
    calibrationData.clear();
    fittedEyeDataPoints.clear();
    cuttoffForCalibrationDataForCumputation.clear();

    for (qint32 i = 0; i < targetVectors.size(); i++){
        calibrationData << QList<EyeTrackerData>();
        cuttoffForCalibrationDataForCumputation << -1;
        QVector3D ntv = targetVectors.at(i);
        xtarget << static_cast<qreal>(ntv.x());
        ytarget << static_cast<qreal>(ntv.y());
        ztarget << static_cast<qreal>(ntv.z());
    }

}

QString EyeCorrectionCoefficients::getCalibrationPointsWithNoDataAsAString() const {
    QVariantList pts = getCalibrationPointsWithNoData();
    if (pts.isEmpty()) return "";
    QStringList ans;
    for (qint32 i = 0; i < pts.size(); i++){
        ans << pts.at(i).toString();
    }
    return ans.join(",");
}

QVariantList EyeCorrectionCoefficients::getCalibrationPointsWithNoData() const {
    QVariantList ans;
    for (qint32 i = 0; i < fittedEyeDataPoints.size(); i++){
        qint32 start_index = cuttoffForCalibrationDataForCumputation.at(i);
        qreal total = fittedEyeDataPoints.at(i).size() - start_index;
        if (total < 2) ans << i;
    }
    return ans;
}

QVariantList EyeCorrectionCoefficients::getCutoffIndexesListAsVariantList() const {
    QVariantList ans;
    for (qint32 i = 0; i < cuttoffForCalibrationDataForCumputation.size(); i++){
        ans << cuttoffForCalibrationDataForCumputation.at(i);
    }
    return ans;
}

bool EyeCorrectionCoefficients::computeCoefficients(){
    error_msg = "";
    resultOfLastComputation = false;
    if (mode3D) resultOfLastComputation = computeCoefficients3D();
    else resultOfLastComputation = computeCoefficients2D();
    return resultOfLastComputation;
}

bool EyeCorrectionCoefficients::computeCoefficients2D(){

    LinearLeastSquaresFit fitterXL;
    LinearLeastSquaresFit fitterXR;
    LinearLeastSquaresFit fitterYL;
    LinearLeastSquaresFit fitterYR;

    for (qint32 i = 0; i < calibrationData.size(); i++){

        qreal xref = xtarget.at(i);
        qreal yref = ytarget.at(i);

        qint32 start_point = cuttoffForCalibrationDataForCumputation.at(i);

        for (qint32 j = start_point; j < calibrationData.at(i).size(); j++){

            EyeTrackerData erd = calibrationData.at(i).at(j);
            fitterXL.addInputAndTarget(erd.xl(),xref);
            fitterYL.addInputAndTarget(erd.yl(),yref);

            fitterXR.addInputAndTarget(erd.xr(),xref);
            fitterYR.addInputAndTarget(erd.yr(),yref);
        }
    }

    //qDebug() << "LINEAR FITTING XL";
    QStringList invalidCoefficients;

    xl = fitterXL.linearFit();
    if (!xl.isValid()) invalidCoefficients << "XL";

    //qDebug() << "LINEAR FITTING XR";
    xr = fitterXR.linearFit();
    if (!xr.isValid()) invalidCoefficients << "XR";

    //qDebug() << "LINEAR FITTING YL";
    yl = fitterYL.linearFit();
    if (!yl.isValid()) invalidCoefficients << "XR";

    //qDebug() << "LINEAR FITTING YR";
    yr = fitterYR.linearFit();
    if (!yr.isValid()) invalidCoefficients << "XR";

//    qDebug() << "Computed Coefficients";
//    qDebug() << xl.toString() << yl.toString();
//    qDebug() << xr.toString() << yr.toString();

    if (!invalidCoefficients.empty()){
        error_msg = "The following components did not have valid coefficients: " + invalidCoefficients.join(",");
        return false;
    }

    // We now compute the the fitted data points, for each target.
    fittedEyeDataPoints.clear();

    for (qint32 i = 0; i < calibrationData.size(); i++){

        QList<EyeTrackerData> f;

        for (qint32 j = 0; j < calibrationData.at(i).size(); j++){
            EyeTrackerData input = calibrationData.at(i).at(j);
            EyeTrackerData prediction;

            prediction.setXR(xr.predict(input.xr()));
            prediction.setYR(yr.predict(input.yr()));

            prediction.setXL(xl.predict(input.xl()));
            prediction.setYL(yl.predict(input.yl()));

            f << prediction;
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

    // QStringList textdata;

    for (qint32 i = 0; i < calibrationData.size(); i++){

        //qDebug() << "Accessing target" << i << "when sizes are" << xtarget.size() << ytarget.size() << ztarget.size() << " Calibration Data Points" << calibrationData.at(i).size();

        qreal xref = xtarget.at(i);
        qreal yref = ytarget.at(i);
        qreal zref = ztarget.at(i);

        qint32 start_point = cuttoffForCalibrationDataForCumputation.at(i);

        // textdata << QString::number(xref) + ", " + QString::number(yref) + ", " + QString::number(zref);

        // if (calibrationData.at(i).size() < 2) return false;

        for (qint32 j = start_point; j < calibrationData.at(i).size(); j++){
            EyeTrackerData input = calibrationData.at(i).at(j);

            // textdata << input.toString(true);

            QVector<qreal> lsample = input.leftVector();
            QVector<qreal> rsample = input.rightVector();
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
    QStringList invalidCoefficients;
    if (!fitterXL.computeCoefficients()) invalidCoefficients << "XL";
    xl.fromSimpleMatrix(fitterXL.getCoefficients());
    if (!xl.isValid()) invalidCoefficients << "XL";

    //qDebug() << "LINEAR FITTING XR";
    if (!fitterXR.computeCoefficients()) invalidCoefficients << "XR";
    xr.fromSimpleMatrix(fitterXR.getCoefficients());
    if (!xr.isValid()) invalidCoefficients << "XR";

    //qDebug() << "LINEAR FITTING YL";
    if (!fitterYL.computeCoefficients()) invalidCoefficients << "YL";
    yl.fromSimpleMatrix(fitterYL.getCoefficients());
    if (!yl.isValid()) invalidCoefficients << "YL";

    //qDebug() << "LINEAR FITTING YR";
    if (!fitterYR.computeCoefficients()) invalidCoefficients << "YR";
    yr.fromSimpleMatrix(fitterYR.getCoefficients());
    if (!yr.isValid()) invalidCoefficients << "YR";

    if (!fitterZR.computeCoefficients()) invalidCoefficients << "ZR";
    zr.fromSimpleMatrix(fitterZR.getCoefficients());
    if (!zr.isValid()) invalidCoefficients << "ZR";

    if (!fitterZL.computeCoefficients()) invalidCoefficients << "ZL";
    zl.fromSimpleMatrix(fitterZL.getCoefficients());
    if (!zl.isValid()) invalidCoefficients << "ZL";

    if (!invalidCoefficients.empty()){
        error_msg = "The following components did not have valid coefficients: " + invalidCoefficients.join(",");
        return false;
    }

    // We now compute the the fitted data points, for each target.
    fittedEyeDataPoints.clear();

    for (qint32 i = 0; i < calibrationData.size(); i++){

        QList<EyeTrackerData> f;

        for (qint32 j = 0; j < calibrationData.at(i).size(); j++){
            EyeTrackerData input = calibrationData.at(i).at(j);
            EyeTrackerData etd;

            etd.setXR(xr.predict(input.xr(),input.yr(),input.zr()));
            etd.setYR(yr.predict(input.xr(),input.yr(),input.zr()));
            etd.setZR(zr.predict(input.xr(),input.yr(),input.zr()));

            etd.setXL(xl.predict(input.xl(),input.yl(),input.zl()));
            etd.setYL(yl.predict(input.xl(),input.yl(),input.zl()));
            etd.setZL(zl.predict(input.xl(),input.yl(),input.zl()));

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

    //    QFile file("calibration_data.txt");
    //    file.open(QFile::WriteOnly);
    //    QTextStream writer (&file);
    //    writer << textdata.join("\n");
    //    file.close();

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

QList<QVector3D> EyeCorrectionCoefficients::getNonNormalizedTargetVector() const {
    return nonNormalizedTargetVectors;
}

EyeCorrectionCoefficients::RCoefficients EyeCorrectionCoefficients::getRCoefficients() const {
    return R2;
}

QString EyeCorrectionCoefficients::getLastError() const {
    return error_msg;
}

bool EyeCorrectionCoefficients::getResultOfLastComputation() const {
    return resultOfLastComputation;
}

EyeTrackerData EyeCorrectionCoefficients::computeCorrections(const EyeTrackerData &input) const {

    qreal nxl,nxr,nyl,nyr,nzl,nzr;

    if (mode3D){

        nxl = xl.predict(input.xl(),input.yl(),input.zl());
        nyl = yl.predict(input.xl(),input.yl(),input.zl());
        nzl = zl.predict(input.xl(),input.yl(),input.zl());

        nxr = xr.predict(input.xr(),input.yr(),input.zr());
        nyr = yr.predict(input.yr(),input.yr(),input.zr());
        nzr = zr.predict(input.yr(),input.yr(),input.zr());

    }
    else {
        nxl = xl.predict(input.xl());
        nxr = xr.predict(input.xr());
        nyl = yl.predict(input.yl());
        nyr = yr.predict(input.yr());
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
    etd.setPupilLeft(input.pl());
    etd.setPupilRight(input.pr());
    etd.setTimeStamp(input.timestamp());
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

    QString error;
    QVariantMap data = Globals::LoadJSONFileToVariantMap(file_name,&error);

    xl.fromMap(data.value("xl").toMap());
    xr.fromMap(data.value("xr").toMap());
    yl.fromMap(data.value("yl").toMap());
    yr.fromMap(data.value("yr").toMap());
    yl.fromMap(data.value("zl").toMap());
    yr.fromMap(data.value("zr").toMap());
    return true;


}

void EyeCorrectionCoefficients::saveCalibrationCoefficients(const QString &file_name) const{
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

QList< QList<qreal> > EyeCorrectionCoefficients::getHitPercentInTarget(qreal dimension, qreal tolerance, bool forLeftEye) const{

    QList<qreal> hitsAsPercent;
    QList<qreal> hitsAsNumber;
    QList<qreal> numberOfPointsTried;


    for (qint32 i = 0; i < fittedEyeDataPoints.size(); i++){

        qreal xref = xtarget.at(i);
        qreal yref = ytarget.at(i);

        qreal counter = 0;
        qreal total = 0;

        qint32 start_index = cuttoffForCalibrationDataForCumputation.at(i);

        for (qint32 j = start_index; j < fittedEyeDataPoints.at(i).size(); j++){

            total++;

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
                QVector3D nonNormalizedTargetVector = nonNormalizedTargetVectors.at(i);
                //qDebug() << "Fitted raw data point of" << x << y << z << "As comparted to " << nonNormalizedTargetVector << validationRadious;
                if (isVectorCloseEnough(nonNormalizedTargetVector,x,y,z)){
                    //qDebug() << "Fitted raw data point of" << x << y << z << "As comparted to " << nonNormalizedTargetVector << validationRadious;
                    counter++;
                }
            }
            else {
                if (isPointInside(x,y,xref,yref,dimension,tolerance)){
                    counter++;
                }
            }

        }

        if (total < 1){
            hitsAsPercent << 0;
            hitsAsNumber << 0;
        }
        else {
            hitsAsPercent << counter*100.0/total;
            hitsAsNumber << counter;
        }

        numberOfPointsTried << total;
    }

    QList< QList<qreal> > ans;

    ans << hitsAsPercent << hitsAsNumber << numberOfPointsTried;

    return ans;

}

bool EyeCorrectionCoefficients::isPointInside(qreal x, qreal y, qreal x_target, qreal y_target, qreal dimension, qreal tolerance) const {

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

bool EyeCorrectionCoefficients::isVectorCloseEnough(QVector3D tv, qreal x, qreal y, qreal z) const{

    qreal module = static_cast<qreal>(tv.length());
    x = x*module;
    y = y*module;
    z = z*module;

    // qDebug() << "TARGET VECTOR" << tv << " Predicted Point" << x << y << z << "After using module" << module;

    qreal xt = static_cast<qreal>(tv.x());
    qreal yt = static_cast<qreal>(tv.y());
    qreal zt = static_cast<qreal>(tv.z());

    qreal sqrt_arg = (xt-x)*(xt-x) + (yt-y)*(yt-y) + (zt -z)*(zt -z);

    qreal dis = qSqrt(sqrt_arg);

    return (dis <= validationRadious);
}


QVariantMap EyeCorrectionCoefficients::getCalibrationControlPacketCompatibleMap(bool left_eye) const{

    QVariantMap map;

    if (left_eye){
        map["ccx"] = xl.toMap();
        map["ccy"] = yl.toMap();
        map["ccz"] = zl.toMap();
    }
    else {
        map["ccx"] = xr.toMap();
        map["ccy"] = yr.toMap();
        map["ccz"] = zr.toMap();
    }

    return map;
}


bool EyeCorrectionCoefficients::loadFromTextMatrix(const QString &text){


    QStringList vectors = text.split("\n",Qt::SkipEmptyParts);
    if (vectors.size() == 6){
        if (!xl.loadFromString(vectors.at(0))) return false;
        if (!yl.loadFromString(vectors.at(1))) return false;
        if (!zl.loadFromString(vectors.at(2))) return false;
        if (!xr.loadFromString(vectors.at(3))) return false;
        if (!yr.loadFromString(vectors.at(4))) return false;
        if (!zr.loadFromString(vectors.at(5))) return false;
    }
    else if (vectors.size() == 4){
        if (!xl.loadFromString(vectors.at(0))) return false;
        if (!yl.loadFromString(vectors.at(1))) return false;
        if (!xr.loadFromString(vectors.at(2))) return false;
        if (!yr.loadFromString(vectors.at(3))) return false;
    }
    else return false;

    resultOfLastComputation = true;

    return true;
}
