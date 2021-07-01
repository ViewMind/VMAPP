#ifndef EDPIMAGES_H
#define EDPIMAGES_H

#include "edpbase.h"
#include "../../common.h"
#include "../../Experiments/bindingparser.h"

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

#define   STR_TEST                                      "test"
#define   STR_SAME                                      "same"
#define   STR_DIFFERENT                                 "different"

class EDPImages: public EDPBase
{
public:

    struct BindingAnswers {
        qreal testCorrect;
        qreal testWrong;
        qreal correct;
        qreal wrong;
    };

    EDPImages(ConfigurationManager *c);

    // Function reimplementation headers.
    bool doEyeDataProcessing(const QString &data);

    BindingAnswers getExperimentAnswers() const {return answers;}

private:

    // Initialization of the data matrix (header row)
    void initializeImageDataMatrix();

    // Adding the data of each image.
    QStringList csvLines;
    bool appendDataToImageMatrix(const DataMatrix &data, const QString &trialName, const QString &isTrial, const QString &response);

    // The answers found.
    BindingAnswers answers;

    // Increments the correct field of answers
    void sumToAnswers(const QString &trialID, const QString &ans);

    // Parser for the experiment description in order to get information on what targets are on each trial.
    BindingParser parser;

    // Saving the buffered to a file on disk.
    bool saveDataToFile();

};

#endif // EDPIMAGES_H
