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

    // This makes it so that the calibration screen is never shown.
    calibrationScreen->setAutoCalibration(true);

}


void MouseInterface::connectToEyeTracker(){
    // "Connecting to the server"
    dataToSend.time = 0;
    pollTimer.start(TIMEOUT);
    emit(eyeTrackeControl(ET_CODE_CONNECTION_SUCCESS));
}

void MouseInterface::enableUpdating(bool enable){
    sendData = enable;
}

void MouseInterface::disconnectFromEyeTracker(){
    pollTimer.stop();
}

void MouseInterface::calibrate(EyeTrackerCalibrationParameters params){
    Q_UNUSED(params);
    isBeingCalibrated = true;
    isCalibrated = false;
    calibrationScreen->exec();
    if (isCalibrated)
        emit(eyeTrackeControl(ET_CODE_CALIBRATION_DONE));
    else{
        logger.appendWarning("MOUSE TRACKER: Calibration was aborted");
        emit(eyeTrackeControl(ET_CODE_CALIBRATION_ABORTED));
    }
}

void MouseInterface::on_calibrationCancelled(){
    isCalibrated = false;
    isBeingCalibrated = false;
}

void MouseInterface::on_pollTimer_Up(){

    // Time is the time stamp in millisecond since the connection to the server started.
    // Time in microseconds.
    dataToSend.time = dataToSend.time + TIMEOUT*1000;

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
    dataToSend.pdRight = 0;
    dataToSend.pdLeft = 0;

    emit (newDataAvailable(dataToSend));

}

MouseInterface::~MouseInterface(){
    delete calibrationScreen;
}
