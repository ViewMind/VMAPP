#include "eyetrackerinterface.h"

EyeTrackerInterface::EyeTrackerInterface(QObject *parent, qreal width, qreal height) : QObject(parent)
{
    qRegisterMetaType<EyeTrackerData>("EyeTrackerData");
    screenHeight = height;
    screenWidth = width;
    eyeToTransmit = VMDC::Eye::BOTH;
    eyeTrackerEnabled = false;
}

void EyeTrackerInterface::setEyeToTransmit(QString eye){
    if (VMDC::Eye::validate(eye) == ""){
        eyeToTransmit = eye;
    }
}

QString EyeTrackerInterface::getCalibrationValidationReport() const {
    return "";
}

QString EyeTrackerInterface::getCalibrationRecommendedEye() const {
    return VMDC::Eye::BOTH;
}

void EyeTrackerInterface::configureCalibrationValidation(QVariantMap calibrationValidationParamters){
    Q_UNUSED(calibrationValidationParamters)
}

QVariantMap EyeTrackerInterface::getCalibrationValidationData() const {
    return QVariantMap();
}

void EyeTrackerInterface::connectToEyeTracker(){
}

void EyeTrackerInterface::enableUpdating(bool enable){
    Q_UNUSED(enable)
}

void EyeTrackerInterface::disconnectFromEyeTracker(){
}

void EyeTrackerInterface::updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l){
    Q_UNUSED(r)
    Q_UNUSED(l)
}

void EyeTrackerInterface::calibrate(EyeTrackerCalibrationParameters params){
    Q_UNUSED(params)
}

QImage EyeTrackerInterface::getCalibrationImage() const{
    return calibrationImage;
}

EyeTrackerData EyeTrackerInterface::getLastData() const{
    return lastData;
}

bool EyeTrackerInterface::isEyeTrackerEnabled() const{
    return eyeTrackerEnabled;
}



