#include "calibrationleastsquares.h"

CalibrationLeastSquares::CalibrationLeastSquares()
{
    connect(&calibrationTimer,&QTimer::timeout,this,&CalibrationLeastSquares::calibrationTimeInTargetUp);
    connect(&calibrationTargets,&CalibrationTargets::newImageAvailable,this,&CalibrationLeastSquares::newCalibrationFrameAvailable);
    isCalibratingFlag = false;
}

void CalibrationLeastSquares::CalibrationData::clear(){
    eyeData.clear();
    x_reference = 0;
    y_reference = 0;
}

bool CalibrationLeastSquares::CalibrationData::isValid(){
    return (eyeData.size() >= 2 );
}

QString CalibrationLeastSquares::CalibrationData::toString(bool justRef) const{
    QString ans = "Reference for point (" + QString::number(x_reference) + "," + QString::number(y_reference) + ") -> [\n";
    if (justRef) return ans;
    for (qint32 i = 0; i < eyeData.size(); i++){
        ans = ans + "   " +  QString::number(eyeData.at(i).xr) + " "
                + "   " + QString::number(eyeData.at(i).yr) + " "
                + "   " + QString::number(eyeData.at(i).xl) + " "
                + "   " + QString::number(eyeData.at(i).yl) + ";\n";
    }
    ans.remove(ans.length()-1,1);
    ans= ans + "];";
    return ans;
}

QImage CalibrationLeastSquares::getCurrentCalibrationImage() const{
    //return currentCalibrationImage;
    return calibrationTargets.getCurrentFrame();
}

bool CalibrationLeastSquares::isCalibrating() const{
    return isCalibratingFlag;
}

void CalibrationLeastSquares::addDataPointForCalibration(float xl, float yl, float xr, float yr){
    if (isDataGatheringEnabled){
        EyeInputData eid;
        eid.xl = static_cast<qreal>(xl);
        eid.xr = static_cast<qreal>(xr);
        eid.yl = static_cast<qreal>(yl);
        eid.yr = static_cast<qreal>(yr);
        currentlyCollectingCalibrationPoints.eyeData << eid;
    }
}

void CalibrationLeastSquares::newCalibrationFrameAvailable(bool isTransitionDone){
    if (isTransitionDone){
        //qDebug() << "Transition is done. Starting calibration timer for wait time";
        calibrationTimer.start(CALIBRATION_WAIT_TIME);
    }
    emit CalibrationLeastSquares::newCalibrationImageAvailable();
}

void CalibrationLeastSquares::startCalibrationSequence(qint32 width, qint32 height, qint32 npoints){

    // Initializing the calibration image generator.
    calibrationTargets.initialize(width,height);

    // Getting the centers of all the targets to be drawn.
    QList<QPointF> calibrationPoints = calibrationTargets.setupCalibrationSequence(npoints);

    //qDebug() << "STARTING CALIBRATION SEQUENCE with " << calibrationPoints.size() << "points";

    // Creating the data point list.
    collectedCalibrationDataPoints.clear();
    for (qint32 i = 0; i < calibrationPoints.size(); i++){
        CalibrationData cd;
        cd.x_reference = calibrationPoints.at(i).x();
        cd.y_reference = calibrationPoints.at(i).y();
        collectedCalibrationDataPoints << cd;
        //qDebug() << "Added calibration point init as" << cd.toString();
    }

    // Initializing the index and the structure for current collecting
    isCalibratingFlag = true;
    currentlyCollectingCalibrationPoints = collectedCalibrationDataPoints.first();
    isDataGatheringEnabled = false;


    //calibrationTimeInTargetUp();
    calibrationTargets.nextSingleTarget();

}

