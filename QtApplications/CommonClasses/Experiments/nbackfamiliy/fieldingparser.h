#ifndef FIELDINGPARSER_H
#define FIELDINGPARSER_H

#include <QString>
#include <QList>
#include <QPoint>
#include <QRectF>
#include <QSet>
#include "../../eyetracker_defines.h"
#include "../drawingconstantscalculator.h"

// Constants for drawing the squares, cross and target on the screen for fielding
// These values are the physical dimensions of the targets in mm.
// These are part of the parser in order to have access to the drawing dimension logic when processing data.
// The following ascii art enumerates the squares.
//               -------                -------
//               |  0  |                |  1  |
//               |     |                |     |
//               -------                -------
//               
//      -------                 |                 -------
//      |  5  |               -- --               |  2  |
//      |     |                 |                 |     |
//      -------                                   -------
//               
//               -------                -------
//               |  4  |                |  3  |
//               |     |                |     |
//               -------                -------


class FieldingParser
{
public:

    // Definition of structures used to load the sequences for each trial.
    struct Trial{
        QString id;
        QList<qint32> sequence;
    };

    FieldingParser();
    bool parseFieldingExperiment(const QString &contents, qreal resolutionWidth, qreal resolutionHeight);
    QList<Trial> getParsedTrials() const;
    QString getError() const;
    QString getVersionString() const;
    qint32 getTargetBoxForImageNumber(const QString &trialID, qint32 imgNum) const;
    QList<qint32> getSequenceForTrial(const QString &trialID);
    QList<QRectF> getDrawTargetBoxes() const;
    QList<QRectF> getHitTargetBoxes() const;

    // Logic used to determine if a fixation is "in" the target box or not.
    // static bool isHitInTargetBox(const QList<QRectF> &hitTargetBoxes, qint32 tbID, qreal x, qreal y);

private:
    QString error;
    QList<Trial> fieldingTrials;
    QString versionString;
    // The coordinates a dimensions for the target boxes to be drawn.
    QList<QRectF> drawTargetBoxes;
    // The coordinates and dimensions for the target boxes to recognize when they are hit.
    QList<QRectF> hitTargetBoxes;

    // Parsing constants.
    static const qint32 TARGET_BOX_0 =  0;
    static const qint32 TARGET_BOX_1 =  1;
    static const qint32 TARGET_BOX_2 =  2;
    static const qint32 TARGET_BOX_3 =  3;
    static const qint32 TARGET_BOX_4 =  4;
    static const qint32 TARGET_BOX_5 =  5;

    static const qint32 MAX_SEQUENCE_LENGTH = 6;
    static const qint32 OLD_SEQUENCE_LENGTH = 3;

    static const qint32 NBACK_EXP_DESCRIPTION_NUM_OF_TARGETS = 6;
    static const qint32 NBACK_RT_NUM_OF_TARGETS = 3;
    static const qint32 NBACK_MS_NUM_OF_TARGETS = 3;

    // Drawing constants.
    static const qreal K_RECT_WIDTH;
    static const qreal K_RECT_HEIGHT;
//    static const qreal TARGET_R;
//    static const qreal TARGET_OFFSET_X;
//    static const qreal TARGET_OFFSET_Y;

    static const qreal K_HORIZONAL_MARGIN;
    static const qreal K_SPACE_BETWEEN_BOXES;
    static const qreal K_VERTICAL_MARGIN;

    // These constants were derived by measuring the with and heigth of the squares in a FullHD 22 inch monitor
    // And eyeballing 1.5 cm margin around the target box. The proportion will be maintained no matter the size or where it is drawn
    // by using a proportion instead of a constant.
    static const qreal TARGET_BOX_EX_W;
    static const qreal TARGET_BOX_EX_H;


};

#endif // FIELDINGPARSER_H

