#include "calibrationleastsquares.h"

CalibrationLeastSquares::CalibrationLeastSquares()
{

    isCalibratingFlag = false;
    connect(&calibrationTargets,&CalibrationTargets::calibrationPointStatus,this,&CalibrationLeastSquares::onCalibrationPointStatus);
    connect(&calibrationTargets,&CalibrationTargets::newAnimationFrame,this,&CalibrationLeastSquares::onNewAnimationFrame);
    connect(&waitToGatherTimer,&QTimer::timeout,this,&CalibrationLeastSquares::onWaitToDataGather);

}

RenderServerScene CalibrationLeastSquares::getCurrentCalibrationImage() const{
    return calibrationTargets.getCurrentCalibrationAnimationFrame();
}

bool CalibrationLeastSquares::isCalibrating() const{
    return isCalibratingFlag;
}

QVariantMap CalibrationLeastSquares::getCalibrationValidationData() const {
    return calibrationValidationData;
}

QString CalibrationLeastSquares::getRecommendedEye() const {
    return recommendedEye;
}

EyeCorrectionCoefficients CalibrationLeastSquares::getCalculatedCoeficients() const {
    return coeffs;
}

void CalibrationLeastSquares::onWaitToDataGather(){
    isDataGatheringEnabled = true;
    waitToGatherTimer.stop();
}

void CalibrationLeastSquares::onNewAnimationFrame(){
    if (!isCalibratingFlag) return;
    emit CalibrationLeastSquares::newCalibrationImageAvailable();
}

void CalibrationLeastSquares::onCalibrationPointStatus(qint32 whichCalibrationPoint, bool isMoving){

    //qDebug() << "Calibration Point Status Update" <<  whichCalibrationPoint << isMoving;

    QString str = QString::number(whichCalibrationPoint);
    if (isMoving) str = str + ". Is Moving: true";
    else str = str + ". Is Moving: false";

    // StaticThreadLogger::log("CalibrationLeastSquares::onCalibrationPointStatus","Calibration Point Update to: " + str);

    if (isMoving){
        isDataGatheringEnabled = false;
        if (whichCalibrationPoint == (numberOfCalibrationPoints-1)){
            // We've reached the end.
            calibrationTargets.calibrationAnimationControl(false);
            finalizeCalibrationProcess();
        }
    }
    else {
        waitToGatherTimer.start(calibration_wait_time);
        currentCalibrationPointIndex = whichCalibrationPoint;
    }
}


void CalibrationLeastSquares::configureValidation(const QVariantMap &calibrationValidationParameters){

    calibrationValidationData.clear();
    calibrationValidationData = calibrationValidationParameters;
    calibrationValidationData[VMDC::CalibrationFields::NUMBER_OF_CALIBRAION_POINTS] = calibrationPointsUsed;
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_PTS_NO_DATA] = QVariantList(); // It is initialized as en empty list.

    validationApproveThreshold = calibrationValidationParameters.value(VMDC::CalibrationFields::VALIDATION_POINT_ACCEPTANCE_THRESHOLD).toReal();
    validationPointHitTolerance = calibrationValidationParameters.value(VMDC::CalibrationFields::VALIDATION_POINT_HIT_TOLERANCE).toReal();
    validationPointsToPassForAcceptedValidation = calibrationValidationParameters.value(VMDC::CalibrationFields::REQ_NUMBER_OF_ACCEPTED_POINTS).toInt();

}

QString CalibrationLeastSquares::getValidationReport() const{
    return validationReport;
}

void CalibrationLeastSquares::addDataPointForCalibration(float xl, float yl, float xr, float yr, float zl, float zr){
    // qDebug() << "Adding data points for calibration" << xl << yl << xr << yr;
    if (isDataGatheringEnabled){
        EyeRealData eid;
        eid.xLeft = static_cast<qreal>(xl);
        eid.xRight = static_cast<qreal>(xr);
        eid.yLeft = static_cast<qreal>(yl);
        eid.yRight = static_cast<qreal>(yr);
        eid.zLeft = static_cast<qreal>(zl);
        eid.zRight = static_cast<qreal>(zr);
        coeffs.addPointForCoefficientComputation(eid,currentCalibrationPointIndex);
    }
}

void CalibrationLeastSquares::set3DCalibrationVectors(const QList<QVector3D> &targetVectors){
    coeffs.set3DTargetVectors(targetVectors);
}

