#ifndef EDPFIELDING_H
#define EDPFIELDING_H

#include "edpbase.h"
#include "../../CommonClasses/Experiments/fieldingmanager.h"

// Defines for fielding experiment raw data file
#define   FIELDING_TI                                   0
#define   FIELDING_XR                                   1
#define   FIELDING_YR                                   2
#define   FIELDING_XL                                   3
#define   FIELDING_YL                                   4
#define   FIELDING_PR                                   5
#define   FIELDING_PL                                   6

// Warning threshold for low number of data points. Unused for now
// #define   FIELDING_WARNING_NUM_DATAPOINTS               6

class EDPFielding : public EDPBase
{
public:
    EDPFielding(ConfigurationManager *c);

    // Function reimplementation headers.
    bool doEyeDataProcessing(const QString &data);

    // The number of trials done.
    qint32 getNumberOfTrials() const {return numberOfTrials;}

private:

    // Initialization of the data matrix (header row)
    bool initializeFieldingDataMatrix();

    // Adding the data of each image.
    bool appendDataToFieldingMatrix(const DataMatrix &data,
                                    const QString &trialID,
                                    const QString &imgID,
                                    const qreal &targetX,
                                    const qreal &targetY);

    // The center of the screen to measure sacadic latency.
    qreal centerX,centerY;

    // The number of trials
    qint32 numberOfTrials;

    // Margin for hit detection
    qreal dH, dW;

};

#endif // EDPFIELDING_H
