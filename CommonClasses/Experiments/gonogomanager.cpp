#include "gonogomanager.h"

GoNoGoManager::GoNoGoManager()
{

}

qint32 GoNoGoManager::size() const{
    return gonogoTrials.size();
}

qreal GoNoGoManager::sizeToProcess() const{
    return gonogoTrials.size();
}

void GoNoGoManager::enableDemoMode(){
    while (gonogoTrials.size() > NUMBER_OF_TRIALS_IN_DEMO_MODE){
        gonogoTrials.removeLast();
    }
}

void GoNoGoManager::init(ConfigurationManager *c){
    ExperimentDataPainter::init(c);
    clearCanvas();

    // Setting the background brush.
    canvas->setBackgroundBrush(QBrush(Qt::gray));

    drawAllElements();
}

bool GoNoGoManager::parseExpConfiguration(const QString &contents){
    GoNoGoParser parser;
    if(!parser.parseGoNoGoExperiment(contents,ScreenResolutionWidth,ScreenResolutionHeight)){
        error = parser.getError();
        return false;
    }

    gonogoTrials = parser.getTrials();
    versionString = parser.getVersionString();
    targetBoxes = parser.getTargetBoxes();
    arrowTargetBox = parser.getArrowTargetBox();
    answerArray = parser.getCorrectAnswerArray();

    // For DEBUG only
    //canvas->addRect(targetBoxes.at(0));
    //canvas->addRect(targetBoxes.at(1));
    //canvas->addRect(arrowTargetBox);

    if (config->getBool(CONFIG_DEMO_MODE)) enableDemoMode();

    return true;
}

bool GoNoGoManager::drawCross(){
    if (trialIndex < gonogoTrials.size()){
        setVisibilityToElementList(gTargets,false);
        setVisibilityToElementList(gRLArrow,false);
        setVisibilityToElementList(gRRArrow,false);
        setVisibilityToElementList(gGRArrow,false);
        setVisibilityToElementList(gGLArrow,false);
        setVisibilityToElementList(gCross,true);
        return true;
    }
    return false;
}

void GoNoGoManager::drawTrialByID(const QString &id){

    setVisibilityToElementList(gRLArrow,false);
    setVisibilityToElementList(gRRArrow,false);
    setVisibilityToElementList(gGRArrow,false);
    setVisibilityToElementList(gGLArrow,false);

    for (qint32 i = 0; i < gonogoTrials.size(); i++){
        if (id == gonogoTrials.at(i).id){
            trialIndex = i;
            drawCurrentTrial();
        }
    }
}

void GoNoGoManager::drawCurrentTrial(){

    setVisibilityToElementList(gCross,false);
    setVisibilityToElementList(gTargets,true);

    switch (gonogoTrials.at(trialIndex).type){
    case GONOGO_TRIAL_TYPE_RLEFT:
        setVisibilityToElementList(gRLArrow,true);
        break;
    case GONOGO_TRIAL_TYPE_GLEFT:
        setVisibilityToElementList(gGLArrow,true);
        break;
    case GONOGO_TRIAL_TYPE_RRIGHT:
        setVisibilityToElementList(gRRArrow,true);
        break;
    case GONOGO_TRIAL_TYPE_GRIGHT:
        setVisibilityToElementList(gGRArrow,true);
        break;
    default:
        qDebug() << "UNKNOWN Go No Go Trial type " << gonogoTrials.at(trialIndex).type;
        break;
    }
    trialIndex++;
}

void GoNoGoManager::setVisibilityToElementList(QList<QGraphicsItem *> list, bool makeVisible){
    for (qint32 i = 0; i < list.size(); i++){
        list[i]->setVisible(makeVisible);
    }
}