void CalibrationLeastSquares::startCalibrationSequence(qint32 width, qint32 height, qint32 npoints,
                                                       qint32 ms_gather_time_for_calib_pt, qint32 ms_wait_time_calib_pt,
                                                       float vFOV, float hFOV){

    // Initializing the calibration image generator.
    calibrationTargets.initialize(width,height);

    // Resetting the recommended eye to both
    recommendedEye = VMDC::Eye::BOTH;

    // Storing the information for the calibration validation data.
    calibrationValidationData[VMDC::CalibrationFields::NUMBER_OF_CALIBRAION_POINTS] = npoints;

    // Getting the centers of all the targets to be drawn.
    QList<QPointF> calibrationPoints = calibrationTargets.setupCalibrationSequence(npoints,ms_gather_time_for_calib_pt + ms_wait_time_calib_pt);
    calibrationCenters = calibrationPoints;

    // Creating the data point list.
    if ((qAbs(vFOV) > 0) && (qAbs(hFOV) > 0)){ // A Less than zero value means we are using 3D Mode.
        coeffs.setMode(true);
        coeffs.configureForCoefficientComputationOf3DVectors(npoints);

        // Now we compute the target vectors.
        // The following code are Gaston's trigonometric computations for the 3D target vectors representing 2D points,
        // Assuming that the rectangle of width and height provided occupies the entire screen, exactly in the HMD.
        // More specifically, it assumes the rectangle lines up perfectly wih the top and bottom of the HMD screen (all visible).

        float w = static_cast<float>(width);
        float h = static_cast<float>(height);

        //float hFOV = vFOV*w/h;
        float zx = w/(2*qTan(qDegreesToRadians(hFOV)/2));
        float zy = h/(2*qTan(qDegreesToRadians(vFOV)/2));
        float z = (zx+zy)/2;

        QList<QVector3D> target_vectors;

        for (qint32 i = 0; i < calibrationPoints.size(); i++){

            float x = static_cast<float>(calibrationPoints.at(i).x());
            float y = static_cast<float>(calibrationPoints.at(i).y());

            x = w/2  - x;
            y = h/2  - y;

            float module = static_cast<float>(qSqrt(x*x + y*y + z*z));

            QVector3D tvector;
            tvector.setX(x/module);
            tvector.setY(y/module);
            tvector.setZ(z/module);

            qDebug() << "TARGET VECTOR OF" << tvector << "hFOV" << hFOV << "compared to" << vFOV*w/h << "vFOV" << vFOV;

            target_vectors << tvector;

        }

        coeffs.set3DTargetVectors(target_vectors);

    }
    else {
        coeffs.setMode(false);
        coeffs.configureForCoefficientComputation(calibrationPoints);
    }

    numberOfCalibrationPoints = static_cast<qint32>(calibrationPoints.size());

    // Initializing the index and the structure for current collecting
    currentCalibrationPointIndex = -1;
    isCalibratingFlag = true;
    isDataGatheringEnabled = false;
    successfullyComputedCoefficients = false;
    calibration_gather_time = ms_gather_time_for_calib_pt;
    calibration_wait_time = ms_wait_time_calib_pt;

    calibrationTargets.calibrationAnimationControl(true);
}

void CalibrationLeastSquares::finalizeCalibrationProcess() {
    qDebug() << "Finalizing Calibration Process";
    isDataGatheringEnabled = false;
    isCalibratingFlag = false;

    qDebug() << "Computing Coefficients";
    successfullyComputedCoefficients = coeffs.computeCoefficients();

    qDebug() << "Generate calibration report";
    // Now we generate the calibration report.
    generateCalibrationReport();

    qDebug() << "Sending Calibration Done Signal";
    emit CalibrationLeastSquares::calibrationDone();
}

