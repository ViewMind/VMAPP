#ifndef REDINTERFACE_H
#define REDINTERFACE_H

#include <QTimer>
#include <QDebug>
#include "../eyetrackerinterface.h"
#include "iViewXAPI.h"

class REDInterface : public EyeTrackerInterface
{

public:
    REDInterface();

    ExitStatus connectToEyeTracker();

    void enableUpdating(bool enable);

    void disconnectFromEyeTracker();

    ExitStatus calibrate(EyeTrackerCalibrationParameters params);


private slots:
    void on_pollTimer_Up();

private:
    QTimer pollTimer;

    EyeTrackerData lastData;

    QString num2Error(int result);

};

#endif // REDINTERFACE_H
