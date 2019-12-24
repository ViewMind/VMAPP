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
    if (!parser.parseFieldingExperiment(contents)){
        error = parser.getError();
        return false;
    }

    fieldingTrials = parser.getParsedTrials();
    versionString = parser.getVersionString();

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

    qreal WScreen = AREA_WIDTH/kx;
    qreal HScreen = AREA_HEIGHT/ky;
    qreal generalOffsetX = (ScreenResolutionWidth - WScreen)/2;
    qreal generalOffsetY = (ScreenResolutionHeight - HScreen)/2;

    // Background
    canvas->clear();
    canvas->addRect(0,0,ScreenResolutionWidth,ScreenResolutionHeight,QPen(),QBrush(Qt::black));

    // Rectangle origins in order, in order
    rectangleLocations.clear();
    rectangleLocations << QPoint(static_cast<qint32>(generalOffsetX+RECT_0_X/kx),static_cast<qint32>(generalOffsetY+RECT_0_Y/ky));
    rectangleLocations << QPoint(static_cast<qint32>(generalOffsetX+RECT_1_X/kx),static_cast<qint32>(generalOffsetY+RECT_1_Y/ky));
    rectangleLocations << QPoint(static_cast<qint32>(generalOffsetX+RECT_2_X/kx),static_cast<qint32>(generalOffsetY+RECT_2_Y/ky));
    rectangleLocations << QPoint(static_cast<qint32>(generalOffsetX+RECT_3_X/kx),static_cast<qint32>(generalOffsetY+RECT_3_Y/ky));
    rectangleLocations << QPoint(static_cast<qint32>(generalOffsetX+RECT_4_X/kx),static_cast<qint32>(generalOffsetY+RECT_4_Y/ky));
    rectangleLocations << QPoint(static_cast<qint32>(generalOffsetX+RECT_5_X/kx),static_cast<qint32>(generalOffsetY+RECT_5_Y/ky));

    // Adding the rectangles to the scene
    for (qint32 i = 0; i < rectangleLocations.size(); i++){
        QGraphicsRectItem *rect = canvas->addRect(0,0,RECT_WIDTH/kx,RECT_HEIGHT/ky,
                                                          QPen(QBrush(Qt::white),6),
                                                          QBrush(Qt::black));
        rect->setPos(rectangleLocations.at(i));
    }

    // Adding the cross
    gCrossLine0 = canvas->addLine(CROSS_P0_X/kx+generalOffsetX,CROSS_P0_Y/ky+generalOffsetY,
                                          CROSS_P2_X/kx+generalOffsetX,CROSS_P2_Y/ky+generalOffsetY,
                                          QPen(QBrush(Qt::white),4));
    gCrossLine1 = canvas->addLine(CROSS_P1_X/kx+generalOffsetX,CROSS_P1_Y/ky+generalOffsetY,
                                          CROSS_P3_X/kx+generalOffsetX,CROSS_P3_Y/ky+generalOffsetY,
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
    gTarget->setPos(rectangleLocations.at(fieldingTrials.at(trial).sequence.at(image)));
}

QPoint FieldingManager::getTargetPoint(qint32 trial, qint32 image) const{
    return rectangleLocations.at(fieldingTrials.at(trial).sequence.at(image));
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
