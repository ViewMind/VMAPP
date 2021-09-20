#include "datasetdownsampler.h"

QList<qreal> DataSetDownsampler::KnownFrequencies = QList<qreal>() << 120 << 150;

DataSetDownsampler::DataSetDownsampler()
{

}

bool DataSetDownsampler::downSampleRawDataFile(const QString &fileName, int targetFrequency){

    outputDownSampledFile = "";

    // First step is obtaining the data set
    DataSetExtractor extractor;    
    if (!extractor.extractDataSetFromFile(fileName)){
        error = "Extracting data from file: " + extractor.getError();
        return false;
    }
    DataSetExtractor::DataSetList dslist = extractor.getDataSetList();

    if (dslist.dataSets.isEmpty()){
        error = "For data set extraction: No data sets found";
        return false;
    }

    // Second step is doing a frequency analysis. Assuming that eye rep gen conf is in the selected file's directory.
    if (!eyeRepGenConf.loadConfiguration(dslist.originalFilePath + QString("/") + FILE_EYE_REP_GEN_CONFIGURATION,COMMON_TEXT_CODEC)){
        error = "Loading Eye Rep Gen Conf: " + eyeRepGenConf.getError();
        return false;
    }

    FreqAnalysis::FreqAnalysisResult far = FreqAnalysis::doFrequencyAnalysis(&eyeRepGenConf,fileName);
    //qDebug() << "Frequency Analysis Done. Freq is: " << far.averageFrequency;

    // Checking for known frequencies.
    int sourceFrequency = -1;
    for (qint32 i = 0; i < KnownFrequencies.size(); i++){
        qreal tol = qAbs(KnownFrequencies.at(i) - far.averageFrequency)*100.0/KnownFrequencies.at(i);
        if (tol <= ACCEPTED_TOLERANCE_FOR_KNOWN_FREQUENCY){
            sourceFrequency = static_cast<qint32>(KnownFrequencies.at(i));
            break;
        }
    }

    if (sourceFrequency == -1){
        error = "Estimated frequency of " + QString::number(far.averageFrequency) + " Hz is too far off known frequencies.";
        return false;
    }

    if ((targetFrequency == sourceFrequency) || (targetFrequency < 0)){
        // Just extraction
        result = dslist;
        return true;
    }

    if (targetFrequency > sourceFrequency){
        error = "Target frequency is larger than source frequency";
        return false;
    }

    // Computing upsampling and downsampling values.
    int gcd = GreatestCommonDivisor(targetFrequency,sourceFrequency);
    int M = sourceFrequency/gcd;
    int L = targetFrequency/gcd;

    // Time step of the upsampled frequency.
    upsampleTimeStep  = 1000.0/static_cast<qreal>(L*sourceFrequency);
    inputTimeStep     = 1000.0/static_cast<qreal>(sourceFrequency);
    masterTimeStamp = 0;

    QList<qint32> indexesToInterpolate;

    if (dslist.dataSetType == EXP_READING){
        indexesToInterpolate  << READ_XR
                              << READ_YR
                              << READ_XL
                              << READ_YL
                              << READ_PL
                              << READ_PR;
    }
    else{
        error = "Unrecognized format type: " + QString::number(dslist.dataSetType);
        return false;
    }

    /* IMPORTARNT! Frequency glitches are IGNORED
     * We assume that each data point had the exact difference in it`s time stamp than 1/sourceFrequency.*/
    result = dslist;    
    result.dataSets.clear();
    for (qint32 i = 0; i < dslist.dataSets.size(); i++){
        DataMatrix dm = downSampleDataMatrix(dslist.dataSets.at(i),L,M,indexesToInterpolate);
        //qDebug() << "Adding a Data Matrix of Length: " << dm.size() << " Input size " << dslist.dataSets.at(i).size() << " Data Set ID: " << dslist.dataSetIDs.at(i);
        result.dataSets << dm;
    }

    // Down sampling is done saving new frequency in eyerepgenconf
    eyeRepGenConf.addKeyValuePair(CONFIG_SAMPLE_FREQUENCY,targetFrequency);

    // Saving to file
    if (dslist.dataSetType == EXP_READING){
        return saveReadingDataSetListToFile(targetFrequency);
    }
    else {
        // If my code is correct this is REDUNDANT as there is an if asking the exact same thing above.
        // Leaving for completion's sake.
        error = "Unknown data set type for file saving";
        return false;
    }

}

