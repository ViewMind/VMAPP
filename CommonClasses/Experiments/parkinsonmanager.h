#ifndef PARKINSONMANAGER_H
#define PARKINSONMANAGER_H

#include "experimentdatapainter.h"
#include "parkinsonparser.h"

class ParkinsonManager: public ExperimentDataPainter
{
public:

    ParkinsonManager();

    // Reimplemented functions for common interface.
    bool parseExpConfiguration(const QString &contents);
    void init(ConfigurationManager *c);
    qint32 size() const { return 1; }
    qreal  sizeToProcess() const { return 1; }

    ParkinsonParser::ParkinsonMaze getCurrentMaze() const;

    // Drawing the mazes.
    bool drawCurrentMaze();

private:

    QList<ParkinsonParser::ParkinsonMaze> mazes;
    qint32 currentMaze;

    void enableDemoMode();

    void drawMaze(const ParkinsonParser::ParkinsonMaze &maze);

};

#endif // PARKINSONMANAGER_H
