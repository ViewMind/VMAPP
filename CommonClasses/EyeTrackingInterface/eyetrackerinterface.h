#ifndef EYETRACKERINTERFACE_H
#define EYETRACKERINTERFACE_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QVector3D>
#include <QMatrix4x4>
#include "eyetrackerdata.h"
#include "../LogInterface/staticthreadlogger.h"
#include "../RenderServerClient/RenderServerGraphics/renderserverscene.h"
#include "../RawDataContainer/VMDC.h"

/**
 * @brief The EyeTrackerInterface class
 * The EyeTrackingMOde (2D or 3D) is determined simply by setting the field of view to values greater than zero.
 */

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

    // Depending on the calibration results, it is sometimes possible to recomend different calibration suggestion.
    // These will only be enabled when one or more of the calibration points cannot be computed.
    typedef enum {CRM_NONE, CRM_HMD_ADJUST, CRM_RECALIBRATE, CRM_HMD_TOO_LOW, CRM_HMD_TOO_HIGH} CalibrationRetryMessage;

    explicit EyeTrackerInterface(QObject *parent = nullptr, qreal width = 1, qreal height = 1);

    virtual void connectToEyeTracker();

    virtual void enableUpdating(bool enable);

    virtual void disconnectFromEyeTracker();

    virtual void calibrate(EyeTrackerCalibrationParameters params);

    virtual void configureCalibrationValidation(QVariantMap calibrationValidationParamters);

    virtual QVariantMap getCalibrationValidationData() const;

    virtual QString getCalibrationValidationReport() const;

    virtual QString getCalibrationRecommendedEye() const;

    virtual void setCalibrationVectors(const QList<QVector3D> &calibVecs, qreal validationR);

    virtual void controlCalibrationPointDataStore(qint32 cpoint, bool enable);

    QList<QPointF> getCalibrationPoints() const;

    CalibrationRetryMessage getCalibrationRetryMessage() const;

    RenderServerScene getCalibrationImage() const;

    void setEyeToTransmit(QString eye);

    void setEyeTrackerTo3DMode(bool enable);

    bool isEyeTrackerIn3DMode() const;

    EyeTrackerData getLastData() const;

    bool isEyeTrackerEnabled() const;

signals:
    void newDataAvailable(EyeTrackerData data);

    // Connection and calibration signals
    void eyeTrackerControl(quint8 code);

    // SHOULD ONLY BE SENT WHEN IT REPRESENTS AN ERROR.
    void disconnectedFromET();

protected:

    // Which EyeDataToTransmit
    QString eyeToTransmit;

    // The resolution, in case it is required by the eyetracker.
    qreal screenWidth;
    qreal screenHeight;

    // The calibration retry message index.
    CalibrationRetryMessage calib_retry_msg;

    // Calibration image if this needs to be displayed.
    //QImage calibrationImage;
    RenderServerScene calibrationImage;

    // The last available data
    EyeTrackerData lastData;

    // If the interface uses the calibration points, these will be available right here.
    QList<QPointF> calibrationPoints;

    bool eyeTrackerEnabled;

    // Flag so that we know if the eyetracker should work in 3D Mode Or not.
    bool mode3D;

    // Shorcut functions for clarity.
    bool canUseLeft() const {return ((eyeToTransmit == VMDC::Eye::BOTH) || (eyeToTransmit == VMDC::Eye::LEFT));}
    bool canUseRight() const {return ((eyeToTransmit == VMDC::Eye::BOTH) || (eyeToTransmit == VMDC::Eye::RIGHT));}
};

#endif // EYETRACKERINTERFACE_H
