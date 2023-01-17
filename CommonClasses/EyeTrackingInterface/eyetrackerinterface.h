#ifndef EYETRACKERINTERFACE_H
#define EYETRACKERINTERFACE_H

#include <QObject>
#include <QString>
#include <QImage>
#include <QVector3D>
#include <QMatrix4x4>
#include "eyetrackerdata.h"
#include "../LogInterface/staticthreadlogger.h"
#include "../RawDataContainer/VMDC.h"

/**
 * @brief The EyeTrackerInterface class
 * This can be implemented as the interface to an EyeTracking SDK or simply to use
 * as the EyeTracking Calibration Control when the Remote Reder Server takes care of eyetracking.
 */

class EyeTrackerInterface : public QObject
{
    Q_OBJECT

public:
    explicit EyeTrackerInterface(QObject *parent = nullptr);

    virtual void connectToEyeTracker();

    virtual void enableUpdating(bool enable);

    virtual void disconnectFromEyeTracker();

    bool isEyeTrackerConnected() const;

    void setEyeToTransmit(QString eye);

    EyeTrackerData getLastData() const;

    bool isEyeTrackerEnabled() const;


signals:
    void newDataAvailable(EyeTrackerData data);

protected:

    // Which EyeDataToTransmit
    QString eyeToTransmit;

    // The last available data
    EyeTrackerData lastData;

    // For checeking if the eyetracker is connected.
    bool eyeTrackerConnected;

    // Enabling/Disabling sending data
    bool eyeTrackerEnabled;

    // Shorcut functions for clarity.
    bool canUseLeft() const {return ((eyeToTransmit == VMDC::Eye::BOTH) || (eyeToTransmit == VMDC::Eye::LEFT));}
    bool canUseRight() const {return ((eyeToTransmit == VMDC::Eye::BOTH) || (eyeToTransmit == VMDC::Eye::RIGHT));}
};

#endif // EYETRACKERINTERFACE_H
