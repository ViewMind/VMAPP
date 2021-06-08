#ifndef PARKINSONPARSER_H
#define PARKINSONPARSER_H

#include <QList>
#include <QString>
#include "../eyetracker_defines.h"

#define MAZE_SEPARATOR        "#=======>"


class ParkinsonParser
{
public:

    typedef enum { PST_EMPTY, PST_WALL, PST_TARGET, PST_ARROW } ParkinsonSquareType;

    struct ParkinsonTarget {
        qint32 sc; qint32 sr; // Square coordinates
        qint32 x; qint32 y;   // Pixel coordinates
        qint32 order;
    };

    struct ParkinsonMaze {
        qint32 squareW;
        qint32 squareH;
        qint32 mazeWinSq;
        qint32 mazeHinSq;
        qint32 xOffset;
        qint32 yOffset;
        QString name;
        QList< QList<ParkinsonSquareType> > mazeDescription;
        QList<ParkinsonTarget> targets;
        void sortTargetsAndComputeCoordinates();
    };

    ParkinsonParser();
    bool parseParkinsonExperiment(const QString &contents, qreal screenWidth, qreal screenHeight);

    QString getError() const {return error;}
    QList<ParkinsonMaze> getMazes() const {return mazes;}
    QString getVersionString() const { return versionString; }

private:

    QString error;
    QString versionString;
    QList<ParkinsonMaze> mazes;

    static float MAZE_AREA_H;
    static float MAZE_AREA_W;
    static float SQUARE_SIDE_X;
    static float SQUARE_SIDE_Y;



};

#endif // PARKINSONPARSER_H
