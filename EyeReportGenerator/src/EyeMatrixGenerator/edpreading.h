#ifndef EDPREADING_H
#define EDPREADING_H

#include "edpbase.h"
#include "../../CommonClasses/Experiments/readingmanager.h"

class EDPReading : public EDPBase
{
public:
    EDPReading(ConfigurationManager *c);

    // Function reimplementation headers.
    bool doEyeDataProcessing(const QString &data);

private:

    // Intializing the output matrix.
    bool initializeReadingDataMatrix();

    // Append data to output matrix.
    bool appendDataToReadingMatrix(const DataMatrix &data, const QString &imgID);

};

#endif // EDPREADING_H
