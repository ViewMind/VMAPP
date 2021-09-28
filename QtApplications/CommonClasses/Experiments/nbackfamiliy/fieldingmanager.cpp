#include "fieldingmanager.h"

//#ifdef EYETRACKER_HTCVIVEPRO
//const qreal FieldingManager::TARGET_R =                                   52.5;
//const qreal FieldingManager::TARGET_OFFSET_X =                            48.75;
//const qreal FieldingManager::TARGET_OFFSET_Y =                            43.75;
//const qreal FieldingManager::K_CROSS_LINE_LENGTH =                        0.05;
//#endif

//#ifdef EYETRACKER_GAZEPOINT
//const qreal FieldingManager::TARGET_R =                                   42;
//const qreal FieldingManager::TARGET_OFFSET_X =                            39;
//const qreal FieldingManager::TARGET_OFFSET_Y =                            39;
//#endif

const qreal FieldingManager::K_TARGET_R          =                        0.022; // of the width.
const qreal FieldingManager::K_TARGET_OFFSET_X   =                        0.02;
const qreal FieldingManager::K_TARGET_OFFSET_Y   =                        0.036;
const qreal FieldingManager::K_CROSS_LINE_LENGTH =                        0.05;

const char * FieldingManager::CONFIG_IS_VR_BEING_USED  = "is_VR_used";
const char * FieldingManager::CONFIG_PAUSE_TEXT_LANG   = "pause_text_lang";

const char * FieldingManager::LANG_EN = "EN";
const char * FieldingManager::LANG_ES = "ES";

const char * FieldingManager::PAUSE_TEXT_SPANISH = "Presione \"G\" para continuar el estudio";
const char * FieldingManager::PAUSE_TEXT_ENGLISH = "Press \"G\" to continue with study";

FieldingManager::FieldingManager(){
    vr_being_used = false;
}

void FieldingManager::enableDemoMode(){
    while (fieldingTrials.size() > NUMBER_OF_TRIALS_IN_DEMO_MODE){
        fieldingTrials.removeLast();
    }
}

bool FieldingManager::parseExpConfiguration(const QString &contents){

    FieldingParser parser;
    if (!parser.parseFieldingExperiment(contents,ScreenResolutionWidth,ScreenResolutionHeight)){
        error = parser.getError();
        return false;
    }

    fieldingTrials  = parser.getParsedTrials();
    versionString   = parser.getVersionString();
    hitTargetBoxes  = parser.getHitTargetBoxes();
    drawTargetBoxes = parser.getDrawTargetBoxes();

    return true;

}

void FieldingManager::configure(const QVariantMap &config){
    vr_being_used = config.value(CONFIG_IS_VR_BEING_USED,false).toBool();
    if (config.value(CONFIG_PAUSE_TEXT_LANG).toString() == LANG_ES) pauseText = PAUSE_TEXT_SPANISH;
    else pauseText = PAUSE_TEXT_ENGLISH;
}

void FieldingManager::init(qreal display_resolution_width, qreal display_resolution_height){
    ExperimentDataPainter::init(display_resolution_width,display_resolution_height);
    clearCanvas();
    drawBackground();
}

