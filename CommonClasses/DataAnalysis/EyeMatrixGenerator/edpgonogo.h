#ifndef EDPGONOGO_H
#define EDPGONOGO_H

#include "edpbase.h"

#include "../../Experiments/gonogomanager.h"

// Defines for Go No Go experiment raw data file
#define   GONOGO_TI                                   0
#define   GONOGO_XR                                   1
#define   GONOGO_YR                                   2
#define   GONOGO_XL                                   3
#define   GONOGO_YL                                   4
#define   GONOGO_PR                                   5
#define   GONOGO_PL                                   6

class EDPGoNoGo : public EDPBase
{
public:
    EDPGoNoGo(ConfigurationManager *c);

    // Function reimplementation headers.
    bool doEyeDataProcessing(const QString &data);

    // The number of trials done.
    qint32 getNumberOfTrials() const {return numberOfTrials;}

private:
    // Initialization of the data matrix (header row)
    void initializeGoNoGoDataMatrix();

    // Adding the data of each image.
    void appendDataToGoNoGoMatrix(const DataMatrix &data,
                                  const QString &trialID,
                                  const qint32 &targetBoxID,
                                  const qint32 &trialType,
                                  const qreal &totalExperimentTime);

    // Parse experiment description and get target hit boxes.
    GoNoGoParser parser;

    // Temporarily store all processed data separated for eye.
    QStringList csvHeader;
    QList<QStringList> ldata;
    QList<QStringList> rdata;

    // The target box for each of the targets.
    QList<QRectF> hitTargetBoxes;

    // The target box to see if a fixation is "on" the arrow.
    QRectF arrowTargetBox;

    // The number of trials
    qint32 numberOfTrials;

    // The center of the screen to measure sacadic latency.
    qreal centerX,centerY;

    void addDataToOneEye(const DataMatrix &data,
                         qint32 eyeID,
                         const QString &trialID,
                         const qint32 &targetBoxID, QList<QStringList> *list,
                         const qint32 &trialType,
                         const qreal &totalExperimentTime);

    // Write the actual CSV file.
    bool finalizeCSVFile();

};

#endif // EDPGONOGO_H
