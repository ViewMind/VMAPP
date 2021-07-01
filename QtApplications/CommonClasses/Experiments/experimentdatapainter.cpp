#include "experimentdatapainter.h"

ExperimentDataPainter::ExperimentDataPainter()
{
    versionString = "v1";
    gazeUpdateEnabled = false;
}

void ExperimentDataPainter::init(qreal display_resolution_width, qreal display_resolution_height){
    ScreenResolutionHeight = display_resolution_height; ; //c->getReal(CONFIG_STUDY_DISPLAY_RESOLUTION_HEIGHT);
    ScreenResolutionWidth = display_resolution_width;  //c->getReal(CONFIG_STUDY_DISPLAY_RESOLUTION_WIDTH);
    canvas = new QGraphicsScene(0,0,ScreenResolutionWidth,ScreenResolutionHeight);
    //qDebug() << "Experiment data painter. Scene Rect" << canvas->sceneRect();
    R = 0.007*ScreenResolutionWidth;
    //config = c;
}

void ExperimentDataPainter::updateGazePosition(){
    if (!gazeUpdateEnabled) return;
    rightEyeTracker->setPos(gazeXr-R,gazeYr-R);
    leftEyeTracker->setPos(gazeXl-R,gazeYl-R);
}

void ExperimentDataPainter::configure(const QVariantMap &configuration){
    Q_UNUSED(configuration);
}


void ExperimentDataPainter::updateGazePoints(qreal xr, qreal xl, qreal yr, qreal yl){
#ifndef ENABLE_GAZE_FOLLOW
    return;
#endif
    gazeXl = xl;
    gazeXr = xr;
    gazeYl = yl;
    gazeYr = yr;
    updateGazePosition();
}

void ExperimentDataPainter::redrawGazePoints(){

#ifndef ENABLE_GAZE_FOLLOW
    return;
#endif

    // Recreating the eye followers
    leftEyeTracker = canvas->addEllipse(0,0,2*R,2*R,QPen(),QBrush(QColor(0,0,255,100)));
    rightEyeTracker = canvas->addEllipse(0,0,2*R,2*R,QPen(),QBrush(QColor(0,255,0,100)));
    leftEyeTracker->setZValue(10);
    rightEyeTracker->setZValue(10);

    // Updating the position based on the last position.
    gazeUpdateEnabled = true;  // WARNING. Its the resposibility of the child class to DISABLE gazeUpdateEnabled whenever the followers are cleared.
    updateGazePosition();

}

QPixmap ExperimentDataPainter::getImage() const{
    // Drawing the graphics scene onto the painter.
    QPixmap image(static_cast<qint32>(canvas->width()),static_cast<qint32>(canvas->height()));
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    canvas->render(&painter);
    return image;
}

QImage ExperimentDataPainter::getQImage() const{
    QImage image(static_cast<qint32>(canvas->width()),static_cast<qint32>(canvas->height()),QImage::Format_RGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    canvas->render(&painter);
    return image;
}

void ExperimentDataPainter::clearCanvas(){
    gazeUpdateEnabled = false;
    canvas->clear();
    redrawGazePoints();
}

ExperimentDataPainter::~ExperimentDataPainter(){
    delete canvas;
    canvas = nullptr;
}
