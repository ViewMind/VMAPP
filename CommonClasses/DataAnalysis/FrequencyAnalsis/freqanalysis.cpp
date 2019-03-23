#include "freqanalysis.h"

FreqAnalysis::FreqAnalysis()
{

}

void FreqAnalysis::FreqAnalysisResult::analysisValid(const FreqCheckParameters p){
    errorList.clear();

    //qreal periodInMs = 1000/p.fexpected;
    qint32 fglitches = 0;
    qreal totalNumberOfDataPoints = 0;

    // Checking frequency glitches
    individualErrorList << "Frequency Analysis Parametrs:\n" + p.toString("      ");
    for (qint32 i = 0; i < diffTimes.size(); i++){

        // Checking first the minimum number of data points.
        qint32 numData = diffTimes.at(i).size() + 1;
        if (numData < p.minNumberOfDataItems){
            errorList << "Data set " + QString::number(i) + " has a total of " + QString::number(numData)
                    + " ET points which is less than the minimum allowed of " + QString::number(p.minNumberOfDataItems);
        }

        totalNumberOfDataPoints = totalNumberOfDataPoints + numData;
        qreal trialFreq = avgFreqPerTrial.at(i);

        fglitches = 0;

        individualErrorList << "---> Data Set " + QString::number(i);

        for (qint32 j = 0; j < diffTimes.at(i).size(); j++){
            qreal dt = diffTimes.at(i).at(j);
            if ((dt < p.periodMin) || (dt > p.periodMax)) {
                fglitches ++;
                individualErrorList << "   ---> Period @ " + QString::number(j) + " is " + QString::number(dt);
            }
        }

        qreal ep = (qreal)fglitches*100.0/(qreal)(numData-1);
        individualErrorList << "---> Number of glitches " + QString::number(fglitches) + " number of measured periods: "
                               + QString::number(diffTimes.at(i).size()) + " (" + QString::number(ep) + "%)"
                               + ". AVG F in Trial: " + QString::number(trialFreq);

        if (ep > p.maxAllowedFreqGlitchesPerTrial){
            errorList << "Data set " + QString::number(i) + " has " + QString::number(ep) + " % of frequency glitches";
        }

    }


    // Checking invalid values. the percent of string sin the times stasmp that could not be converted to number of the COMPLETE STUDY.
    qreal invalidPercent = (qreal)invalidValues.size()*100.0/totalNumberOfDataPoints;
    if (invalidPercent > p.maxAllowedPercentOfInvalidValues){
        errorList << "The number of invalid values is above the allowed tolerance: " + QString::number(invalidValues.size()) + " which is "
                + QString::number(invalidPercent) + "% (Tolerance at: " + QString::number(p.maxAllowedPercentOfInvalidValues) + "%)";
    }

}

FreqAnalysis::FreqAnalysisResult FreqAnalysis::analyzeFile(const QString &fname, qreal timeUnitInSeconds){
    fileToAnalyze = fname;
    timeUnit = timeUnitInSeconds;
    QFileInfo info(fileToAnalyze);
    QStringList parts = info.baseName().split("_");
    FreqAnalysisBehaviour fab;    
    if (parts.first() == FILE_BINDING){
        fab.trialChangeBehaviour = TCB_LINE_SIZE_CHANGE;
        fab.dataLineSize = 7;
        fab.timeColumn = 0;
        fab.valueChangeColumn = 0;
        fab.header = HEADER_IMAGE_EXPERIMENT;
        return freqAnalysis(fab);
    }
    else if (parts.first() == FILE_OUTPUT_READING){
        fab.trialChangeBehaviour = TCB_LINE_VALUE_CHANGE;
        fab.dataLineSize = 13;
        fab.timeColumn = 1;
        fab.valueChangeColumn = 0;
        fab.header = HEADER_READING_EXPERIMENT;
        return freqAnalysis(fab);
    }
    else{
        FreqAnalysisResult far;
        far.errorList << "Unrecognized file name. Cannot perform frequency analysis";
        return far;
    }
}

QStringList FreqAnalysis::getFileLines(QString *error){
    QFile file(fileToAnalyze);
    if (!file.open(QFile::ReadOnly)){
        *error = "Could not open file: " + fileToAnalyze + " for reading";
        return QStringList();
    }

    QTextStream reader(&file);
    reader.setCodec(COMMON_TEXT_CODEC);
    QString content = reader.readAll();
    file.close();

    return content.split("\n");
}

QStringList FreqAnalysis::removeHeader(const QStringList &lines, const QString &header){
    qint32 lineid = 0;
    qint32 counter = 0;

    // Removing the header
    while (lineid < lines.size()){
        if (lines.at(lineid).startsWith(header)) counter++;
        lineid++;
        if (counter == 2) break;
    }

    if (lineid < lines.size())  return lines.mid(lineid);
    else return QStringList();
}

FreqAnalysis::FreqAnalysisResult FreqAnalysis::freqAnalysis(const FreqAnalysis::FreqAnalysisBehaviour &fab){

    FreqAnalysisResult far;

    QString local_error;
    QStringList content = getFileLines(&local_error);
    if (!local_error.isEmpty()){
        far.errorList << local_error;
        return far;
    }
    if (content.isEmpty()) {
        far.errorList << "No content found";
        return far;
    }

    content = removeHeader(content,fab.header);
    if (content.isEmpty()){
        far.errorList << "Header segment for binding was not found";
        return far;
    }

    QList<qreal> times;
    qreal freqAcc = 0;
    qreal freqCounter = 0;

    bool trialChange;
    bool ok;
    QString previousValue = "";

    for (qint32 i = 0; i < content.size(); i++){

        QStringList parts = content.at(i).split(" ",QString::SkipEmptyParts);

        trialChange = false;
        if (fab.trialChangeBehaviour == TCB_LINE_SIZE_CHANGE){
            trialChange = (parts.size() != fab.dataLineSize);
        }
        else if (fab.trialChangeBehaviour == TCB_LINE_VALUE_CHANGE){
            // For simplicity all lines that are not the size of dataLineSize are ignored.
            if (fab.dataLineSize != parts.size()) continue;
            trialChange = (parts.at(fab.valueChangeColumn) != previousValue);
            previousValue = parts.at(fab.valueChangeColumn);
        }

        if (trialChange){
            if (times.size() >= 2) {
                QList<qreal> dtimes;
                freqAcc = freqAcc + calculateFrequency(times, &dtimes);
                freqCounter++;
                far.avgFreqPerTrial << freqAcc/freqCounter;
                far.diffTimes << dtimes;
            }
            times.clear();
        }
        else if (parts.size() == fab.dataLineSize){
            ok = true;
            qreal value =  parts.at(fab.timeColumn).toDouble(&ok);
            if (!ok){
                far.invalidValues << parts.at(fab.timeColumn);
            }
            else{
                times << value;
            }
        }
    }

    if (freqCounter == 0){
        frequencyResult  = 0;
        far.errorList << "Did not find two consecutive time stamps in a trial";
        return far;
    }
    else far.averageFrequency = freqAcc/freqCounter;

    return far;

}

qreal FreqAnalysis::calculateFrequency(const QList<qreal> &times, QList<qreal> *dtimes){
    qreal freqsAcc = 0;
    qreal freqCounter = 0;
    qreal f;
    for (qint32 i = 1; i < times.size(); i++){
        f = (1.0)/((times.at(i) - times.at(i-1))*timeUnit);
        dtimes->append(times.at(i) - times.at(i-1));
        freqsAcc = freqsAcc + f;
        freqCounter++;
    }
    return freqsAcc/freqCounter;
}




