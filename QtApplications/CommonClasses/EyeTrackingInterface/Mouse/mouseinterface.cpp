#include "mouseinterface.h"

MouseInterface::MouseInterface()
{
    // Connecting the poll timer to time out to the slot.
    connect(&pollTimer,&QTimer::timeout,this,&MouseInterface::on_pollTimerUp);
    sendData = false;
    isCalibrated = false;
    isBeingCalibrated = false;

    connect(&calibration,&CalibrationLeastSquares::newCalibrationImageAvailable,this,&MouseInterface::onNewCalibrationImageAvailable);
    connect(&calibration,&CalibrationLeastSquares::calibrationDone,this,&MouseInterface::onCalibrationFinished);

    calibrationFailureType = ETCFT_NONE;

    overrideCalibrationFlag = false;

}

void MouseInterface::overrideCalibration(){
    overrideCalibrationFlag = true;
}

void MouseInterface::setCalibrationAreaDimensions(qreal w, qreal h){
    screenWidth = w;
    screenHeight = h;
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


void MouseInterface::onNewCalibrationImageAvailable(){
    calibrationArea.setCurrentImage(calibration.getCurrentCalibrationImage());
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
    else {
        calibration.startCalibrationSequence(static_cast<qint32>(screenWidth),
                                             static_cast<qint32>(screenHeight),
                                             params.number_of_calibration_points);
        calibrationArea.exec();
        // Non empty file name will indicate coefficient storage.
    }

//    isBeingCalibrated = true;
//    isCalibrated = false;
//    qDebug() << "Before executing calibration screen" << isCalibrated;
//    calibrationScreen->exec();
//    calibrationFailureType = ETCFT_NONE;
//    qDebug() << "Starting calibration with isCalibrated" << isCalibrated;
//    if (isCalibrated)
//        emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_CALIBRATION_DONE);
//    else{
//        logger.appendWarning("MOUSE TRACKER: Calibration was aborted");
//        emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_CALIBRATION_ABORTED);
//    }
}

void MouseInterface::onCalibrationFinished(){
    // Mouse calibration cannot fail as there data and coefficients are ignored. It's just for showing the calibration screen.
    calibrationArea.hide();
    emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_CALIBRATION_DONE);
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
//    //qDebug() << point;
//    if (isBeingCalibrated){
//        if (calibrationScreen->isInCalibrationPoint(point.x(),point.y())){
//            isCalibrated = true;
//            isBeingCalibrated = false;
//            calibrationScreen->hide();
//        }
//        else return;
//    }

    if (calibration.isCalibrating()){
        calibration.addDataPointForCalibration(point.x(),point.y(),point.x(),point.y());
    }
    else {

        if (!sendData) return;

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

//    // Assuming this is not a calibration.
//    if (!isCalibrated || !sendData) return;


}

MouseInterface::~MouseInterface(){

}
