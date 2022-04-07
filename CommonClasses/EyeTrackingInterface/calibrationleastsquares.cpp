#include "calibrationleastsquares.h"

CalibrationLeastSquares::CalibrationLeastSquares()
{
    connect(&calibrationTimer,&QTimer::timeout,this,&CalibrationLeastSquares::calibrationTimeInTargetUp);
    isCalibratingFlag = false;
    isValidatingFlag = false;
}

void CalibrationLeastSquares::CalibrationData::clear(){
    eyeData.clear();
    x_reference = 0;
    y_reference = 0;
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

QImage CalibrationLeastSquares::getCurrentCalibrationImage() const{
    return currentCalibrationImage;
}

bool CalibrationLeastSquares::isCalibrating() const{
    return isCalibratingFlag;
}

bool CalibrationLeastSquares::isValidating() const{
    return isValidatingFlag;
}

bool CalibrationLeastSquares::wereCalibrationCoefficientsComputedSuccessfully() const{
    return successfullyComputedCoefficients;
}

QVariantMap CalibrationLeastSquares::getCalibrationValidationData() const {
    return calibrationValidationData;
}

void CalibrationLeastSquares::configureValidation(const QVariantMap &calibrationValidationParameters){
    // Computing the number of data points required for the constant hold time based on the sampling frequency.
    qreal sampling_frequency = calibrationValidationParameters.value(VMDC::CalibrationFields::SAMPLING_FREQUENCY).toReal();
    qreal hold_time_for_validation = calibrationValidationParameters.value(VMDC::CalibrationFields::VALIDATION_POINT_LENGTH).toReal();
    validationMaxNumberOfDataPoints = static_cast<qint32>(sampling_frequency*hold_time_for_validation/1000);

    calibrationValidationData.clear();
    calibrationValidationData = calibrationValidationParameters;
    calibrationValidationData[VMDC::CalibrationFields::NUMBER_OF_CALIBRAION_POINTS] = calibrationPointsUsed;

    //qDebug() << "Validation number of Data points" << validationMaxNumberOfDataPoints << " FROM " << sampling_frequency << hold_time_for_validation;

    validationApproveThreshold = calibrationValidationParameters.value(VMDC::CalibrationFields::VALIDATION_POINT_ACCEPTANCE_THRESHOLD).toReal();
    validationPointHitTolerance = calibrationValidationParameters.value(VMDC::CalibrationFields::VALIDATION_POINT_HIT_TOLERANCE).toReal();
    validationPointsToPassForAcceptedValidation = calibrationValidationParameters.value(VMDC::CalibrationFields::REQ_NUMBER_OF_ACCEPTED_POINTS).toInt();

    if (calibrationValidationParameters.value(VMDC::CalibrationFields::ENABLE_GAZEFOLLOWING_DURING_VALIDATION).toBool()){
        calibrationTargets.enableEyeFollowersDuringValidation(true);
    }

}

QString CalibrationLeastSquares::getValidationReport() const{
    return validationReport;
}

void CalibrationLeastSquares::generateCalibrationReport(){
    QStringList lines;

    lines << "Validation Threshold " + QString::number(validationApproveThreshold)
             + " %. Required Number of Passing Targets: " + QString::number(validationPointsToPassForAcceptedValidation);

    leftEyeValidated = false;
    rightEyeValidated = false;

    qint32 leftEyePass = 0;
    qint32 rightEyePass = 0;

    QVariantMap rightEyeData;
    QVariantMap leftEyeData;

    for (qsizetype i = 0; i < validationData.size(); i++){

        qreal pl = validationData.at(i).eyeLeftHits*100/validationData.at(i).eyedata.size();
        qreal pr = validationData.at(i).eyeRightHits*100/validationData.at(i).eyedata.size();

        QVariantList rightDataList;
        QVariantList leftDataList;
        for (qsizetype j = 0; j < validationData.at(i).eyedata.size(); j++){
            QVariantMap left;
            QVariantMap right;
            left["x"] = validationData.at(i).eyedata.at(j).xLeft; left["y"] = validationData.at(i).eyedata.at(j).yLeft;
            right["x"] = validationData.at(i).eyedata.at(j).xRight; right["y"] = validationData.at(i).eyedata.at(j).yRight;
            rightDataList << right;
            leftDataList << left;
        }
        rightEyeData["validation_target_" + QString::number(i)] = rightDataList;
        leftEyeData["validation_target_" + QString::number(i)] = leftDataList;

        if (pl >= validationApproveThreshold){
            leftEyePass++;
        }

        if (pr >= validationApproveThreshold){
            rightEyePass++;
        }

        lines << " Validation Target " + QString::number(i) +  ". LEFT EYE: "
                 + QString::number(validationData.at(i).eyeLeftHits) + " out of " + QString::number(validationData.at(i).eyedata.size()) + ": " + QString::number(pl,'f',1) + " %"
                 + ". RIGHT EYE: "
                 + QString::number(validationData.at(i).eyeRightHits) + " out of " + QString::number(validationData.at(i).eyedata.size()) + ": " + QString::number(pr,'f',1) + " %";
    }

    if (leftEyePass >= validationPointsToPassForAcceptedValidation) {
        leftEyeValidated = true;
        lines << "LEFT EYE VALIDATED";
    }
    else{
        lines << "LEFT EYE VALIDATION FAILED";
    }
    if (rightEyePass >= validationPointsToPassForAcceptedValidation) {
        rightEyeValidated = true;
        lines << "RIGHT EYE VALIDATED";
    }
    else {
        lines << "RIGHT EYE VALIDATION FAILED";
    }

    calibrationValidationData[VMDC::CalibrationFields::LEFT_EYE_VALIDATION_DATA] = leftEyeData;
    calibrationValidationData[VMDC::CalibrationFields::RIGHT_EYE_VALIDATION_DATA] = rightEyeData;
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_TARGET_DIAMETER] = calibrationTargets.getCalibrationTargetDiameter();
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_TARGET_LOCATION] = calibrationTargets.getCalibrationTargetCorners();

    validationReport = lines.join("\n");
}

