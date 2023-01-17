#include "eyetrackerinterface.h"

EyeTrackerInterface::EyeTrackerInterface(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<EyeTrackerData>("EyeTrackerData");
    eyeToTransmit = VMDC::Eye::BOTH;
    eyeTrackerEnabled = false;
    eyeTrackerConnected = false;
}

void EyeTrackerInterface::setEyeToTransmit(QString eye){
    if (VMDC::Eye::validate(eye) == ""){
        eyeToTransmit = eye;
    }
}

bool EyeTrackerInterface::isEyeTrackerConnected() const {
    return eyeTrackerConnected;
}

void EyeTrackerInterface::connectToEyeTracker(){
}

void EyeTrackerInterface::enableUpdating(bool enable){
    Q_UNUSED(enable)
}

void EyeTrackerInterface::disconnectFromEyeTracker(){
}

EyeTrackerData EyeTrackerInterface::getLastData() const{
    return lastData;
}

bool EyeTrackerInterface::isEyeTrackerEnabled() const{
    return eyeTrackerEnabled;
}


