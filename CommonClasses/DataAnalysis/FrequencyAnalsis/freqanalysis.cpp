#include "freqanalysis.h"

FreqAnalysis::FreqAnalysis()
{

}

void FreqAnalysis::FreqAnalysisResult::analysisValid(const FreqCheckParameters p){

    errorList.clear();

    qint32 fglitches = 0;
    qreal totalNumberOfDataPoints = 0;

    // Checking frequency glitches
    individualErrorList << "Frequency Analysis Parametrs:\n" + p.toString("      ");

    QStringList tempErrors;
    qreal tNumOfInvalidValues = 0;

    qint32 numberOfDataSetsWithLittleDataPoints = 0;
    qint32 numberOfDataSetsWithTooManyFreqGlitches = 0;

    for (qint32 i = 0; i < freqAnalysisForEachDataSet.size(); i++){

        FreqAnalyisDataSet fads = freqAnalysisForEachDataSet.at(i);

        if (fads.diffTimes.isEmpty()){
            errorList << "Data set " + fads.trialName + " does not have ANY data points";
            numberOfDataSetsWithLittleDataPoints++;
            continue;
        }

        // Checking first the minimum number of data points.
        if (fads.numberOfDataPoints < p.minNumberOfDataItems){
            tempErrors << " Data set " + fads.trialName + " has a total of " + QString::number(fads.numberOfDataPoints)
                    + " ET points which is less than the minimum allowed of " + QString::number(p.minNumberOfDataItems);
            numberOfDataSetsWithLittleDataPoints++;
        }

        totalNumberOfDataPoints = totalNumberOfDataPoints + fads.numberOfDataPoints;
        qreal trialFreq = fads.averageFrequency;

        fglitches = 0;

        individualErrorList << "---> Data Set " + fads.trialName;

        for (qint32 j = 0; j < fads.diffTimes.size(); j++){
            qreal dt = fads.diffTimes.at(j).getDiff();
            if ((dt < p.periodMin) || (dt > p.periodMax)) {
                fglitches ++;
                individualErrorList << "   ---> Period @ " + QString::number(j) + " is " + QString::number(dt) + " " +  fads.diffTimes.at(j).toString();
            }
        }

        QString temp = "Data set duration: " + QString::number(fads.duration) + ". Expected Number of Points: " + QString::number(fads.expectedNumberOfDataPoints);

        qreal ep = (qreal)fglitches*100.0/fads.numberOfDataPoints;
        individualErrorList << "   ---> Number of glitches " + QString::number(fglitches) + ". Number of measured periods: "
                               + QString::number(fads.diffTimes.size()) + " (" + QString::number(ep) + "%)"
                               + ". AVG F in Trial: " + QString::number(trialFreq) + ". " + temp;

        if (ep > p.maxAllowedFreqGlitchesPerTrial){
            errorList << "Data set " + QString::number(i) + " has " + QString::number(ep) + " % of frequency glitches";
            numberOfDataSetsWithTooManyFreqGlitches++;
        }

        tNumOfInvalidValues = fads.invalidValues.size() + tNumOfInvalidValues;

    }

    if (numberOfDataSetsWithTooManyFreqGlitches > 0){
        errorList.prepend("Number of data sets with too many frequency glitches: " + QString::number(numberOfDataSetsWithTooManyFreqGlitches)
                          + " Specified Threshold " + QString::number(p.maxAllowedFailedTrials) );
    }

    if (numberOfDataSetsWithLittleDataPoints > 0){
        errorList.prepend("Number of data sets with too few data points: " + QString::number(numberOfDataSetsWithLittleDataPoints));
    }

    errorList << tempErrors;

    // Checking invalid values. the percent of string sin the times stasmp that could not be converted to number of the COMPLETE STUDY.
    qreal invalidPercent = tNumOfInvalidValues*100.0/totalNumberOfDataPoints;
    if (invalidPercent > p.maxAllowedPercentOfInvalidValues){
        errorList << "The number of invalid values is above the allowed tolerance: " + QString::number(tNumOfInvalidValues) + " which is "
                + QString::number(invalidPercent) + "% (Tolerance at: " + QString::number(p.maxAllowedPercentOfInvalidValues) + "%)";
    }

}

FreqAnalysis::FreqAnalysisResult FreqAnalysis::analyzeFile(const QString &fname, qreal timeUnitInSeconds){
    fileToAnalyze = fname;
    timeUnit = timeUnitInSeconds;
    QFileInfo info(fileToAnalyze);
    QStringList parts = info.baseName().split("_");
    if (parts.first() == FILE_BINDING){
        return performBindingAnalyis();
    }
    else if (parts.first() == FILE_OUTPUT_READING){
        return performReadingAnalyis();
    }
    else{
        FreqAnalysisResult far;
        far.errorList << "Unrecognized file name. Cannot perform frequency analysis";
        return far;
    }
}

