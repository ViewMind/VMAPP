#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{

}


QImage Control::image() const{
    return displayImage;
}

void Control::setRenderArea(qint32 w, qint32 h){
    renderAreaWidth = w;
    renderAreaHeight = h;
}

void Control::setEyeTracker(quint8 eye_tracker){
    selectedEyeTracker = eye_tracker;
    if (selectedEyeTracker == ET_HP_REBERV){
        openvrco = new OpenVRControlObject(this);
        openvrco->setScreenColor(QColor("#aaaaaa"));
        connect(openvrco,SIGNAL(requestUpdate()),this,SLOT(onRequestUpdate()));
        renderState = RENDERING_NONE;
    }
    else if (selectedEyeTracker == ET_HTC_VIVE){
        openvrco = new OpenVRControlObject(this);
        openvrco->setScreenColor(QColor("#aaaaaa"));
        connect(openvrco,SIGNAL(requestUpdate()),this,SLOT(onRequestUpdate()));
        renderState = RENDERING_NONE;
    }
    else{
        MouseInterface *mi = new MouseInterface();
        eyetracker = mi;
        connect(eyetracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
        eyetracker->setEyeToTransmit(VMDC::Eye::BOTH);
        eyetracker->connectToEyeTracker();
        eyetracker->enableUpdating(true);
        mi->mouseSetCalibrationToTrue();
    }
}


void Control::testEyeTracking(){

    renderState = RENDERING_TARGET_TEST;


    if (selectedEyeTracker == ET_MOUSE){
        bool ans = connect(eyetracker,SIGNAL(newDataAvailable(EyeTrackerData)),this,SLOT(onNewEyeTrackingData(EyeTrackerData)));
        qDebug() << "Connect to mouse new data" << ans;
        QSize s = getRenderingSize();
        ct.initialize(s.width(),s.height());
        ct.setTargetTest();
    }
    else{
        if (!openvrco->isRendering()) {
            if (!openvrco->startRendering()){
                qDebug() << "Failed to start rendering";
                return;
            }
        }
        QSize s = getRenderingSize();
        ct.initialize(s.width(),s.height());

        // Scaling the font appropiately
        ct.setTestTargetFontScale(VRSCALING);
        ct.setTargetTest();

        if (selectedEyeTracker == ET_HP_REBERV){
            HPOmniceptInterface *hp = new HPOmniceptInterface(nullptr,s.width(),s.height());
            eyetracker = hp;
        }
        else if (selectedEyeTracker == ET_HTC_VIVE){
            HTCViveEyeProEyeTrackingInterface *htc = new HTCViveEyeProEyeTrackingInterface(nullptr,s.width(),s.height());
            eyetracker = htc;
        }

        connect(eyetracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
        connect(openvrco,SIGNAL(newProjectionMatrixes(QMatrix4x4,QMatrix4x4)),eyetracker,SLOT(updateProjectionMatrices(QMatrix4x4,QMatrix4x4)));
        eyetracker->setEyeToTransmit(VMDC::Eye::BOTH);
        eyetracker->connectToEyeTracker();
        eyetracker->enableUpdating(true);
        EyeTrackerCalibrationParameters etcp;
        etcp.forceCalibration = false;
        etcp.name = "adjustment_coefficients.json";
        eyetracker->calibrate(etcp);
    }

}

void Control::startCalibration(){
    if ((selectedEyeTracker == ET_HP_REBERV) || (selectedEyeTracker == ET_HTC_VIVE)){

        if (!openvrco->isRendering()) {
            if (!openvrco->startRendering()){
                qDebug() << "Failed to start rendering";
                return;
            }
        }

        // CRITICAL: Using the rendering size must be obtained AFTER we start rendering.
        QSize s = getRenderingSize();

        if (selectedEyeTracker == ET_HP_REBERV){
           HPOmniceptInterface *hp = new HPOmniceptInterface(nullptr,s.width(),s.height());
           eyetracker = hp;
        }
        else{
           HTCViveEyeProEyeTrackingInterface *htc = new HTCViveEyeProEyeTrackingInterface(nullptr,s.width(),s.height());
           eyetracker = htc;
        }

        ct.initialize(s.width(),s.height());

        connect(eyetracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
        connect(openvrco,SIGNAL(newProjectionMatrixes(QMatrix4x4,QMatrix4x4)),eyetracker,SLOT(updateProjectionMatrices(QMatrix4x4,QMatrix4x4)));
        eyetracker->setEyeToTransmit(VMDC::Eye::BOTH);
        eyetracker->connectToEyeTracker();
        eyetracker->enableUpdating(true);
        EyeTrackerCalibrationParameters ecp;
        ecp.forceCalibration = true;
        ecp.number_of_calibration_points = 9;
        ecp.name = "adjustment_coefficients.json";
        eyetracker->calibrate(ecp);


        renderState = RENDERING_CALIBRATION;
    }
}

void Control::switchEyeTrackerEnableState(){
    if (eyetracker != nullptr){
        eyetracker->enableUpdating(!eyetracker->isEyeTrackerEnabled());
    }
}

void Control::onNewEyeTrackingData(EyeTrackerData data){
    Q_UNUSED(data)
    //qDebug() << "New data";
    onRequestUpdate();
}


void Control::onRequestUpdate(){
    if (renderState != RENDERING_NONE){
        //QElapsedTimer m;
        //m.start();
        QImage image = generateHMDImage();  // This function will generate the image to the HMD and update displayImage for the screen.
        //qDebug() << "Generated Image in" << m.elapsed();
        //m.start();
        if ((selectedEyeTracker == ET_HP_REBERV) || (selectedEyeTracker == ET_HTC_VIVE)){
            openvrco->setImage(&image);
            //qDebug() << "Set image in" << m.elapsed();
        }
        emit(newImageAvailable());
    }
}


QImage Control::generateHMDImage(){
    if (renderState == RENDERING_TARGET_TEST){
        EyeTrackerData data;
        data = eyetracker->getLastData();
        //qDebug() << "GENIMAGE: LAST DATA IS" << data.toString();
        displayImage = ct.renderCurrentPosition(data.xRight,data.yRight,data.xLeft,data.yLeft,data.time);
        return displayImage;
    }
    else if (renderState == RENDERING_CALIBRATION){
        //qDebug() << "Rendering Calibration";
        return displayImage;
    }
    return QImage();
}


///////////////////////////////////////// EYE TRACKING CONTROL

void Control::onEyeTrackerControl(quint8 code){
    switch (code) {
    case EyeTrackerInterface::ET_CODE_CONNECTION_FAIL:
        qDebug() << "EyeTracking connection failed";
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_DONE:

        displayImage = ct.getClearScreen();

        qDebug() << "Calibration Finished. Disconnecting from EyeTracker";

        switch (eyetracker->getCalibrationFailureType()){
        case EyeTrackerInterface::ETCFT_NONE:
            qDebug() << "Calibration Failure NONE";
            break;
        case EyeTrackerInterface::ETCFT_UNKNOWN:
            qDebug() << "Calibration Failure UNKNOWN";
            break;
        case EyeTrackerInterface::ETCFT_FAILED_BOTH:
            qDebug() << "Calibration Failure BOTH";
            break;
        case EyeTrackerInterface::ETCFT_FAILED_LEFT:
            qDebug() << "Calibration Failure LEFT";
            break;
        case EyeTrackerInterface::ETCFT_FAILED_RIGHT:
            qDebug() << "Calibration Failure RIGHT";
            break;
        }

        eyetracker->enableUpdating(false);
        eyetracker->disconnectFromEyeTracker();
        delete eyetracker;

        break;
    case EyeTrackerInterface::ET_CODE_CONNECTION_SUCCESS:
        qDebug() << "EyeTracking is running";
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_ABORTED:
        break;
    case EyeTrackerInterface::ET_CODE_DISCONNECTED_FROM_ET:
        break;
    case EyeTrackerInterface::ET_CODE_NEW_CALIBRATION_IMAGE_AVAILABLE:
        displayImage = eyetracker->getCalibrationImage();
        break;
    }
}

QSize Control::getRenderingSize(){
    if (selectedEyeTracker == ET_MOUSE){
        QSize s;
        s.setWidth(renderAreaWidth);
        s.setHeight(renderAreaHeight);
        return s;
    }
    else{
        if (openvrco != nullptr){
            QSize s = openvrco->getRecommendedSize();
            s.setWidth(static_cast<qint32>(s.width()*VRSCALING));
            s.setHeight(static_cast<qint32>(s.height()*VRSCALING));
            return s;
        }
    }
    return QSize();
}
