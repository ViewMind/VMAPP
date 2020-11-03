#ifndef FIELDINGPARSER_H
#define FIELDINGPARSER_H

#include <QString>
#include <QList>
#include <QPoint>
#include <QRectF>
#include <QSet>

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
             

#define   RECT_WIDTH                                    163/4
#define   RECT_HEIGHT                                   155/4
#define   TARGET_R                                      42/4
#define   TARGET_OFFSET_X                               39/4
#define   TARGET_OFFSET_Y                               35/4

#define   TARGET_BOX_0                                  0
#define   TARGET_BOX_1                                  1
#define   TARGET_BOX_2                                  2
#define   TARGET_BOX_3                                  3
#define   TARGET_BOX_4                                  4
#define   TARGET_BOX_5                                  5

#define   K_HORIZONAL_MARGIN                            0.06
#define   K_SPACE_BETWEEN_BOXES                         0.09
#define   K_VERTICAL_MARGIN                             0.06

#define   K_CROSS_LINE_LENGTH                           0.05

// These constants were derived by measuring the with and heigth of the squares in a FullHD 22 inch monitor
// And eyeballing 1.5 cm margin around the target box. The proportion will be maintained no matter the size or where it is drawn
// by using a proportion instead of a constant.

#define   TARGET_BOX_EX_W                               1.5/6.0
#define   TARGET_BOX_EX_H                               1.5/5.6

#define   MAX_SEQUENCE_LENGTH                           6
#define   OLD_SEQUENCE_LENGTH                           3


class FieldingParser
{
public:

    // Definition of structures used to load the sequences for each trial.
    struct Trial{
        QString id;
        QList<qint32> sequence;
    };

    FieldingParser();
    bool parseFieldingExperiment(const QString &contents, qreal resolutionWidth, qreal resolutionHeight, qreal x_px_2_mm, qreal y_px_2_mm);
    QList<Trial> getParsedTrials() const;
    QString getError() const;
    QString getVersionString() const;
    qint32 getTargetBoxForImageNumber(const QString &trialID, qint32 imgNum) const;
    QList<qint32> getSequenceForTrial(const QString &trialID);
    QList<QRectF> getDrawTargetBoxes() const;
    QList<QRectF> getHitTargetBoxes() const;

    // Logic used to determine if a fixation is "in" the target box or not.
    static bool isHitInTargetBox(const QList<QRectF> &hitTargetBoxes, qint32 tbID, qreal x, qreal y);

private:
    QString error;
    QList<Trial> fieldingTrials;
    QString versionString;
    // The coordinates a dimensions for the target boxes to be drawn.
    QList<QRectF> drawTargetBoxes;
    // The coordinates and dimensions for the target boxes to recognize when they are hit.
    QList<QRectF> hitTargetBoxes;
};

#endif // FIELDINGPARSER_H

