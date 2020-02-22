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

    // Initialization of the data matrix (header row)
    void initializeFieldingDataMatrix();

    // Adding the data of each image.
    void appendDataToFieldingMatrix(const DataMatrix &data,
                                    const QString &trialID,
                                    const QString &imgID,
                                    QList<qint32> trialSequence);


    // Actually saving the stored data to a file on disk.
    bool finalizeFieldingDataMatrix();

    // Parser for the fielding data
    FieldingParser parser;

    // The center of the screen to measure sacadic latency.
    qreal centerX, centerY;

    // The number of trials
    qint32 numberOfTrials;

    // Margin for hit detection
    qreal dH, dW;

    // Center margins
    qreal centerMinX, centerMaxX, centerMinY, centerMaxY;

    // The target box tollerance as a function of which box I'm looking.
    QList<QRectF> targetBoxes;

    // Drawing constants. Transform the fix measurmente to monitor or HMD sizes.
    qreal fieldingKx, fieldingKy;

    // Temporarily store all processed data separated for eye.
    QStringList csvHeader;
    QList<QStringList> ldata;
    QList<QStringList> rdata;

};

#endif // EDPNBACKRT_H
