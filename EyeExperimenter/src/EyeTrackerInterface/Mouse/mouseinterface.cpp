#include "mouseinterface.h"

MouseInterface::MouseInterface()
{
    // Connecting the poll timer to time out to the slot.
    connect(&pollTimer,&QTimer::timeout,this,&MouseInterface::on_pollTimer_Up);
    sendData = false;
    isCalibrated = false;
    isBeingCalibrated = false;

    calibrationScreen = new CalibrationArea();
    connect(calibrationScreen,&CalibrationArea::calibrationCanceled,this,&MouseInterface::on_calibrationCancelled);

#ifdef DEVELOPMENT_MODE
    isCalibrated = true;
    isBeingCalibrated = false;
    sendData = true;
#endif

}


EyeTrackerInterface::ExitStatus MouseInterface::connectToEyeTracker(){
    // "Connecting to the server"
    dataToSend.time = 0;
    pollTimer.start(TIMEOUT);
    return ES_SUCCESS;
}

void MouseInterface::enableUpdating(bool enable){
    sendData = enable;
}

void MouseInterface::disconnectFromEyeTracker(){
    pollTimer.stop();
}



EyeTrackerInterface::ExitStatus MouseInterface::calibrate(EyeTrackerCalibrationParameters params){
    Q_UNUSED(params);
    isBeingCalibrated = true;
    isCalibrated = false;
    calibrationScreen->exec();
    if (isCalibrated)
        return ES_SUCCESS;
    else{
        message = "Calibration was aborted";
        return ES_FAIL;
    }
}

void MouseInterface::on_calibrationCancelled(){
    isCalibrated = false;
    isBeingCalibrated = false;
}

void MouseInterface::on_pollTimer_Up(){

    // Time is the time stamp in millisecond since the connection to the server started.
    dataToSend.time = dataToSend.time + TIMEOUT;

    QPoint point = QCursor::pos();
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


    dataToSend.xLeft = point.x();
    dataToSend.xRight = point.x();
    dataToSend.yLeft = point.y();
    dataToSend.yRight = point.y();
    dataToSend.pdRight = 1;
    dataToSend.pdLeft = 1;

    emit (newDataAvailable(dataToSend));

}

MouseInterface::~MouseInterface(){
    delete calibrationScreen;
}
