#ifndef EDPFIELDING_H
#define EDPFIELDING_H

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

// Defines used to indentified critical outptut columns in the CSV
#define   CSV_TRIALID                                   1
#define   CSV_IMGNUM                                    2
#define   CSV_RESP_TIME                                 8

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


    struct MinMaxTimeStamp {
        qreal crossTime;
        qreal firstHit;
        qreal lastHit;
        qreal timeToStore;
        void reset(){
            crossTime = -1;
            firstHit = -1;
            lastHit = -1;
            timeToStore = -1;
        }
        QString toString() const {
            return "Cross Time: " + QString::number(crossTime) + ". First/Last Hit: " + QString::number(firstHit) + "/" + QString::number(lastHit) + ". To Store: "
                    + QString::number(timeToStore);
        }
    };

    typedef  QHash< QString, MinMaxTimeStamp > FieldingTrialTimes;
    typedef  QHash< QString, FieldingTrialTimes > ResponseTimeStruct;

    // Initialization of the data matrix (header row)
    void initializeFieldingDataMatrix();

    // Adding the data of each image.
    void appendDataToFieldingMatrix(const DataMatrix &data,
                                    const QString &trialID,
                                    const QString &imgID,
                                    const qint32 &targetBoxID);

    // The response time structures are used for easy computation of the response times.
    void computeResponseTimes(ResponseTimeStruct *responseTimeStruct);

    // Filling the computed response times. The parameter is used as a pointer to save the data in the same structure and avoid a deep copy
    void fillResponseTimes(QList<QStringList> *pdata, ResponseTimeStruct *respTimeStruct);

    // Actually saving the stored data to a file on disk.
    bool finalizeFieldingDataMatrix();

    // Parser for the fielding data
    FieldingParser parser;

    // The center of the screen to measure sacadic latency.
    qreal centerX,centerY;

    // The number of trials
    qint32 numberOfTrials;

    // Center margins
    qreal centerMinX, centerMaxX, centerMinY, centerMaxY;

    // The target box tollerance as a function of which box I'm looking.
    QList<QRectF> hitTargetBoxes;

    // Temporarily store all processed data separated for eye.
    QStringList csvHeader;
    QList<QStringList> ldata;
    QList<QStringList> rdata;

    ResponseTimeStruct lResponseTimeStruct;
    ResponseTimeStruct rResponseTimeStruct;

};

#endif // EDPFIELDING_H
