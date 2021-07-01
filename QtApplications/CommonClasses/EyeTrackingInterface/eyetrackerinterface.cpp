#include "eyetrackerinterface.h"

EyeTrackerInterface::EyeTrackerInterface(QObject *parent, qreal width, qreal height) : QObject(parent)
{
    qRegisterMetaType<EyeTrackerData>("EyeTrackerData");
    screenHeight = height;
    screenWidth = width;
    calibrationFailureType = ETCFT_UNKNOWN;
    eyeToTransmit = VMDC::Eye::BOTH;
    eyeTrackerEnabled = false;
}

void EyeTrackerInterface::setEyeToTransmit(QString eye){
    if (VMDC::Eye::validate(eye) == ""){
        eyeToTransmit = eye;
    }
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

EyeTrackerInterface::ETCalibrationFailureType EyeTrackerInterface::getCalibrationFailureType() const{
    return calibrationFailureType;
}
