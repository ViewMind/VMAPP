#include "viveeyepoller.h"

VIVEEyePoller::VIVEEyePoller()
{
    keepGoing = false;
    isCalibrating = false;
    resetEyeCorrectionCoeffs();
    wasInitialized = false;
}

bool VIVEEyePoller::initalizeEyeTracking(){

    if (wasInitialized) return true;

    int error = ViveSR::anipal::Initial(ViveSR::anipal::Eye::ANIPAL_TYPE_EYE, nullptr);
    if (error == ViveSR::Error::WORK) {
        wasInitialized = true;
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
    keepGoing = true;
    bool areValid;
    CalibrationLeastSquares::EyeInputData eyeData;
    eyeData.xl = 0; eyeData.xr = 0; eyeData.yl = 0; eyeData.xr = 0;
    qreal pl = 0.0, pr = 0.0;
    timestampTimer.start();

    //qDebug() << "VIVE EYE POLLER ENABLED";

    while (keepGoing) {
        int result = ViveSR::anipal::Eye::GetEyeData(&eye_data);
        if (result == ViveSR::Error::WORK) {
            ViveSR::anipal::Eye::SingleEyeData le = eye_data.verbose_data.left;
            ViveSR::anipal::Eye::SingleEyeData re = eye_data.verbose_data.right;

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

            areValid = (vecL.z() != 0.0f) || (vecR.z() != 0.0f);
            
            if (areValid){
                if (vecL.z() != 0.0f){
                   eyeData.xl = static_cast<qreal>(vecL.x())/static_cast<qreal>(vecL.z());
                   eyeData.yl = static_cast<qreal>(vecL.y())/static_cast<qreal>(vecL.z());
                }
                else{
                   eyeData.xl = 0;
                   eyeData.yl = 0;
                }
                if (vecR.z() != 0.0f){
                   eyeData.xr = static_cast<qreal>(vecR.x())/static_cast<qreal>(vecR.z());                
                   eyeData.yr = static_cast<qreal>(vecR.y())/static_cast<qreal>(vecR.z());
                }
                else{
                   eyeData.xr = 0;
                   eyeData.yr = 0;                 
                }
            }

            if (isCalibrating && shouldStoreCalibrationData && areValid ){
                currentCalibrationData.eyeData << eyeData;
            }

            //qDebug() << "About to emmit" << areValid;

            if (areValid){
                updateEyeTrackerData(eyeData,pl,pr,timestampTimer.elapsed());
                if (isCalibrated) {
                    //qDebug() << "Sending signal";
                    emit(newEyeData(lastData));
                }
            }

        }
    }
}

void VIVEEyePoller::updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l){
    pRe = r;
    pLe = l;
}

void VIVEEyePoller::updateEyeTrackerData(CalibrationLeastSquares::EyeInputData eyeData, qreal pl, qreal pr, qint64 timestamp){
    lastData.time = timestamp;
    eyeData = eyeCorrectionCoeffs.computeCorrections(eyeData);
    //    lastData.xLeft = static_cast<qint32>(eyeCorrectionCoeffs.xl.m*xl + eyeCorrectionCoeffs.xl.b);
    //    lastData.xRight = static_cast<qint32>(eyeCorrectionCoeffs.xr.m*eyeData + eyeCorrectionCoeffs.xr.b);
    //    lastData.yLeft = static_cast<qint32>(eyeCorrectionCoeffs.yl.m*yl + eyeCorrectionCoeffs.yl.b);
    //    lastData.yRight = static_cast<qint32>(eyeCorrectionCoeffs.yr.m*yr + eyeCorrectionCoeffs.yr.b);
    lastData.xLeft = qMax(0,static_cast<qint32>(eyeData.xl));
    lastData.xRight = qMax(0,static_cast<qint32>(eyeData.xr));
    lastData.yLeft = qMax(0,static_cast<qint32>(eyeData.yl));
    lastData.yRight = qMax(0,static_cast<qint32>(eyeData.yr));

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

void VIVEEyePoller::stop(){
    keepGoing = false;
    //qDebug() << "VIVE EYE Poller Was Disabled";
}

void VIVEEyePoller::saveCalibrationCoefficients(const QString &file){
    eyeCorrectionCoeffs.saveCalibrationCoefficients(file);
}

void VIVEEyePoller::loadCalibrationCoefficients(const QString &file){
    // There are no checks here as this will only be used for debugging.
    isCalibrated = true;
    isCalibrating = false;
    eyeCorrectionCoeffs.loadCalibrationCoefficients(file);
    qDebug() << "Loaded calibration coefficients";
    qDebug() << eyeCorrectionCoeffs.xr.m << eyeCorrectionCoeffs.xr.b;
    qDebug() << eyeCorrectionCoeffs.xl.m << eyeCorrectionCoeffs.xl.b;
    qDebug() << eyeCorrectionCoeffs.yr.m << eyeCorrectionCoeffs.yr.b;
    qDebug() << eyeCorrectionCoeffs.yl.m << eyeCorrectionCoeffs.yl.b;
}

void VIVEEyePoller::newCalibrationPoint(qint32 xtarget, qint32 ytarget){
    if (!isCalibrating){
        isCalibrating = true;
        resetEyeCorrectionCoeffs();
        calibrationPointData.clear();
    }
    else{
        //qDebug() << "New calibration point on VIVE EYE Poller Adding calibration data of size" << currentCalibrationData.eyeData.size();
        calibrationPointData << currentCalibrationData;
    }
    //qDebug() << "New calibration point" << xtarget << ytarget;
    currentCalibrationData.clear();
    CalibrationLeastSquares::EyeInputData cpoint;
    cpoint.xl = xtarget; cpoint.xr = xtarget;
    cpoint.yl = ytarget; cpoint.yr = ytarget;
    currentCalibrationData.eyeData << cpoint;
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

