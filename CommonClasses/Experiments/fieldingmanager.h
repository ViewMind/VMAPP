#ifndef FIELDINGDRAW_H
#define FIELDINGDRAW_H

#include "experimentdatapainter.h"
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QTextStream>
#include <QFile>
#include <QSet>

// Constants for drawing the squares, cross and target on the screen for fielding
#define   AREA_WIDTH                                    1024
#define   AREA_HEIGHT                                   768
#define   RECT_WIDTH                                    163
#define   RECT_HEIGHT                                   155
#define   RECT_0_X                                      200
#define   RECT_0_Y                                      76
#define   RECT_1_X                                      660
#define   RECT_1_Y                                      76
#define   RECT_2_X                                      830
#define   RECT_2_Y                                      306
#define   RECT_3_X                                      660
#define   RECT_3_Y                                      536
#define   RECT_4_X                                      200
#define   RECT_4_Y                                      536
#define   RECT_5_X                                      30
#define   RECT_5_Y                                      306
#define   TARGET_R                                      42
#define   TARGET_OFFSET_X                               39
#define   TARGET_OFFSET_Y                               35
#define   CROSS_P0_X                                    511
#define   CROSS_P0_Y                                    362
#define   CROSS_P1_X                                    533
#define   CROSS_P1_Y                                    383
#define   CROSS_P2_X                                    511
#define   CROSS_P2_Y                                    405
#define   CROSS_P3_X                                    490
#define   CROSS_P3_Y                                    383

class FieldingManager: public ExperimentDataPainter
{
public:

    // Draw states for the Fielding experiments
    typedef enum {DS_CROSS, DS_CROSS_TARGET, DS_1, DS_2, DS_3} DrawState;

    // Definition of structures used to load the sequences for each trial.
    struct Trial{
        QString id;
        QList<qint32> sequence;
    };

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
    Trial getTrial(qint32 i) const { return fieldingTrials.at(i);}

private:

    // The list of trials
    QList<Trial> fieldingTrials;

    // List of the rectangle final positions, used to postion the target base on rectangle id
    QList<QPoint> rectangleLocations;

    // Graphical items shown in the screen
    QGraphicsEllipseItem    *gTarget;
    QGraphicsLineItem       *gCrossLine0;
    QGraphicsLineItem       *gCrossLine1;
    QGraphicsSimpleTextItem *gText1;
    QGraphicsSimpleTextItem *gText2;
    QGraphicsSimpleTextItem *gText3;

};

#endif // FIELDINGDRAW_H
