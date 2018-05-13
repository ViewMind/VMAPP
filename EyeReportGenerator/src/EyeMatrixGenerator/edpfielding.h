#ifndef EDPFIELDING_H
#define EDPFIELDING_H

#include "edpbase.h"
#include "../../CommonClasses/Experiments/fieldingmanager.h"



class EDPFielding : public EDPBase
{
public:
    EDPFielding(ConfigurationManager *c);

    // Function reimplementation headers.
    bool doEyeDataProcessing(const QString &data);

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

    // Margin for hit detection
    qreal dH, dW;

};

#endif // EDPFIELDING_H
