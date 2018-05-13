#ifndef BINDINGFILEGENERATOR_H
#define BINDINGFILEGENERATOR_H

#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include "../../CommonClasses/Experiments/common.h"
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
        Slide(qint32 nt);
        Slide(const Slide &s);

        QString toDescription() const;
        void setColors();
        void boundChange();
        void unboundChange();
        void generateRandomPositions();
        void resetPositions();
        bool areEqual(const Slide &s);

        QStringList pos;
        QStringList backs;
        QStringList crosses;
        QStringList remainingColors;
        QStringList remainingPos;
        qint32 nTargets;
    private:
        QList<qint32> select2RandomPositions();
    };

};

#endif // BINDINGFILEGENERATOR_H