DataMatrix DataSetDownsampler::downSampleDataMatrix(const DataMatrix &source, int L, int M, QList<qint32> indexesToInterpolate){

    DataMatrix upsampled;
    DataMatrix result;

    // Doing the upsampling.
    for (qint32 i = 0; i < source.size()-1; i++){
        DataMatrix dm = inteporlateValuesBetweenRows(source.at(i),source.at(i+1),
                                                     L,indexesToInterpolate);
        upsampled << dm;
        masterTimeStamp =  masterTimeStamp+inputTimeStep;
    }

    // Doing the downsampling.
    for (qint32 i = 0; i < upsampled.size(); i = i + M){
        result << upsampled.at(i);
    }

    return result;
}

DataMatrix DataSetDownsampler::inteporlateValuesBetweenRows(const DataRow &r0,
                                                          const DataRow &r1,
                                                          int L, QList<qint32> indexesToInterpolate){

    // Always assuming first column is time column.
    DataMatrix res;

    qreal t0 = masterTimeStamp;
    qreal t1 = masterTimeStamp+inputTimeStep;

    // Computing linear coefficients for all affected rows
    QList<qreal> m; QList<qreal> b;
    for (qint32 i = 0; i < indexesToInterpolate.size(); i++){
        qint32 k = indexesToInterpolate.at(i);
        qreal temp_m = (r1.at(k) - r0.at(k))/(t1 - t0);
        qreal temp_b = r1.at(k) - temp_m*t1;
        m << temp_m;
        b << temp_b;
    }

    // The first row is the same as the r0, but with time t0.
    res << r0;
    res.first().replace(0,t0);

    // Interporlating L-1 values
    qreal t = t0;  // Starting time step.
    for (qint32 i = 0;  i < L-1; i++){
        DataRow dr;

        // Time step is increased.
        t = t + upsampleTimeStep;
        dr << t;

        for (qint32 j = 1; j < r0.size(); j++){
            dr << r0.at(j);   // Non interpolated values remain as in the beginning row.
        }

        // Computing the interpolated values
        for (qint32 j = 0; j < indexesToInterpolate.size(); j++){
            qint32 k = indexesToInterpolate.at(j);
            dr[k] = m.at(j)*t + b.at(j);
        }

        // Adding the row.
        res << dr;
    }

    return res;

}



int DataSetDownsampler::GreatestCommonDivisor(int a, int b){
    //qDebug() << "GCD: " << a << b;
    if (b == 0) return a;
    return GreatestCommonDivisor(b, a  % b);
}


bool DataSetDownsampler::saveReadingDataSetListToFile(int targetFrequency){

    // First we need to create the output directory.
    QString downsampleDirName = DIR_DOWNSAMPLED + QString("-") + QString::number(targetFrequency);
    QDir(result.originalFilePath).mkdir(downsampleDirName);
    QString path = result.originalFilePath + QString("/") + downsampleDirName;

    if (!QDir(path).exists()){
        error = "Could not create downsample dir at: " + path;
        return false;
    }

    // Saving the eye rep gen conf.
    if (!eyeRepGenConf.saveToFile(path + "/" + QString(FILE_EYE_REP_GEN_CONFIGURATION),COMMON_TEXT_CODEC)){
        error = "Saving eye rep gen conf: " + eyeRepGenConf.getError();
        return false;
    }

    // Saving the file itself.
    QStringList fileContent;
    fileContent << QString(HEADER_READING_EXPERIMENT) + " " + result.version;
    fileContent << result.experimentDescription;
    fileContent << QString(HEADER_READING_EXPERIMENT);
    fileContent << QString::number(result.resolution.width()) + " " + QString::number(result.resolution.height());

    for (qint32 i = 0; i < result.dataSets.size(); i++){  // Each data set.
        for (qint32 j = 0; j < result.dataSets.at(i).size(); j++){ // Each row in each data set
            QString row = result.dataSetIDs.at(i) + " ";
            for (qint32 k = 0; k < result.dataSets.at(i).at(j).size(); k++){ // Each column in each row of each data set.
                // The first five values are the time samp and coordinates and should be printed as integers
                if (k < 5)
                    row = row + QString::number(static_cast<qint32>(result.dataSets.at(i).at(j).at(k))) + " ";
                else
                    row = row + QString::number(result.dataSets.at(i).at(j).at(k)) + " ";
            }
            fileContent << row;
        }
    }

    outputDownSampledFile = path + "/" + result.originalFileName + ".dat";
    QFile outputFile(outputDownSampledFile);
    if (!outputFile.open(QFile::WriteOnly)){
        error = "Could not open file " + outputFile.fileName() + " for writing";
        return false;
    }

    QTextStream writer(&outputFile);
    writer << fileContent.join("\n");
    writer << "\n";
    outputFile.close();

    qDebug() << "Saved file to " << outputFile.fileName();

    return true;

}
