#include "hpomniceptinterface.h"

const float HPOmniceptInterface::SAMPLING_FREQ = 120;

HPOmniceptInterface::HPOmniceptInterface(QObject *parent, qreal width, qreal height):EyeTrackerInterface(parent,width,height)
{
    connect(&calibration,&CalibrationLeastSquares::newCalibrationImageAvailable,this,&HPOmniceptInterface::onNewCalibrationImageAvailable);
    connect(&calibration,&CalibrationLeastSquares::calibrationDone,this,&HPOmniceptInterface::onCalibrationFinished);
    connect(&hpprovider,&HPOmniceptProvider::eyeDataAvailable,this,&HPOmniceptInterface::newEyeData);
    connect(&hpprovider,&HPOmniceptProvider::started,this,&HPOmniceptInterface::providedStarted);
}


void HPOmniceptInterface::connectToEyeTracker(){
    if (hpprovider.connectToHPRuntime()) {
        emit(eyeTrackerControl(ET_CODE_CONNECTION_SUCCESS));
    }
    else{
        logger.appendError("Failed connection to HPRuntime. Reason" + hpprovider.getError());
        emit(eyeTrackerControl(ET_CODE_CONNECTION_FAIL));
    }
}

void HPOmniceptInterface::enableUpdating(bool enable){
    if (enable){
        if (!hpprovider.isRunning()) {
            //qDebug() << "Starting the thread";
            hpprovider.start();
        }
    }
    else{
        //qDebug() << "Stopping the thread";
        hpprovider.stopProvider();
        eyeTrackerEnabled = false;
    }
}

void HPOmniceptInterface::providedStarted(){
    //qDebug() << "Thread sucessfully started";
    eyeTrackerEnabled = true;
}

void HPOmniceptInterface::newEyeData(QVariantMap eyedata){

    // Passing the current values throught the transformation matrix.
    float xl, yl, zl, xr, yr, zr;

    xl = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::X).toFloat();
    yl = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Y).toFloat();
    zl = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Z).toFloat();

    xr = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::X).toFloat();
    yr = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Y).toFloat();
    zr = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Z).toFloat();

    QVector4D vecL(xl/zl, yl/zl, zl,1);
    QVector4D vecR(xr/zr, yr/zr, zr,1);

    //QVector4D vecL(xl, yl, zl,0);
    //QVector4D vecR(xr, yr, zr,0);

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
        CalibrationLeastSquares::EyeInputData eid;
        eid.xl = static_cast<qreal>(xl);
        eid.xr = static_cast<qreal>(xr);
        eid.yl = static_cast<qreal>(yl);
        eid.yr = static_cast<qreal>(yr);

        if (calibration.isValidating()){
            // While validating wee need to pass the converted data points to verify that the user is looking where we think.
            calibration.addDataPointForVerification(eid);
        }
        else {
            lastData = correctionCoefficients.computeCorrections(eid);
            lastData.time    = eyedata.value(HPProvider::Timestamp).toLongLong();
            lastData.pdLeft  = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Pupil).toReal();
            lastData.pdRight = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Pupil).toReal();

            emit HPOmniceptInterface::newDataAvailable(lastData);
        }

    }

}

void HPOmniceptInterface::disconnectFromEyeTracker(){
    hpprovider.disconnectFromHPRuntime();
}

void HPOmniceptInterface::calibrate(EyeTrackerCalibrationParameters params){
    if (params.forceCalibration){
        calibration.startCalibrationSequence(static_cast<qint32>(screenWidth),
                                             static_cast<qint32>(screenHeight),                                             
                                             params.number_of_calibration_points);
        // Non empty file name will indicate coefficient storage.
        coefficientsFile = params.name;
    }
    else{
        if (!correctionCoefficients.loadCalibrationCoefficients(params.name)){
            logger.appendError("Failed to set calibration parameters from file: " + params.name);
            calibrationFailureType = ETCFT_UNKNOWN;
            emit(eyeTrackerControl(ET_CODE_CALIBRATION_DONE));
        }
        else {
            calibrationFailureType = ETCFT_NONE;
            emit(eyeTrackerControl(ET_CODE_CALIBRATION_DONE));
        }
    }
}

void HPOmniceptInterface::updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l){
    rVRTransform = r;
    lVRTransform = l;
}

void HPOmniceptInterface::onCalibrationFinished(){
    if (!calibration.wereCalibrationCoefficientsComputedSuccessfully()){
        calibrationFailureType = ETCFT_UNKNOWN;
    }
    else{

        correctionCoefficients = calibration.getCalculatedCoeficients();
        if (coefficientsFile != ""){
            correctionCoefficients.saveCalibrationCoefficients(coefficientsFile);
        }

        // Checking the coefficients are correctly computed and that the corresponding eye validation was successfull.
        CalibrationLeastSquares::EyeValidationsStatus evs = calibration.getEyeValidationStatus();

        bool fail_left =  ( (!correctionCoefficients.xl.valid) || (!correctionCoefficients.yl.valid) || (evs == CalibrationLeastSquares::EVS_RIGHT) || (evs == CalibrationLeastSquares::EVS_NONE) );
        bool fail_right = ( (!correctionCoefficients.xr.valid) || (!correctionCoefficients.yr.valid) || (evs == CalibrationLeastSquares::EVS_LEFT)  || (evs == CalibrationLeastSquares::EVS_NONE) );

        if (fail_left){
            if (fail_right){
                calibrationFailureType = ETCFT_FAILED_BOTH;
            }
            else calibrationFailureType = ETCFT_FAILED_LEFT;
        }
        else if (fail_right){
            calibrationFailureType = ETCFT_FAILED_RIGHT;
        }
        else {
            calibrationFailureType = ETCFT_NONE;
        }

    }
    emit(eyeTrackerControl(ET_CODE_CALIBRATION_DONE));
}

QString HPOmniceptInterface::getCalibrationValidationReport() const{
    return calibration.getValidationReport();
}

QVariantMap HPOmniceptInterface::getCalibrationValidationData() const {
    return calibration.getCalibrationValidationData();
}

void HPOmniceptInterface::configureCalibrationValidation(QVariantMap calibrationValidationParameters){
    calibrationValidationParameters[VMDC::CalibrationFields::SAMPLING_FREQUENCY] = SAMPLING_FREQ;
    calibration.configureValidation(calibrationValidationParameters);
}

void HPOmniceptInterface::onNewCalibrationImageAvailable(){
    this->calibrationImage = calibration.getCurrentCalibrationImage();
    emit(eyeTrackerControl(ET_CODE_NEW_CALIBRATION_IMAGE_AVAILABLE));
}
