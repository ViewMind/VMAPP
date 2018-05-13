#include "fieldingmanager.h"

FieldingManager::FieldingManager()
{

}

bool FieldingManager::parseExpConfiguration(const QString &contents){

    // Generating the contents from the phrases
    QStringList lines = contents.split('\n',QString::KeepEmptyParts);

    fieldingTrials.clear();

    // Needed to check for unique ids.
    QSet<QString> uniqueIDs;

    for (qint32 i = 0; i < lines.size(); i++){

        if (lines.at(i).isEmpty()) continue;

        QStringList tokens = lines.at(i).split(' ',QString::SkipEmptyParts);
        if (tokens.size() != 4){
            error = "Invalid line: " + lines.at(i) + " should only have 4 items separated by space, the id and three numbers";
            return false;
        }

        Trial t;
        t.id = tokens.first();

        // If the id was defined before, its an error. Otherwise we add it to the ids found list.
        if (uniqueIDs.contains(t.id)){
            error = "Cannot use the same id twice: " + t.id + " has already been found";
            return false;
        }
        uniqueIDs << t.id;

        for (qint32 j =1; j < tokens.size(); j++){
            bool ok;
            qint32 value = tokens.at(j).toInt(&ok);
            if (!ok){
                error = "In line: " + lines.at(i) + ", " + tokens.at(j) + " is not a valid integer";
                return false;
            }
            if ((value < 0) || (value > 5)){
                error = "In line: " + lines.at(i) + ", " + tokens.at(j) + " is not an integer between 0 and 5";
                return false;
            }
            t.sequence << value;
        }

        fieldingTrials << t;

    }

    return true;

}

void FieldingManager::init(ConfigurationManager *c){
    ExperimentDataPainter::init(c);
    drawBackground();
}

void FieldingManager::drawBackground(){
    qint32 WScreen = canvas->width();
    qint32 HScreen = canvas->height();
    qint32 generalOffsetX = (WScreen - AREA_WIDTH)/2;
    qint32 generalOffsetY = (HScreen - AREA_HEIGHT)/2;

    // Background
    canvas->addRect(generalOffsetX,generalOffsetY,AREA_WIDTH,AREA_HEIGHT,QPen(),QBrush(Qt::black));

    // Rectangle origins in order, in order
    rectangleLocations.clear();
    rectangleLocations << QPoint(generalOffsetX+RECT_0_X,generalOffsetY+RECT_0_Y);
    rectangleLocations << QPoint(generalOffsetX+RECT_1_X,generalOffsetY+RECT_1_Y);
    rectangleLocations << QPoint(generalOffsetX+RECT_2_X,generalOffsetY+RECT_2_Y);
    rectangleLocations << QPoint(generalOffsetX+RECT_3_X,generalOffsetY+RECT_3_Y);
    rectangleLocations << QPoint(generalOffsetX+RECT_4_X,generalOffsetY+RECT_4_Y);
    rectangleLocations << QPoint(generalOffsetX+RECT_5_X,generalOffsetY+RECT_5_Y);

    // Adding the rectangles to the scene
    for (qint32 i = 0; i < rectangleLocations.size(); i++){
        QGraphicsRectItem *rect = canvas->addRect(0,0,RECT_WIDTH,RECT_HEIGHT,
                                                          QPen(QBrush(Qt::white),2),
                                                          QBrush(Qt::black));
        rect->setPos(rectangleLocations.at(i));
    }

    // Adding the cross
    gCrossLine0 = canvas->addLine(CROSS_P0_X+generalOffsetX,CROSS_P0_Y+generalOffsetY,
                                          CROSS_P2_X+generalOffsetX,CROSS_P2_Y+generalOffsetY,
                                          QPen(QBrush(Qt::white),4));
    gCrossLine1 = canvas->addLine(CROSS_P1_X+generalOffsetX,CROSS_P1_Y+generalOffsetY,
                                          CROSS_P3_X+generalOffsetX,CROSS_P3_Y+generalOffsetY,
                                          QPen(QBrush(Qt::white),4));

    gCrossLine0->setZValue(-1);
    gCrossLine1->setZValue(-1);

    // Adding the target
    gTarget = canvas->addEllipse(TARGET_OFFSET_X,
                                 TARGET_OFFSET_Y,
                                 TARGET_R*2,TARGET_R*2,
                                 QPen(),QBrush(Qt::red));

    // Adding the text
    QFont letterFont;
    letterFont.setFamily("Courier New");
    letterFont.setPointSize(50);
    letterFont.setBold(true);

    gText1 = canvas->addSimpleText("1",letterFont);
    qint32 x = (AREA_WIDTH - gText1->boundingRect().width())/2 + generalOffsetX;
    qint32 y = (AREA_HEIGHT - gText1->boundingRect().height())/2 + generalOffsetY;
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
