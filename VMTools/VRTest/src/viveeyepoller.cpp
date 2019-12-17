#include "viveeyepoller.h"

VIVEEyePoller::VIVEEyePoller()
{
    keepGoing = true;
    isCalibrating = false;
}

bool VIVEEyePoller::initalizeEyeTracking(){
    int error = ViveSR::anipal::Initial(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE, nullptr);
    if (error == ViveSR::Error::WORK) {
        //setMaxWidthAndHeight(w,h);
        return true;
    }
    else if (error == ViveSR::Error::RUNTIME_NOT_FOUND) {
        qDebug() << "Eye Tracking Runtime Not Found";
        return false;
    }
    else {
        qDebug() << "Failed to initialize Eye engine. please refer the code " << error << " of ViveSR::Error";
        return false;
    }
}

void VIVEEyePoller::run(){
    ViveSR::anipal::Eye::EyeData eye_data;
    previous.init(4,-2);
    while (keepGoing) {
        int result = ViveSR::anipal::Eye::GetEyeData(&eye_data);
        if (result == ViveSR::Error::WORK) {
            ViveSR::anipal::Eye::SingleEyeData le = eye_data.verbose_data.left;
            ViveSR::anipal::Eye::SingleEyeData re = eye_data.verbose_data.right;

            //            qDebug() << "LEFT validity " << le.eye_data_validata_bit_mask << " nomalized gaze dir "
            //                     << le.gaze_direction_normalized.x
            //                     << le.gaze_direction_normalized.y
            //                     << le.gaze_direction_normalized.z
            //                     << "PS: " << le.pupil_diameter_mm
            //                     << "RIGHT validity " << re.eye_data_validata_bit_mask << " nomalized gaze dir "
            //                     << re.gaze_direction_normalized.x
            //                     << re.gaze_direction_normalized.y
            //                     << re.gaze_direction_normalized.z
            //                     << "PS: " << le.pupil_diameter_mm;

            ManyPointCompare now;
            now.values << le.gaze_direction_normalized.x << le.gaze_direction_normalized.y << re.gaze_direction_normalized.x << re.gaze_direction_normalized.y;
            if (now.areTheSame(previous)) {
                qDebug() << "Skipping";
                previous = now;
                continue;
            }
            previous = now;

            QVector4D vecL(le.gaze_direction_normalized.x,le.gaze_direction_normalized.y,le.gaze_direction_normalized.z,0);
            QVector4D vecR(re.gaze_direction_normalized.x,re.gaze_direction_normalized.y,re.gaze_direction_normalized.z,0);

            vecL = pLe*vecL;
            vecR = pRe*vecR;

            qreal scaleFactor = 1;

            //qDebug() << isCalibrating << shouldStoreCalibrationData;
            if (isCalibrating && shouldStoreCalibrationData && (vecL.z() != 0.0f) && (vecR.z() != 0.0f) ){
                currentCalibrationData.xl << static_cast<qreal>(vecL.x())*scaleFactor/static_cast<qreal>(vecL.z());
                currentCalibrationData.xr << static_cast<qreal>(vecR.x())*scaleFactor/static_cast<qreal>(vecR.z());
                currentCalibrationData.yl << static_cast<qreal>(vecL.y())*scaleFactor/static_cast<qreal>(vecL.z());
                currentCalibrationData.yr << static_cast<qreal>(vecR.y())*scaleFactor/static_cast<qreal>(vecR.z());
            }

//            qreal scaleFactor = 2;

//            EyeTrackerData data = computeValues(vecR.x()*scaleFactor/vecR.z(),
//                                                vecR.y()*scaleFactor/vecR.z(),
//                                                vecL.x()*scaleFactor/vecL.z(),
//                                                vecL.y()*scaleFactor/vecL.z());


//            qDebug() << data.toString();

//            lastData = data;
//            emit(newEyeData(data));

        }
    }
}

void VIVEEyePoller::updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l){
    pRe = r;
    pLe = l;
}

void VIVEEyePoller::calibrationDone(){
    isCalibrating = false;
    calibrationPointData << currentCalibrationData;

    logger.setLogFileLocation("calibration_data");

    for (qint32 i = 0; i < calibrationPointData.size(); i++){
        logger.appendStandard("#POINT " + QString::number(i) + "\n");
        QString temp = calibrationPointData.at(i).toString();
        logger.appendStandard(temp);
    }

}

void VIVEEyePoller::newCalibrationPoint(qint32 xtarget, qint32 ytarget){
    if (!isCalibrating){
        isCalibrating = true;
        calibrationPointData.clear();
    }
    else{
        calibrationPointData << currentCalibrationData;
    }
    //qDebug() << "New calibration point" << xtarget << ytarget;
    currentCalibrationData.clear();
    currentCalibrationData.xl << xtarget;
    currentCalibrationData.xr << xtarget;
    currentCalibrationData.yl << ytarget;
    currentCalibrationData.yr << ytarget;
    shouldStoreCalibrationData = false;

}

void VIVEEyePoller::startStoringCalibrationData(){
    shouldStoreCalibrationData = true;
}

void VIVEEyePoller::setProjectionMatrices(quint8 whichEye, QMatrix4x4 p){
    if (whichEye == 0){
        pLe = p;
    }
    else if (whichEye == 1){
        pRe = p;
    }
}

//void VIVEEyePoller::setMaxWidthAndHeight(qreal W, qreal H){
//    MaxX = W;
//    MaxY = H;
//    bX = MaxX/2;
//    mX = -MaxX/2;
//    bY = MaxY/2;
//    mY = -MaxY/2;
//}

//EyeTrackerData VIVEEyePoller::computeValues(qreal xr, qreal yr, qreal xl, qreal yl){
//    EyeTrackerData input;
//    input.xLeft = static_cast<qint32>(mX*xl+ bX);
//    input.xRight = static_cast<qint32>(mX*xr+ bX);
//    input.yLeft = static_cast<qint32>(mY*yl+ bY);
//    input.yRight = static_cast<qint32>(mY*yr+ bY);
//    return input;
//}
