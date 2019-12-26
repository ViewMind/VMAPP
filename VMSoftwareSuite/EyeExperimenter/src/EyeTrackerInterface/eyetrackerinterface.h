#ifndef EYETRACKERINTERFACE_H
#define EYETRACKERINTERFACE_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QMatrix4x4>
#include "eyetrackerdata.h"
#include "../../../CommonClasses/common.h"

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

    typedef enum {ETCFT_FAILED_LEFT, ETCFT_FAILED_RIGHT, ETCFT_FAILED_BOTH, ETCFT_NONE ,ETCFT_UNKNOWN}  ETCalibrationFailureType;

    explicit EyeTrackerInterface(QObject *parent = nullptr, qreal width = 1, qreal height = 1);

    virtual void connectToEyeTracker();

    virtual void enableUpdating(bool enable);

    virtual void disconnectFromEyeTracker();

    virtual void calibrate(EyeTrackerCalibrationParameters params);

    ETCalibrationFailureType getCalibrationFailureType() const;

    QImage getCalibrationImage() const;

    void setEyeToTransmit(quint8 eye) {eyeToTransmit = eye;}

    EyeTrackerData getLastData() const;

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
    quint8 eyeToTransmit;

    // The resolution, in case it is required by the
    qreal screenWidth;
    qreal screenHeight;

    // Calibration image if this needs to be displayed.
    QImage calibrationImage;

    // Code that defines which eye failed the calibration.
    ETCalibrationFailureType calibrationFailureType;

    // The last available data
    EyeTrackerData lastData;

    // Shorcut functions for clarity.
    bool canUseLeft() const {return ((eyeToTransmit == EYE_BOTH) || (eyeToTransmit == EYE_L));}
    bool canUseRight() const {return ((eyeToTransmit == EYE_BOTH) || (eyeToTransmit == EYE_R));}
};

#endif // EYETRACKERINTERFACE_H
