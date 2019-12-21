#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<EyeTrackerData>("EyeTrackerData");
    //qRegisterMetaType<Experiment::ExperimentResult>("ExperimentResult");
    openvrco = new OpenVRControlObject(this);
    connect(&calibrationTimer,&QTimer::timeout,this,&Control::onCalibrationTimerTimeout);
    connect(openvrco,SIGNAL(newProjectionMatrixes(QMatrix4x4,QMatrix4x4)),&eyetracker,SLOT(updateProjectionMatrices(QMatrix4x4,QMatrix4x4)));
    connect(openvrco,SIGNAL(requestUpdate()),this,SLOT(onRequestUpdate()));
    renderState = RENDERING_NONE;
    experiment = nullptr;
}

////////////////////////////////////////////////// Calibration Functions.

void Control::startCalibration(){
    renderState = RENDERING_NONE;
    calibrationPassed = false;
    openvrco->setScreenColor(QColor(Qt::gray));
    if (!openvrco->isRendering()) openvrco->start();
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

    calibrationPoints.clear();
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
            //qDebug() << "Calibration point " << calibrationPointIndex << ":" << p;
            displayImage = tt.setSingleTarget(p.x(),p.y());
            openvrco->setImage(&displayImage);
            isWaiting = true;
            calibrationTimer.start(CALIBRATION_WAIT);
            eyetracker.newCalibrationPoint(p.x(),p.y());
            emit(newImageAvailable());
        }
        else{
            if (!eyetracker.calibrationDone()){
                qDebug() << "CALIBRATION FAILED";
                displayImage = tt.getClearScreen();
                openvrco->setImage(&displayImage);
                emit(newImageAvailable());
                emit(calibrationFinished());
                return;
            }
            qDebug() << "Calibration finished";
            tt.setTargetTest();
            displayImage = tt.getClearScreen();
            openvrco->setImage(&displayImage);
            calibrationPassed = true;
            emit(newImageAvailable());
            emit(calibrationFinished());
        }
    }
}

bool Control::isCalibrated(){
    return calibrationPassed;
}

////////////////////////////////////////////////// Update display image

QImage Control::image() const{
    return displayImage;
}


////////////////////////////////////////////////// Keyboard press listener
void Control::keyboardKeyPressed(int key){
    if (experiment != nullptr){
        experiment->keyboardKeyPressed(key);
    }
}

void Control::startReadingExperiment(QString lang){
    if (experiment != nullptr) delete experiment;
    experiment = new ReadingExperiment();

    // TODO connect to experiment ended.
    // void experimentEndend(const ExperimentResult &result);

    // Connecting the eyetracker to teh experiment.
    configExperiments.clear();
    QSize s = openvrco->getRecommendedSize();
    qreal w = static_cast<qreal>(s.width());
    qreal h = static_cast<qreal>(s.height());
    connect(&eyetracker,SIGNAL(newEyeData(EyeTrackerData)),experiment,SLOT(newEyeDataAvailable(EyeTrackerData)));
    connect(experiment,SIGNAL(updateVRDisplay()),this,SLOT(onRequestUpdate()));
    connect(experiment,&Experiment::experimentEndend,this,&Control::onExperimentFinished);

    // Configuring the experiment.
    configExperiments.addKeyValuePair(CONFIG_VR_ENABLED,true);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,w);
    configExperiments.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,h);
    configExperiments.addKeyValuePair(CONFIG_PATIENT_DIRECTORY,"outputs");
    configExperiments.addKeyValuePair(CONFIG_EXP_CONFIG_FILE,QString(":/experiment_data/Reading_") + lang + ".dat");
    configExperiments.addKeyValuePair(CONFIG_DEMO_MODE,false);
    configExperiments.addKeyValuePair(CONFIG_USE_MOUSE,false);
    configExperiments.addKeyValuePair(CONFIG_VALID_EYE,2);
    configExperiments.addKeyValuePair(CONFIG_READING_EXP_LANG,lang);
    configExperiments.addKeyValuePair(CONFIG_READING_PX_TOL,60);


    renderState = RENDERING_EXPERIMENT;
    experiment->startExperiment(&configExperiments);

}

void Control::onExperimentFinished(const Experiment::ExperimentResult &result){
    qDebug() << "Experiment finished: "  <<  result;
    renderState = RENDERING_NONE;
}


////////////////////////////////////////////////// Eye Tracking Testing.
void Control::testEyeTracking(){
    renderState = RENDERING_TARGET_TEST;

    // Starting the EyeTracker.
    if (!eyetracker.initalizeEyeTracking()){
        qDebug() << "Error intializing eyetracker";
        return;
    }

    if (!eyetracker.isRunning()) eyetracker.start();

    openvrco->setScreenColor(QColor(Qt::gray));
    if (!openvrco->isRendering()) openvrco->start();
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
        EyeTrackerData data = eyetracker.getLastData();
        displayImage = tt.renderCurrentPosition(data.xRight,data.yRight,data.xLeft,data.yLeft);
        return displayImage;        
    }
    if (renderState == RENDERING_EXPERIMENT){
        EyeTrackerData data = eyetracker.getLastData();
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
    else return QImage();
}
