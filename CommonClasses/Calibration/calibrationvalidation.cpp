#include "calibrationvalidation.h"

CalibrationValidation::CalibrationValidation()
{
    recommendedEye = VMDC::Eye::BOTH;
}


QVariantMap CalibrationValidation::getCalibrationValidationData() const {
    return calibrationValidationData;
}

QString CalibrationValidation::getRecommendedEye() const {
    return recommendedEye;
}


void CalibrationValidation::configureValidation(const QVariantMap &calibrationValidationParameters,
                                                const QList<QVector3D> &nonNormalizedTargetVecs,
                                                qreal validationRadious,
                                                const QVariantList &calibrationTargetCornersFor2D,
                                                qreal calibrationDiameterFor2D){

    calibrationValidationData.clear();
    calibrationValidationData = calibrationValidationParameters;    
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_PTS_NO_DATA] = QVariantList(); // It is initialized as en empty list.

    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_TARGET_DIAMETER] = calibrationDiameterFor2D;
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_TARGET_LOCATION] = calibrationTargetCornersFor2D;

    validationApproveThreshold = calibrationValidationParameters.value(VMDC::CalibrationFields::VALIDATION_POINT_ACCEPTANCE_THRESHOLD).toReal();
    validationPointHitTolerance = calibrationValidationParameters.value(VMDC::CalibrationFields::VALIDATION_POINT_HIT_TOLERANCE).toReal();
    validationPointsToPassForAcceptedValidation = calibrationValidationParameters.value(VMDC::CalibrationFields::REQ_NUMBER_OF_ACCEPTED_POINTS).toInt();

    recommendedEye = VMDC::Eye::BOTH;

    if (nonNormalizedTargetVecs.isEmpty()){
        configuredFor3D = false;
        calibrationValidationData[VMDC::CalibrationFields::NUMBER_OF_CALIBRAION_POINTS] = calibrationTargetCornersFor2D.size();
    }
    else {

        configuredFor3D = true;

        // In order to properly stored the no normalized vectors, we need to transform to a map. As a list of lists gets transformed into a single list.
        QVariantMap tvecs;
        for (qint32 i = 0; i < nonNormalizedTargetVecs.size(); i++){
            QVariantList tv;
            QVector3D vect = nonNormalizedTargetVecs.at(i);
            tv << vect.x() << vect.y() << vect.z();
            tvecs[QString::number(i)] = tv;
        }

        calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_NON_NORM_VECS] = tvecs;
        calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_VALIDATION_R] = validationRadious;
        calibrationValidationData[VMDC::CalibrationFields::NUMBER_OF_CALIBRAION_POINTS] = tvecs.size();
    }

}

QString CalibrationValidation::getValidationReport() const{
    return validationReport;
}

