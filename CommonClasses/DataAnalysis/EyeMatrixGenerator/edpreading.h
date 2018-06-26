#ifndef EDPREADING_H
#define EDPREADING_H

#include "edpbase.h"
#include "../../CommonClasses/common.h"
#include "../../CommonClasses/Experiments/readingmanager.h"

// Warning threshold for low number of data points.
#define   READING_WARNING_NUM_DATAPOINTS                10

// Defines for the reading experiment raw data file.
#define   READ_TI                                       0
#define   READ_XR                                       1
#define   READ_YR                                       2
#define   READ_XL                                       3
#define   READ_YL                                       4
#define   READ_WR                                       5
#define   READ_CR                                       6
#define   READ_WL                                       7
#define   READ_CL                                       8
#define   READ_SL                                       9
#define   READ_PR                                       10
#define   READ_PL                                       11

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
