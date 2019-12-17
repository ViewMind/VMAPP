#ifndef VIVEEYEPOLLER_H
#define VIVEEYEPOLLER_H

#include <QThread>
#include <QTimer>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QPoint>

#include "eyetrackerdata.h"

// Eye tracking libraries
#include "sranipal/SRanipal.h"
#include "sranipal/SRanipal_Eye.h"
#include "sranipal/SRanipal_Enums.h"

#include "../../../CommonClasses/LogInterface/loginterface.h"


class VIVEEyePoller : public QThread
{
    Q_OBJECT

public:
    VIVEEyePoller();    
    bool initalizeEyeTracking();
    void run() override;
    void stop() { keepGoing = false; }
    //EyeTrackerData getLastData() {return lastData;}
    void setProjectionMatrices(quint8 whichEye, QMatrix4x4 p);

    void calibrationDone();
    void startStoringCalibrationData();
    void newCalibrationPoint(qint32 xtarget, qint32 ytarget);

signals:
    void newEyeData(EyeTrackerData data);

public slots:
    void updateProjectionMatrices(QMatrix4x4 r, QMatrix4x4 l);

private:
    QTimer timer;
    bool keepGoing;

//    // Transformation parameters
//    qreal MaxX;
//    qreal MaxY;
//    qreal bX, mX;
//    qreal bY, mY;

//    EyeTrackerData computeValues(qreal xr, qreal yr, qreal xl, qreal yl);
//    EyeTrackerData lastData;

    // Projection Matrices
    QMatrix4x4 pRe;
    QMatrix4x4 pLe;

    // Data for computing the calibration points.
    bool isCalibrating;
    bool shouldStoreCalibrationData;

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

    struct CalibrationData {
        QList<qreal> xr;
        QList<qreal> yr;
        QList<qreal> xl;
        QList<qreal> yl;
        void clear(){
            xr.clear(); yr.clear(); xl.clear(); yl.clear();
        }
        QString toString() const{
            QString ans = "[";
            for (qint32 i = 0; i < xr.size(); i++){
                ans = ans + QString::number(xr.at(i)) + " " + QString::number(yr.at(i)) + " " + QString::number(xl.at(i)) + " " + QString::number(yl.at(i)) + ";\n";
            }
            ans.remove(ans.length()-1,1);
            ans= ans + "];";
            return ans;
        }

    };

    QList< CalibrationData> calibrationPointData;
    CalibrationData currentCalibrationData;

    LogInterface logger;

    ManyPointCompare previous;

    // For the linear conversion
//    void setMaxWidthAndHeight(qreal W, qreal H);

};

#endif // VIVEEYEPOLLER_H
