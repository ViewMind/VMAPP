#ifndef PERCEPTIONPARSER_H
#define PERCEPTIONPARSER_H

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

#define  K_WIDTH_MARGIN             0.16
#define  NUMBER_OF_ELEMENTS_PER_ROW 5
#define  NUMBER_OF_ELEMENTS_PER_COL 5
#define  YES_NO_FONT_SIZE           32
#define  AIR_YES_NO_TEXT_W          4    // Bouding rect of text is multiplied by this to generate surrounding frame's width.
#define  AIR_YES_NO_TEXT_H          2    // Bouding rect of text is multiplied by this to generate surrounding frame's height.
#define  MARGIN_YES_NO_TEXT         0.15 // Width percent.

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
