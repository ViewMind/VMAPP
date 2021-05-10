#ifndef TARGETHITSEARCHER_H
#define TARGETHITSEARCHER_H

#include "../../Experiments/fieldingparser.h"
#include <QDebug>
#include <QString>
#include <QRectF>
#include <QList>

/**
 * The TargetHitSearcher class is used for generating data required for different nback experiments
 * including the RT and MS versions.
 */

class TargetHitSearcher
{
public:

    struct TargetHitSearcherReturn{
        QString targetHit;
        QString isIn;
        QString sequenceCompleted;
        QString nback;
    };

    typedef enum {THL_RT, THL_MS} TargetHitLogic;

    TargetHitSearcher();

    // Set the type of target hit to use
    void setTargetHitLogic(TargetHitLogic thl);

    // Set the rectangles that represetn the target boxes givent the geometry of the monitor.
    void setTargetBoxes(const QList<QRectF> &tBoxes);

    // Configuring the logic for a new trial with a given id and sequence.
    void setNewTrial(const QString &id, const QList<qint32> trialSeq);

    // Computes whether the target has hit a target box, and, if it is in the correct sequence, which is the current order of the sequence.
    TargetHitSearcherReturn isHit(qreal x, qreal y, const QString &imgID);

    // Reset the state machinge for the isHit Logic.
    void reset();


private:
    QString trialID;
    QList<QRectF> hitTargetBoxes;
    QList<qint32> trialSequence;
    qint32 expectedTargetIndexInSequence;
    QList<bool> allMSTargetHits;

    TargetHitLogic thl;

    TargetHitSearcherReturn isHitRTVariation(qreal x, qreal y, const QString &imgID);
    TargetHitSearcherReturn isHitMSVariation(qreal x, qreal y, const QString &imgID);

};

#endif // TARGETHITSEARCHER_H