QStringList FreqAnalysis::separateHeaderFromData(QString *error, const QString &header){

    QFile file(fileToAnalyze);
    if (!file.open(QFile::ReadOnly)){
        *error = "Could not open file: " + fileToAnalyze + " for reading";
        return QStringList();
    }

    QTextStream reader(&file);
    reader.setCodec(COMMON_TEXT_CODEC);
    QString content = reader.readAll();
    file.close();

    QStringList lines = content.split("\n");
    QStringList headerData;
    QStringList data;
    qint32 headerCounter = 0;

    for (qint32 i = 0; i < lines.size(); i++){
        if (lines.at(i).startsWith(header)) headerCounter++;
        else if (headerCounter < 2) headerData << lines.at(i);
        else data << lines.at(i);
    }

    QStringList ans; ans << headerData.join("\n") << data.join("\n");
    return ans;

}

FreqAnalysis::FreqAnalysisResult FreqAnalysis::performReadingAnalyis(){

    FreqAnalysisResult far;

    QString local_error;
    QStringList contentAndHeader = separateHeaderFromData(&local_error,HEADER_READING_EXPERIMENT);
    if (!local_error.isEmpty()){
        far.errorList << local_error;
        return far;
    }

    QString header = contentAndHeader.first();
    QString content = contentAndHeader.last();
    contentAndHeader.clear();

    if (content.isEmpty()) {
        far.errorList << "No content found";
        return far;
    }

    // Parsing the reading parameters
    ReadingParser parser;
    if (!parser.parseReadingDescription(header)){
        far.errorList << "PARSING READING DESCRIPTION: " + parser.getError();
    }
    far.expectedNumberOfDataSets = parser.getPhrases().size();

    // Parsing the experiment data.
    QHash<QString, QList<qreal> > dataSetTimes;
    QHash<QString, QStringList > invalidTimes;

    // TODO: FIRST LINE IS RESOLUTION
    QStringList lines = content.split("\n");
    for (qint32 i = 1; i < lines.size(); i++){

        if (lines.at(i).trimmed().isEmpty()) continue;

        QStringList parts = lines.at(i).split(" ",QString::SkipEmptyParts);
        if (parts.size() != 13){
            far.errorList << "Reading line :" + lines.at(i) + " does not have 13 parts";
            return far;
        }
        QString name = parts.at(0);
        QString time = parts.at(1);
        if (!dataSetTimes.contains(name)){
            QList<qreal> list;
            QStringList invalids;
            dataSetTimes[name] = list;
            invalidTimes[name] = invalids;
        }

        bool ok;
        qreal value = time.toDouble(&ok);
        if (ok){
            dataSetTimes[name].append(value);
        }
        else{
            invalidTimes[name].append(time);
        }
    }

    // Doing the frequency analysis per data set;
    qreal freqAcc = 0;
    qreal freqAccCounter = 0;
    for (qint32 i = 0; i < parser.getPhrases().size(); i++){

        FreqAnalyisDataSet faDataSet;
        QString name = parser.getPhrases().at(i).getStringID();
        QList<qreal> times = dataSetTimes.value(name);

        faDataSet.numberOfDataPoints = times.size();
        faDataSet.trialName = name;

        if (times.size() < 2){
            faDataSet.averageFrequency = 0;
            faDataSet.duration = 0;
            faDataSet.expectedNumberOfDataPoints = 0;
        }
        else {
            QList<TimePair> dtimes;
            faDataSet.averageFrequency = calculateFrequency(times,&dtimes);
            freqAcc = freqAcc + faDataSet.averageFrequency;
            freqAccCounter++;
            faDataSet.diffTimes = dtimes;
            faDataSet.duration = times.last() - times.first();
            qreal TinMS = 1000.0/faDataSet.averageFrequency;
            faDataSet.expectedNumberOfDataPoints = faDataSet.duration/TinMS;
            faDataSet.numberOfDataPoints = times.size();
            faDataSet.invalidValues = invalidTimes.value(name);
        }

        far.freqAnalysisForEachDataSet << faDataSet;

    }
    far.averageFrequency = freqAcc/freqAccCounter;
    return far;

}

