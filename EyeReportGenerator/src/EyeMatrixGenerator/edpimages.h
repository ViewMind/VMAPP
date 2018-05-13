#ifndef EDPIMAGES_H
#define EDPIMAGES_H

#include "edpbase.h"
#include "../../CommonClasses/Experiments/bindingmanager.h"


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
