#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{
#ifndef DESIGN_MODE_ENABLED
    openvrco = new OpenVRControlObject(this);
    connect(openvrco,SIGNAL(requestUpdate()),this,SLOT(onRequestUpdate()));
    renderState = RENDERING_NONE;
#else
    eyetracker = new MouseInterface();
    connect(eyetracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
    eyetracker->setEyeToTransmit(EYE_BOTH);
    eyetracker->connectToEyeTracker();
    eyetracker->mouseSetCalibrationToTrue();
    eyetracker->enableUpdating(true);
#endif
    experiment = nullptr;
}

void Control::initialize(){
#ifndef DESIGN_MODE_ENABLED
    if (!openvrco->isRendering()) openvrco->startRendering();
    QSize s = openvrco->getRecommendedSize();
    eyetracker = new HTCViveEyeProEyeTrackingInterface(this,s.width(),s.height());
    connect(eyetracker,SIGNAL(eyeTrackerControl(quint8)),this,SLOT(onEyeTrackerControl(quint8)));
    connect(openvrco,SIGNAL(newProjectionMatrixes(QMatrix4x4,QMatrix4x4)),eyetracker,SLOT(updateProjectionMatrices(QMatrix4x4,QMatrix4x4)));
    eyetracker->setEyeToTransmit(EYE_BOTH);
    eyetracker->connectToEyeTracker();
#endif
}

////////////////////////////////////////////////// Calibration Functions.

void Control::startCalibration(){
#ifndef DESIGN_MODE_ENABLED
    renderState = RENDERING_NONE;
    openvrco->setScreenColor(QColor(Qt::gray));
    EyeTrackerCalibrationParameters params;
    params.forceCalibration = true;
    params.name = "";
    eyetracker->calibrate(params);
#endif
}

void Control::loadLastCalibration(){
#ifndef DESIGN_MODE_ENABLED
    EyeTrackerCalibrationParameters params;
    params.forceCalibration = false;
    params.name = "coeffs.kof";
    eyetracker->calibrate(params);
    eyetracker->enableUpdating(true);
#endif
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
    connect(eyetracker,SIGNAL(newDataAvailable(EyeTrackerData)),experiment,SLOT(newEyeDataAvailable(EyeTrackerData)));
    connect(experiment,SIGNAL(updateVRDisplay()),this,SLOT(onRequestUpdate()));
    connect(experiment,&Experiment::experimentEndend,this,&Control::onExperimentFinished);

    // Configuring the experiment.
#ifndef DESIGN_MODE_ENABLED
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,true);
    configExperiments.addKeyValuePair(CONFIG_EYETRACKER_CONFIGURED,CONFIG_P_ET_HTCVIVEEYEPRO);
    QSize s = openvrco->getRecommendedSize();
    qreal w = static_cast<qreal>(s.width());
    qreal h = static_cast<qreal>(s.height());
    configExperiments.addKeyValuePair(CONFIG_VR_RECOMMENDED_WIDTH,w);
    configExperiments.addKeyValuePair(CONFIG_VR_RECOMMENDED_HEIGHT,h);
    openvrco->setScreenColor(QColor(Qt::gray).darker(110));
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_WIDTH,1920);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_HEIGHT,1080);
#else
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,false);
    configExperiments.addKeyValuePair(CONFIG_EYETRACKER_CONFIGURED,CONFIG_P_ET_MOUSE);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_WIDTH,1366);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_HEIGHT,768);
#endif


    configExperiments.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,"outputs");
    configExperiments.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,QString(":/experiment_data/Reading_") + lang + ".dat");
    configExperiments.addKeyValuePair(CONFIG_DEMO_MODE,false);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,true);
    configExperiments.addKeyValuePair(CONFIG_VALID_EYE,2);
    configExperiments.addKeyValuePair(CONFIG_READING_EXP_LANG,lang);
    configExperiments.addKeyValuePair(CONFIG_READING_PX_TOL,60);

    renderState = RENDERING_EXPERIMENT;    
    //experiment->startExperiment(&configExperiments);
    if (!experiment->startExperiment(&configExperiments)){
        qDebug() << "Experiment Start Error: " + experiment->getError();
    }


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
    connect(eyetracker,SIGNAL(newDataAvailable(EyeTrackerData)),experiment,SLOT(newEyeDataAvailable(EyeTrackerData)));
    connect(experiment,SIGNAL(updateVRDisplay()),this,SLOT(onRequestUpdate()));
    connect(experiment,&Experiment::experimentEndend,this,&Control::onExperimentFinished);

    // Configuring the experiment.

