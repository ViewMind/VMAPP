#include "parkinsonparser.h"

ParkinsonParser::ParkinsonParser()
{

}

// Simple Bubble Sort for the targets. This way the list of targets is ordered according to how the patient should see them.
void ParkinsonParser::ParkinsonMaze::sortTargetsAndComputeCoordinates(){
    bool done = false;
    while (!done){
        done = true;
        for (qint32 i = 0; i < targets.size()-1; i++){
            if (targets.at(i).order > targets.at(i+1).order){
                ParkinsonTarget t = targets.at(i);
                targets[i] = targets.at(i+1);
                targets[i+1] = t;
                done = false;
            }
        }
    }

    // Sorting done. Coordinates are computed. NOTE, this DOES not depend on targets being ordered. This could have been done before the sorting.
    for (qint32 i = 0; i < targets.size(); i++){
        targets[i].x = static_cast<qint32>(xOffset + (targets.at(i).sc+0.5)*squareW);
        targets[i].y = static_cast<qint32>(yOffset + (targets.at(i).sr+0.5)*squareH);
    }
}


bool ParkinsonParser::parseParkinsonExperiment(const QString &contents, qreal screenWidth, qreal screenHeight, qreal xpx2MM, qreal ypx2MM){

    QStringList lines = contents.split("\n");
    QList< QList<ParkinsonSquareType> > maze;
    QList<ParkinsonTarget> targets;

    // First line is the version string
    versionString = lines.first();
    lines.removeFirst();

    qint32 previousLineSize = 0;
    QString name = "";
    QMap<QString,ParkinsonSquareType> descriptorMap;
    descriptorMap["WW"] = PST_EMPTY;
    descriptorMap["BB"] = PST_WALL;
    descriptorMap["AA"] = PST_ARROW;

    qint32 squareSideY = static_cast<qint32>(SQUARE_SIDE_IN_MM/ypx2MM);
    qint32 squareSideX = static_cast<qint32>(SQUARE_SIDE_IN_MM/xpx2MM);

    error = "";

    for (qint32 i = 0; i < lines.size(); i++){

        QString line = lines.at(i);
        line = line.trimmed();

        if (line.isEmpty()) continue;

        if (line.startsWith(MAZE_SEPARATOR)) {

            QStringList parts = line.split(" ",QString::SkipEmptyParts);
            if (parts.size() != 2){
                error = "Error in maze title format: " + lines.at(i);
                return false;
            }
            if (name.isEmpty()){
                name = parts.last();
                continue;
            }

            // This is a completed maze.
            ParkinsonMaze pmaze;
            pmaze.mazeDescription = maze;
            pmaze.name = name;
            pmaze.mazeHinSq = maze.size();
            pmaze.mazeWinSq = maze.first().size();
            pmaze.squareH = squareSideY;
            pmaze.squareW = squareSideX;
            pmaze.xOffset = static_cast<qint32>((screenWidth - pmaze.squareW*pmaze.mazeWinSq)/2);
            pmaze.yOffset = static_cast<qint32>((screenHeight - pmaze.squareH*pmaze.mazeHinSq)/2);
            pmaze.targets = targets;
            pmaze.sortTargetsAndComputeCoordinates();
            mazes << pmaze;

//            qWarning() << "TARGETS FOUND: " << targets.size();
//            for (qint32 k = 0; k < pmaze.targets.size(); k++){
//                qWarning() << "TARGET" << pmaze.targets.at(k).order << pmaze.targets.at(k).sr << pmaze.targets.at(k).sc << pmaze.targets.at(k).x << pmaze.targets.at(k).y;
//            }

            name = parts.last();
            targets.clear();
            maze.clear();

        }
        else {

            QStringList squares = line.split("|",QString::SkipEmptyParts);

            // Checking column size consistency
            if (previousLineSize == 0) previousLineSize = squares.size();
            else if (previousLineSize != squares.size()){
                error = "Line size inconsitency for line: " + line + ". Its size is " + QString::number(squares.size())
                        + " while previous size was: " + QString::number(previousLineSize);
                return false;
            }

            // Checking line content consistency.
            QList<ParkinsonSquareType> row;
            for (qint32 j = 0; j < previousLineSize; j++){

                if (descriptorMap.contains(squares.at(j))){
                    row << descriptorMap.value(squares.at(j));
                }
                else{
                     bool ok;
                     ParkinsonTarget target;
                     target.order = static_cast<qint32>(squares.at(j).toUInt(&ok));
                     if (!ok){
                         error = "Line contains invalid target number: " + squares.at(j);
                         return false;
                     }
                     target.sc = j;
                     target.sr = maze.size();
                     targets << target;
                     row << PST_TARGET;
                }
            }

            maze << row;
        }
    }

    if (!maze.isEmpty()){
        // This is a completed maze.
        ParkinsonMaze pmaze;
        pmaze.mazeDescription = maze;
        pmaze.name = name;
        pmaze.mazeHinSq = maze.size();
        pmaze.mazeWinSq = maze.first().size();
        pmaze.squareH = squareSideY;
        pmaze.squareW = squareSideX;
        pmaze.xOffset = static_cast<qint32>((screenWidth - pmaze.squareW*pmaze.mazeWinSq)/2);
        pmaze.yOffset = static_cast<qint32>((screenHeight - pmaze.squareH*pmaze.mazeHinSq)/2);
        pmaze.targets = targets;
        pmaze.sortTargetsAndComputeCoordinates();
        mazes << pmaze;

//        qWarning() << "TARGETS FOUND: " << targets.size();
//        for (qint32 k = 0; k < pmaze.targets.size(); k++){
//            qWarning() << "TARGET" << pmaze.targets.at(k).order << pmaze.targets.at(k).sr << pmaze.targets.at(k).sc << pmaze.targets.at(k).x << pmaze.targets.at(k).y;
//        }

    }

    return true;
}