void FieldingManager::drawBackground(){

    qreal centerX = ScreenResolutionWidth/2;
    qreal centerY = ScreenResolutionHeight/2;

    DrawingConstantsCalculator dcc;
    dcc.setTargetResolution(ScreenResolutionWidth,ScreenResolutionHeight);

    // Background
    canvas->clear();
    canvas->addRect(0,0,ScreenResolutionWidth,ScreenResolutionHeight,QPen(),QBrush(Qt::black));

    for (qint32 i = 0; i < drawTargetBoxes.size(); i++){
#ifdef ENABLE_DRAW_OF_HIT_TARGET_BOXES        

        QRectF hitBoxToDraw = hitTargetBoxes.at(i);
        if ((i != TARGET_BOX_5) && (i != TARGET_BOX_2)){
            hitBoxToDraw.setY(hitBoxToDraw.top()-hitBoxToDraw.height()/2);
            hitBoxToDraw.setHeight(hitBoxToDraw.height()*2);
        }

        QGraphicsRectItem *rect2 = canvas->addRect(0,0,hitBoxToDraw.width(),hitBoxToDraw.height(),
                                                   QPen(QBrush(Qt::green),2),
                                                   QBrush(Qt::black));

        rect2->setPos(hitBoxToDraw.x(),hitBoxToDraw.y());
#endif

        QGraphicsRectItem *rect = canvas->addRect(0,0,drawTargetBoxes.at(i).width(),drawTargetBoxes.at(i).height(),
                                                  QPen(QBrush(Qt::white),6),
                                                  QBrush(Qt::black));
        rect->setPos(drawTargetBoxes.at(i).x(),drawTargetBoxes.at(i).y());


    }

    // Adding the cross
    qreal halfLine = ScreenResolutionWidth*dcc.getHorizontalRatio(K_CROSS_LINE_LENGTH)/2.0;
    gCrossLine0 = canvas->addLine(centerX-halfLine,centerY,
                                  centerX+halfLine,centerY,
                                  QPen(QBrush(Qt::white),4));
    gCrossLine1 = canvas->addLine(centerX,centerY-halfLine,
                                  centerX,centerY+halfLine,
                                  QPen(QBrush(Qt::white),4));

    gCrossLine0->setZValue(-1);
    gCrossLine1->setZValue(-1);

    // Adding the target
    qreal TARGET_R = dcc.getHorizontalRatio(K_TARGET_R)*ScreenResolutionWidth;
    gTarget = canvas->addEllipse(ScreenResolutionWidth*dcc.getHorizontalRatio(K_TARGET_OFFSET_X),
                                 ScreenResolutionHeight*dcc.getVerticalRatio(K_TARGET_OFFSET_Y),
                                 TARGET_R*2,TARGET_R*2,
                                 QPen(),QBrush(Qt::red));

    // Adding the text
    QFont letterFont;
    letterFont.setFamily("Courier New");
    letterFont.setPointSize(50);
    letterFont.setBold(true);

    gText1 = canvas->addSimpleText("1",letterFont);
    qreal x = (ScreenResolutionWidth - gText1->boundingRect().width())/2;
    qreal y = (ScreenResolutionHeight - gText1->boundingRect().height())/2;
    gText1->setPos(x,y);
    gText1->setPen(QPen(Qt::white));
    gText1->setBrush(QBrush(Qt::white));

    gText2 = canvas->addSimpleText("2",letterFont);
    gText2->setPos(x,y);
    gText2->setPen(QPen(Qt::white));
    gText2->setBrush(QBrush(Qt::white));

    gText3 = canvas->addSimpleText("3",letterFont);
    gText3->setPos(x,y);
    gText3->setPen(QPen(Qt::white));
    gText3->setBrush(QBrush(Qt::white));

}

