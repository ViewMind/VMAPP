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
    float oxl, oyl, ozl, oxr, oyr, ozr;

    xl = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::X).toFloat();
    yl = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Y).toFloat();
    zl = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Z).toFloat();

    xr = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::X).toFloat();
    yr = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Y).toFloat();
    zr = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Z).toFloat();

    oxl = xl; oyl = yl; ozl = zl; oyr = yr; ozr = zr; oxr = xr;

    QVector4D vecL, vecR;

    if (qAbs(static_cast<double>(zl)) > 1e-6 ){ // This is to avoid warning of comparing float to zero and to avoid a division by zero
        vecL = QVector4D(xl/zl, yl/zl, zl,1);
    }
    else {
        vecL = QVector4D(0, 0, 0,0);
    }

    if (qAbs(static_cast<double>(zr)) > 1e-6 ){ // This is to avoid warning of comparing float to zero and to avoid a division by zero.
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
        if (qIsNaN(xl) || qIsNaN(yl) || qIsNaN(xr) || qIsNaN(yr)){
            qDebug() << "Original values" << oxl << oyl << ozl << "|" << oxr << oyr << ozr;
            qDebug() << vecL;
            qDebug() << vecR;
            qDebug() << lVRTransform;
            qDebug() << rVRTransform;
            qDebug() << "==========================";
        }
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
        lastData.time    = eyedata.value(HPProvider::Timestamp).toLongLong();
        lastData.pdLeft  = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Pupil).toReal();
        lastData.pdRight = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Pupil).toReal();

        emit HPOmniceptInterface::newDataAvailable(lastData);

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
            emit(eyeTrackerControl(ET_CODE_CALIBRATION_ABORTED));
        }
        else {
            emit(eyeTrackerControl(ET_CODE_CALIBRATION_DONE));
        }
    }
}

void HPOmniceptInterface::updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l){
    rVRTransform = r;
    lVRTransform = l;
}

void HPOmniceptInterface::onCalibrationFinished(){

    // NOTE at this point the correction coefficients might be entirely wrong.
    // This information will be contained in the validation data and report.
    // Even if no data was gathered or we had a zero determinant when doing least squares.

    correctionCoefficients = calibration.getCalculatedCoeficients();

    if (coefficientsFile != ""){
        correctionCoefficients.saveCalibrationCoefficients(coefficientsFile);
    }

    emit(eyeTrackerControl(ET_CODE_CALIBRATION_DONE));
}

QString HPOmniceptInterface::getCalibrationValidationReport() const{
    return calibration.getValidationReport();
}

QVariantMap HPOmniceptInterface::getCalibrationValidationData() const {
    return calibration.getCalibrationValidationData();
}

QString HPOmniceptInterface::getCalibrationRecommendedEye() const {
    return calibration.getRecommendedEye();
}

void HPOmniceptInterface::configureCalibrationValidation(QVariantMap calibrationValidationParameters){
    calibration.configureValidation(calibrationValidationParameters);
}

void HPOmniceptInterface::onNewCalibrationImageAvailable(){
    this->calibrationImage = calibration.getCurrentCalibrationImage();
    emit(eyeTrackerControl(ET_CODE_NEW_CALIBRATION_IMAGE_AVAILABLE));
}
