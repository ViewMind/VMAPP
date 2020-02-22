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

    QStringList tempErrorSummary;
    QStringList tempErrorList;
    qreal tNumOfInvalidValues = 0;

    numberOfDataSetsWithLittleDataPoints = 0;
    numberOfDataSetsWithTooManyFreqGlitches = 0;

    for (qint32 i = 0; i < freqAnalysisForEachDataSet.size(); i++){

        FreqAnalyisDataSet fads = freqAnalysisForEachDataSet.at(i);

        if (fads.diffTimes.isEmpty()){
            tempErrorList << "Data set " + fads.trialName + " does not have ANY data points";
            numberOfDataSetsWithLittleDataPoints++;
            continue;
        }

        // Checking first the minimum number of data points.
        if (fads.numberOfDataPoints < p.minNumberOfDataItems){
            tempErrorSummary << " Data set " + fads.trialName + " has a total of " + QString::number(fads.numberOfDataPoints)
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

        qreal ep = static_cast<qreal>(fglitches)*100.0/fads.numberOfDataPoints;
        individualErrorList << "   ---> Number of glitches " + QString::number(fglitches) + ". Number of measured periods: "
                               + QString::number(fads.diffTimes.size()) + " (" + QString::number(ep) + "%)"
                               + ". AVG F in Trial: " + QString::number(trialFreq) + ". " + temp;

        if (ep > p.maxAllowedFreqGlitchesPerTrial){
            tempErrorList << "Data set " + fads.trialName + " has " + QString::number(ep) + " % of frequency glitches";
            numberOfDataSetsWithTooManyFreqGlitches++;
        }

        tNumOfInvalidValues = fads.invalidValues.size() + tNumOfInvalidValues;

    }

    qint32 totalFailedTrials = numberOfDataSetsWithTooManyFreqGlitches + numberOfDataSetsWithLittleDataPoints;
    if ((totalFailedTrials) > p.maxAllowedFailedTrials){
        errorList.prepend("Number of failed trials is "  + QString::number(totalFailedTrials) + " Specified Threshold " + QString::number(p.maxAllowedFailedTrials));
        if (numberOfDataSetsWithTooManyFreqGlitches > 0){
            errorList.prepend("Number of data sets with too many frequency glitches: " + QString::number(numberOfDataSetsWithTooManyFreqGlitches));
        }

        if (numberOfDataSetsWithLittleDataPoints > 0){
            errorList.prepend("Number of data sets with too few data points: " + QString::number(numberOfDataSetsWithLittleDataPoints));
        }
        errorList << tempErrorList;
        errorList << tempErrorSummary;
    }

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

    FreqAnalysisResult far;
    qint32 firstNDataSetsToFilter;

    if (parts.first() == FILE_BINDING){
        far = performBindingAnalysis();
        firstNDataSetsToFilter = TEST_DATA_SETS_BINDING;
    }
    else if (parts.first() == FILE_OUTPUT_READING){
        far =  performReadingAnalysis();
        firstNDataSetsToFilter = TEST_DATA_SETS_READING;
    }
    else if (parts.first() == FILE_OUTPUT_FIELDING){
        far = performFieldingAnalysis();
        firstNDataSetsToFilter = TEST_DATA_SETS_FIELDING;
    }
    else if (parts.first() == FILE_OUTPUT_NBACKRT){
        far = performNBackRTAnalysis();
        firstNDataSetsToFilter = TEST_DATA_SETS_NBACKRT;
    }
    else{
        far.fileError = "Unrecognized file name. Cannot perform frequency analysis";
        return far;
    }

    // Filtering out the fist N data sets.
    for (qint32 i = 0; i < firstNDataSetsToFilter; i++){
        far.freqAnalysisForEachDataSet.pop_front();
    }
    return far;
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

FreqAnalysis::FreqAnalysisResult FreqAnalysis::performReadingAnalysis(){

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

FreqAnalysis::FreqAnalysisResult FreqAnalysis::performBindingAnalysis(){
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
    far.averageFrequency = freqAcc/static_cast<qreal>(freqAccCounter);
    return far;
}

FreqAnalysis::FreqAnalysisResult FreqAnalysis::performFieldingAnalysis(){
    FreqAnalysisResult far;

    QString local_error;
    QStringList contentAndHeader = separateHeaderFromData(&local_error,HEADER_FIELDING_EXPERIMENT);
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
    FieldingParser parser;

    // The first line is the resolution
    QStringList lines = content.split("\n");
    if (!parser.parseFieldingExperiment(header)){
        far.errorList << "PARSING FIELDING DESCRIPTION: " + parser.getError();
    }
    far.expectedNumberOfDataSets = parser.getParsedTrials().size();

    // Parsing the experiment data.
    QHash<QString, QList<qreal> > dataSetTimes;
    QHash<QString, QStringList > invalidTimes;

    QString name;
    for (qint32 i = 1; i < lines.size(); i++){

        if (lines.at(i).trimmed().isEmpty()) continue;

        QStringList parts = lines.at(i).split(" ",QString::SkipEmptyParts);

        // Trial description line
        if (parts.size() == 4){
            name = parts.at(0) + "_" + parts.at(1);
            continue;
        }
        else if (parts.size() != 7){
            far.errorList << "Fielding line :" + lines.at(i) + " does not have 7 parts";
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
    for (qint32 i = 0; i < parser.getParsedTrials().size(); i++){

        QString bname = parser.getParsedTrials().at(i).id;
        // In fielding trials are numbered like this: 1-3 are the slides that show the red dot. While 5 through 7 are empty square boxes.
        QStringList list; list << "1" << "2" << "3" << "5" << "6" << "7";
        for (qint32 j = 0; j < list.size(); j++){

            FreqAnalyisDataSet faDataSet;
            name = bname + "_" + list.at(j);
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
    far.averageFrequency = freqAcc/static_cast<qreal>(freqAccCounter);
    return far;
}

FreqAnalysis::FreqAnalysisResult FreqAnalysis::performNBackRTAnalysis(){
    FreqAnalysisResult far;

    QString local_error;
    QStringList contentAndHeader = separateHeaderFromData(&local_error,HEADER_NBACKRT_EXPERIMENT);
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
    FieldingParser parser;

    // The first line is the resolution
    QStringList lines = content.split("\n");
    if (!parser.parseFieldingExperiment(header)){
        far.errorList << "PARSING NBACK RT DESCRIPTION: " + parser.getError();
    }
    far.expectedNumberOfDataSets = parser.getParsedTrials().size();

    // Parsing the experiment data.
    QHash<QString, QList<qreal> > dataSetTimes;
    QHash<QString, QStringList > invalidTimes;

    QString name;
    for (qint32 i = 1; i < lines.size(); i++){

        if (lines.at(i).trimmed().isEmpty()) continue;

        QStringList parts = lines.at(i).split(" ",QString::SkipEmptyParts);

        // Trial description line has only two parts
        if (parts.size() == 2){
            name = parts.at(0) + "_" + parts.at(1);
            continue;
        }
        else if (parts.size() != 7){
            far.errorList << "NBack RT line :" + lines.at(i) + " does not have 7 parts";
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
    for (qint32 i = 0; i < parser.getParsedTrials().size(); i++){

        QString bname = parser.getParsedTrials().at(i).id;
        // In N Back RT trials are numbered like this: 1-3 are the slides that show the red dot. While 4 are empty square boxes.
        QStringList list; list << "1" << "2" << "3" << "4";
        for (qint32 j = 0; j < list.size(); j++){

            FreqAnalyisDataSet faDataSet;
            name = bname + "_" + list.at(j);
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
    far.averageFrequency = freqAcc/static_cast<qreal>(freqAccCounter);
    return far;
}

qreal FreqAnalysis::calculateFrequency(const QList<qreal> &times, QList<TimePair> *dtimes){
    qreal freqsAcc = 0;
    qreal freqCounter = 0;
    qreal f;
    for (qint32 i = 1; i < times.size(); i++){
        TimePair tp;
        tp.start = times.at(i-1);
        tp.end   = times.at(i);
        dtimes->append(tp);
        if (times.at(i) > times.at(i-1)){
            f = (1.0)/((times.at(i) - times.at(i-1))*timeUnit);
            freqsAcc = freqsAcc + f;
            freqCounter++;
        }
    }
    return freqsAcc/freqCounter;
}


///////////////////////////////////////// STATIC CALL TO PERFORM A FREQUENCY ANALYSIS ///////////////////////////////

FreqAnalysis::FreqAnalysisResult FreqAnalysis::doFrequencyAnalysis(ConfigurationManager *config, const QString &fileName){
    FreqAnalysis freqChecker;
    FreqAnalysis::FreqAnalysisResult fres;
    fres = freqChecker.analyzeFile(fileName);

    if (!fres.fileError.isEmpty()){
        return fres;
    }

    QFileInfo info(fileName);
    bool isFielding = info.baseName().startsWith(CONFIG_P_EXP_FIELDING) || info.baseName().startsWith(CONFIG_P_EXP_NBACKRT);

    FreqAnalysis::FreqCheckParameters fcp;
    fcp.fexpected                        = config->getReal(CONFIG_SAMPLE_FREQUENCY);
    fcp.periodMax                        = config->getReal(CONFIG_TOL_MAX_PERIOD_TOL);
    fcp.periodMin                        = config->getReal(CONFIG_TOL_MIN_PERIOD_TOL);
    fcp.maxAllowedFreqGlitchesPerTrial   = config->getReal(CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL);
    fcp.maxAllowedPercentOfInvalidValues = config->getReal(CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES);
    if (isFielding){
        fcp.minNumberOfDataItems         = config->getInt(CONFIG_TOL_NUM_MIN_PTS_IN_FIELDING_TRIAL);
    }
    else{
        fcp.minNumberOfDataItems         = config->getInt(CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL);
    }
    fcp.maxAllowedFailedTrials           = config->getInt(CONFIG_TOL_NUM_ALLOWED_FAILED_DATA_SETS);
    fres.analysisValid(fcp);

    return fres;

}
