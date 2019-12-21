#ifndef VIVEEYEPOLLER_H
#define VIVEEYEPOLLER_H

#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QPoint>

#include "eyetrackerdata.h"

// Eye tracking libraries
#include "sranipal/SRanipal.h"
#include "sranipal/SRanipal_Eye.h"
#include "sranipal/SRanipal_Enums.h"

#include "../../../CommonClasses/LogInterface/loginterface.h"
#include "../../../CommonClasses/CalibrationLeastSquares/calibrationleastsquares.h"


class VIVEEyePoller : public QThread
{
    Q_OBJECT

public:
    VIVEEyePoller();    
    bool initalizeEyeTracking();
    void run() override;
    void stop() { keepGoing = false; }
    EyeTrackerData getLastData() {return lastData;}
    bool isRunning() { return keepGoing;  }
    void setProjectionMatrices(quint8 whichEye, QMatrix4x4 p);
    bool calibrationDone();
    void startStoringCalibrationData();
    void newCalibrationPoint(qint32 xtarget, qint32 ytarget);

signals:
    void newEyeData(EyeTrackerData data);

public slots:
    void updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l);

private:

    struct ManyPointCompare{
        QList<float> values;
        void init(qint32 n, float v){
            for (qint32 i = 0; i < n; i++){
                values << v;
            }
        }
        bool areTheSame(ManyPointCompare fpc){
            if (fpc.values.size() != values.size()) return false;
            for (int i = 0; i < values.size(); i++){
                if (values.at(i) != fpc.values.at(i)) return false;
            }
            return true;
        }
    };

    // Control variables.
    QTimer timer;
    QElapsedTimer timestampTimer;
    bool keepGoing;

    QElapsedTimer mtimer;

    // Projection Matrices
    QMatrix4x4 pRe;
    QMatrix4x4 pLe;

    // To inialize only ONCE.
    bool wasInitialized;

    // The last valid data point.
    EyeTrackerData lastData;

    // Data for computing the calibration points.
    bool isCalibrating;
    bool shouldStoreCalibrationData;
    bool isCalibrated;
    QList<CalibrationLeastSquares::CalibrationData> calibrationPointData;
    CalibrationLeastSquares::CalibrationData currentCalibrationData;
    CalibrationLeastSquares::EyeCorrectionCoeffs eyeCorrectionCoeffs;
    void updateEyeTrackerData(qreal xr, qreal yr, qreal xl, qreal yl, qreal pl, qreal pr, qint64 timestamp);
    void resetEyeCorrectionCoeffs();

    // Used for debuggin.
    LogInterface logger;

    // Used to filter data that is the same.
    ManyPointCompare previous;





};

#endif // VIVEEYEPOLLER_H