void FieldingManager::setDrawState(DrawState ds){
    switch (ds){
    case DS_1:
        gText1->setZValue(1);
        gText2->setZValue(-1);
        gText3->setZValue(-1);
        gCrossLine0->setZValue(-1);
        gCrossLine1->setZValue(-1);
        gTarget->setZValue(-1);
        break;
    case DS_2:
        gText1->setZValue(-1);
        gText2->setZValue(1);
        gText3->setZValue(-1);
        gCrossLine0->setZValue(-1);
        gCrossLine1->setZValue(-1);
        gTarget->setZValue(-1);
        break;
    case DS_3:
        gText1->setZValue(-1);
        gText2->setZValue(-1);
        gText3->setZValue(1);
        gCrossLine0->setZValue(-1);
        gCrossLine1->setZValue(-1);
        gTarget->setZValue(-1);
        break;
    case DS_CROSS:
        gText1->setZValue(-1);
        gText2->setZValue(-1);
        gText3->setZValue(-1);
        gCrossLine0->setZValue(1);
        gCrossLine1->setZValue(1);
        gTarget->setZValue(-1);
        break;
    case DS_CROSS_TARGET:
        gText1->setZValue(-1);
        gText2->setZValue(-1);
        gText3->setZValue(-1);
        gCrossLine0->setZValue(1);
        gCrossLine1->setZValue(1);
        gTarget->setZValue(1);
        break;
    case DS_TARGET_BOX_ONLY:
        gText1->setZValue(-1);
        gText2->setZValue(-1);
        gText3->setZValue(-1);
        gCrossLine0->setZValue(-1);
        gCrossLine1->setZValue(-1);
        gTarget->setZValue(-1);
        break;
    }
}

bool FieldingManager::setTargetPositionFromTrialName(const QString &trial, qint32 image){
    qint32 id = -1;
    for (qint32 i = 0; i < fieldingTrials.size(); i++){
        if (fieldingTrials.at(i).id == trial){
            id = i;
            break;
        }
    }

    if (id == -1) return false;
    setTargetPosition(id,image);
    return true;
}

void FieldingManager::setTargetPosition(qint32 trial, qint32 image){

    // Check to fix a small bug when using a sequence of six, when changing to a new image image number will be 6 and the following instruction will break.
    if (image >= fieldingTrials.at(trial).sequence.size()) return;

    QRectF r = drawTargetBoxes.at(fieldingTrials.at(trial).sequence.at(image));
    gTarget->setPos(r.x(),r.y());
}

QPoint FieldingManager::getTargetPoint(qint32 trial, qint32 image) const{
    QRectF r = drawTargetBoxes.at(fieldingTrials.at(trial).sequence.at(image));
    return QPoint(static_cast<qint32>(r.x()),static_cast<qint32>(r.y()));
}

QList<qint32> FieldingManager::getExpectedTargetSequenceForTrial(qint32 trial, qint32 targetNum) const{
    QList<qint32> hits;
    QList<qint32> sequence = fieldingTrials.at(trial).sequence.mid(0,targetNum);

    for (qint32 i = sequence.size()-1; i >= 0; i--){
        hits << sequence.at(i);
    }
    return hits;
}

QString FieldingManager::getFullSequenceAsString(qint32 trial){
    QList<qint32> sequence = fieldingTrials.at(trial).sequence;
    QStringList strnumbers;
    for (qint32 i = 0; i < sequence.size(); i++){
        strnumbers << QString::number(sequence.at(i));
    }
    return strnumbers.join(" ");
}

bool FieldingManager::isPointInTargetBox(qreal x, qreal y, qint32 targetBox) const{
    //return FieldingParser::isHitInTargetBox(hitTargetBoxes,targetBox,x,y);
    return hitTargetBoxes.at(targetBox).contains(x,y);
}

void FieldingManager::drawPauseScreen(){

    canvas->clear();
    qreal xpos, ypos;

    canvas->addRect(0,0,canvas->width(),canvas->height(),QPen(),QBrush(QColor(Qt::black)));

    QFont font;
    if (vr_being_used){
        font = QFont("Mono",32);
    }
    else{
        font = QFont("Courier New",20);
    }

    // Chaging the current target point to escape point
    QGraphicsSimpleTextItem *phraseToShow = canvas->addSimpleText(pauseText,font);
    phraseToShow->setPen(QPen(QColor(Qt::white)));
    phraseToShow->setBrush(QBrush(QColor(Qt::white)));
    xpos = (canvas->width() - phraseToShow->boundingRect().width())/2;
    ypos = (canvas->height() - phraseToShow->boundingRect().height())/2;
    phraseToShow->setPos(xpos,ypos);
    phraseToShow->setZValue(1);

}
