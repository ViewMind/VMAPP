#ifndef EYETRACKERINTERFACE_H
#define EYETRACKERINTERFACE_H

#include <QObject>
#include <QString>
#include "eyetrackerdata.h"
#include "../../../CommonClasses/Experiments/common.h"

class EyeTrackerInterface : public QObject
{
    Q_OBJECT
public:

    typedef enum {ES_SUCCESS, ES_WARNING, ES_FAIL} ExitStatus;

    explicit EyeTrackerInterface(QObject *parent = 0);

    virtual ExitStatus connectToEyeTracker();

    virtual void enableUpdating(bool enable);

    virtual void disconnectFromEyeTracker();

    virtual ExitStatus calibrate(EyeTrackerCalibrationParameters params);

    QString getMessage() const {return message;}


signals:
    void newDataAvailable(EyeTrackerData data);

protected:

    QString message;

};

#endif // EYETRACKERINTERFACE_H
