#include "gonogomanager.h"

const char* GoNoGoManager::RED_ARROW_COLOR           = "#cc1f1f";
const char* GoNoGoManager::GREEN_ARROW_COLOR         = "#28ab14";

GoNoGoManager::GoNoGoManager()
{

}

qint32 GoNoGoManager::size() const{
    return gonogoTrials.size();
}

qreal GoNoGoManager::sizeToProcess() const{
    return gonogoTrials.size();
}

QRectF GoNoGoManager::getArrowBox() const{
    return arrowTargetBox;
}

void GoNoGoManager::resetStudy(){
    trialIndex = 0;
}

QList<QRectF> GoNoGoManager::getLeftAndRightHitBoxes() const {
    return targetBoxes;
}

void GoNoGoManager::enableDemoMode(){
    //while (gonogoTrials.size() > 20){
    while (gonogoTrials.size() > NUMBER_OF_TRIALS_IN_DEMO_MODE){
        gonogoTrials.removeLast();
    }
}

void GoNoGoManager::init(qreal display_resolution_width, qreal display_resolution_height){
    ExperimentDataPainter::init(display_resolution_width,display_resolution_height);
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

    if (DBUGBOOL(Debug::Options::RENDER_HITBOXES)){
        // Target boxes must be drawn first.
        qDebug() << "DBUG Drawing GoNoGo TargetBoxes";

        QColor bkg(Qt::white);
        bkg.setAlpha(0);

        for (qint32 i = 0; i < targetBoxes.size(); i++){
            // And Draws the Target Boxes.
            QRectF hitBoxToDraw = targetBoxes.at(i);

            qDebug() << "DBUG: GONOGO HITBOX" << i << hitBoxToDraw;

            QGraphicsRectItem *rect2 = canvas->addRect(0,0,hitBoxToDraw.width(),hitBoxToDraw.height(),
                                                       QPen(QBrush(Qt::blue),2),
                                                       QBrush(bkg)); // Background is fully transparent

            rect2->setPos(hitBoxToDraw.x(),hitBoxToDraw.y());
            //rect2->setOpacity(0.5);
        }

        QRectF hitBoxToDraw = arrowTargetBox;
        QGraphicsRectItem *rect2 = canvas->addRect(0,0,hitBoxToDraw.width(),hitBoxToDraw.height(),
                                                   QPen(QBrush(Qt::blue),2),
                                                   QBrush(bkg));
        //rect2->setOpacity(0.5);
        rect2->setPos(hitBoxToDraw.x(),hitBoxToDraw.y());

    }

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

    if (gonogoTrials.at(trialIndex).type == GoNoGoParser::TRIAL_TYPE_RLEFT){
        setVisibilityToElementList(gRLArrow,true);
    }
    else if (gonogoTrials.at(trialIndex).type == GoNoGoParser::TRIAL_TYPE_GLEFT){
        setVisibilityToElementList(gGLArrow,true);
    }
    else if (gonogoTrials.at(trialIndex).type == GoNoGoParser::TRIAL_TYPE_RRIGHT){
        setVisibilityToElementList(gRRArrow,true);
    }
    else if (gonogoTrials.at(trialIndex).type == GoNoGoParser::TRIAL_TYPE_GRIGHT){
        setVisibilityToElementList(gGRArrow,true);
    }
    else{
        qDebug() << "UNKNOWN Go No Go Trial type " << gonogoTrials.at(trialIndex).type;
    }

    trialIndex++;
    if (trialCountLoopValue > -1){
        if (trialIndex > trialCountLoopValue){
            trialIndex = 0;
        }
    }

}

void GoNoGoManager::setVisibilityToElementList(QList<QGraphicsItem *> list, bool makeVisible){
    for (qint32 i = 0; i < list.size(); i++){
        list[i]->setVisible(makeVisible);
    }
}

