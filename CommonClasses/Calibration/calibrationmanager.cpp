#include "calibrationmanager.h"

CalibrationManager::CalibrationManager(QObject *parent) : QObject(parent) {


}

////////////////////// Configure and Starte Calibration //////////////////
void CalibrationManager::startCalibration(bool mode3D,
                                          const QVariantMap &calib_valid_params,
                                          const QString coefficient_file_name){

    calibrationMode3D         = mode3D;
    coefficientFileName       = coefficient_file_name;
    calibration_wait_time     = calib_valid_params[VMDC::CalibrationAttemptFields::CALIBRATION_POINT_WAITTIME].toInt();
    calibration_gather_time   = calib_valid_params[VMDC::CalibrationAttemptFields::CALIBRATION_POINT_GATHERTIME].toInt();
    numberOfCalibrationPoints = calib_valid_params[VMDC::CalibrationConfigurationFields::NUMBER_OF_CALIBRAION_POINTS].toInt();
    calibrationValidationData = calib_valid_params;

    isCalibrated = false;

    if (DBUGBOOL(Debug::Options::LOAD_PREFIX_CALIB)){
        if (mode3D){            
            if (debugLoadFixed3DCalibrationParameters()) return;
        }
        else {
            if (debugLoadFixed2DCalibrationParameters()) return;
        }
    }

    renderServerPacket.resetForRX();
    renderServerPacket.setPacketType(RRS::PacketType::TYPE_CALIB_CONTROL);
    renderServerPacket.setPayloadField(CalibrationControlPacketFields::GATHER_TIME,calibration_gather_time);
    renderServerPacket.setPayloadField(CalibrationControlPacketFields::MOVE_TIME,calibration_wait_time);
    renderServerPacket.setPayloadField(CalibrationControlPacketFields::N_CALIB_POINTS,numberOfCalibrationPoints);

    // If we got here, this is a normal calibration.
    if (calibrationMode3D){
        // We need to send a packet to the render server to start the 3D Calibration.
        renderServerPacket.setPayloadField(RRS::PacketCalibrationControl::COMMAND,RRS::CommandCalibrationControls::CMD_START_3D);
    }
    else {
        // We need to send a packet to the render server to start the 3D Calibration.
        renderServerPacket.setPayloadField(RRS::PacketCalibrationControl::COMMAND,RRS::CommandCalibrationControls::CMD_START_2D);
    }   

    emit CalibrationManager::newPacketAvailable();

}

////////////////////// Interface Functions //////////////////
RenderServerPacket CalibrationManager::getRenderServerPacket() const {
    return renderServerPacket;
}

QVariantMap CalibrationManager::getCalibrationAttemptData() const {
    return calibrationValidation.getCalibrationAttemptData();
}

QVariantMap CalibrationManager::getCalibrationConfigurationParameters() const {
    return calibrationValidation.getCalibrationConfigurationData();
}

QString CalibrationManager::getRecommendedEye() const {
    return calibrationValidation.getRecommendedEye();
}

QString CalibrationManager::getCalibrationValidationReport() const {
    return calibrationValidation.getValidationReport();
}

bool CalibrationManager::isCalibrationCompleted() const{
    return isCalibrated;
}

void CalibrationManager::resetCalibrationCompleted(){
    isCalibrated = false;
}

QSize CalibrationManager::getResolution() const{
    return QSize(resolutionWidth,resolutionHeight);
}

////////////////////// Add calibration data to target and finalize the calibration process //////////////////
void CalibrationManager::addEyeDataToCalibrationPoint(float xl, float xr, float yl, float yr, float zl, float zr, qint32 index){
    EyeTrackerData eid;
    eid.setXL(static_cast<qreal>(xl));
    eid.setXR(static_cast<qreal>(xr));
    eid.setYL(static_cast<qreal>(yl));
    eid.setYR(static_cast<qreal>(yr));
    eid.setZL(static_cast<qreal>(zl));
    eid.setZR(static_cast<qreal>(zr));
    correctionCoefficients.addPointForCoefficientComputation(eid,index);
}

