#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{
    openvrco = new OpenVRControlObject(this);
    connect(openvrco,SIGNAL(requestUpdate()),this,SLOT(onRequestUpdate()));
    renderState = RENDERING_NONE;
    experiment = nullptr;
}

void Control::initialize(){
    if (!openvrco->isRendering()) openvrco->startRendering();
    QSize s = openvrco->getRecommendedSize();
    eyetracker = new HTCViveEyeProEyeTrackingInterface(this,s.width(),s.height());
    connect(eyetracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
    connect(openvrco,SIGNAL(newProjectionMatrixes(QMatrix4x4,QMatrix4x4)),eyetracker,SLOT(updateProjectionMatrices(QMatrix4x4,QMatrix4x4)));
    eyetracker->setEyeToTransmit(EYE_BOTH);
    eyetracker->connectToEyeTracker();
}

////////////////////////////////////////////////// Calibration Functions.

void Control::startCalibration(){
    renderState = RENDERING_NONE;
    openvrco->setScreenColor(QColor(Qt::gray));
    EyeTrackerCalibrationParameters params;
    params.forceCalibration = true;
    params.name = "";
    eyetracker->calibrate(params);
}

void Control::loadLastCalibration(){
    EyeTrackerCalibrationParameters params;
    params.forceCalibration = false;
    params.name = "coeffs.kof";
    eyetracker->calibrate(params);
    eyetracker->enableUpdating(true);
}

////////////////////////////////////////////////// Update display image

QImage Control::image() const{
    return displayImage;
}

void Control::loadViewMindWaitScreen(){
    QColor base(Qt::white);
    base = base.darker(110);

    QString message = "This ia test message\nWhen something has finished";
    message = "La calibración ha finalizdo";
    
    QFont waitFont("Mono",32);
    QFontMetrics fmetrics(waitFont);
    QRect btextrect = fmetrics.boundingRect(message);

    
    displayImage = QImage(":/viewmind.png");
    if (displayImage.isNull()){
        qDebug() << "Loaded null image";
        return;
    }
    QSize s = openvrco->getRecommendedSize();
    openvrco->setScreenColor(base);
    displayImage = displayImage.scaled(s.width(),s.height(),Qt::KeepAspectRatio);

    QImage canvas (s.width(), s.height(), QImage::Format_RGB32);
    QPainter painter(&canvas);
    painter.fillRect(0,0,s.width(),s.height(),QBrush(base));

    // X and Y values for the text.
    qreal xoffset = (s.width() - displayImage.width())/2;
    qreal yoffset = (s.height() - displayImage.height())/2;
    
    QRectF source(0,0,displayImage.width(),displayImage.height());
    QRectF target(xoffset,yoffset,displayImage.width(),displayImage.height());
    painter.drawImage(target,displayImage,source);


    // Drawing the text, below the image.
    painter.setPen(QColor(Qt::black));
    painter.setFont(waitFont);
    xoffset = (s.width() - btextrect.width())/2;
    yoffset = yoffset + displayImage.height() + btextrect.height();

    QRect targetTextRect(0,static_cast<qint32>(yoffset),s.width(),static_cast<qint32>(btextrect.height()*2.2));

    //message = "La calibración ha finalizdo"; targetTextRect = QRect(0,2390,1532,107);

    qDebug() << targetTextRect;

    painter.drawText(targetTextRect,Qt::AlignCenter,message);

    painter.end();
    displayImage = canvas;

    displayImage.save("test.png");
    renderState = RENDER_WAIT_SCREEN;
}

////////////////////////////////////////////////// Keyboard press listener
void Control::keyboardKeyPressed(int key){
    if (experiment != nullptr){
        experiment->keyboardKeyPressed(key);
    }
}


////////////////////////////////////////////////// Experiment controls
void Control::startReadingExperiment(QString lang){
    if (experiment != nullptr) delete experiment;
    experiment = new ReadingExperiment();

    // Connecting the eyetracker to teh experiment.
    configExperiments.clear();
    QSize s = openvrco->getRecommendedSize();
    qreal w = static_cast<qreal>(s.width());
    qreal h = static_cast<qreal>(s.height());
    connect(eyetracker,SIGNAL(newDataAvailable(EyeTrackerData)),experiment,SLOT(newEyeDataAvailable(EyeTrackerData)));
    connect(experiment,SIGNAL(updateVRDisplay()),this,SLOT(onRequestUpdate()));
    connect(experiment,&Experiment::experimentEndend,this,&Control::onExperimentFinished);

    // Configuring the experiment.
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,true);

    configExperiments.addKeyValuePair(CONFIG_EYETRACKER_CONFIGURED,CONFIG_P_ET_HTCVIVEEYEPRO);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_WIDTH,1920);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_HEIGHT,1080);
    configExperiments.addKeyValuePair(CONFIG_VR_RECOMMENDED_WIDTH,w);
    configExperiments.addKeyValuePair(CONFIG_VR_RECOMMENDED_HEIGHT,h);

    configExperiments.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,"outputs");
    configExperiments.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,QString(":/experiment_data/Reading_") + lang + ".dat");
    configExperiments.addKeyValuePair(CONFIG_DEMO_MODE,false);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,true);
    configExperiments.addKeyValuePair(CONFIG_VALID_EYE,2);
    configExperiments.addKeyValuePair(CONFIG_READING_EXP_LANG,lang);
    configExperiments.addKeyValuePair(CONFIG_READING_PX_TOL,60);


    renderState = RENDERING_EXPERIMENT;
    openvrco->setScreenColor(QColor(Qt::gray).darker(110));
    experiment->startExperiment(&configExperiments);

}

