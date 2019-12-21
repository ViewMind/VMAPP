#include "eyetrackerinterface.h"

EyeTrackerInterface::EyeTrackerInterface(QObject *parent, qreal width, qreal height) : QObject(parent)
{
    qRegisterMetaType<EyeTrackerData>("EyeTrackerData");
    screenHeight = height;
    screenWidth = width;
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
