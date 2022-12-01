#include "experimentdatapainter.h"

ExperimentDataPainter::ExperimentDataPainter()
{
    versionString = "v1";
    gazeUpdateEnabled = false;
    numberOfExplanationScreens = 1;
    shortModeEnabled = false;
}

void ExperimentDataPainter::init(qreal display_resolution_width, qreal display_resolution_height){
    ScreenResolutionHeight = display_resolution_height; //c->getReal(CONFIG_STUDY_DISPLAY_RESOLUTION_HEIGHT);
    ScreenResolutionWidth = display_resolution_width;  //c->getReal(CONFIG_STUDY_DISPLAY_RESOLUTION_WIDTH);
    //canvas = new QGraphicsScene(0,0,ScreenResolutionWidth,ScreenResolutionHeight);
    canvas.setSceneRect(0,0,ScreenResolutionWidth,ScreenResolutionHeight);
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
    Q_UNUSED(configuration)
}

qint32 ExperimentDataPainter::getNumberOfStudyExplanationScreens() const {
    return numberOfExplanationScreens;
}

QString ExperimentDataPainter::getStringKeyForStudyExplanationList() const {
    return explanationListTextKey;
}

void ExperimentDataPainter::updateGazePoints(qreal xr, qreal xl, qreal yr, qreal yl){
    if (!DBUGBOOL(Debug::Options::ENABLE_GAZE_FOLLOW)) return;
    gazeXl = xl;
    gazeXr = xr;
    gazeYl = yl;
    gazeYr = yr;
    updateGazePosition();
}

void ExperimentDataPainter::enableShortStudyMode() {
    shortModeEnabled = true;
}

void ExperimentDataPainter::redrawGazePoints(){
    if (!DBUGBOOL(Debug::Options::ENABLE_GAZE_FOLLOW)) return;
    // Recreating the eye followers
    leftEyeTracker = canvas.addEllipse(0,0,2*R,2*R,QPen(),QBrush(QColor(0,0,255,100)));
    rightEyeTracker = canvas.addEllipse(0,0,2*R,2*R,QPen(),QBrush(QColor(0,255,0,100)));
    leftEyeTracker->setZValue(10);
    rightEyeTracker->setZValue(10);

    // Updating the position based on the last position.
    gazeUpdateEnabled = true;  // WARNING. Its the resposibility of the child class to DISABLE gazeUpdateEnabled whenever the followers are cleared.
    updateGazePosition();
}


void ExperimentDataPainter::setTrialCountLoopValue(qint32 loopValue){
    trialCountLoopValue = loopValue;
}

qint32 ExperimentDataPainter::getLoopValue() const {
    return trialCountLoopValue;
}


RenderServerScene ExperimentDataPainter::getImage() const{
    return canvas;
}

void ExperimentDataPainter::clearCanvas(){
    gazeUpdateEnabled = false;
    canvas.clear();
    redrawGazePoints();
}

ExperimentDataPainter::~ExperimentDataPainter(){
}
