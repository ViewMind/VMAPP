#include "calibrationvalidation.h"

CalibrationValidation::CalibrationValidation()
{
    recommendedEye = VMDC::Eye::BOTH;
}


QVariantMap CalibrationValidation::getCalibrationAttemptData() const {
    return calibrationAttempt;
}

QVariantMap CalibrationValidation::getCalibrationConfigurationData() const {
    return calibrationConfiguration;
}

QString CalibrationValidation::getRecommendedEye() const {
    return recommendedEye;
}


void CalibrationValidation::configureValidation(const QVariantMap &calibrationValidationParameters,
                                                const QList<QVector3D> &nonNormalizedTargetVecs,
                                                qreal validationRadious,
                                                const QVariantList &calibrationTargetCornersFor2D,
                                                qreal calibrationDiameterFor2D,
                                                bool is3DMode){

    calibrationAttempt.clear();
    calibrationConfiguration.clear();

    validationApproveThreshold                  = calibrationValidationParameters.value(VMDC::CalibrationAttemptFields::VALIDATION_POINT_ACCEPTANCE_THRESHOLD).toReal();
    validationPointHitTolerance                 = calibrationValidationParameters.value(VMDC::CalibrationConfigurationFields::VALIDATION_POINT_HIT_TOLERANCE).toReal();
    validationPointsToPassForAcceptedValidation = calibrationValidationParameters.value(VMDC::CalibrationConfigurationFields::REQ_NUMBER_OF_ACCEPTED_POINTS).toInt();

    // Setting up the configuration. This will actually be overwritten with each attempt. and the values never change.
    // But in order to keep the function interfaces with previous versions we simply recreate it, every single time.
    calibrationConfiguration[VMDC::CalibrationConfigurationFields::CALIBRATION_NON_NORM_VECS]      = QVariantList();
    calibrationConfiguration[VMDC::CalibrationConfigurationFields::CALIBRATION_TARGET_DIAMETER]    = calibrationDiameterFor2D;
    calibrationConfiguration[VMDC::CalibrationConfigurationFields::CALIBRATION_TARGET_LOCATION]    = calibrationTargetCornersFor2D;
    calibrationConfiguration[VMDC::CalibrationConfigurationFields::CALIBRATION_VALIDATION_R]       = 0;
    calibrationConfiguration[VMDC::CalibrationConfigurationFields::NUMBER_OF_CALIBRAION_POINTS]    = 0;
    calibrationConfiguration[VMDC::CalibrationConfigurationFields::REQ_NUMBER_OF_ACCEPTED_POINTS]  = validationPointsToPassForAcceptedValidation;
    calibrationConfiguration[VMDC::CalibrationConfigurationFields::VALIDATION_POINT_HIT_TOLERANCE] = validationPointHitTolerance;

    // Creating the new attempt.
    calibrationAttempt[VMDC::CalibrationAttemptFields::CALIBRATION_DATA_USE_START_INDEX]           = QVariantList();
    calibrationAttempt[VMDC::CalibrationAttemptFields::CALIBRATION_POINT_GATHERTIME]               = calibrationValidationParameters[VMDC::CalibrationAttemptFields::CALIBRATION_POINT_GATHERTIME];
    calibrationAttempt[VMDC::CalibrationAttemptFields::CALIBRATION_POINT_WAITTIME]                 = calibrationValidationParameters[VMDC::CalibrationAttemptFields::CALIBRATION_POINT_WAITTIME];
    calibrationAttempt[VMDC::CalibrationAttemptFields::CALIBRATION_PTS_NO_DATA]                    = QVariantList();
    calibrationAttempt[VMDC::CalibrationAttemptFields::CALIBRATION_TARGET_PERCENTS]                = QVariantList();
    calibrationAttempt[VMDC::CalibrationAttemptFields::COFICIENT_OF_DETERMINATION]                 = QVariantList();
    calibrationAttempt[VMDC::CalibrationAttemptFields::CORRECTION_COEFICIENTS]                     = QVariantList();
    calibrationAttempt[VMDC::CalibrationAttemptFields::LEFT_EYE_DATA]                              = QVariantList();
    calibrationAttempt[VMDC::CalibrationAttemptFields::RIGHT_EYE_DATA]                             = QVariantList();
    calibrationAttempt[VMDC::CalibrationAttemptFields::MATH_ISSUES_FOR_CALIBRATION]                = false;
    calibrationAttempt[VMDC::CalibrationAttemptFields::SUCCESSFUL]                                 = false;
    calibrationAttempt[VMDC::CalibrationAttemptFields::VALIDATION_POINT_ACCEPTANCE_THRESHOLD]      = validationApproveThreshold;
    calibrationAttempt[VMDC::CalibrationAttemptFields::TIMESTAMP]                                  = "";
    calibrationAttempt[VMDC::CalibrationAttemptFields::IS_3D]                                      = false;

    recommendedEye = VMDC::Eye::BOTH;

    if (!is3DMode){
        configuredFor3D = false;
        calibrationConfiguration[VMDC::CalibrationConfigurationFields::NUMBER_OF_CALIBRAION_POINTS] = calibrationTargetCornersFor2D.size();
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

        calibrationConfiguration[VMDC::CalibrationConfigurationFields::CALIBRATION_NON_NORM_VECS] = tvecs;
        calibrationConfiguration[VMDC::CalibrationConfigurationFields::CALIBRATION_VALIDATION_R] = validationRadious;
        calibrationConfiguration[VMDC::CalibrationConfigurationFields::NUMBER_OF_CALIBRAION_POINTS] = tvecs.size();
    }

}