void CalibrationManager::finalizeCalibrationProcess(qint32 code){

    // We compute the corrections.
    if (!correctionCoefficients.computeCoefficients()){
        StaticThreadLogger::error("CalibrationManager::finalizeCalibrationProcess","Failed in computing the correction coefficients. Reason: " + correctionCoefficients.getLastError());
        code = CALIBRATION_FAILED;    
    }

    // We need to compute the target diameter.
    qreal targetDiameter = resolutionWidth*K_LARGE_D;

    // And we need to compute the target "corners" of the square which contains each fo the 2D targets.
    QVariantList list;
    // Index starts at 1 as the first target point is the dummy center.
    for (qint32 i = 1; i < targetPoints2D.size(); i++){
        QVariantMap point;
        QPointF p = targetPoints2D.at(i);
        point["x"] = p.x();
        point["y"] = p.y();
        list << point;
    }

    // We now configure everthing to get the calibration report.
    calibrationValidation.configureValidation(calibrationValidationData,
                                              nonNormalizedTargetVectors,
                                              validationRadious,
                                              list,
                                              targetDiameter,
                                              calibrationMode3D);

    // And we actually generate it.
    bool wasSuccessfull = calibrationValidation.generateCalibrationReport(correctionCoefficients);

    if (wasSuccessfull){
        // We now send the coefficients back to the remote render server so they can be used.
        sendCalibrationCoefficientPacket();
        isCalibrated = true;
    }

    // We save the coefficient file, only if the calibration is successful.
    if (code == CALIBRATION_SUCCESSFUL){
        if (coefficientFileName != ""){
            correctionCoefficients.saveCalibrationCoefficients(coefficientFileName);
        }
    }

    emit CalibrationManager::calibrationDone(code);
}

void CalibrationManager::processCalibrationData(const RenderServerPacket &calibrationData){

    nonNormalizedTargetVectors.clear();
    targetPoints2D.clear();
    validationRadious = calibrationData.getPayloadField(RRS::PacketCalibrationControl::VALIDATION_R).toReal();

    // Storing the resolution
    resolutionHeight = calibrationData.getPayloadField(RRS::PacketCalibrationControl::RES_H).toInt();
    resolutionWidth  = calibrationData.getPayloadField(RRS::PacketCalibrationControl::RES_W).toInt();

    StaticThreadLogger::log("CalibrationManager::process3DCalibrationEyeTrackingData","Setting the validation radious to " + QString::number(validationRadious));

    QVariantList tx = calibrationData.getPayloadField(RRS::PacketCalibrationControl::CALIBRATION_TARGETS_X).toList();
    QVariantList ty = calibrationData.getPayloadField(RRS::PacketCalibrationControl::CALIBRATION_TARGETS_Y).toList();
    QVariantList tz = calibrationData.getPayloadField(RRS::PacketCalibrationControl::CALIBRATION_TARGETS_Z).toList();

    // Creating the target vectors.
    qsizetype N = tx.size();

    QVariantList start_index_list = calibrationData.getPayloadField(RRS::PacketCalibrationControl::CALIB_DATA_START_IND).toList();

    if (start_index_list.size() != N){
        StaticThreadLogger::error("CalibrationManager::processCalibrationData","The number start indexes for calibration (" + QString::number(start_index_list.size()) + ") differs from the number of points " + QString::number(N));
        start_index_list.clear();
        for (qint32 i = 0; i < N; i++){
            start_index_list << 0;
        }
    }

    // List starts at 1 as the first target is the middle dummy target.
    for (qsizetype i = 1; i < N; i++){        
        if (calibrationMode3D){
            QVector3D vec;
            vec.setX(tx.at(i).toFloat());
            vec.setY(ty.at(i).toFloat());
            vec.setZ(tz.at(i).toFloat());
            nonNormalizedTargetVectors << vec;
        }
        else {
            QVector2D vec;
            vec.setX(tx.at(i).toFloat());
            vec.setY(ty.at(i).toFloat());
        }
    }

    // Configuring the coefficient correction computation for 3D.
    if (calibrationMode3D){
        correctionCoefficients.configureFor3DCoefficientComputation(nonNormalizedTargetVectors,validationRadious);
    }
    else {
        correctionCoefficients.configureFor2DCoefficientComputation(targetPoints2D);
    }


    // Now we add all the data.
    QVariantList rx = calibrationData.getPayloadField(RRS::PacketCalibrationControl::CALIB_DATA_RX).toList();
    QVariantList ry = calibrationData.getPayloadField(RRS::PacketCalibrationControl::CALIB_DATA_RY).toList();
    QVariantList rz = calibrationData.getPayloadField(RRS::PacketCalibrationControl::CALIB_DATA_RZ).toList();
    QVariantList lx = calibrationData.getPayloadField(RRS::PacketCalibrationControl::CALIB_DATA_LX).toList();
    QVariantList ly = calibrationData.getPayloadField(RRS::PacketCalibrationControl::CALIB_DATA_LY).toList();
    QVariantList lz = calibrationData.getPayloadField(RRS::PacketCalibrationControl::CALIB_DATA_LZ).toList();

    for (qsizetype i = 1; i < N; i++){

        QVariantList cdxr = rx.value(i).toList();
        QVariantList cdxl = lx.value(i).toList();
        QVariantList cdyr = ry.value(i).toList();
        QVariantList cdyl = ly.value(i).toList();
        QVariantList cdzr = rz.value(i).toList();
        QVariantList cdzl = lz.value(i).toList();

        qsizetype M = cdxr.size(); // They should all be the same size.
        qint32 currentCalibrationPointIndex = static_cast<qint32>(i)-1;

        qint32 start_index = start_index_list.value(i).toInt();
        if (start_index == -1){
            StaticThreadLogger::error("CalibrationManager::process3DCalibrationEyeTrackingData","The calibration start index for calibration point  " + QString::number(i) + " has not been set");
            start_index = 0;
        }
        else if (start_index >= (M-1)) {
            StaticThreadLogger::error("CalibrationManager::process3DCalibrationEyeTrackingData","The calibration start index for calibration point  " + QString::number(i) + " has been set to "
                                      + QString::number(start_index) + " however the number of data points is " + QString::number(M));
            start_index = 0;
        }

        //qDebug() << "DATA FOR TARGET VECTOR " << nonNormalizedTargetVectors.at(currentCalibrationPointIndex);

        // Adding ieach of the sample to the coefficients using the convenience funtion.
        for (qsizetype j = 0; j < M; j++){

            EyeTrackerData eid;
            eid.setXL(cdxl.at(j).toReal());
            eid.setXR(cdxr.at(j).toReal());
            eid.setYL(cdyl.at(j).toReal());
            eid.setYR(cdyl.at(j).toReal());
            eid.setZL(cdzl.at(j).toReal());
            eid.setZR(cdzl.at(j).toReal());

            correctionCoefficients.addPointForCoefficientComputation(eid,currentCalibrationPointIndex);

            if (j == start_index){
                // Setting the cuttoff index for this target.
                correctionCoefficients.setStartPointForValidCalibrationRawData(currentCalibrationPointIndex);
            }
        }

    }

    // Now that the data we can finalize the calibration process.
    finalizeCalibrationProcess(CALIBRATION_SUCCESSFUL);

}

