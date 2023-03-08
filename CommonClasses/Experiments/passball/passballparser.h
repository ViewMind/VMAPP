#ifndef PASSBALLPARSER_H
#define PASSBALLPARSER_H


#include <QString>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "../../BallPathGenerator/ballpathgenerator.h"
#include "../../eyetracker_defines.h"

class PassBallParser
{
public:

    static const QString ORIGIN_TARGETS;
    static const QString MIDDLE_TARGETS;
    static const QString LAST_TARGETS;
    static const QString PASS_TARGETS;
    static const QString HAND_CONTROL;
    static const QString SPEED_LIMIT;

    PassBallParser();

    bool generatePassBallSequence();
    QVariantMap getStudyDescriptionPayloadContent() const;
    QString getError() const;
    qint32 getSize() const;


private:
    QString error;
    QVariantMap study_description;

    // Parameters required by teh ball path generator algorithm.
    const qint32 N_SEQUENCES = 100;
    const qint32 N_PASSES    = 50;
    const qint32 NROWS = 5;
    const qint32 NCOLS = 5;
    const qint32 TIMES_PER_HOLE = 3;
    const qreal  MIN_DISTANCE = 2.0;
    const qreal  MEAN_DISTANCE = 3;

};

#endif // PASSBALLPARSER_H
