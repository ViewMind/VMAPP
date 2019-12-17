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
    void run() override;
    void stop() { keepGoing = false; }

private:
    QTimer timer;
    bool keepGoing;
};

#endif // VIVEEYEPOLLER_H
