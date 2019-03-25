#ifndef FREQANALYSIS_H
#define FREQANALYSIS_H

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include "../../CommonClasses/common.h"

#define  FILE_BINDING  "binding"

class FreqAnalysis
{
public:

    struct FreqCheckParameters {
        qreal periodMax;
        qreal periodMin;
        qreal fexpected;
        qint32 maxAllowedFreqGlitchesPerTrial;
        qint32 minNumberOfDataItems;
        qreal maxAllowedPercentOfInvalidValues;
        QString toString(const QString tab = "") const { return
                      tab + "Period should be in ["  + QString::number(periodMin) + ", " + QString::number(periodMax)+ "] ms\n"
                    + tab + "Maximum % of Frequency Glitches allowed in a trial: " + QString::number(maxAllowedFreqGlitchesPerTrial) + "\n"
                    + tab + "Maximum % of Invalid values allowed in a trial: " + QString::number(maxAllowedPercentOfInvalidValues) + "\n"
                    + tab + "Minimum number of data points allowed in a trial: " + QString::number(minNumberOfDataItems) + "\n"
                    + tab + "Expected Frequency: " + QString::number(fexpected) + " Hz\n"; }
    };

    struct TimePair {
        qreal start;
        qreal end;
        qreal getDiff() const { return end  - start; }
        QString toString() const { return "[" + QString::number(start,'f') + "," + QString::number(end,'f') + "]"; }
    };

    struct FreqAnalysisResult {
        QStringList errorList;
        QStringList individualErrorList;
        QStringList invalidValues;
        QList< QList<TimePair> > diffTimes;
        QList<qreal> avgFreqPerTrial;
        qreal averageFrequency;
        void analysisValid(const FreqCheckParameters p);
    };

    FreqAnalysis();
    FreqAnalysisResult analyzeFile(const QString &fname, qreal timeUnitInSeconds = 1e-3);

private:

    typedef enum {TCB_LINE_SIZE_CHANGE, TCB_LINE_VALUE_CHANGE} TrialChangeBehaviour;

    struct FreqAnalysisBehaviour {
        TrialChangeBehaviour trialChangeBehaviour;
        qint32 dataLineSize;
        qint32 timeColumn;
        qint32 valueChangeColumn;
        QString header;
    };

    QString fileToAnalyze;
    qreal timeUnit;
    qreal frequencyResult;

    // Saving all glitches and values.

    QStringList getFileLines(QString *error);
    QStringList removeHeader(const QStringList &lines, const QString &header);
    qreal calculateFrequency(const QList<qreal> &times, QList<TimePair> *dtimes);
    FreqAnalysisResult freqAnalysis(const FreqAnalysisBehaviour &fab);
};

#endif // FREQANALYSIS_H
