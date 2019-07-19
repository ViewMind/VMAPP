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
    Q_UNUSED(enable);
}

void EyeTrackerInterface::disconnectFromEyeTracker(){
}

void EyeTrackerInterface::calibrate(EyeTrackerCalibrationParameters params){
    Q_UNUSED(params);
}

