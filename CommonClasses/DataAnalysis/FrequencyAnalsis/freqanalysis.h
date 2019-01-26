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

    typedef enum {FAR_OK, FAR_FAILED, FAR_WITH_WARNINGS} FreqAnalysisResult;

    FreqAnalysis();    
    FreqAnalysisResult analyzeFile(const QString &fname, qreal timeUnitInSeconds = 1e-3);
    QString getError() const {return error; }
    QStringList getWarnings() const {return warnings;}
    qreal getFrequencyResult() const { return frequencyResult; }

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
    QString error;
    QStringList warnings;
    qreal timeUnit;
    qreal frequencyResult;

    QStringList getFileLines();
    QStringList removeHeader(const QStringList &lines, const QString &header);
    qreal calculateFrequency(const QList<qreal> &times);
    FreqAnalysisResult freqAnalysis(const FreqAnalysisBehaviour &fab);
};

#endif // FREQANALYSIS_H