CalibrationLeastSquares::EyeValidationsStatus CalibrationLeastSquares::getEyeValidationStatus() const {
    if (leftEyeValidated){
        if (rightEyeValidated) return EVS_BOTH;
        else return EVS_LEFT;
    }
    else if (rightEyeValidated){
        return EVS_RIGHT;
    }
    return EVS_NONE;
}

void CalibrationLeastSquares::addDataPointForCalibration(float xl, float yl, float xr, float yr){
    //qDebug() << "Adding data points for calibration" << xl << yl << xr << yr;
    if (isDataGatheringEnabled){
        EyeInputData eid;
        eid.xl = static_cast<qreal>(xl);
        eid.xr = static_cast<qreal>(xr);
        eid.yl = static_cast<qreal>(yl);
        eid.yr = static_cast<qreal>(yr);
        currentlyCollectingCalibrationPoints.eyeData << eid;
    }
}

void CalibrationLeastSquares::addDataPointForVerification(const EyeInputData &eid){
    if (isValidatingFlag) {

        //qDebug() << "CLS: Received Verifcation data point";

        EyeTrackerData etd = coeffs.computeCorrections(eid);

        if (calibrationTargets.isPointWithinCurrentTarget(etd.xLeft,etd.yLeft,validationPointHitTolerance)){
            validationData[currentCalibrationPointIndex].eyeLeftHits++;
        }

        if (calibrationTargets.isPointWithinCurrentTarget(etd.xRight,etd.yRight,validationPointHitTolerance)){
            validationData[currentCalibrationPointIndex].eyeRightHits++;
        }

        validationData[currentCalibrationPointIndex].eyedata << etd;
        if (validationData.at(currentCalibrationPointIndex).eyedata.size() >= validationMaxNumberOfDataPoints){

            currentCalibrationPointIndex++;
            if (currentCalibrationPointIndex >= validationData.size()){
                isValidatingFlag = false;

                // Computing the calibration results and generating the report.
                generateCalibrationReport();

                currentCalibrationImage = calibrationTargets.getClearScreen();
                emit CalibrationLeastSquares::newCalibrationImageAvailable();
                emit CalibrationLeastSquares::calibrationDone();
            }
            else {
                if (calibrationTargets.isGazeFollowingEnabled()){
                    calibrationTargets.nextSingleTarget();
                    currentCalibrationImage = calibrationTargets.renderCurrentPosition(etd.xRight,etd.yRight,etd.xLeft,etd.yLeft);
                    emit CalibrationLeastSquares::newCalibrationImageAvailable();
                }
                else {
                    currentCalibrationImage = calibrationTargets.nextSingleTarget();
                    emit CalibrationLeastSquares::newCalibrationImageAvailable();
                }
            }
        }
        else if (calibrationTargets.isGazeFollowingEnabled()){
            currentCalibrationImage = calibrationTargets.renderCurrentPosition(etd.xRight,etd.yRight,etd.xLeft,etd.yLeft);
            emit CalibrationLeastSquares::newCalibrationImageAvailable();
        }
    }
}