FreqAnalysis::FreqAnalysisResult FreqAnalysis::performBindingAnalyis(){
    FreqAnalysisResult far;

    QString local_error;
    QStringList contentAndHeader = separateHeaderFromData(&local_error,HEADER_IMAGE_EXPERIMENT);
    if (!local_error.isEmpty()){
        far.errorList << local_error;
        return far;
    }

    QString header = contentAndHeader.first();
    QString content = contentAndHeader.last();
    contentAndHeader.clear();

    if (content.isEmpty()) {
        far.errorList << "No content found";
        return far;
    }

    // Parsing the reading parameters
    BindingParser parser;

    // Creating a configuration of false values JUST to be able to parse.
    ConfigurationManager config;
    config.addKeyValuePair(CONFIG_XPX_2_MM,1);
    config.addKeyValuePair(CONFIG_YPX_2_MM,1);
    config.addKeyValuePair(CONFIG_DEMO_MODE,false);

    // The first line is the resolution
    QStringList lines = content.split("\n");
    QStringList res = lines.first().split(" ",QString::SkipEmptyParts);
    qreal w = 0; qreal h = 0;
    if (res.size() == 2){
        w = res.first().toDouble();
        h = res.last().toDouble();
    }
    // Just in case, avoid any division by zero.
    if (w == 0) w = 1;
    if (h == 0) h = 1;

    if (!parser.parseBindingExperiment(header,&config,w,h,1)){
        far.errorList << "PARSING BINDING DESCRIPTION: " + parser.getError();
    }
    far.expectedNumberOfDataSets = parser.getTrialList().size();

    // Parsing the experiment data.
    QHash<QString, QList<qreal> > dataSetTimes;
    QHash<QString, QStringList > invalidTimes;

    QString name;
    for (qint32 i = 1; i < lines.size(); i++){

        if (lines.at(i).trimmed().isEmpty()) continue;

        QStringList parts = lines.at(i).split(" ",QString::SkipEmptyParts);

        // Answer line
        if (parts.size() == 1) continue;
        if (parts.size() == 2) {
            // Name change line.
            name = parts.join("_");
            QList<qreal> list;
            QStringList invalids;
            dataSetTimes[name] = list;
            invalidTimes[name] = invalids;
            continue;
        }
        else if (parts.size() != 7){
            far.errorList << "Binding line :" + lines.at(i) + " does not have 7 parts";
            return far;
        }

        QString time = parts.at(0);
        bool ok;
        qreal value = time.toDouble(&ok);
        if (ok){
            dataSetTimes[name].append(value);
        }
        else{
            invalidTimes[name].append(time);
        }
    }

    // Doing the frequency analysis per data set;

    qreal freqAcc = 0;
    qreal freqAccCounter = 0;
    for (qint32 i = 0; i < parser.getTrialList().size(); i++){

        QString bname = parser.getTrialList().at(i).name;
        //qWarning() << name << parser.getTrialList().size();
        for (qint32 j = 0; j < 2; j++){

            FreqAnalyisDataSet faDataSet;
            name = bname + "_" + QString::number(j);
            QList<qreal> times = dataSetTimes.value(name);

            //qWarning() << "DOES DATA SET CONTAIN" << name << dataSetTimes.contains(name) << times.size();

            faDataSet.numberOfDataPoints = times.size();
            faDataSet.trialName = name;

            if (times.size() < 2){
                faDataSet.averageFrequency = 0;
                faDataSet.duration = 0;
                faDataSet.expectedNumberOfDataPoints = 0;
            }
            else {
                QList<TimePair> dtimes;
                faDataSet.averageFrequency = calculateFrequency(times,&dtimes);
                freqAcc = freqAcc + faDataSet.averageFrequency;
                freqAccCounter++;
                faDataSet.diffTimes = dtimes;
                faDataSet.duration = times.last() - times.first();
                qreal TinMS = 1000.0/faDataSet.averageFrequency;
                faDataSet.expectedNumberOfDataPoints = faDataSet.duration/TinMS;
                faDataSet.numberOfDataPoints = times.size();
                faDataSet.invalidValues = invalidTimes.value(name);
            }

            far.freqAnalysisForEachDataSet << faDataSet;

        }
    }
    far.averageFrequency = freqAcc/(qreal)(freqAccCounter);
    return far;
}

qreal FreqAnalysis::calculateFrequency(const QList<qreal> &times, QList<TimePair> *dtimes){
    qreal freqsAcc = 0;
    qreal freqCounter = 0;
    qreal f;
    for (qint32 i = 1; i < times.size(); i++){
        f = (1.0)/((times.at(i) - times.at(i-1))*timeUnit);
        TimePair tp;
        tp.start = times.at(i-1);
        tp.end   = times.at(i);
        dtimes->append(tp);
        freqsAcc = freqsAcc + f;
        freqCounter++;
    }
    return freqsAcc/freqCounter;
}