void CalibrationLeastSquares::generateCalibrationReport(){
    QStringList lines;

    qDebug() << "Generating Calibration Report. Is Mode 3D" << coeffs.isIn3DMode();

    lines << "Validation Threshold " + QString::number(validationApproveThreshold)
             + " %. Required Number of Passing Targets: " + QString::number(validationPointsToPassForAcceptedValidation);

    qint32 leftEyePass = 0;
    qint32 rightEyePass = 0;

    QVariantMap rightEyeData;
    QVariantMap leftEyeData;

    rightEyeData["is_validated"] = false;
    leftEyeData["is_validated"] = false;

    QList< QList<EyeTrackerData> > validationData = coeffs.getFittedData();
    QList< qint32 > rightHits = coeffs.getHitsInTarget(calibrationTargets.getCalibrationTargetDiameter(),validationPointHitTolerance,false);
    QList< qint32 > leftHits = coeffs.getHitsInTarget(calibrationTargets.getCalibrationTargetDiameter(),validationPointHitTolerance,true);

    for (qsizetype i = 0; i < validationData.size(); i++){

        //qDebug() << "Validation data at" << i << " of " << validationData.size();

        qreal pl = leftHits.at(i)*100/validationData.at(i).size();
        qreal pr = rightHits.at(i)*100/validationData.at(i).size();

        QVariantList rightDataList;
        QVariantList leftDataList;
        for (qsizetype j = 0; j < validationData.at(i).size(); j++){
            QVariantMap left;
            QVariantMap right;
            left["x"] = validationData.at(i).at(j).xl(); left["y"] = validationData.at(i).at(j).yl();
            right["x"] = validationData.at(i).at(j).xr(); right["y"] = validationData.at(i).at(j).yr();
            if (coeffs.isIn3DMode()){
                left["z"] = validationData.at(i).at(j).zl();
                right["z"] = validationData.at(i).at(j).zr();
            }

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
                 + QString::number(leftHits.at(i)) + " out of " + QString::number(validationData.at(i).size()) + ": " + QString::number(pl,'f',1) + " %"
                 + ". RIGHT EYE: "
                 + QString::number(rightHits.at(i)) + " out of " + QString::number(validationData.at(i).size()) + ": " + QString::number(pr,'f',1) + " %";
    }


    if (leftEyePass >= validationPointsToPassForAcceptedValidation) {
        leftEyeData["is_validated"] = true;
        lines << "LEFT EYE VALIDATED";
    }
    else{
        lines << "LEFT EYE VALIDATION FAILED";
    }
    if (rightEyePass >= validationPointsToPassForAcceptedValidation) {
        rightEyeData["is_validated"] = true;
        lines << "RIGHT EYE VALIDATED";
    }
    else {
        lines << "RIGHT EYE VALIDATION FAILED";
    }

    // Adding each of the R squared coefficients to the calibration reprot data.
    QVariantMap Rright;
    QVariantMap Rleft;

    EyeCorrectionCoefficients::RCoefficients R = coeffs.getRCoefficients();

    Rright["x"] = R.xr; Rleft["x"] = R.xl;
    Rright["y"] = R.yr; Rleft["y"] = R.yl;
    Rright["z"] = R.zr; Rleft["z"] = R.zl;

    QVariantMap Rreport;
    Rreport[VMDC::Eye::LEFT] = Rleft;
    Rreport[VMDC::Eye::RIGHT] = Rright;

    if (coeffs.isIn3DMode()){
        lines << "Right Eye R^2: x: " + QString::number(R.xr,'f',3) + ". y: " + QString::number(R.yr,'f',3) + ". z: " + QString::number(R.zr,'f',3);
        lines << "Left Eye R^2: x: " + QString::number(R.xl,'f',3) + ". y: " + QString::number(R.yl,'f',3) + ". z: " + QString::number(R.zl,'f',3);
    }
    else {
        lines << "Right Eye R^2: x: " + QString::number(R.xr,'f',3) + ". y: " + QString::number(R.yr,'f',3);
        lines << "Left Eye R^2: x: " + QString::number(R.xl,'f',3) + ". y: " + QString::number(R.yl,'f',3);
    }

    QString pointsWithNoData = coeffs.getCalibrationPointsWithNoDataAsAString();
    if (pointsWithNoData != ""){
        lines << "The following calibration points had too few data points for calibration: " + pointsWithNoData;
    }

    calibrationValidationData[VMDC::CalibrationFields::LEFT_EYE_VALIDATION_DATA]    = leftEyeData;
    calibrationValidationData[VMDC::CalibrationFields::RIGHT_EYE_VALIDATION_DATA]   = rightEyeData;
    calibrationValidationData[VMDC::CalibrationFields::MATH_ISSUES_FOR_CALIBRATION] = successfullyComputedCoefficients;
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_TARGET_DIAMETER] = calibrationTargets.getCalibrationTargetDiameter();
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_TARGET_LOCATION] = calibrationTargets.getCalibrationTargetCorners();
    calibrationValidationData[VMDC::CalibrationFields::COFICIENT_OF_DETERMINATION]  = Rreport;
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_PTS_NO_DATA]     = coeffs.getCalibrationPointsWithNoData();

    validationReport = lines.join("\n");

    // The very simple logic to select the best eye for the study.
    //qDebug() << "ON CalibrationLeastSquares::generateCalibrationReport. ADD EYE COMPUTING LOGIC";
    if (leftEyeData.value("is_validated").toBool()){
        recommendedEye = VMDC::Eye::LEFT;
        if (rightEyeData.value("is_validated").toBool()){
            recommendedEye = VMDC::Eye::BOTH;
        }
    }
    else if (rightEyeData.value("is_validated").toBool()){
        recommendedEye = VMDC::Eye::RIGHT;
    }

}

QList<QPointF> CalibrationLeastSquares::getCalibratiionTargetCenters() const {
    return calibrationCenters;
}




