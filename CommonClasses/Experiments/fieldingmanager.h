#ifndef FIELDINGDRAW_H
#define FIELDINGDRAW_H

#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QTextStream>
#include <QFile>
#include <QSet>

#include "fieldingparser.h"
#include "experimentdatapainter.h"

//#define ENABLE_DRAW_OF_HIT_TARGET_BOXES

class FieldingManager: public ExperimentDataPainter
{
public:

    // Draw states for the Fielding experiments
    typedef enum {DS_CROSS, DS_CROSS_TARGET, DS_1, DS_2, DS_3, DS_TARGET_BOX_ONLY} DrawState;

    // Constructor
    FieldingManager();

    // Basic functions to reimplement.
    bool parseExpConfiguration(const QString &contents);
    void init(ConfigurationManager *c);
    qint32 size() const {return fieldingTrials.size();}
    qreal sizeToProcess() const {return fieldingTrials.size()*3;}

    // The actual drawing function for the background.
    void drawBackground();

    // Selects which element to show based on the draw state
    void setDrawState(DrawState ds);

    // Setting the target (Red Circle) position.
    void setTargetPosition(qint32 trial, qint32 image);

    // Setting the target position from the trial name.
    bool setTargetPositionFromTrialName(const QString &trial, qint32 image);

    // The coordinates where the target is drawn.
    QPoint getTargetPoint(qint32 trial, qint32 image) const;

    // Getting an indivual trial.
    FieldingParser::Trial getTrial(qint32 i) const { return fieldingTrials.at(i);}

    // Gets the expected hit sequence for a given trial
    QList<qint32> getExpectedTargetSequenceForTrial(qint32 trial) const;

    // Auxiliary function that check if a point is in a given target box
    bool isPointInTargetBox(qreal x, qreal y, qint32 targetBox) const;

    // Drawing the pause text
    void drawPauseScreen();

private:

    // The list of trials
    QList<FieldingParser::Trial> fieldingTrials;

    // Graphical items shown in the screen
    QGraphicsEllipseItem    *gTarget;
    QGraphicsLineItem       *gCrossLine0;
    QGraphicsLineItem       *gCrossLine1;
    QGraphicsSimpleTextItem *gText1;
    QGraphicsSimpleTextItem *gText2;
    QGraphicsSimpleTextItem *gText3;

    // The actual target boxes. (To recognize a hit AND to be drawn)
    QList<QRectF> hitTargetBoxes;
    QList<QRectF> drawTargetBoxes;

    void enableDemoMode();

};

#endif // FIELDINGDRAW_H
