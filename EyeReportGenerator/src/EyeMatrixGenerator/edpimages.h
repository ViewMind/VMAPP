#ifndef EDPIMAGES_H
#define EDPIMAGES_H

#include "edpbase.h"
#include "../../CommonClasses/common.h"
#include "../../CommonClasses/Experiments/bindingmanager.h"

// Warning threshold for low number of data points.
#define   BINDING_WARNING_NUM_DATAPOINTS                10

// Defines for the image experiment raw data file
#define   IMAGE_TI                                      0
#define   IMAGE_XR                                      1
#define   IMAGE_YR                                      2
#define   IMAGE_XL                                      3
#define   IMAGE_YL                                      4
#define   IMAGE_PR                                      5
#define   IMAGE_PL                                      6

class EDPImages: public EDPBase
{
public:
    EDPImages(ConfigurationManager *c);

    // Function reimplementation headers.
    bool doEyeDataProcessing(const QString &data);

private:

    // Initialization of the data matrix (header row)
    bool initializeImageDataMatrix();

    // Adding the data of each image.
    bool appendDataToImageMatrix(const DataMatrix &data, const QString &trialName, const QString &isTrial, const QString &response);

};

#endif // EDPIMAGES_H
