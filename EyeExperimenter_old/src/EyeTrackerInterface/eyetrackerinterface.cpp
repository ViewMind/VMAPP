#include "eyetrackerinterface.h"

EyeTrackerInterface::EyeTrackerInterface(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<EyeTrackerData>("EyeTrackerData");
}


EyeTrackerInterface::ExitStatus EyeTrackerInterface::connectToEyeTracker(){
    message = "Unimplemented Eye Tracker Interface";
    return ES_FAIL;
}

void EyeTrackerInterface::enableUpdating(bool enable){
    Q_UNUSED(enable);
}

void EyeTrackerInterface::disconnectFromEyeTracker(){
}

EyeTrackerInterface::ExitStatus EyeTrackerInterface::calibrate(EyeTrackerCalibrationParameters params){
    Q_UNUSED(params);
    message = "Unimplemented Eye Tracker Interface";
    return ES_FAIL;
}

