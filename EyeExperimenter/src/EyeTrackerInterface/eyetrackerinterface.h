#ifndef EYETRACKERINTERFACE_H
#define EYETRACKERINTERFACE_H

#include <QObject>
#include <QString>
#include "eyetrackerdata.h"
#include "../../../CommonClasses/common.h"

class EyeTrackerInterface : public QObject
{
    Q_OBJECT
public:

    static const quint8 ET_CODE_CALIBRATION_DONE     = 0;
    static const quint8 ET_CODE_CALIBRATION_ABORTED  = 1;
    static const quint8 ET_CODE_CALIBRATION_FAILED   = 2;
    static const quint8 ET_CODE_CONNECTION_SUCCESS   = 3;
    static const quint8 ET_CODE_CONNECTION_FAIL      = 4;
    static const quint8 ET_CODE_DISCONNECTED_FROM_ET = 5;

    explicit EyeTrackerInterface(QObject *parent = 0, qreal width = 1, qreal height = 1);

    virtual void connectToEyeTracker();

    virtual void enableUpdating(bool enable);

    virtual void disconnectFromEyeTracker();

    virtual void calibrate(EyeTrackerCalibrationParameters params);

signals:
    void newDataAvailable(EyeTrackerData data);

    // Connection and calibration signals
    void eyeTrackerControl(quint8 code);

    // SHOULD ONLY BE SENT WHEN IT REPRESENTS AN ERROR.
    void disconnectedFromET();

protected:
    LogInterface logger;

    // The resolution, in case it is required by the
    qreal screenWidth;
    qreal screenHeight;
};

#endif // EYETRACKERINTERFACE_H
