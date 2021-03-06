#ifndef EYETRACKERINTERFACE_H
#define EYETRACKERINTERFACE_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QMatrix4x4>
#include "eyetrackerdata.h"
#include "../LogInterface/loginterface.h"
#include "../RawDataContainer/VMDC.h"

class EyeTrackerInterface : public QObject
{
    Q_OBJECT
public:

    static const quint8 ET_CODE_CALIBRATION_DONE                = 0;
    static const quint8 ET_CODE_CALIBRATION_ABORTED             = 1;
    static const quint8 ET_CODE_CONNECTION_SUCCESS              = 2;
    static const quint8 ET_CODE_CONNECTION_FAIL                 = 3;
    static const quint8 ET_CODE_DISCONNECTED_FROM_ET            = 4;
    static const quint8 ET_CODE_NEW_CALIBRATION_IMAGE_AVAILABLE = 5;

    explicit EyeTrackerInterface(QObject *parent = nullptr, qreal width = 1, qreal height = 1);

    virtual void connectToEyeTracker();

    virtual void enableUpdating(bool enable);

    virtual void disconnectFromEyeTracker();

    virtual void calibrate(EyeTrackerCalibrationParameters params);


    virtual void configureCalibrationValidation(QVariantMap calibrationValidationParamters);

    virtual QVariantMap getCalibrationValidationData() const;

    virtual QString getCalibrationValidationReport() const;

    virtual QString getCalibrationRecommendedEye() const;

    QImage getCalibrationImage() const;

    void setEyeToTransmit(QString eye);

    EyeTrackerData getLastData() const;

    bool isEyeTrackerEnabled() const;

public slots:
    virtual void updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l);

signals:
    void newDataAvailable(EyeTrackerData data);

    // Connection and calibration signals
    void eyeTrackerControl(quint8 code);

    // SHOULD ONLY BE SENT WHEN IT REPRESENTS AN ERROR.
    void disconnectedFromET();

protected:
    LogInterface logger;

    // Which EyeDataToTransmit
    QString eyeToTransmit;

    // The resolution, in case it is required by the eyetracker.
    qreal screenWidth;
    qreal screenHeight;

    // Calibration image if this needs to be displayed.
    QImage calibrationImage;

    // The last available data
    EyeTrackerData lastData;

    bool eyeTrackerEnabled;

    // Shorcut functions for clarity.
    bool canUseLeft() const {return ((eyeToTransmit == VMDC::Eye::BOTH) || (eyeToTransmit == VMDC::Eye::LEFT));}
    bool canUseRight() const {return ((eyeToTransmit == VMDC::Eye::BOTH) || (eyeToTransmit == VMDC::Eye::RIGHT));}
};

#endif // EYETRACKERINTERFACE_H