#ifndef DESIGN_MODE_ENABLED
    QSize s = openvrco->getRecommendedSize();
    qreal w = static_cast<qreal>(s.width());
    qreal h = static_cast<qreal>(s.height());
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,w);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,h);
    openvrco->setScreenColor(QColor(Qt::gray));
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,true);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,false);
#else
    configExperiments.addKeyValuePair(CONFIG_EYETRACKER_CONFIGURED,CONFIG_P_ET_MOUSE);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_WIDTH,1366);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_HEIGHT,768);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,1366);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,768);
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,false);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,true);
#endif

    configExperiments.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,"outputs");
    configExperiments.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,expFileName);
    configExperiments.addKeyValuePair(CONFIG_DEMO_MODE,false);
    configExperiments.addKeyValuePair(CONFIG_VALID_EYE,2);

    configExperiments.addKeyValuePair(CONFIG_YPX_2_MM,0.25);
    configExperiments.addKeyValuePair(CONFIG_XPX_2_MM,0.25);

    //    configExperiments.addKeyValuePair(CONFIG_YPX_2_MM,0.2);
    //    configExperiments.addKeyValuePair(CONFIG_XPX_2_MM,0.2);

    configExperiments.addKeyValuePair(CONFIG_BINDING_TARGET_SMALL,areTargetsSmall);
    configExperiments.addKeyValuePair(CONFIG_BINDING_NUMBER_OF_TARGETS,targetNum);

    renderState = RENDERING_EXPERIMENT;
    experiment->startExperiment(&configExperiments);
}

void Control::startFieldingExperiment(){
    if (experiment != nullptr) delete experiment;
    experiment = new FieldingExperiment();

    QString expFileName =  ":/experiment_data/fielding.dat";

    // Connecting the eyetracker to teh experiment.
    configExperiments.clear();
    connect(eyetracker,SIGNAL(newDataAvailable(EyeTrackerData)),experiment,SLOT(newEyeDataAvailable(EyeTrackerData)));
    connect(experiment,SIGNAL(updateVRDisplay()),this,SLOT(onRequestUpdate()));
    connect(experiment,&Experiment::experimentEndend,this,&Control::onExperimentFinished);


#ifndef DESIGN_MODE_ENABLED
    QSize s = openvrco->getRecommendedSize();
    qreal w = static_cast<qreal>(s.width());
    qreal h = static_cast<qreal>(s.height());
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,w);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,h);
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,true);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,false);
    openvrco->setScreenColor(QColor(Qt::black));
#else
    configExperiments.addKeyValuePair(CONFIG_EYETRACKER_CONFIGURED,CONFIG_P_ET_MOUSE);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_WIDTH,1366);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_HEIGHT,768);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,1366);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,768);
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,false);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,true);
#endif

    // Configuring the experiment.

    configExperiments.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,"outputs");
    configExperiments.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,expFileName);
    configExperiments.addKeyValuePair(CONFIG_DEMO_MODE,false);    
    configExperiments.addKeyValuePair(CONFIG_VALID_EYE,2);
    configExperiments.addKeyValuePair(CONFIG_FIELDING_YPX_2_MM,0.20);
    configExperiments.addKeyValuePair(CONFIG_FIELDING_XPX_2_MM,0.20);
    configExperiments.addKeyValuePair(CONFIG_FIELDING_PAUSE_TEXT,"Press any key to continue");

    renderState = RENDERING_EXPERIMENT;    
    if (!experiment->startExperiment(&configExperiments)){
        qDebug() << "Experiment Start Error: " + experiment->getError();
    }

}

void Control::startNBackRTExperiment(qint32 numTargets, qint32 nbacktype) {
    if (experiment != nullptr) delete experiment;

    NBackRTExperiment::NBackType nbt = NBackRTExperiment::NBT_DEFAULT;
    if (nbacktype == 0) nbt = NBackRTExperiment::NBT_VARIABLE_SPEED;

    experiment = new NBackRTExperiment(nullptr,nbt);

    QString expFileName =  ":/experiment_data/fielding.dat";

    // Connecting the eyetracker to teh experiment.
    configExperiments.clear();
    connect(eyetracker,SIGNAL(newDataAvailable(EyeTrackerData)),experiment,SLOT(newEyeDataAvailable(EyeTrackerData)));
    connect(experiment,SIGNAL(updateVRDisplay()),this,SLOT(onRequestUpdate()));
    connect(experiment,&Experiment::experimentEndend,this,&Control::onExperimentFinished);


#ifndef DESIGN_MODE_ENABLED
    QSize s = openvrco->getRecommendedSize();
    qreal w = static_cast<qreal>(s.width());
    qreal h = static_cast<qreal>(s.height());
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,w);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,h);
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,true);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,false);
    openvrco->setScreenColor(QColor(Qt::black));
    // Values for Mouse EyeTracker
    configExperiments.addKeyValuePair(CONFIG_SAMPLE_FREQUENCY,120);
    configExperiments.addKeyValuePair(CONFIG_MIN_FIXATION_LENGTH,50);
    configExperiments.addKeyValuePair(CONFIG_MOVING_WINDOW_DISP,190);
