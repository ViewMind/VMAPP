#include "fieldingmanager.h"

FieldingManager::FieldingManager(){

}

void FieldingManager::enableDemoMode(){
    while (fieldingTrials.size() > NUMBER_OF_TRIALS_IN_DEMO_MODE){
        fieldingTrials.removeLast();
    }
}

bool FieldingManager::parseExpConfiguration(const QString &contents){

    FieldingParser parser;
    if (!parser.parseFieldingExperiment(contents,ScreenResolutionWidth,ScreenResolutionHeight,
                                        config->getReal(CONFIG_FIELDING_XPX_2_MM),
                                        config->getReal(CONFIG_FIELDING_YPX_2_MM))){
        error = parser.getError();
        return false;
    }

    fieldingTrials  = parser.getParsedTrials();
    versionString   = parser.getVersionString();
    hitTargetBoxes  = parser.getHitTargetBoxes();
    drawTargetBoxes = parser.getDrawTargetBoxes();

    if (config->getBool(CONFIG_DEMO_MODE)) enableDemoMode();

    return true;

}

void FieldingManager::init(ConfigurationManager *c){
    ExperimentDataPainter::init(c);
    clearCanvas();
    drawBackground();
}

void FieldingManager::drawBackground(){

    qreal kx = config->getReal(CONFIG_FIELDING_XPX_2_MM);
    qreal ky = config->getReal(CONFIG_FIELDING_YPX_2_MM);

    qreal centerX = ScreenResolutionWidth/2;
    qreal centerY = ScreenResolutionHeight/2;

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
    qreal halfLine = ScreenResolutionWidth*K_CROSS_LINE_LENGTH/2.0;
    gCrossLine0 = canvas->addLine(centerX-halfLine,centerY,
                                  centerX+halfLine,centerY,
                                  QPen(QBrush(Qt::white),4));
    gCrossLine1 = canvas->addLine(centerX,centerY-halfLine,
                                  centerX,centerY+halfLine,
                                  QPen(QBrush(Qt::white),4));

    gCrossLine0->setZValue(-1);
    gCrossLine1->setZValue(-1);

    // Adding the target
    gTarget = canvas->addEllipse(TARGET_OFFSET_X/kx,
                                 TARGET_OFFSET_Y/ky,
                                 TARGET_R*2/kx,TARGET_R*2/ky,
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

bool FieldingManager::isPointInTargetBox(qreal x, qreal y, qint32 targetBox) const{
   return FieldingParser::isHitInTargetBox(hitTargetBoxes,targetBox,x,y);
}

void FieldingManager::drawPauseScreen(){

    canvas->clear();
    QString pauseText = config->getString(CONFIG_FIELDING_PAUSE_TEXT);
    qreal xpos, ypos;

    canvas->addRect(0,0,canvas->width(),canvas->height(),QPen(),QBrush(QColor(Qt::black)));

    QFont font;
    if (config->getBool(CONFIG_VR_ENABLED)){
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
