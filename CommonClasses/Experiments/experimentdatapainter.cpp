#include "experimentdatapainter.h"

ExperimentDataPainter::ExperimentDataPainter()
{
    versionString = "v1";
    numberOfExplanationScreens = 1;
    shortModeEnabled = false;
}

void ExperimentDataPainter::init(qreal display_resolution_width, qreal display_resolution_height){
    ScreenResolutionHeight = display_resolution_height; //c->getReal(CONFIG_STUDY_DISPLAY_RESOLUTION_HEIGHT);
    ScreenResolutionWidth = display_resolution_width;  //c->getReal(CONFIG_STUDY_DISPLAY_RESOLUTION_WIDTH);
    canvas.setSceneRect(0,0,ScreenResolutionWidth,ScreenResolutionHeight);

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

void ExperimentDataPainter::enableShortStudyMode() {
    shortModeEnabled = true;
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
    canvas.clear();
}

ExperimentDataPainter::~ExperimentDataPainter(){
}
