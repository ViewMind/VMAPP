#ifndef VIVEEYEPOLLER_H
#define VIVEEYEPOLLER_H

#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QPoint>
#include <QVariant>

// Std libraries.
#include <mutex>
#include <iostream>

// Eye tracking libraries
#include "sranipal/SRanipal.h"
#include "sranipal/SRanipal_Eye.h"
#include "sranipal/SRanipal_Enums.h"


namespace HTCVIVE {

   static const QString LeftEye   = "Left Eye";
   static const QString RightEye  = "Right Eye";
   static const QString Timestamp = "TimeStamp";

   namespace Eye {

      static const QString X = "x";
      static const QString Y = "y";
      static const QString Z = "z";
      static const QString Pupil = "Pupil";

   }

}

class VIVEEyePoller : public QThread
{
    Q_OBJECT

public:
    VIVEEyePoller();    
    bool initalizeEyeTracking();

    void run() override;

    void stop();

    bool isRunning() const;

    QString getError() const;


signals:
    void eyeDataAvailable(QVariantMap eyedata);

private:

    QString error;

    // Flag to ensure that this is initialized only ONCE.
    bool wasInitialized;

    // Timer used to compute the time stamp.
    QElapsedTimer timestampTimer;

    // Variable that starts and stop the EyeTracking Data flow.
    bool keepGoing;

    // Used to filter data that is the same.
    QVariantMap previousSample;

    // Tolerance used to compare two floating point samples.
    float compareTolerance;

    // Function to compare two data points to one another to know if the sample is new or not.
    bool isSampleSameAsPrevious(const QString &whichEye, float x, float y, float z);

    // Functions that need to register a call back need to be static.
    static void EyeCallback(ViveSR::anipal::Eye::EyeData const &eye_data);

    // The static variable will be used to comunicate the new data to the control loop.
    static ViveSR::anipal::Eye::EyeData newEyeData;
    static bool isNewDataAvailable;

    // Mutex to ensure atomic access to newEyeData from this thread and the main thread
    static std::mutex mtx;

};

#endif // VIVEEYEPOLLER_H
