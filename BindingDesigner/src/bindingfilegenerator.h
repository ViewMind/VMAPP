#ifndef BINDINGFILEGENERATOR_H
#define BINDINGFILEGENERATOR_H

#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include "../../CommonClasses/common.h"
#include "../../CommonClasses/Experiments/bindingmanager.h"

class BindingFileGenerator
{
public:
    BindingFileGenerator();
    void generateFile(const QString &filename, bool bc, bool numbered, int ntargets);

private:

    // A slide class to do all the color manipulation required.
    class Slide {

    public:

        typedef enum {GT_3x2, GT_3x3} GridType;

        Slide(qint32 nt, GridType gt, bool mManhattanDof1);
        Slide(const Slide &s);

        QString toDescription() const;
        void setColors();
        void boundChange();
        void unboundChange();
        void generateRandomPositions(bool avoidExistingPositions = false);
        void resetPositions();
        bool areEqual(const Slide &s);

        QStringList pos;
        QStringList backs;
        QStringList crosses;
        QStringList remainingColors;
        QStringList remainingPos;
        GridType gridType;
        bool minimumManhattanDistanceOf1;
        qint32 nTargets;

    private:
        QList<qint32> select2RandomPositions();
        qint32 manhattanDistance(const QString &a, const QString &b);
    };

};

#endif // BINDINGFILEGENERATOR_H
