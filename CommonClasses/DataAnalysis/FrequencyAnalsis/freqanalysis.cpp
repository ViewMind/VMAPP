#include "freqanalysis.h"

FreqAnalysis::FreqAnalysis()
{

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
        error = "Unrecognized file name. Cannot perform frequency analysis";
        return FAR_FAILED;
    }
}

QStringList FreqAnalysis::getFileLines(){
    QFile file(fileToAnalyze);
    if (!file.open(QFile::ReadOnly)){
        error = "Could not open file: " + fileToAnalyze + " for reading";
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

    warnings.clear();

    QStringList content = getFileLines();
    if (content.isEmpty()) return FAR_FAILED;

    content = removeHeader(content,fab.header);
    if (content.isEmpty()){
        error = "Header segment for binding was not found";
        return FAR_FAILED;
    }

    QList<qreal> times;
    qreal freqAcc = 0;
    qreal freqCounter = 0;

    bool trialChange;
    bool ok;
    QString previousValue = "";

    for (qint32 i = 0; i < content.size(); i++){

        QStringList parts = content.at(i).split(" ");

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
                freqAcc = freqAcc + calculateFrequency(times);
                freqCounter++;
            }
            times.clear();
        }
        else if (parts.size() == fab.dataLineSize){
            ok = true;
            qreal value =  parts.at(fab.timeColumn).toDouble(&ok);
            if (!ok){
                warnings << "Found invalid time value: " + parts.first();
            }
            else{
                times << value;
            }
        }
    }

    if (freqCounter == 0){
        frequencyResult  = 0;
        error = "Did not find two consecutive time stamps in a trial";
        return FAR_FAILED;
    }
    else frequencyResult = freqAcc/freqCounter;

    return FAR_OK;

}

qreal FreqAnalysis::calculateFrequency(const QList<qreal> &times){
    qreal freqsAcc = 0;
    qreal freqCounter = 0;
    for (qint32 i = 1; i < times.size(); i++){
        freqsAcc = freqsAcc + (1.0)/((times.at(i) - times.at(i-1))*timeUnit);
        freqCounter++;
    }
    return freqsAcc/freqCounter;
}




