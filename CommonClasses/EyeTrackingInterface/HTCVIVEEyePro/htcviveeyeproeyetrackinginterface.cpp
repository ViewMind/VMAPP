#include "htcviveeyeproeyetrackinginterface.h"

const float HTCViveEyeProEyeTrackingInterface::SAMPLING_FREQ = 120;

HTCViveEyeProEyeTrackingInterface::HTCViveEyeProEyeTrackingInterface(QObject *parent, qreal width, qreal height):EyeTrackerInterface(parent,width,height)
{
    connect(&calibration,&CalibrationLeastSquares::newCalibrationImageAvailable,this,&HTCViveEyeProEyeTrackingInterface::onNewCalibrationImageAvailable);
    connect(&calibration,&CalibrationLeastSquares::calibrationDone,this,&HTCViveEyeProEyeTrackingInterface::onCalibrationFinished);
    connect(&eyetracker,&VIVEEyePoller::eyeDataAvailable,this,&HTCViveEyeProEyeTrackingInterface::newEyeData);
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
    rVRTransform = r;
    lVRTransform = l;
}

void HTCViveEyeProEyeTrackingInterface::newEyeData(QVariantMap eyedata){

    // Passing the current values throught the transformation matrix.
    float xl, yl, zl, xr, yr, zr;

    xl = eyedata.value(HTCVIVE::LeftEye).toMap().value(HTCVIVE::Eye::X).toFloat();
    yl = eyedata.value(HTCVIVE::LeftEye).toMap().value(HTCVIVE::Eye::Y).toFloat();
    zl = eyedata.value(HTCVIVE::LeftEye).toMap().value(HTCVIVE::Eye::Z).toFloat();

    xr = eyedata.value(HTCVIVE::RightEye).toMap().value(HTCVIVE::Eye::X).toFloat();
    yr = eyedata.value(HTCVIVE::RightEye).toMap().value(HTCVIVE::Eye::Y).toFloat();
    zr = eyedata.value(HTCVIVE::RightEye).toMap().value(HTCVIVE::Eye::Z).toFloat();

    QVector4D vecL, vecR;

    if (qAbs(static_cast<double>(zl)) > 1e-6 ){ // This is to avoid warning of comparing float to zero.
        vecL = QVector4D(xl/zl, yl/zl, zl,1);
    }
    else {
        vecL = QVector4D(0, 0, 0,0);
    }

    if (qAbs(static_cast<double>(zr)) > 1e-6 ){ // This is to avoid warning of comparing float to zero.
        vecR = QVector4D(xr/zr, yr/zr, zr,1);
    }
    else {
        vecR = QVector4D(0, 0, 0,0);
    }

    vecL = lVRTransform*vecL;
    xl = vecL.x();
    yl = vecL.y();

    vecR = rVRTransform*vecR;
    xr = vecR.x();
    yr = vecR.y();

    if (calibration.isCalibrating()){
        //qDebug() << "Calib";
        calibration.addDataPointForCalibration(xl,yl,xr,yr);
    }
    else{
        // We need to convert.
        EyeRealData eid;
        eid.xLeft  = static_cast<qreal>(xl);
        eid.xRight = static_cast<qreal>(xr);
        eid.yLeft  = static_cast<qreal>(yl);
        eid.yRight = static_cast<qreal>(yr);

        lastData = correctionCoefficients.computeCorrections(eid);

        lastData.time    = eyedata.value(HTCVIVE::Timestamp).toLongLong();
        lastData.pdLeft  = eyedata.value(HTCVIVE::LeftEye).toMap().value(HTCVIVE::Eye::Pupil).toReal();
        lastData.pdRight = eyedata.value(HTCVIVE::RightEye).toMap().value(HTCVIVE::Eye::Pupil).toReal();

        emit EyeTrackerInterface::newDataAvailable(lastData);
    }

}

void HTCViveEyeProEyeTrackingInterface::calibrate(EyeTrackerCalibrationParameters params){

    if (params.forceCalibration){
        calibration.startCalibrationSequence(static_cast<qint32>(screenWidth),
                                             static_cast<qint32>(screenHeight),
                                             params.number_of_calibration_points,
                                             params.gather_time,params.wait_time);
        // Non empty file name will indicate coefficient storage.
        coefficientsFile = params.name;
    }
    else{
        if (!correctionCoefficients.loadCalibrationCoefficients(params.name)){
            logger.appendError("Failed to set calibration parameters from file: " + params.name);

            emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_CALIBRATION_DONE);
        }
        else{

            emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_CALIBRATION_DONE);
        }
    }

}

QString HTCViveEyeProEyeTrackingInterface::getCalibrationValidationReport() const{
    return calibration.getValidationReport();
}

void HTCViveEyeProEyeTrackingInterface::enableUpdating(bool enable){
    if (enable && !eyetracker.isRunning()) eyetracker.start();
    else if (!enable) eyetracker.stop();
}

void HTCViveEyeProEyeTrackingInterface::disconnectFromEyeTracker(){
    enableUpdating(false);
    emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_DISCONNECTED_FROM_ET);
}

void HTCViveEyeProEyeTrackingInterface::onCalibrationFinished(){
    correctionCoefficients = calibration.getCalculatedCoeficients();

    if (coefficientsFile != ""){
        correctionCoefficients.saveCalibrationCoefficients(coefficientsFile);
    }

    emit(eyeTrackerControl(ET_CODE_CALIBRATION_DONE));
}

void HTCViveEyeProEyeTrackingInterface::onNewCalibrationImageAvailable(){
    this->calibrationImage = calibration.getCurrentCalibrationImage();
    emit EyeTrackerInterface::eyeTrackerControl(ET_CODE_NEW_CALIBRATION_IMAGE_AVAILABLE);
}
