#ifndef FIELDINGPARSER_H
#define FIELDINGPARSER_H

#include <QString>
#include <QList>

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
             

#define   AREA_WIDTH                                    256
#define   AREA_HEIGHT                                   192
#define   RECT_WIDTH                                    163/4
#define   RECT_HEIGHT                                   155/4
#define   RECT_0_X                                      200/4
#define   RECT_0_Y                                      76/4
#define   RECT_1_X                                      660/4
#define   RECT_1_Y                                      76/4
#define   RECT_2_X                                      830/4
#define   RECT_2_Y                                      306/4
#define   RECT_3_X                                      660/4
#define   RECT_3_Y                                      536/4
#define   RECT_4_X                                      200/4
#define   RECT_4_Y                                      536/4
#define   RECT_5_X                                      30/4
#define   RECT_5_Y                                      306/4
#define   TARGET_R                                      42/4
#define   TARGET_OFFSET_X                               39/4
#define   TARGET_OFFSET_Y                               35/4
#define   CROSS_P0_X                                    511/4
#define   CROSS_P0_Y                                    362/4
#define   CROSS_P1_X                                    533/4
#define   CROSS_P1_Y                                    383/4
#define   CROSS_P2_X                                    511/4
#define   CROSS_P2_Y                                    405/4
#define   CROSS_P3_X                                    490/4
#define   CROSS_P3_Y                                    383/4



class FieldingParser
{
public:

    // Definition of structures used to load the sequences for each trial.
    struct Trial{
        QString id;
        QList<qint32> sequence;
    };

    FieldingParser();
    bool parseFieldingExperiment(const QString &contents);
    QList<Trial> getParsedTrials() const;
    QString getError() const;
    QString getVersionString() const;
    qint32 getTargetBoxForImageNumber(const QString &trialID, qint32 imgNum) const;

private:
    QString error;
    QList<Trial> fieldingTrials;
    QString versionString;

};

#endif // FIELDINGPARSER_H
