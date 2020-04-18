#ifndef EDPNBACKRT_H
#define EDPNBACKRT_H

#include <QRectF>

#include "edpbase.h"
#include "../../Experiments/fieldingmanager.h"

// Defines for fielding experiment raw data file
#define   FIELDING_TI                                   0
#define   FIELDING_XR                                   1
#define   FIELDING_YR                                   2
#define   FIELDING_XL                                   3
#define   FIELDING_YL                                   4
#define   FIELDING_PR                                   5
#define   FIELDING_PL                                   6

class EDPNBackRT : public EDPBase
{
public:
    EDPNBackRT(ConfigurationManager *c);

    // Function reimplementation headers.
    bool doEyeDataProcessing(const QString &data);

    // The number of trials done.
    qint32 getNumberOfTrials() const {return numberOfTrials;}

private:

    struct TargetHitSearcherReturn{
        QString targetHit;
        QString isIn;
        QString sequenceCompleted;
        QString nback;
    };

    struct TargetHitSearcher {
    public:
        void setTargetBoxes(const QList<QRectF> &tBoxes);
        void setNewTrial(const QString &id, const QList<qint32> trialSeq);
        TargetHitSearcherReturn isHit(qreal x, qreal y, const QString &imgID);
        void reset();
    private:
        QString trialID;
        QList<QRectF> hitTargetBoxes;
        QList<qint32> trialSequence;
        qint32 expectedTargetIndexInSequence;
    };

    // Initialization of the data matrix (header row)
    void initializeFieldingDataMatrix();

    // Adding the data of each image.
    void appendDataToFieldingMatrix(const DataMatrix &data,
                                    const QString &trialID,
                                    const QString &imgID,
                                    const QList<qint32> &trialSequence);


    // Actually saving the stored data to a file on disk.
    bool finalizeFieldingDataMatrix();

    // Parser for the fielding data
    FieldingParser parser;

    // The center of the screen to measure sacadic latency.
    qreal centerX, centerY;

    // The number of trials
    qint32 numberOfTrials;

    // Center margins
    qreal centerMinX, centerMaxX, centerMinY, centerMaxY;

    // Target Hit Searcher object
    TargetHitSearcher targetHitSearcher;

    // Temporarily store all processed data separated for eye.
    QStringList csvHeader;
    QList<QStringList> ldata;
    QList<QStringList> rdata;

};

#endif // EDPNBACKRT_H