#else
    configExperiments.addKeyValuePair(CONFIG_EYETRACKER_CONFIGURED,CONFIG_P_ET_MOUSE);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_WIDTH,1366);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_HEIGHT,768);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,1366);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,768);
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,false);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,true);
    // Values for Mouse EyeTracker
    configExperiments.addKeyValuePair(CONFIG_SAMPLE_FREQUENCY,120);
    configExperiments.addKeyValuePair(CONFIG_MIN_FIXATION_LENGTH,50);
    configExperiments.addKeyValuePair(CONFIG_MOVING_WINDOW_DISP,105);
#endif



    // Configuring the experiment.
    configExperiments.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,"outputs");
    configExperiments.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,expFileName);
    configExperiments.addKeyValuePair(CONFIG_DEMO_MODE,false);
    configExperiments.addKeyValuePair(CONFIG_VALID_EYE,2);
    configExperiments.addKeyValuePair(CONFIG_FIELDING_YPX_2_MM,0.20);
    configExperiments.addKeyValuePair(CONFIG_FIELDING_XPX_2_MM,0.20);
    configExperiments.addKeyValuePair(CONFIG_FIELDING_PAUSE_TEXT,"Press any key to continue");

    // Configure NBack Parameters.
    configExperiments.addKeyValuePair(CONFIG_NBACKVS_MAX_HOLD_TIME,250);
    //configExperiments.addKeyValuePair(CONFIG_NBACKVS_MAX_HOLD_TIME,700);
    configExperiments.addKeyValuePair(CONFIG_NBACKVS_MIN_HOLD_TIME,50);
    //configExperiments.addKeyValuePair(CONFIG_NBACKVS_START_HOLD_TIME,250);
    //configExperiments.addKeyValuePair(CONFIG_NBACKVS_START_HOLD_TIME,700);
    configExperiments.addKeyValuePair(CONFIG_NBACKVS_STEP_HOLD_TIME,50);
    configExperiments.addKeyValuePair(CONFIG_NBACKVS_NTRIAL_FOR_STEP_CHANGE,2);

    configExperiments.addKeyValuePair(CONFIG_NBACKVS_SEQUENCE_LENGTH,numTargets);


    renderState = RENDERING_EXPERIMENT;
    if (!experiment->startExperiment(&configExperiments)){
        qDebug() << "Experiment Start Error: " + experiment->getError();
    }
}

void Control::startGoNoGoExperiment(){
    if (experiment != nullptr) delete experiment;
    experiment = new GoNoGoExperiment();

    QString expFileName =  ":/experiment_data/go_no_go.dat";

    // Connecting the eyetracker to teh experiment.
    configExperiments.clear();
    connect(eyetracker,SIGNAL(newDataAvailable(EyeTrackerData)),experiment,SLOT(newEyeDataAvailable(EyeTrackerData)));
    connect(experiment,SIGNAL(updateVRDisplay()),this,SLOT(onRequestUpdate()));
    connect(experiment,&Experiment::experimentEndend,this,&Control::onExperimentFinished);

#ifndef DESIGN_MODE_ENABLED
    QSize s = openvrco->getRecommendedSize();
    qreal w = static_cast<qreal>(s.width());
    qreal h = static_cast<qreal>(s.height());
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,w);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,h);
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,true);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,false);
    openvrco->setScreenColor(QColor(Qt::gray).darker(110));
    configExperiments.addKeyValuePair(CONFIG_SAMPLE_FREQUENCY,120);
    configExperiments.addKeyValuePair(CONFIG_MIN_FIXATION_LENGTH,50);
    configExperiments.addKeyValuePair(CONFIG_MOVING_WINDOW_DISP,100);
#else
    configExperiments.addKeyValuePair(CONFIG_EYETRACKER_CONFIGURED,CONFIG_P_ET_MOUSE);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_WIDTH,1366);
    configExperiments.addKeyValuePair(CONFIG_PRIMARY_MONITOR_HEIGHT,768);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,1366);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,768);
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,false);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,true);
#endif

    // Configuring the experiment.

    configExperiments.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,"outputs");
    configExperiments.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,expFileName);
    configExperiments.addKeyValuePair(CONFIG_DEMO_MODE,false);
    configExperiments.addKeyValuePair(CONFIG_VALID_EYE,2);
    configExperiments.addKeyValuePair(CONFIG_SAMPLE_FREQUENCY,120);
    configExperiments.addKeyValuePair(CONFIG_MIN_FIXATION_LENGTH,50);
    configExperiments.addKeyValuePair(CONFIG_MOVING_WINDOW_DISP,105);

    renderState = RENDERING_EXPERIMENT;
    if (!experiment->startExperiment(&configExperiments)){
        qDebug() << "Experiment Start Error: " + experiment->getError();
    }
}

void Control::onExperimentFinished(const Experiment::ExperimentResult &result){
    qDebug() << "Experiment finished: "  <<  result;
    renderState = RENDERING_NONE;
    experiment->hide();
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
