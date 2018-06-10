#include "redinterface.h"

REDInterface::REDInterface():EyeTrackerInterface()
{
    connect(&pollTimer,&QTimer::timeout,this,&REDInterface::on_pollTimer_Up);
    lastData.xRight = 0;
    lastData.yRight = 0;
    lastData.xLeft = 0;
    lastData.yLeft = 0;
    lastData.time = 0;
    lastData.pdLeft = 0;
    lastData.pdRight = 0;
    lastData.timeUnit = EyeTrackerData::TU_MS;
    pollTimer.start(5);
}

void REDInterface::connectToEyeTracker(){
    // Connecting the server
    int result = iV_ConnectLocal();

    if (result != RET_SUCCESS){
        logger.appendError("SMI REDM: Connecting to server. " + num2Error(result));
        emit(eyeTrackerControl(ET_CODE_CONNECTION_FAIL));
        return;
    }

    emit(eyeTrackerControl(ET_CODE_CONNECTION_SUCCESS));
}

void REDInterface::calibrate(EyeTrackerCalibrationParameters params){

    if (!params.name.isEmpty() && !params.forceCalibration){
        QByteArray buffer = params.name.toLatin1();
        int result = iV_LoadCalibration(buffer.data());

        if (result != RET_SUCCESS){
            logger.appendError("SMI REDM: Loading Calibration. " + num2Error(result));
            emit(eyeTrackerControl(ET_CODE_CONNECTION_FAIL));
        }
        else{
            emit(eyeTrackerControl(ET_CODE_CONNECTION_SUCCESS));
            return;
        }
    }

    CalibrationStruct calibrationData;
    calibrationData.method = 9;
    calibrationData.speed = 1;
    calibrationData.displayDevice = 0;
    calibrationData.targetShape = 2;
    calibrationData.foregroundBrightness = 250;
    calibrationData.backgroundBrightness = 230;
    calibrationData.autoAccept = 0;
    calibrationData.targetSize = 20;
    calibrationData.visualization = 1;

    int result = iV_SetupCalibration(&calibrationData);

    if (result != RET_SUCCESS){
        logger.appendError("SMI REDM: Setting up calibration. " + num2Error(result));
        emit(eyeTrackerControl(ET_CODE_CONNECTION_FAIL));
        return;
    }

    result = iV_Calibrate();

    if (result != RET_SUCCESS){
        logger.appendError("SMI REDM: During calibration. " + num2Error(result));
        emit(eyeTrackerControl(ET_CODE_CONNECTION_FAIL));
        return;
    }

    // Attempting to save calibration
    if (!params.name.isEmpty()){
        QByteArray buffer = params.name.toLatin1();
        result = iV_SaveCalibration(buffer.data());

        if (result != RET_SUCCESS){
            logger.appendWarning("SMI REDM: Saving calibration. " +  num2Error(result));
        }
    }

    emit(eyeTrackerControl(ET_CODE_CONNECTION_SUCCESS));
}

void REDInterface::enableUpdating(bool enable){
    if (!enable){
        pollTimer.stop();
    }
    else{
        pollTimer.start(5);
    }
}

void REDInterface::disconnectFromEyeTracker(){
   iV_Disconnect();
}

void REDInterface::on_pollTimer_Up(){
    SampleStruct data;
    iV_GetSample(&data);

#ifdef SHOW_CONSOLE
    if ((data.timestamp/1000) > 20000000){
        qWarning() << "FOUND LARGE TIME FROM SOURCE OF: " << lastData.time;
    }
    else{
        qWarning() << lastData.time;
    }
#endif

    if (data.timestamp == lastData.time) return;    
    lastData.xLeft = data.leftEye.gazeX;
    lastData.yLeft = data.leftEye.gazeY;
    lastData.xRight = data.rightEye.gazeX;
    lastData.yRight = data.rightEye.gazeY;

    // The data is returned in us so it is turned to ms.
    lastData.time = data.timestamp/1000;
    lastData.pdLeft = data.leftEye.diam;
    lastData.pdRight = data.rightEye.diam;

#ifdef SHOW_CONSOLE
    if (lastData.time > 20000000){
        qWarning() << "FOUND LARGE TIME OF: " << lastData.time;
    }
    else{
        qWarning() << lastData.time;
    }
#endif

    // Sending the new data.
    emit(newDataAvailable(lastData));

    // The time data is loaded again to make sure the time stamp comparison is done correctly.
    lastData.time = data.timestamp;
}

QString REDInterface::num2Error(int result){
    switch (result){
    case ERR_NOT_CONNECTED:
        return "No connection established to the device";
    case ERR_NOT_CALIBRATED:
        return "Device not calibrated";
    case ERR_WRONG_IVIEWX_VERSION:
        return "Wrong version of iView X";
    case ERR_WRONG_DEVICE:
        return "Eye tracking device required for this function is not connected";
    case ERR_WRONG_CALIBRATION_METHOD:
        return "Wrong calibration method used";
    case ERR_WRONG_PARAMETER:
        return "Invalid parameter was provided";
    case ERR_FEATURE_NOT_LICENSED:
        return "At least one of the calibration featured requested is not licensed";
    case RET_CALIBRATION_ABORTED:
        return "Calibration aborted";
    case ERR_EYETRACKING_APPLICATION_NOT_RUNNING:
        return "No eye tracking application running";
    case ERR_WRONG_COMMUNICATION_PARAMETER:
        return "Invalid licesense set (Invalid communication parameter)";
    case ERR_COULD_NOT_CONNECT:
        return "Failed to establish connection";
    case ERR_FUNC_NOT_LOADED:
        return "The called API function is not (yet) loaded, iV_Start, iV_Connect or iV_ConnectLocal have not been called";
    case ERR_IVIEWX_NOT_FOUND:
        return "The IVIEW X application was not found";
    default:
        return "Unknown error : " + QString::number(result);
    }
}