void GoNoGoManager::drawAllElements(){

    //qDebug() << "Draw All Elements";

    trialIndex = 0;

    qreal centerX = ScreenResolutionWidth/2;
    qreal centerY = ScreenResolutionHeight/2;

    gTargets.clear();

    // Drawing targets.
    qreal diameter = (ScreenResolutionWidth*GONOGO_TARGET_SIZE);
    qreal lwidth = diameter*GONOGO_TARGET_LINE_WIDTH;
    qreal target_radious = diameter/2.0;
    QRectF targetRect(0,0,diameter,diameter);

    QGraphicsEllipseItem *leftTarget = canvas->addEllipse(targetRect,QPen(Qt::black,lwidth),QBrush(QColor(Qt::gray).darker(150)));
    QGraphicsEllipseItem *rightTarget = canvas->addEllipse(targetRect,QPen(Qt::black,lwidth),QBrush(QColor(Qt::gray).darker(150)));

    leftTarget->setPos(ScreenResolutionWidth*GONOGO_SIDE_MARGIN,centerY-target_radious);
    rightTarget->setPos(ScreenResolutionWidth*(1-GONOGO_SIDE_MARGIN)-diameter,centerY-target_radious);
    gTargets << leftTarget << rightTarget;

    // Drawing the cross.
    qreal line_length = ScreenResolutionWidth*GONOGO_CROSS_LINE_LENGTH;
    lwidth = line_length*GONOGO_LINE_WIDTH;
    QGraphicsLineItem *vline = canvas->addLine(centerX,centerY-line_length/2,centerX,centerY+line_length/2,QPen(Qt::black,lwidth));
    QGraphicsLineItem *hline = canvas->addLine(centerX-line_length/2,centerY,centerX+line_length/2,centerY,QPen(Qt::black,lwidth));
    gCross << vline << hline;

    // Drawing the arrows.
    qreal indicator_line_length = GONOGO_INDICATOR_LINE_LENGTH*line_length;

    // Red arrow main trunk
    qreal left_x0 = centerX-line_length/2;
    qreal right_x0 = centerX+line_length/2;
    QGraphicsLineItem *red_arrow = canvas->addLine(left_x0,centerY,right_x0,centerY,QPen(QColor(GONOGO_RED_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    QGraphicsLineItem *green_arrow = canvas->addLine(left_x0,centerY,right_x0,centerY,QPen(QColor(GONOGO_GREEN_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));

    // Offset to build indicators
    qreal ka = qSqrt(2)*indicator_line_length/2;

    // Red Left Arrow
    QGraphicsLineItem *red_left_arrow_ind_up = canvas->addLine(left_x0,centerY,left_x0+ka,centerY-ka,QPen(QColor(GONOGO_RED_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    QGraphicsLineItem *red_left_arrow_ind_dn = canvas->addLine(left_x0,centerY,left_x0+ka,centerY+ka,QPen(QColor(GONOGO_RED_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    gRLArrow << red_arrow << red_left_arrow_ind_up << red_left_arrow_ind_dn;

    // Red Right Arrow.
    QGraphicsLineItem *red_right_arrow_ind_up = canvas->addLine(right_x0,centerY,right_x0-ka,centerY-ka,QPen(QColor(GONOGO_RED_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    QGraphicsLineItem *red_right_arrow_ind_dn = canvas->addLine(right_x0,centerY,right_x0-ka,centerY+ka,QPen(QColor(GONOGO_RED_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    gRRArrow << red_arrow << red_right_arrow_ind_dn << red_right_arrow_ind_up;

    // Green Left Arrow
    QGraphicsLineItem *green_left_arrow_ind_up = canvas->addLine(left_x0,centerY,left_x0+ka,centerY-ka,QPen(QColor(GONOGO_GREEN_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    QGraphicsLineItem *green_left_arrow_ind_dn = canvas->addLine(left_x0,centerY,left_x0+ka,centerY+ka,QPen(QColor(GONOGO_GREEN_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    gGLArrow << green_arrow << green_left_arrow_ind_up << green_left_arrow_ind_dn;

    // Green Right Arrow
    QGraphicsLineItem *green_right_arrow_ind_up = canvas->addLine(right_x0,centerY,right_x0-ka,centerY-ka,QPen(QColor(GONOGO_GREEN_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    QGraphicsLineItem *green_right_arrow_ind_dn = canvas->addLine(right_x0,centerY,right_x0-ka,centerY+ka,QPen(QColor(GONOGO_GREEN_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    gGRArrow << green_arrow << green_right_arrow_ind_up << green_right_arrow_ind_dn;

    //qDebug() << "Finished Draw All Elements";

}


QString GoNoGoManager::getCurrentTrialHeader() const{
    if (trialIndex >= gonogoTrials.size()) return "";
    return gonogoTrials.at(trialIndex).id + " " + QString::number(gonogoTrials.at(trialIndex).type);
}

bool GoNoGoManager::isPointInSideCorrectTargetForCurrentTrial(qreal x, qreal y) const{
    if (trialIndex <= 0) return false;
    if (trialIndex > gonogoTrials.size()) return false;
    qint32 targetRectIndex = answerArray.at(gonogoTrials.at(trialIndex-1).type);
    //qDebug() << "Checking target box " << targetBoxes.at(targetRectIndex) << ". Trial of Type" << gonogoTrials.at(trialIndex-1).type;
    return targetBoxes.at(targetRectIndex).contains(x,y);
}