QString CalibrationValidation::getValidationReport() const{
    return validationReport;
}

bool CalibrationValidation::generateCalibrationReport(const EyeCorrectionCoefficients &coeffs){

    QStringList lines;

    qDebug() << "Generating Calibration Report. Is Mode 3D" << configuredFor3D;

    QString mode = "2D";
    if (configuredFor3D) mode = "3D";

    qreal diameterFor2D = calibrationConfiguration.value(VMDC::CalibrationConfigurationFields::CALIBRATION_TARGET_DIAMETER).toReal();

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

        qDebug() << "Validation data at" << i << " of " << validationData.size();

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

    calibrationAttempt[VMDC::CalibrationAttemptFields::LEFT_EYE_DATA]                    = leftEyeData;
    calibrationAttempt[VMDC::CalibrationAttemptFields::RIGHT_EYE_DATA]                   = rightEyeData;
    calibrationAttempt[VMDC::CalibrationAttemptFields::CALIBRATION_DATA_USE_START_INDEX] = coeffs.getCutoffIndexesListAsVariantList();
    calibrationAttempt[VMDC::CalibrationAttemptFields::MATH_ISSUES_FOR_CALIBRATION]      = !coeffs.getResultOfLastComputation();
    calibrationAttempt[VMDC::CalibrationAttemptFields::COFICIENT_OF_DETERMINATION]       = Rreport;
    calibrationAttempt[VMDC::CalibrationAttemptFields::CALIBRATION_PTS_NO_DATA]          = coeffs.getCalibrationPointsWithNoData();
    calibrationAttempt[VMDC::CalibrationAttemptFields::IS_3D]                            = configuredFor3D;

    // Storing the coefficients. This works wheter it is a 2D calibration or a 3D one.
    QVariantMap cc;
    cc["left"] = coeffs.getCalibrationControlPacketCompatibleMap(true);
    cc["right"] = coeffs.getCalibrationControlPacketCompatibleMap(false);

    calibrationAttempt[VMDC::CalibrationAttemptFields::CORRECTION_COEFICIENTS]          = cc;

    //if (configuredFor3D){
    percents["l"] = percent_left;
    percents["r"] = percent_right;
    calibrationAttempt[VMDC::CalibrationAttemptFields::CALIBRATION_TARGET_PERCENTS]     = percents;
    //}

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

    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString hour = QDateTime::currentDateTime().toString("HH:mm:ss");

    calibrationAttempt[VMDC::CalibrationAttemptFields::SUCCESSFUL] = wasCalibrationSuccessFull;
    calibrationAttempt[VMDC::CalibrationAttemptFields::TIMESTAMP] = date + " " + hour;

    return wasCalibrationSuccessFull;

}

// This is strictly a debugging function. So no checkes of any kind are done.
void CalibrationValidation::saveToJSONFile(const QString &filename){

    QVariantMap map;
    map[VMDC::CalibrationFields::CALIBRATION_ATTEMPTS] = calibrationAttempt;
    map[VMDC::CalibrationFields::CONFIG_PARAMS] = calibrationConfiguration;
    QJsonDocument json = QJsonDocument::fromVariant(map);
    QByteArray data  = json.toJson(QJsonDocument::Indented);

    QFile file(filename);
    file.open(QFile::WriteOnly);
    QTextStream writer(&file);

    writer << QString(data);

    file.close();

}

void CalibrationValidation::setDataFromString(const QString &json){
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(),&json_error);
    if (doc.isNull()){
        StaticThreadLogger::error("CalibrationValidation::setDataFromString","Error parsing the JSON string of the study description: " + json_error.errorString());
        return;
    }
    QVariantMap map = doc.object().toVariantMap();

    // Debug::prettpPrintQVariantMap(map);

    calibrationAttempt = map.value(VMDC::CalibrationFields::CALIBRATION_ATTEMPTS).toList().first().toMap();
    calibrationConfiguration = map.value(VMDC::CalibrationFields::CONFIG_PARAMS).toMap();
}
