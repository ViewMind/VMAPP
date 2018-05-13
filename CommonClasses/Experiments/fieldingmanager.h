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

class FieldingManager: public ExperimentDataPainter
{
public:

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