void GoNoGoManager::drawAllElements(){

    trialIndex = 0;

    qreal centerX = ScreenResolutionWidth/2;
    qreal centerY = ScreenResolutionHeight/2;

    gTargets.clear();

    // Drawing targets.
    DrawingConstantsCalculator dcc;
    dcc.setTargetResolution(ScreenResolutionWidth,ScreenResolutionHeight);

    qreal diameter = (ScreenResolutionWidth*dcc.getHorizontalRatio(GoNoGoParser::TARGET_SIZE));
    qreal lwidth = diameter*GoNoGoParser::TARGET_LINE_WIDTH;
    qreal target_radious = diameter/2.0;
    QRectF targetRect(0,0,diameter,diameter);

    QGraphicsEllipseItem *leftTarget = canvas->addEllipse(targetRect,QPen(Qt::black,lwidth),QBrush(QColor(Qt::gray).darker(150)));
    QGraphicsEllipseItem *rightTarget = canvas->addEllipse(targetRect,QPen(Qt::black,lwidth),QBrush(QColor(Qt::gray).darker(150)));

    qreal side_margin = dcc.getHorizontalRatio(GoNoGoParser::SIDE_MARGIN);

    leftTarget->setPos(ScreenResolutionWidth*side_margin,centerY-target_radious);
    rightTarget->setPos(ScreenResolutionWidth*(1-side_margin)-diameter,centerY-target_radious);
    gTargets << leftTarget << rightTarget;

    // Drawing the cross.
    qreal line_length = ScreenResolutionWidth*dcc.getHorizontalRatio(GoNoGoParser::CROSS_LINE_LENGTH);
    lwidth = line_length*GoNoGoParser::LINE_WIDTH;
    QGraphicsLineItem *vline = canvas->addLine(centerX,centerY-line_length/2,centerX,centerY+line_length/2,QPen(Qt::black,lwidth));
    QGraphicsLineItem *hline = canvas->addLine(centerX-line_length/2,centerY,centerX+line_length/2,centerY,QPen(Qt::black,lwidth));
    gCross << vline << hline;

    // Drawing the arrows.
    qreal indicator_line_length = GoNoGoParser::INDICATOR_LINE_LENGTH*line_length;

    // Red arrow main trunk
    qreal left_x0 = centerX-line_length/2;
    qreal right_x0 = centerX+line_length/2;
    QGraphicsLineItem *red_arrow = canvas->addLine(left_x0,centerY,right_x0,centerY,QPen(QColor(RED_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    QGraphicsLineItem *green_arrow = canvas->addLine(left_x0,centerY,right_x0,centerY,QPen(QColor(GREEN_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));

    // Offset to build indicators
    qreal ka = qSqrt(2)*indicator_line_length/2;

    // Red Left Arrow
    QGraphicsLineItem *red_left_arrow_ind_up = canvas->addLine(left_x0,centerY,left_x0+ka,centerY-ka,QPen(QColor(RED_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    QGraphicsLineItem *red_left_arrow_ind_dn = canvas->addLine(left_x0,centerY,left_x0+ka,centerY+ka,QPen(QColor(RED_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    gRLArrow << red_arrow << red_left_arrow_ind_up << red_left_arrow_ind_dn;

    // Red Right Arrow.
    QGraphicsLineItem *red_right_arrow_ind_up = canvas->addLine(right_x0,centerY,right_x0-ka,centerY-ka,QPen(QColor(RED_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    QGraphicsLineItem *red_right_arrow_ind_dn = canvas->addLine(right_x0,centerY,right_x0-ka,centerY+ka,QPen(QColor(RED_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    gRRArrow << red_arrow << red_right_arrow_ind_dn << red_right_arrow_ind_up;

    // Green Left Arrow
    QGraphicsLineItem *green_left_arrow_ind_up = canvas->addLine(left_x0,centerY,left_x0+ka,centerY-ka,QPen(QColor(GREEN_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    QGraphicsLineItem *green_left_arrow_ind_dn = canvas->addLine(left_x0,centerY,left_x0+ka,centerY+ka,QPen(QColor(GREEN_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    gGLArrow << green_arrow << green_left_arrow_ind_up << green_left_arrow_ind_dn;

    // Green Right Arrow
    QGraphicsLineItem *green_right_arrow_ind_up = canvas->addLine(right_x0,centerY,right_x0-ka,centerY-ka,QPen(QColor(GREEN_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    QGraphicsLineItem *green_right_arrow_ind_dn = canvas->addLine(right_x0,centerY,right_x0-ka,centerY+ka,QPen(QColor(GREEN_ARROW_COLOR),lwidth,Qt::SolidLine,Qt::RoundCap));
    gGRArrow << green_arrow << green_right_arrow_ind_up << green_right_arrow_ind_dn;

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

void GoNoGoManager::renderStudyExplanationScreen(qint32 screen_index){
    canvas->clear();
    canvas->addRect(0,0,canvas->width(),canvas->height(),QPen(),QBrush(QColor(Qt::blue)));
}