bool CalibrationValidation::generateCalibrationReport(const EyeCorrectionCoefficients &coeffs){

    QStringList lines;

    //qDebug() << "Generating Calibration Report. Is Mode 3D" << coeffs.isIn3DMode();

    QString mode = "2D";
    if (configuredFor3D) mode = "3D";

    qreal diameterFor2D = calibrationValidationData.value(VMDC::CalibrationFields::CALIBRATION_TARGET_DIAMETER).toReal();

    lines << "Calibration Report. Mode: " + mode;
    lines << "Validation Threshold " + QString::number(validationApproveThreshold)
             + " %. Required Number of Passing Targets: " + QString::number(validationPointsToPassForAcceptedValidation);

    qint32 leftEyePass = 0;
    qint32 rightEyePass = 0;

    QVariantMap rightEyeData;
    QVariantMap leftEyeData;

    rightEyeData["is_validated"] = false;
    leftEyeData["is_validated"] = false;

    QList< QList<EyeTrackerData> > validationData = coeffs.getFittedData();
    QList< qreal > rightHits = coeffs.getHitPercentInTarget(diameterFor2D,validationPointHitTolerance,false);
    QList< qreal > leftHits  = coeffs.getHitPercentInTarget(diameterFor2D,validationPointHitTolerance,true);

    QVariantMap percents;
    QVariantList percent_right;
    QVariantList percent_left;

    for (qsizetype i = 0; i < validationData.size(); i++){

        //qDebug() << "Validation data at" << i << " of " << validationData.size();

        qreal pl = leftHits.at(i);
        qreal pr = rightHits.at(i);

        QVariantList rightDataList;
        QVariantList leftDataList;

        for (qsizetype j = 0; j < validationData.at(i).size(); j++){
            QVariantMap left;
            QVariantMap right;

            left["x"] = validationData.at(i).at(j).xl(); left["y"] = validationData.at(i).at(j).yl();
            right["x"] = validationData.at(i).at(j).xr(); right["y"] = validationData.at(i).at(j).yr();

            if (configuredFor3D){
                left["z"] = validationData.at(i).at(j).zl();
                right["z"] = validationData.at(i).at(j).zr();
            }

            rightDataList << right;
            leftDataList << left;
        }

        rightEyeData["validation_target_" + QString::number(i)] = rightDataList;
        leftEyeData["validation_target_" + QString::number(i)] = leftDataList;

        // Stroing the percent for the target point and if it passed or not. However this is only truly used in 3D Mode.
        QVariantMap mpl, mpr;
        mpl["p"] = pl;
        mpr["p"] = pr;

        if (pl >= validationApproveThreshold){
            leftEyePass++;
            mpl["ok"] = true;
        }
        else mpl["ok"] = false;

        if (pr >= validationApproveThreshold){
            rightEyePass++;
            mpr["ok"] = true;
        }
        else mpr["ok"] = false;

        percent_left << mpl;
        percent_right << mpr;

        lines << " Validation Target " + QString::number(i) +  ". LEFT EYE: "
                 + QString::number(leftHits.at(i)) + " out of " + QString::number(validationData.at(i).size()) + ": " + QString::number(pl,'f',1) + " %"
                 + ". RIGHT EYE: "
                 + QString::number(rightHits.at(i)) + " out of " + QString::number(validationData.at(i).size()) + ": " + QString::number(pr,'f',1) + " %";

    }

    bool wasCalibrationSuccessFull = false;

    if (leftEyePass >= validationPointsToPassForAcceptedValidation) {
        leftEyeData["is_validated"] = true;
        lines << "LEFT EYE VALIDATED";
        wasCalibrationSuccessFull = wasCalibrationSuccessFull || true;
    }
    else{
        lines << "LEFT EYE VALIDATION FAILED";
    }

    if (rightEyePass >= validationPointsToPassForAcceptedValidation) {
        rightEyeData["is_validated"] = true;
        lines << "RIGHT EYE VALIDATED";
        wasCalibrationSuccessFull = wasCalibrationSuccessFull || true;
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

    if (configuredFor3D){
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

    calibrationValidationData[VMDC::CalibrationFields::LEFT_EYE_VALIDATION_DATA]         = leftEyeData;
    calibrationValidationData[VMDC::CalibrationFields::RIGHT_EYE_VALIDATION_DATA]        = rightEyeData;
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_DATA_USE_START_INDEX] = coeffs.getCutoffIndexesListAsVariantList();

    calibrationValidationData[VMDC::CalibrationFields::MATH_ISSUES_FOR_CALIBRATION]      = !coeffs.getResultOfLastComputation();
    calibrationValidationData[VMDC::CalibrationFields::COFICIENT_OF_DETERMINATION]       = Rreport;
    calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_PTS_NO_DATA]          = coeffs.getCalibrationPointsWithNoData();

    // Storing the coefficients. This works wheter it is a 2D calibration or a 3D one.
    QVariantMap cc;
    cc["left"] = coeffs.getCalibrationControlPacketCompatibleMap(true);
    cc["right"] = coeffs.getCalibrationControlPacketCompatibleMap(false);

    calibrationValidationData[VMDC::CalibrationFields::CORRECTION_COEFICIENTS]      = cc;

    if (configuredFor3D){
        percents["l"] = percent_left;
        percents["r"] = percent_right;
        calibrationValidationData[VMDC::CalibrationFields::CALIBRATION_TARGET_PERCENTS]     = percents;
    }

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

    return wasCalibrationSuccessFull;

}

void CalibrationValidation::setDataFromString(const QString &json){
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(),&json_error);
    if (doc.isNull()){
        StaticThreadLogger::error("CalibrationValidation::setDataFromString","Error parsing the JSON string of the study description: " + json_error.errorString());
        return;
    }
    calibrationValidationData = doc.object().toVariantMap();
}