void CalibrationManager::sendCalibrationCoefficientPacket(){
    renderServerPacket.resetForRX();
    renderServerPacket.setPacketType(RRS::PacketType::TYPE_CALIB_CONTROL);
    renderServerPacket.setPayloadField(RRS::PacketCalibrationControl::COMMAND,RRS::CommandCalibrationControls::CMD_COEFFICIENTS);
    renderServerPacket.setPayloadField(RRS::PacketCalibrationControl::COEFFS_L,correctionCoefficients.getCalibrationControlPacketCompatibleMap(true));
    renderServerPacket.setPayloadField(RRS::PacketCalibrationControl::COEFFS_R,correctionCoefficients.getCalibrationControlPacketCompatibleMap(false));
    emit CalibrationManager::newPacketAvailable();
}

////////////////////// DEBUG LOAD FUNCTIONS //////////////////
void CalibrationManager::debugSaveCalibrationValidationData(const QString &filename){
    calibrationValidation.saveToJSONFile(filename);
}

bool CalibrationManager::debugLoadFixed3DCalibrationParameters() {

    QFile file(":/debug_files/3dCalibData.json");
    if (!file.open(QFile::ReadOnly)){
        qDebug() << "Failed in loading DEBUG 3D File";
        return false;
    }
    QTextStream reader(&file);
    QString fixed_dat = reader.readAll();
    file.close();

    calibrationValidation.setDataFromString(fixed_dat);

    QString matrix = "0.8676431,0.0202019159, -0.00122752122,-0.06256262\n\
-0.006712373,0.996084332,-0.171135783,0.132956\n\
0.08176654,-0.0281286743,0.822351158,0.170508042\n\
0.9300998,-0.008620117,-0.238664374,0.224052832\n\
0.008516804,1.01506889,-0.08259078,0.0297427\n\
-0.003751203,0.003546328,0.9096183,0.08610141";

    if (!correctionCoefficients.loadFromTextMatrix(matrix)) return false;

    // Sending the calibration packet.
    sendCalibrationCoefficientPacket();

    emit CalibrationManager::calibrationDone(CALIBRATION_SUCCESSFUL);

    return true;
}

bool CalibrationManager::debugLoadFixed2DCalibrationParameters(){
    QFile file(":/debug_files/2dCalibData.json");
    if (!file.open(QFile::ReadOnly)){
        qDebug() << "Failed in loading DEBUG 2D File";
        return false;
    }
    QTextStream reader(&file);
    QString fixed_dat = reader.readAll();
    file.close();

    //qDebug() << "Setting data";

    calibrationValidation.setDataFromString(fixed_dat);

    QString matrix = "-1921.2350476661336,1103.633319863327\n\
    -2032.2447889194718,1279.4145830880336\n\
    -2063.753443167157,1245.5719666971615\n\
    -2095.759211028236,1337.245378319595";

    //qDebug() << "Loading from matrix 2D Parameters";

    if (!correctionCoefficients.loadFromTextMatrix(matrix)) return false;

    //qDebug() << "Emitting calibration successfull";

    emit CalibrationManager::calibrationDone(CALIBRATION_SUCCESSFUL);

    return true;

}
