#ifndef FREQANALYSIS_H
#define FREQANALYSIS_H

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "../../../CommonClasses/common.h"
#include "../../../CommonClasses/Experiments/readingparser.h"
#include "../../../CommonClasses/Experiments/bindingparser.h"

#define  FILE_BINDING  "binding"

// The number of data sets which are tests and should NOT factor into the frequency analysis, as they will be discarded.
#define  TEST_DATA_SETS_READING    10
#define  TEST_DATA_SETS_BINDING    20

class FreqAnalysis
{
public:

    struct FreqCheckParameters {
        qreal periodMax;
        qreal periodMin;
        qreal fexpected;
        qreal maxAllowedFreqGlitchesPerTrial;
        qint32 maxAllowedFailedTrials;
        qint32 minNumberOfDataItems;
        qreal maxAllowedPercentOfInvalidValues;
        QString toString(const QString tab = "") const { return
                      tab + "Period should be in ["  + QString::number(periodMin) + ", " + QString::number(periodMax)+ "] ms\n"
                    + tab + "Maximum % of Frequency Glitches allowed in a trial: " + QString::number(maxAllowedFreqGlitchesPerTrial) + "\n"
                    + tab + "Maximum % of Invalid values allowed in a trial: " + QString::number(maxAllowedPercentOfInvalidValues) + "\n"
                    + tab + "Minimum number of data points allowed in a trial: " + QString::number(minNumberOfDataItems) + "\n"
                    + tab + "Max failed trials allowed: " + QString::number(maxAllowedFailedTrials) + "\n"
                    + tab + "Expected Frequency: " + QString::number(fexpected) + " Hz\n"; }
    };

    struct TimePair {
        qreal start;
        qreal end;
        qreal getDiff() const { return end  - start; }
        QString toString() const { return "[" + QString::number(start,'f') + "," + QString::number(end,'f') + "]"; }
    };

    struct FreqAnalyisDataSet{
        QString trialName;
        qreal duration;
        qreal averageFrequency;
        qreal expectedNumberOfDataPoints;
        QList<TimePair> diffTimes;
        qint32 numberOfDataPoints;
        QStringList invalidValues;
    };

    struct FreqAnalysisResult {
        QStringList errorList;
        QStringList individualErrorList;
        qreal averageFrequency;
        qint32 expectedNumberOfDataSets;
        QList<FreqAnalyisDataSet> freqAnalysisForEachDataSet;
        qint32 numberOfDataSetsWithLittleDataPoints;
        qint32 numberOfDataSetsWithTooManyFreqGlitches;
        void analysisValid(const FreqCheckParameters p);
    };

    FreqAnalysis();
    FreqAnalysisResult analyzeFile(const QString &fname, qreal timeUnitInSeconds = 1e-3);


private:

    // The experiment parsers
    ReadingParser readingParser;
    BindingParser bindingParser;

    QString fileToAnalyze;
    qreal timeUnit;
    qreal frequencyResult;

    // Saving all glitches and values.

    QStringList separateHeaderFromData(QString *error, const QString &header);
    qreal calculateFrequency(const QList<qreal> &times, QList<TimePair> *dtimes);

    FreqAnalysisResult performReadingAnalyis();
    FreqAnalysisResult performBindingAnalyis();

};

#endif // FREQANALYSIS_H