void CalibrationLeastSquares::calibrationTimeInTargetUp(){

    calibrationTimer.stop();

    // Safety check as the application calls this function AFTER the calibration has finished, one last time.
    // if (currentCalibrationPointIndex >= collectedCalibrationDataPoints.size()) return;

    if (isDataGatheringEnabled){

        // This means that the We finished data gathreing and we should move on.
        if (calibrationTargets.getCurrentlyShownTarget() >= 0){
            //qDebug() << "Storing collected calibration data with reference" << currentlyCollectingCalibrationPoints.toString(true);
            collectedCalibrationDataPoints[calibrationTargets.getCurrentlyShownTarget()] = currentlyCollectingCalibrationPoints;
        }

        // Checking if we are done.
        //qDebug() << "Comparing calibration index with " << calibrationTargets.getCurrentlyShownTarget() << " last expected index of " << (collectedCalibrationDataPoints.size()-1);
        if (calibrationTargets.getCurrentlyShownTarget() == (collectedCalibrationDataPoints.size()-1)){
            isDataGatheringEnabled = false;
            isCalibratingFlag = false;
            //qDebug() << "Throwing Calibration Done";
            emit CalibrationLeastSquares::calibrationDone();
            return;
        }

        //qDebug() << "Calibration least squares calling calibration next single target";
        calibrationTargets.nextSingleTarget();

        //qDebug() << "Obtained new calibration point" << currentlyCollectingCalibrationPoints.toString(true) << " for index " << calibrationTargets.getCurrentlyShownTarget();
        currentlyCollectingCalibrationPoints = collectedCalibrationDataPoints.at(calibrationTargets.getCurrentlyShownTarget());

        isDataGatheringEnabled = false;
    }
    else{
        // This means that we just finished waiting for the first part of the target. So we can start collecting data.
        isDataGatheringEnabled = true;
        //qDebug() << "Start calibration timer for gathering data";
        calibrationTimer.start(CALIBRATION_GATHER_TIME);
    }

}

bool CalibrationLeastSquares::computeCalibrationCoeffs(){

    //QList<QPoint> calibrationPoints;

    // Getting the calibration points
    LeastSquaresData xr;
    LeastSquaresData yr;
    LeastSquaresData xl;
    LeastSquaresData yl;

    for (int i = 0; i < collectedCalibrationDataPoints.size(); i++){
        CalibrationData d = collectedCalibrationDataPoints.at(i);                

        if (!d.isValid()) {
            ///qDebug() << "Calibration failed because a calibration data contains less that two values. Which means that NO DATA was gathered for a given calibration point";
            return false;
        }

        // Getting the calibraton point.
        //QPoint point(static_cast<qint32>(d.xl.first()),static_cast<qint32>(d.yl.first()));
        qreal targetX = d.x_reference;
        qreal targetY = d.y_reference;

        //qDebug().noquote() << d.toString();

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

    //qDebug() << "Dividing by det " << det << " when calculating the coefficients";
    qreal im11 = m22/det;
    qreal im12 = -m12/det;
    qreal im21 = -m21/det;
    qreal im22 = m11/det;
    //qDebug() << "After the division";

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

EyeTrackerData CalibrationLeastSquares::EyeCorrectionCoeffs::computeCorrections(const EyeInputData &input){
    qreal nxl  = xl.m*input.xl  + xl.b;
    qreal nxr  = xr.m*input.xr + xr.b;
    qreal nyl  = yl.m*input.yl  + yl.b;
    qreal nyr  = yr.m*input.yr + yr.b;

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

QString CalibrationLeastSquares::EyeCorrectionCoeffs::toString() const{
    QString ans;
    ans = ans + "XR: " + xr.toString() + "\n";
    ans = ans + "YR: " + yr.toString() + "\n";
    ans = ans + "XL: " + xl.toString() + "\n";
    ans = ans + "YL: " + yl.toString() + "\n";
    return ans;
}

QVariantMap CalibrationLeastSquares::LinearCoeffs::toMap(){
    QVariantMap map;
    map.insert("m",m);
    map.insert("b",b);
    map.insert("valid",valid);
    return map;
}

QString CalibrationLeastSquares::LinearCoeffs::toString() const{
    QString ans =  "M: " + QString::number(m) + ". B: " + QString::number(b);
    if (valid) ans = ans + ". VALID";
    else ans = ans + ". INVALID";
    return ans;
}


void CalibrationLeastSquares::LinearCoeffs::fromMap(const QVariantMap &map){
    m = map.value("m").toDouble();
    b = map.value("b").toDouble();
    valid = map.value("valid").toBool();
}

bool CalibrationLeastSquares::EyeCorrectionCoeffs::loadCalibrationCoefficients(const QString &file_name){

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

void CalibrationLeastSquares::EyeCorrectionCoeffs::saveCalibrationCoefficients(const QString &file_name){
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
