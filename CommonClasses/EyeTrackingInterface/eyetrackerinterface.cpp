#include "eyetrackerinterface.h"

EyeTrackerInterface::EyeTrackerInterface(QObject *parent, qreal width, qreal height) : QObject(parent)
{
    qRegisterMetaType<EyeTrackerData>("EyeTrackerData");
    screenHeight = height;
    screenWidth = width;
    eyeToTransmit = VMDC::Eye::BOTH;
    eyeTrackerEnabled = false;
    calib_retry_msg = CRM_NONE; // By setting this to none, basically nothing will change unless the message is especifically implemented.
    vFOV = -1;
    hFOV = -1;
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

void EyeTrackerInterface::setFieldOfView(qreal vfov, qreal hfov){
    vFOV = static_cast<float>(vfov);
    hFOV = static_cast<float>(hfov);
}

void EyeTrackerInterface::calibrate(EyeTrackerCalibrationParameters params){
    Q_UNUSED(params)
}

//QImage EyeTrackerInterface::getCalibrationImage() const{
RenderServerScene EyeTrackerInterface::getCalibrationImage() const{
    return calibrationImage;
}

EyeTrackerInterface::CalibrationRetryMessage EyeTrackerInterface::getCalibrationRetryMessage() const{
    return calib_retry_msg;
}

EyeTrackerData EyeTrackerInterface::getLastData() const{
    return lastData;
}

bool EyeTrackerInterface::isEyeTrackerEnabled() const{
    return eyeTrackerEnabled;
}

QList<QPointF> EyeTrackerInterface::getCalibrationPoints() const{
    return calibrationPoints;
}

void EyeTrackerInterface::setCalibrationVectors(const QList<QVector3D> &calibVecs){
    Q_UNUSED(calibVecs)
}



