#ifndef PERCEPTIONPARSER_H
#define PERCEPTIONPARSER_H

#include "../../ConfigurationManager/configurationmanager.h"
#include "../../eyetracker_defines.h"
#include "../drawingconstantscalculator.h"

class PerceptionParser
{
public:

    typedef enum {TT_UP, TT_DOWN, TT_RIGHT, TT_LEFT, TT_NONE, TT_X} TriangleType;

    struct PerceptionTrial {
        QString name;
        TriangleType trialType;
        QList< QList<TriangleType> > trialDescription;
        qint32 uniqueRow;
        qint32 uniqueCol;
        QString toString() const;
    };


    PerceptionParser();

    bool parsePerceptionExperiment(const QString &contents);
    QString getError() const;
    QString getVersionString() const;
    QList<PerceptionTrial> getTrials() const;

private:

    QString error;
    QString versionString;
    QList<PerceptionTrial> trials;
    TriangleType stringCharToTriangleType(const QString &tt) const;

};

#endif // PERCEPTIONPARSER_H