void Control::startBindingExperiment(bool isBound, qint32 targetNum, bool areTargetsSmall){
    if (experiment != nullptr) delete experiment;
    experiment = new ImageExperiment(isBound);

    QString expFileName = "";
    if (isBound) expFileName = ":/experiment_data/bc";
    else expFileName = ":/experiment_data/uc";

    if (targetNum == 2) expFileName = expFileName + ".dat";
    else expFileName = expFileName + "_3.dat";

    // Connecting the eyetracker to teh experiment.
    configExperiments.clear();
    QSize s = openvrco->getRecommendedSize();
    qreal w = static_cast<qreal>(s.width());
    qreal h = static_cast<qreal>(s.height());
    connect(eyetracker,SIGNAL(newDataAvailable(EyeTrackerData)),experiment,SLOT(newEyeDataAvailable(EyeTrackerData)));
    connect(experiment,SIGNAL(updateVRDisplay()),this,SLOT(onRequestUpdate()));
    connect(experiment,&Experiment::experimentEndend,this,&Control::onExperimentFinished);

    // Configuring the experiment.
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,true);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,w);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,h);
    configExperiments.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,"outputs");
    configExperiments.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,expFileName);
    configExperiments.addKeyValuePair(CONFIG_DEMO_MODE,false);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,false);
    configExperiments.addKeyValuePair(CONFIG_VALID_EYE,2);

    configExperiments.addKeyValuePair(CONFIG_YPX_2_MM,0.25);
    configExperiments.addKeyValuePair(CONFIG_XPX_2_MM,0.25);

    //    configExperiments.addKeyValuePair(CONFIG_YPX_2_MM,0.2);
    //    configExperiments.addKeyValuePair(CONFIG_XPX_2_MM,0.2);

    configExperiments.addKeyValuePair(CONFIG_BINDING_TARGET_SMALL,areTargetsSmall);
    configExperiments.addKeyValuePair(CONFIG_BINDING_NUMBER_OF_TARGETS,targetNum);


    renderState = RENDERING_EXPERIMENT;
    openvrco->setScreenColor(QColor(Qt::gray));
    experiment->startExperiment(&configExperiments);
}

