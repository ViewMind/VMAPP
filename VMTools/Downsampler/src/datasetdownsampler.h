#ifndef DATASETDOWNSAMPLER_H
#define DATASETDOWNSAMPLER_H

#include "../../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h"
#include "../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpreading.h"
#include "../../../CommonClasses/DataAnalysis/DataSetExtractor/datasetextractor.h"

#define  ACCEPTED_TOLERANCE_FOR_KNOWN_FREQUENCY   5 //This is %
#define  DIR_DOWNSAMPLED                          "downsampled"

class DataSetDownsampler
{
public:
    DataSetDownsampler();

    bool downSampleRawDataFile(const QString &fileName, int targetFrequency);

    DataSetExtractor::DataSetList getResult() const {return result;}
    QString getError() const { return error; }
    QString getOutputDownSampledFile() const { return outputDownSampledFile; }

    // Euclid's Algorimth for Greatest Common Divisor
    static int GreatestCommonDivisor(int a, int b);

    // The list of the frequencies with which we have worked.
    static QList<qreal> KnownFrequencies;

private:
    QString error;
    QString outputDownSampledFile;
    ConfigurationManager eyeRepGenConf;
    DataSetExtractor::DataSetList result;
    qreal upsampleTimeStep;
    qreal inputTimeStep;
    qreal masterTimeStamp;
    DataMatrix downSampleDataMatrix (const DataMatrix &source, int L, int M, QList<qint32> indexesToInterpolate);
    DataMatrix inteporlateValuesBetweenRows(const DataRow &r0, const DataRow &r1,
                                            int L, QList<qint32> indexesToInterpolate);

    bool saveReadingDataSetListToFile(int targetFrequency);

};

#endif // DATASETDOWNSAMPLER_H
