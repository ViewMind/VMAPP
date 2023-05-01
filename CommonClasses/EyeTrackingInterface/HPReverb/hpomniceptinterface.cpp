#include "hpomniceptinterface.h"

const float HPOmniceptInterface::SAMPLING_FREQ = 120;

HPOmniceptInterface::HPOmniceptInterface(QObject *parent):EyeTrackerInterface(parent)
{
    connect(&hpprovider,&HPOmniceptProvider::eyeDataAvailable,this,&HPOmniceptInterface::newEyeData);
    eyeTrackerConnected = false;
    eyeTrackerEnabled = false;
}


void HPOmniceptInterface::connectToEyeTracker(){
    if (eyeTrackerConnected) return;

    if (hpprovider.connectToHPRuntime()) {
        eyeTrackerConnected = true;
    }
    else{
        StaticThreadLogger::error("HPOmniceptInterface::connectToEyeTracker","Failed connection to HPRuntime. Reason" + hpprovider.getError());
    }
}


void HPOmniceptInterface::enableUpdating(bool enable){

    if (!eyeTrackerConnected) return;

    if (enable){
        eyeTrackerEnabled = true;
        if (!hpprovider.isRunning()) {
            hpprovider.start();
        }
    }
    else{
        hpprovider.stopProvider();        
        eyeTrackerEnabled = false;
    }
}

void HPOmniceptInterface::newEyeData(QVariantMap eyedata){

    // Passing the current values throught the transformation matrix.
    float xl, yl, zl, xr, yr, zr;
    float oxl, oyl, ozl, oxr, oyr, ozr;

    xl = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::X).toFloat();
    yl = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Y).toFloat();
    zl = eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Z).toFloat();

    xr = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::X).toFloat();
    yr = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Y).toFloat();
    zr = eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Z).toFloat();

    oxl = xl; oyl = yl; ozl = zl; oyr = yr; ozr = zr; oxr = xr;

    QVector4D vecL, vecR;

    if (qAbs(static_cast<double>(zl)) > 1e-6 ){ // This is to avoid warning of comparing float to zero and to avoid a division by zero
        vecL = QVector4D(xl/zl, yl/zl, zl,1);
    }
    else {
        vecL = QVector4D(0, 0, 0,0);
    }

    if (qAbs(static_cast<double>(zr)) > 1e-6 ){ // This is to avoid warning of comparing float to zero and to avoid a division by zero.
        vecR = QVector4D(xr/zr, yr/zr, zr,1);
    }
    else {
        vecR = QVector4D(0, 0, 0,0);
    }

    xl = vecL.x();
    yl = vecL.y();

    xr = vecR.x();
    yr = vecR.y();

    lastData.setXL(static_cast<qreal>(xl));
    lastData.setXR(static_cast<qreal>(xr));
    lastData.setYL(static_cast<qreal>(yl));
    lastData.setYR(static_cast<qreal>(yr));
    //qDebug() << "SETTING TIME STAMP TO" << eyedata.value(HPProvider::Timestamp).toInt();
    lastData.setTimeStamp(eyedata.value(HPProvider::Timestamp).toLongLong());
    lastData.setPupilLeft(eyedata.value(HPProvider::LeftEye).toMap().value(HPProvider::Eye::Pupil).toReal());
    lastData.setPupilRight(eyedata.value(HPProvider::RightEye).toMap().value(HPProvider::Eye::Pupil).toReal());

    emit HPOmniceptInterface::newDataAvailable(lastData);


}

void HPOmniceptInterface::disconnectFromEyeTracker(){
    eyeTrackerConnected = false;
    hpprovider.disconnectFromHPRuntime();
}