void Control::startFieldingExperiment(){
    if (experiment != nullptr) delete experiment;
    experiment = new FieldingExperiment();

    QString expFileName =  ":/experiment_data/fielding.dat";

    // Connecting the eyetracker to teh experiment.
    configExperiments.clear();
    QSize s = openvrco->getRecommendedSize();
    qreal w = static_cast<qreal>(s.width());
    qreal h = static_cast<qreal>(s.height());
    connect(eyetracker,SIGNAL(newDataAvailable(EyeTrackerData)),experiment,SLOT(newEyeDataAvailable(EyeTrackerData)));
    connect(experiment,SIGNAL(updateVRDisplay()),this,SLOT(onRequestUpdate()));
    connect(experiment,&Experiment::experimentEndend,this,&Control::onExperimentFinished);

    // Configuring the experiment.
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,true);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,w);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,h);
    configExperiments.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,"outputs");
    configExperiments.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,expFileName);
    configExperiments.addKeyValuePair(CONFIG_DEMO_MODE,false);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,false);
    configExperiments.addKeyValuePair(CONFIG_VALID_EYE,2);
    configExperiments.addKeyValuePair(CONFIG_FIELDING_YPX_2_MM,0.20);
    configExperiments.addKeyValuePair(CONFIG_FIELDING_XPX_2_MM,0.20);
    configExperiments.addKeyValuePair(CONFIG_FIELDING_PAUSE_TEXT,"Press any key to continue");


    renderState = RENDERING_EXPERIMENT;
    openvrco->setScreenColor(QColor(Qt::black));
    experiment->startExperiment(&configExperiments);

}

void Control::onExperimentFinished(const Experiment::ExperimentResult &result){
    qDebug() << "Experiment finished: "  <<  result;
    renderState = RENDERING_NONE;
}


////////////////////////////////////////////////// Eye Tracking Testing.

void Control::onEyeTrackerControl(quint8 code){
    switch (code) {
    case EyeTrackerInterface::ET_CODE_CONNECTION_FAIL:
        qDebug() << "EyeTracking connection failed";
        break;
    case EyeTrackerInterface::ET_CODE_CALIBRATION_DONE:
        displayImage = eyetracker->getCalibrationImage();
        openvrco->setImage(&displayImage);
        emit(newImageAvailable());
        qDebug() << "CALIBRATION SUCESSFULL";

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
        openvrco->setImage(&displayImage);
        emit(newImageAvailable());
        break;
    }
}

void Control::testEyeTracking(){    
    renderState = RENDERING_TARGET_TEST;
    openvrco->setScreenColor(QColor(Qt::gray));
    QSize s = openvrco->getRecommendedSize();
    tt.initialize(s.width(),s.height());
    tt.setTargetTest();
}


////////////////////////////////////////////////// Rendering.


void Control::onRequestUpdate(){
    if (renderState != RENDERING_NONE){
        QImage image = generateHMDImage();  // This function will generate the image to the HMD and update displayImage for the screen.
        openvrco->setImage(&image);
        emit(newImageAvailable());
    }
}


QImage Control::generateHMDImage(){
    if (renderState == RENDERING_TARGET_TEST){
        EyeTrackerData data = eyetracker->getLastData();
        //qDebug() << "GENIMAGE: LAST DATA IS" << data.toString();
        displayImage = tt.renderCurrentPosition(data.xRight,data.yRight,data.xLeft,data.yLeft);
        return displayImage;
    }
    if (renderState == RENDERING_EXPERIMENT){
        EyeTrackerData data = eyetracker->getLastData();
        QImage image = experiment->getVRDisplayImage();
        displayImage = image;
        QPainter painter(&displayImage);
        qreal r = 0.01*displayImage.width();
        painter.setBrush(QBrush(QColor(0,255,0,100)));
        painter.drawEllipse(static_cast<qint32>(data.xLeft-r),static_cast<qint32>(data.yLeft-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r));
        painter.setBrush(QBrush(QColor(0,0,255,100)));
        painter.drawEllipse(static_cast<qint32>(data.xRight-r),static_cast<qint32>(data.yRight-r),static_cast<qint32>(2*r),static_cast<qint32>(2*r));
        return image;
    }
    if (renderState == RENDER_WAIT_SCREEN){
        return displayImage;
    }
    else return QImage();
}
