#include "calibrationleastsquares.h"

CalibrationLeastSquares::CalibrationLeastSquares()
{
    connect(&calibrationTimer,&QTimer::timeout,this,&CalibrationLeastSquares::calibrationTimeInTargetUp);
    isCalibratingFlag = false;
}

QImage CalibrationLeastSquares::getCurrentCalibrationImage() const{
    return currentCalibrationImage;
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

void CalibrationLeastSquares::configureValidation(const QVariantMap &calibrationValidationParameters){

    calibrationValidationData.clear();
    calibrationValidationData = calibrationValidationParameters;
    calibrationValidationData[VMDC::CalibrationFields::NUMBER_OF_CALIBRAION_POINTS] = calibrationPointsUsed;

    validationApproveThreshold = calibrationValidationParameters.value(VMDC::CalibrationFields::VALIDATION_POINT_ACCEPTANCE_THRESHOLD).toReal();
    validationPointHitTolerance = calibrationValidationParameters.value(VMDC::CalibrationFields::VALIDATION_POINT_HIT_TOLERANCE).toReal();
    validationPointsToPassForAcceptedValidation = calibrationValidationParameters.value(VMDC::CalibrationFields::REQ_NUMBER_OF_ACCEPTED_POINTS).toInt();

}

QString CalibrationLeastSquares::getValidationReport() const{
    return validationReport;
}

void CalibrationLeastSquares::addDataPointForCalibration(float xl, float yl, float xr, float yr){
    // qDebug() << "Adding data points for calibration" << xl << yl << xr << yr;
    if (isDataGatheringEnabled){
        EyeRealData eid;
        eid.xLeft = static_cast<qreal>(xl);
        eid.xRight = static_cast<qreal>(xr);
        eid.yLeft = static_cast<qreal>(yl);
        eid.yRight = static_cast<qreal>(yr);
        coeffs.addPointForCoefficientComputation(eid,currentCalibrationPointIndex);
    }
}

void CalibrationLeastSquares::startCalibrationSequence(qint32 width, qint32 height, qint32 npoints,
                                                       qint32 ms_gather_time_for_calib_pt, qint32 ms_wait_time_calib_pt){

    // Initializing the calibration image generator.
    calibrationTargets.initialize(width,height);

    // Resetting the recommended eye to both
    recommendedEye = VMDC::Eye::BOTH;

    // Storing the information for the calibration validation data.
    calibrationValidationData[VMDC::CalibrationFields::NUMBER_OF_CALIBRAION_POINTS] = npoints;

    // Getting the centers of all the targets to be drawn.
    QList<QPointF> calibrationPoints = calibrationTargets.setupCalibrationSequence(npoints);

    // Creating the data point list.
    coeffs.configureForCoefficientComputation(calibrationPoints);

    numberOfCalibrationPoints = static_cast<qint32>(calibrationPoints.size());

    // Initializing the index and the structure for current collecting
    currentCalibrationPointIndex = -1;
    isCalibratingFlag = true;
    isDataGatheringEnabled = true;
    successfullyComputedCoefficients = false;
    calibration_gather_time = ms_gather_time_for_calib_pt;
    calibration_wait_time = ms_wait_time_calib_pt;
    calibrationTimeInTargetUp();

}

void CalibrationLeastSquares::calibrationTimeInTargetUp(){
    calibrationTimer.stop();

    if (isDataGatheringEnabled){

        // Checking if we are done.
        if (currentCalibrationPointIndex == (numberOfCalibrationPoints-1) ){
            isDataGatheringEnabled = false;
            isCalibratingFlag = false;
            //qDebug() << "Throwing Calibration Done";

            successfullyComputedCoefficients = coeffs.computeCoefficients();

            // Now we generate the calibration report.
            generateCalibrationReport();

            emit CalibrationLeastSquares::calibrationDone();
            return;

        }

        currentCalibrationPointIndex++;
        //qDebug() << "Calibrating point" << currentCalibrationPointIndex;
        currentCalibrationImage = calibrationTargets.nextSingleTarget();

        emit CalibrationLeastSquares::newCalibrationImageAvailable();
        calibrationTimer.start(calibration_wait_time);
        isDataGatheringEnabled = false;
    }
    else{
        // This means that we just finished waiting for the first part of the target. So we can start collecting data.
        isDataGatheringEnabled = true;
        calibrationTimer.start(calibration_gather_time);
    }

}

void CalibrationLeastSquares::generateCalibrationReport(){
    QStringList lines;

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

        qreal pl = leftHits.at(i)*100/validationData.at(i).size();
        qreal pr = rightHits.at(i)*100/validationData.at(i).size();

        QVariantList rightDataList;
        QVariantList leftDataList;
        for (qsizetype j = 0; j < validationData.at(i).size(); j++){
            QVariantMap left;
            QVariantMap right;
            left["x"] = validationData.at(i).at(j).xLeft; left["y"] = validationData.at(i).at(j).yLeft;
            right["x"] = validationData.at(i).at(j).xRight; right["y"] = validationData.at(i).at(j).yRight;
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

    QVariantMap Rreport;
    Rreport[VMDC::Eye::LEFT] = Rleft;
    Rreport[VMDC::Eye::RIGHT] = Rright;

    lines << "Right Eye R^2: x: " + QString::number(R.xr,'f',3) + ". y: " + QString::number(R.yr,'f',3);
    lines << "Left Eye R^2: x: " + QString::number(R.xl,'f',3) + ". y: " + QString::number(R.yl,'f',3);

    calibrationValidationData[VMDC::CalibrationFields::LEFT_EYE_VALIDATION_DATA]    = leftEyeData;
    calibrationValidationData[VMDC::CalibrationFields::RIGHT_EYE_VALIDATION_DATA]   = rightEyeData;
    calibrationValidationData[VMDC::CalibrationFields::MATH_ISSUES_FOR_CALIBRATION] = successfullyComputedCoefficients;
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_TARGET_DIAMETER] = calibrationTargets.getCalibrationTargetDiameter();
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_TARGET_LOCATION] = calibrationTargets.getCalibrationTargetCorners();
    calibrationValidationData[VMDC::CalibrationFields::COFICIENT_OF_DETERMINATION]  = Rreport;

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





