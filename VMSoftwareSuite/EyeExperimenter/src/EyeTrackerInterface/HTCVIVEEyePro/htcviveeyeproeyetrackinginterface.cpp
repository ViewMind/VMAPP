#include "htcviveeyeproeyetrackinginterface.h"

HTCViveEyeProEyeTrackingInterface::HTCViveEyeProEyeTrackingInterface(QObject *parent, qreal width, qreal height):EyeTrackerInterface(parent,width,height)
{
    calibrationTargets.initialize(static_cast<qint32>(width),static_cast<qint32>(height));
    connect(&calibrationTimer,&QTimer::timeout,this,&HTCViveEyeProEyeTrackingInterface::onCalibrationTimerTimeout);
    connect(&eyetracker,&VIVEEyePoller::newEyeData,this,&HTCViveEyeProEyeTrackingInterface::onNewData);
}

void HTCViveEyeProEyeTrackingInterface::connectToEyeTracker(){
    if (!eyetracker.initalizeEyeTracking()){
        emit(eyeTrackerControl(ET_CODE_CONNECTION_FAIL));
        return;
    }
    else {
        emit(eyeTrackerControl(ET_CODE_CONNECTION_SUCCESS));
        return;
    }
}

void HTCViveEyeProEyeTrackingInterface::updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l){
    eyetracker.updateProjectionMatrices(r,l);
}

void HTCViveEyeProEyeTrackingInterface::onNewData(EyeTrackerData etd){
    if (!canUseLeft()){
        etd.xLeft = 0;
        etd.yLeft = 0;
    }
    if (!canUseRight()){
        etd.xRight = 0;
        etd.yRight = 0;
    }
    lastData = etd;
    emit(newDataAvailable(etd));
}

void HTCViveEyeProEyeTrackingInterface::onCalibrationTimerTimeout(){
    calibrationTimer.stop();
    if (isWaiting){
        calibrationTimer.start(CALIBRATION_SHOW);
        isWaiting = false;
        eyetracker.startStoringCalibrationData();
    }
    else{
        calibrationPointIndex++;
        if (calibrationPointIndex < calibrationPoints.size()){
            QPoint p = calibrationPoints.at(calibrationPointIndex);
            calibrationImage = calibrationTargets.setSingleTarget(p.x(),p.y());
            isWaiting = true;
            calibrationTimer.start(CALIBRATION_WAIT);
            eyetracker.newCalibrationPoint(p.x(),p.y());
            emit(eyeTrackerControl(ET_CODE_NEW_CALIBRATION_IMAGE_AVAILABLE));
        }
        else{
            calibrationImage = calibrationTargets.getClearScreen();
            if (!eyetracker.calibrationDone()){
                // This means that BOTH eyes failed to gather data.
                calibrationFailureType = ETCFT_FAILED_BOTH;
                emit(eyeTrackerControl(ET_CODE_CALIBRATION_DONE));
                return;
            }
            /// Used for debugging
            /// eyetracker.saveCalibrationCoefficients("coeffs.kof");

            if (eyetracker.isRightEyeCalibrated()){
                if (eyetracker.isLeftEyeCalibrate()){
                    // Both eyes are ok
                    calibrationFailureType = ETCFT_NONE;
                }
                else{
                    // Right Eye OK. Left Failed
                    calibrationFailureType = ETCFT_FAILED_LEFT;
                }
            }
            else{
                if (eyetracker.isLeftEyeCalibrate()){
                    // Right Eye Failed. Left OK
                    calibrationFailureType = ETCFT_FAILED_RIGHT;
                }
                else{
                    // Enough data for both, but no coefficients could be computed. Not sure if tthis is possible but putting the possibility just in case.
                    calibrationFailureType = ETCFT_FAILED_BOTH;
                    return;
                }
            }

            emit(eyeTrackerControl(ET_CODE_CALIBRATION_DONE));
        }
    }
}


void HTCViveEyeProEyeTrackingInterface::calibrate(EyeTrackerCalibrationParameters params){

    if (params.forceCalibration){

        // Creating the calibration points
        qreal w = static_cast<qreal>(screenWidth);
        qreal h = static_cast<qreal>(screenHeight);

        QPoint p1(static_cast<qint32>(w*0.25),static_cast<qint32>(h*0.25)); // Top Left
        QPoint p2(static_cast<qint32>(w*0.75),static_cast<qint32>(h*0.25)); // Top right
        QPoint p3(static_cast<qint32>(w*0.50),static_cast<qint32>(h*0.50)); // Center
        QPoint p4(static_cast<qint32>(w*0.25),static_cast<qint32>(h*0.75)); // Bottom Left
        QPoint p5(static_cast<qint32>(w*0.75),static_cast<qint32>(h*0.75)); // Bottom Right.

        calibrationPoints.clear();
        calibrationPoints << p1 << p2 << p3 << p4 << p5;
        calibrationPointIndex = -1;
        isWaiting = false;
        calibrationTimer.start(CALIBRATION_WAIT);

        calibrationFailureType = ETCFT_UNKNOWN;

        eyetracker.start();

    }
    else{
        if (!params.name.isEmpty()){
            // This is interpreted as a file that loads calibration data
            eyetracker.loadCalibrationCoefficients(params.name);
        }
    }

}

void HTCViveEyeProEyeTrackingInterface::enableUpdating(bool enable){
    if (enable && !eyetracker.isRunning()) eyetracker.start();
    else if (!enable) eyetracker.stop();
}

void HTCViveEyeProEyeTrackingInterface::disconnectFromEyeTracker(){
    enableUpdating(false);
    emit(eyeTrackerControl(ET_CODE_DISCONNECTED_FROM_ET));
}