void CalibrationLeastSquares::startCalibrationSequence(qint32 width, qint32 height, qint32 npoints){

    // Initializing the calibration image generator.
    calibrationTargets.initialize(width,height);

    // Storing the information for the calibration validation data.
    calibrationValidationData[VMDC::CalibrationFields::NUMBER_OF_CALIBRAION_POINTS] = npoints;

    // Getting the centers of all the targets to be drawn.
    QList<QPointF> calibrationPoints = calibrationTargets.setupCalibrationSequence(npoints);

    // Creating the data point list.
    collectedCalibrationDataPoints.clear();
    for (qint32 i = 0; i < calibrationPoints.size(); i++){
        CalibrationData cd;
        cd.x_reference = calibrationPoints.at(i).x();
        cd.y_reference = calibrationPoints.at(i).y();
        collectedCalibrationDataPoints << cd;
        validationData << ValidationTarget();
    }

    //qDebug() << "startingUpCalibration Seq. Number of Colleted Calibration Data Points Structs Created: " << collectedCalibrationDataPoints.size();

    // Initializing the index and the structure for current collecting
    currentCalibrationPointIndex = -1;
    isCalibratingFlag = true;
    isDataGatheringEnabled = true;
    isValidatingFlag = false;
    successfullyComputedCoefficients = false;
    calibrationTimeInTargetUp();

}

void CalibrationLeastSquares::calibrationTimeInTargetUp(){
    calibrationTimer.stop();

    if (isDataGatheringEnabled){

        // This means that the We finished data gathreing and we should move on.
        if (currentCalibrationPointIndex >= 0){
            collectedCalibrationDataPoints[currentCalibrationPointIndex] = currentlyCollectingCalibrationPoints;
        }

        // Checking if we are done.
        if (currentCalibrationPointIndex == (collectedCalibrationDataPoints.size()-1)){
            isDataGatheringEnabled = false;
            isCalibratingFlag = false;
            //qDebug() << "Throwing Calibration Done";

            successfullyComputedCoefficients = computeCalibrationCoeffs();

            if (!successfullyComputedCoefficients){
                // Failed to compute the calibration coefficients, so there is no point in validating.
                emit CalibrationLeastSquares::calibrationDone();
                return;
            }

            // Calibration has finished. So we setup verification. and continue gathering data.
            isValidatingFlag = true;
            currentCalibrationPointIndex = 0;
            calibrationTargets.verificationInitialization(VALIDATION_NPOINTS);
            validationData.clear();
            for (qint32 i = 0; i < VALIDATION_NPOINTS; i++){
                validationData << ValidationTarget();
            }

            //qDebug() << "CLS: Initialized Verification";

            // Showing the first target for verification.
            currentCalibrationImage = calibrationTargets.nextSingleTarget();
            emit CalibrationLeastSquares::newCalibrationImageAvailable();

            //emit CalibrationLeastSquares::calibrationDone();
            return;
        }

        currentCalibrationPointIndex++;
        //qDebug() << "Calibrating point" << currentCalibrationPointIndex;
        currentlyCollectingCalibrationPoints = collectedCalibrationDataPoints.at(currentCalibrationPointIndex);
        currentCalibrationImage = calibrationTargets.nextSingleTarget();

        emit CalibrationLeastSquares::newCalibrationImageAvailable();
        calibrationTimer.start(CALIBRATION_WAIT_TIME);
        isDataGatheringEnabled = false;
    }
    else{
        // This means that we just finished waiting for the first part of the target. So we can start collecting data.
        isDataGatheringEnabled = true;
        calibrationTimer.start(CALIBRATION_GATHER_TIME);
    }

}

bool CalibrationLeastSquares::computeCalibrationCoeffs(){

    // Getting the calibration points
    LeastSquaresData xr;
    LeastSquaresData yr;
    LeastSquaresData xl;
    LeastSquaresData yl;

    //qDebug() << "computeCalibrationCoefss. Number of Colleted Calibration Data Points Structs To Use: " << collectedCalibrationDataPoints.size();

    for (int i = 0; i < collectedCalibrationDataPoints.size(); i++){
        CalibrationData d = collectedCalibrationDataPoints.at(i);

        //qDebug() << "Collected Calibration Data for point " << i << d.eyeData.size();

        if (!d.isValid()) {
            ///qDebug() << "Calibration failed because a calibration data contains less that two values. Which means that NO DATA was gathered for a given calibration point";
            return false;
        }

        // Getting the calibraton point.
        //QPoint point(static_cast<qint32>(d.xl.first()),static_cast<qint32>(d.yl.first()));
        qreal targetX = d.x_reference;
        qreal targetY = d.y_reference;

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
