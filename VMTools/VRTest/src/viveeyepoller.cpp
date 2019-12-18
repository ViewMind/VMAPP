#include "viveeyepoller.h"

VIVEEyePoller::VIVEEyePoller()
{
    keepGoing = true;
    isCalibrating = false;
    resetEyeCorrectionCoeffs();
}

bool VIVEEyePoller::initalizeEyeTracking(){
    int error = ViveSR::anipal::Initial(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE, nullptr);
    if (error == ViveSR::Error::WORK) {
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

    bool areValid;
    qreal xr = 0.0, xl = 0.0, yr = 0.0, yl = 0.0;
    qreal pl = 0.0, pr = 0.0;
    timestampTimer.start();

    while (keepGoing) {
        int result = ViveSR::anipal::Eye::GetEyeData(&eye_data);
        if (result == ViveSR::Error::WORK) {
            mtimer.start();
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
                previous = now;
                continue;
            }
            previous = now;

            QVector4D vecL(le.gaze_direction_normalized.x,le.gaze_direction_normalized.y,le.gaze_direction_normalized.z,0);
            QVector4D vecR(re.gaze_direction_normalized.x,re.gaze_direction_normalized.y,re.gaze_direction_normalized.z,0);

            pr = static_cast<qreal>(re.pupil_diameter_mm);
            pl = static_cast<qreal>(le.pupil_diameter_mm);

            vecL = pLe*vecL;
            vecR = pRe*vecR;

            areValid = (vecL.z() != 0.0f) && (vecR.z() != 0.0f);
            if (areValid){
                xl = static_cast<qreal>(vecL.x())/static_cast<qreal>(vecL.z());
                xr = static_cast<qreal>(vecR.x())/static_cast<qreal>(vecR.z());
                yl = static_cast<qreal>(vecL.y())/static_cast<qreal>(vecL.z());
                yr = static_cast<qreal>(vecR.y())/static_cast<qreal>(vecR.z());
            }

            if (isCalibrating && shouldStoreCalibrationData && areValid ){
                currentCalibrationData.xl << xl;
                currentCalibrationData.xr << xr;
                currentCalibrationData.yl << yl;
                currentCalibrationData.yr << yr;
            }

            if (areValid){
                updateEyeTrackerData(xr,yr,xl,yl,pl,pr,timestampTimer.elapsed());
                if (isCalibrated) {
                    //qDebug() << "Sending signal";
                    emit(newEyeData(lastData));
                }
            }
            else{
            }

        }
    }
}

void VIVEEyePoller::updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l){
    pRe = r;
    pLe = l;
}

void VIVEEyePoller::updateEyeTrackerData(qreal xr, qreal yr, qreal xl, qreal yl, qreal pl, qreal pr, qint64 timestamp){
    lastData.time = timestamp;
    lastData.xLeft = static_cast<qint32>(eyeCorrectionCoeffs.xl.m*xl + eyeCorrectionCoeffs.xl.b);
    lastData.xRight = static_cast<qint32>(eyeCorrectionCoeffs.xr.m*xr + eyeCorrectionCoeffs.xr.b);
    lastData.yLeft = static_cast<qint32>(eyeCorrectionCoeffs.yl.m*yl + eyeCorrectionCoeffs.yl.b);
    lastData.yRight = static_cast<qint32>(eyeCorrectionCoeffs.yr.m*yr + eyeCorrectionCoeffs.yr.b);
    lastData.pdLeft = pl;
    lastData.pdRight = pr;
}

void VIVEEyePoller::resetEyeCorrectionCoeffs(){
    eyeCorrectionCoeffs.xl.b = 0;
    eyeCorrectionCoeffs.xl.m = 1;
    eyeCorrectionCoeffs.xr.b = 0;
    eyeCorrectionCoeffs.xr.m = 1;
    eyeCorrectionCoeffs.yl.b = 0;
    eyeCorrectionCoeffs.yl.m = 1;
    eyeCorrectionCoeffs.yr.b = 0;
    eyeCorrectionCoeffs.yr.m = 1;
    isCalibrated = false;
}

bool VIVEEyePoller::calibrationDone(){
    isCalibrating = false;
    calibrationPointData << currentCalibrationData;
    CalibrationLeastSquares cls;
    if (!cls.computeCalibrationCoeffs(calibrationPointData)){
        return false;
    }
    eyeCorrectionCoeffs = cls.getCalculatedCoeficients();

    qDebug() << "Printing calibration coefficients";
    qDebug() << eyeCorrectionCoeffs.xr.m << eyeCorrectionCoeffs.xr.b;
    qDebug() << eyeCorrectionCoeffs.xl.m << eyeCorrectionCoeffs.xl.b;
    qDebug() << eyeCorrectionCoeffs.yr.m << eyeCorrectionCoeffs.yr.b;
    qDebug() << eyeCorrectionCoeffs.yl.m << eyeCorrectionCoeffs.yl.b;

    isCalibrated = true;

    return true;

    //    logger.setLogFileLocation("calibration_data");

    //    for (qint32 i = 0; i < calibrationPointData.size(); i++){
    //        logger.appendStandard("#POINT " + QString::number(i) + "\n");
    //        QString temp = calibrationPointData.at(i).toString();
    //        logger.appendStandard(temp);
    //    }

}

void VIVEEyePoller::newCalibrationPoint(qint32 xtarget, qint32 ytarget){
    if (!isCalibrating){
        isCalibrating = true;
        resetEyeCorrectionCoeffs();
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
