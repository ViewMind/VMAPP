#ifndef VIVEEYEPOLLER_H
#define VIVEEYEPOLLER_H

#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include "eyetrackerdata.h"

// Eye tracking libraries
#include "sranipal/SRanipal.h"
#include "sranipal/SRanipal_Eye.h"
#include "sranipal/SRanipal_Enums.h"

class VIVEEyePoller : public QThread
{
    Q_OBJECT

public:
    VIVEEyePoller();
    void setMaxWidthAndHeight(qreal W, qreal H);
    void run() override;
    void stop() { keepGoing = false; }
    EyeTrackerData getLastData() {return lastData;}

signals:
    void newEyeData(EyeTrackerData data);

private:
    QTimer timer;
    bool keepGoing;

    // Transformation parameters
    qreal MaxX;
    qreal MaxY;
    qreal bX, mX;
    qreal bY, mY;

    EyeTrackerData computeValues(qreal xr, qreal yr, qreal xl, qreal yl);
    EyeTrackerData lastData;

};

#endif // VIVEEYEPOLLER_H
