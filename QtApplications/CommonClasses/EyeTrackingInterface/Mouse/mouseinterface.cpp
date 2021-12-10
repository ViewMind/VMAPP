#include "mouseinterface.h"

MouseInterface::MouseInterface()
{
    // Connecting the poll timer to time out to the slot.
    connect(&pollTimer,&QTimer::timeout,this,&MouseInterface::on_pollTimerUp);
    sendData = false;
    isCalibrated = false;
    isBeingCalibrated = false;

    calibrationScreen = new CalibrationArea();
    connect(calibrationScreen,&CalibrationArea::calibrationCanceled,this,&MouseInterface::on_calibrationCancelled);

    // This makes it so that the calibration screen is never shown.
    calibrationScreen->setAutoCalibration(true);

    calibrationFailureType = ETCFT_NONE;

    overrideCalibrationFlag = false;

}

void MouseInterface::overrideCalibration(){
    overrideCalibrationFlag = true;
}

void MouseInterface::connectToEyeTracker(){
    // "Connecting to the server"
    dataToSend.time = 0;
    pollTimer.start(TIMEOUT);
    emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_CONNECTION_SUCCESS);
}

void MouseInterface::enableUpdating(bool enable){
    sendData = enable;
}

void MouseInterface::disconnectFromEyeTracker(){
    pollTimer.stop();
}

void MouseInterface::mouseSetCalibrationToTrue(){
    isCalibrated = true;
}

void MouseInterface::calibrate(EyeTrackerCalibrationParameters params){
    Q_UNUSED(params)

    if (overrideCalibrationFlag){
        calibrationFailureType = ETCFT_NONE;
        isCalibrated = true;
        sendData = true;
        emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_CALIBRATION_DONE);
        return;
    }

    isBeingCalibrated = true;
    isCalibrated = false;
    calibrationScreen->exec();
    calibrationFailureType = ETCFT_NONE;
    if (isCalibrated)
        emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_CALIBRATION_DONE);
    else{
        logger.appendWarning("MOUSE TRACKER: Calibration was aborted");
        emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_CALIBRATION_ABORTED);
    }
}

void MouseInterface::on_calibrationCancelled(){
    isCalibrated = false;
    isBeingCalibrated = false;
}

void MouseInterface::on_pollTimerUp(){

    // Time is the time stamp in millisecond since the connection to the server started.
    // Time in microseconds.
   //  dataToSend.time = dataToSend.time + TIMEOUT*1000;
    dataToSend.time = dataToSend.time + TIMEOUT;

    QPoint point = QCursor::pos();
    //qDebug() << point;
    if (isBeingCalibrated){
        if (calibrationScreen->isInCalibrationPoint(point.x(),point.y())){
            isCalibrated = true;
            isBeingCalibrated = false;
            calibrationScreen->hide();
        }
        else return;
    }

    // Assuming this is not a calibration.
    if (!isCalibrated || !sendData) return;

    if (canUseLeft()){
        dataToSend.xLeft = point.x();
        dataToSend.yLeft = point.y();
    }
    else{
        dataToSend.yLeft = 0;
        dataToSend.xLeft = 0;
    }
    if (canUseRight()){
        dataToSend.xRight = point.x();
        dataToSend.yRight = point.y();
    }
    else{
        dataToSend.xRight = 0;
        dataToSend.yRight = 0;
    }

    dataToSend.pdRight = 0;
    dataToSend.pdLeft = 0;

    lastData = dataToSend;

    //qDebug() << "Eye to transmit" << eyeToTransmit;
    //qDebug() << eyeToTransmit << dataToSend.toString();

    emit EyeTrackerInterface::newDataAvailable(dataToSend);

}

MouseInterface::~MouseInterface(){
    delete calibrationScreen;
}
