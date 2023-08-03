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
        RenderServerPacket p;
        if (mode3D){            
            p = debugLoadFixed3DCalibrationParameters();
        }
        else {
            p = debugLoadFixed2DCalibrationParameters();
        }
        this->processCalibrationData(p);
        return;
    }

    renderServerPacket.resetForRX();
    renderServerPacket.setPacketType(RRS::PacketType::TYPE_CALIB_CONTROL);
    renderServerPacket.setPayloadField(RRS::PacketCalibrationControl::GATHER_TIME,calibration_gather_time);
    renderServerPacket.setPayloadField(RRS::PacketCalibrationControl::MOVE_TIME,calibration_wait_time);
    renderServerPacket.setPayloadField(RRS::PacketCalibrationControl::N_CALIB_POINTS,numberOfCalibrationPoints);

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

////////////////////// Finalize the calibration process //////////////////

void CalibrationManager::finalizeCalibrationProcess(qint32 code){

    if (code != CALIBRATION_SUCCESSFUL){
        emit CalibrationManager::calibrationDone(code);
        return;
    }

    // We compute the corrections.
    if (!correctionCoefficients.computeCoefficients()){
        StaticThreadLogger::error("CalibrationManager::finalizeCalibrationProcess","Failed in computing the correction coefficients. Reason: " + correctionCoefficients.getLastError());
        code = CALIBRATION_FAILED;    
    }

    // We need to compute the target diameter.
    qreal targetDiameter = resolutionWidth*K_LARGE_D;

    // And we need to compute the target "corners" of the square which contains each fo the 2D targets.
    QVariantList list;
    for (qint32 i = 0; i < targetPoints2D.size(); i++){
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

    //qDebug() << "Printing The Calibration Data";
    //Debug::prettpPrintQVariantMap(calibrationData.getPayload());
//    QFile file("test_calib_data.json");
//    file.open(QFile::WriteOnly);
//    QTextStream writer(&file);
//    writer << Debug::QVariantMapToString(calibrationData.getPayload());
//    file.close();

    nonNormalizedTargetVectors.clear();
    targetPoints2D.clear();
    validationRadious = calibrationData.getPayloadField(RRS::PacketCalibrationControl::VALIDATION_R).toReal();

    // Storing the resolution
    resolutionHeight = calibrationData.getPayloadField(RRS::PacketCalibrationControl::RES_H).toInt();
    resolutionWidth  = calibrationData.getPayloadField(RRS::PacketCalibrationControl::RES_W).toInt();

    StaticThreadLogger::log("CalibrationManager::processCalibrationData","Setting the validation radious to " + QString::number(validationRadious));

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
            QPointF vec;
            vec.setX(tx.at(i).toFloat());
            vec.setY(ty.at(i).toFloat());
            targetPoints2D << vec;
        }
    }

    if (calibrationMode3D){
        // In 3D Mode, if we don't do this, we don't have the 2D target vector and hence can't draw in the UI.
        compute2DTargetLocations();
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
        bool there_was_an_error = false;
        if (start_index == -1){
            StaticThreadLogger::error("CalibrationManager::processCalibrationData","The calibration start index for calibration point  " + QString::number(i) + " has not been set");
            start_index = 0;
            there_was_an_error = true;
        }
        else if (start_index >= (M-1)) {
            StaticThreadLogger::error("CalibrationManager::processCalibrationData","The calibration start index for calibration point  " + QString::number(i) + " has been set to "
                                      + QString::number(start_index) + " however the number of data points is " + QString::number(M));
            start_index = 0;
            there_was_an_error = true;
        }

        if (there_was_an_error){
            finalizeCalibrationProcess(CALIBRATION_FAILED);
            return;
        }

        //qDebug() << "DATA FOR TARGET VECTOR " << nonNormalizedTargetVectors.at(currentCalibrationPointIndex);

        // Adding ieach of the sample to the coefficients using the convenience funtion.
        for (qsizetype j = 0; j < M; j++){

            EyeTrackerData eid;
            eid.setXL(cdxl.at(j).toReal());
            eid.setXR(cdxr.at(j).toReal());
            eid.setYL(cdyl.at(j).toReal());
            eid.setYR(cdyr.at(j).toReal());
            eid.setZL(cdzl.at(j).toReal());
            eid.setZR(cdzr.at(j).toReal());

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

void CalibrationManager::compute2DTargetLocations(){

    qreal Wo = resolutionWidth;
    qreal Ho = resolutionHeight;

    qreal mx = Wo/VIRTUAL_3D_W;
    qreal my = Ho/VIRTUAL_3D_H;
    qreal bx = Wo/2;
    qreal by = Ho/2;

    for (qint32 i = 0; i < nonNormalizedTargetVectors.size(); i++){
        qreal x = nonNormalizedTargetVectors.at(i).x();
        qreal y = nonNormalizedTargetVectors.at(i).y();
        qreal x0 = mx*x + bx;
        qreal y0 = my*y + by;
        y0 = Ho - y0; // We need to do this becuse in monitor coordinates, higher y values are lower on the screen.
        //qDebug() << "3D Values" << x << y << ". 2D Values" << x0 << y0;
        targetPoints2D << QPointF(x0,y0);
    }

}

////////////////////// DEBUG LOAD FUNCTIONS //////////////////
void CalibrationManager::debugSaveCalibrationValidationData(const QString &filename){
    calibrationValidation.saveToJSONFile(filename);
}

RenderServerPacket CalibrationManager::debugLoadFixed3DCalibrationParameters() {

    QString error;
    QVariantMap payload = Globals::LoadJSONFileToVariantMap(":/debug_files/3dCalibData.json",&error);
    RenderServerPacket p;
    p.setPacketType(RRS::PacketType::TYPE_CALIB_CONTROL);
    p.setFullPayload(payload);
    return p;
}

RenderServerPacket CalibrationManager::debugLoadFixed2DCalibrationParameters(){

    QString error;
    QVariantMap payload = Globals::LoadJSONFileToVariantMap(":/debug_files/2dCalibData.json",&error);
    RenderServerPacket p;
    p.setPacketType(RRS::PacketType::TYPE_CALIB_CONTROL);
    p.setFullPayload(payload);
    return p;

}
