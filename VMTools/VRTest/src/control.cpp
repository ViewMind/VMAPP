#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{
    //qRegisterMetaType<EyeTrackerData>("EyeTrackerData");
    openvrco = new OpenVRControlObject(this);
    connect(&calibrationTimer,&QTimer::timeout,this,&Control::onCalibrationTimerTimeout);
    connect(openvrco,SIGNAL(newProjectionMatrixes(QMatrix4x4,QMatrix4x4)),&eyetracker,SLOT(updateProjectionMatrices(QMatrix4x4,QMatrix4x4)));
    connect(openvrco,SIGNAL(requestUpdate()),this,SLOT(onRequestUpdate()));
    isTargetTest = false;
}

void Control::startTest(){
    openvrco->setScreenColor(QColor(Qt::gray));
    openvrco->start();
    QSize s = openvrco->getRecommendedSize();

    // Starting the EyeTracker.
    if (!eyetracker.initalizeEyeTracking()){
        qDebug() << "Error intializing eyetracker";
        return;
    }

    // Creating the calibration points
    qreal w = static_cast<qreal>(s.width());
    qreal h = static_cast<qreal>(s.height());

    QPoint p1(static_cast<qint32>(w*0.25),static_cast<qint32>(h*0.25)); // Top Left
    QPoint p2(static_cast<qint32>(w*0.75),static_cast<qint32>(h*0.25)); // Top right
    QPoint p3(static_cast<qint32>(w*0.50),static_cast<qint32>(h*0.50)); // Center
    QPoint p4(static_cast<qint32>(w*0.25),static_cast<qint32>(h*0.75)); // Bottom Left
    QPoint p5(static_cast<qint32>(w*0.75),static_cast<qint32>(h*0.75)); // Bottom Right.

    calibrationPoints << p1 << p2 << p3 << p4 << p5;
    calibrationPointIndex = -1;
    isWaiting = false;
    tt.initialize(s.width(),s.height());
    calibrationTimer.start(CALIBRATION_WAIT);

    eyetracker.start();
}

void Control::onCalibrationTimerTimeout(){
    calibrationTimer.stop();
    if (isWaiting){
        calibrationTimer.start(CALIBRATION_SHOW);
        isWaiting = false;
        eyetracker.startStoringCalibrationData();
    }
    else{
        calibrationPointIndex++;
        if (calibrationPointIndex < calibrationPoints.size()){
            QPoint p = calibrationPoints.at(calibrationPointIndex);
            qDebug() << "Calibration point " << calibrationPointIndex << ":" << p;
            QImage image = tt.setSingleTarget(p.x(),p.y());
            openvrco->setImage(&image);
            isWaiting = true;
            calibrationTimer.start(CALIBRATION_WAIT);
            eyetracker.newCalibrationPoint(p.x(),p.y());
        }
        else{
            if (!eyetracker.calibrationDone()){
                qDebug() << "CALIBRATION FAILED";
                QImage image = tt.getClearScreen();
                openvrco->setImage(&image);
                return;
            }
            qDebug() << "Calibration done";
            tt.setTargetTest();
            QImage image = tt.renderCurrentPosition(0,0,0,0);
            openvrco->setImage(&image);
            isTargetTest = true;
        }
    }
}


void Control::onRequestUpdate(){
    if (isTargetTest){
        EyeTrackerData data = eyetracker.getLastData();
        QImage image = tt.renderCurrentPosition(data.xRight,data.yRight,data.xLeft,data.yLeft);
        openvrco->setImage(&image);
    }
}

void Control::stopTest(){
    calibrationTimer.stop();
    openvrco->stopRendering();
    isTargetTest = false;
    eyetracker.stop();
}
