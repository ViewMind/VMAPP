#include "frequencyanddispersionsweeper.h"

FrequencyAndDispersionSweeper::FrequencyAndDispersionSweeper()
{

}

void FrequencyAndDispersionSweeper::setSweepParameters(const SweepParameters &sp){
    sweepParameters = sp;
}

void FrequencyAndDispersionSweeper::run(){

    // Empty error will signal all is ok.
    error = "";
    totalNumerOfFixations.clear();

    if (sweepParameters.startMaxDispersionValue >= sweepParameters.endMaxDispersionValue){
        error = "The end max dispersion value should be larger than the start max dispersion value";
        return;
    }

    // Generating the sweep lists.
    frequencyList.clear();

    QFileInfo info(sweepParameters.fileName);
    QString workPath = info.path();
    QString csvBaseName = info.baseName();

    ConfigurationManager eyeRepGenConf;
    QString eyeRepGenConfFileName = workPath + QString("/") + FILE_EYE_REP_GEN_CONFIGURATION;
    if (!eyeRepGenConf.loadConfiguration(eyeRepGenConfFileName,COMMON_TEXT_CODEC)){
        error = "Loading Eye Rep Gen Conf: " + eyeRepGenConf.getError();
        return;
    }

    int fileFrequency = eyeRepGenConf.getInt(CONFIG_SAMPLE_FREQUENCY);
    if (DataSetDownsampler::KnownFrequencies.indexOf(fileFrequency) == -1){
        error = "Unknown sample frequency: " + QString::number(fileFrequency);
        return;
    }

    // Final step, save all to an ouput file
    QString outputFileName = workPath + QString("/") + OUTPUT_SWEEP_FILE_BASE + csvBaseName
            + "_" + QString::number(sweepParameters.startFrequency) + "-" + QString::number(fileFrequency)
            + "_" + QString::number(sweepParameters.startMaxDispersionValue) + "-" + QString::number(sweepParameters.endMaxDispersionValue)
            + ".out";

    // Opening the output text file.
    QFile outputFile(outputFileName);
    if (!outputFile.open(QFile::WriteOnly)){
        error = "Could not create output file name: " + outputFileName;
        return;
    }
    QTextStream writer(&outputFile);


    // For the original one we don't do any preprocessing but just copy the file.
    QDir workDir(workPath);
    QString destPath = workPath + "/original";
    workDir.mkdir("original");
    if (!QDir(destPath).exists()){
        error = "Could not create directory for the original file: " + destPath;
        outputFile.close();
        return;
    }

    // Copying the original file to the "original" directory as the final process.
    QString originalFileName = destPath + "/" + csvBaseName + ".dat";
    QString originalConfFile = destPath + QString("/") + FILE_EYE_REP_GEN_CONFIGURATION;
    QFile::copy(sweepParameters.fileName,originalFileName);
    QFile::copy(eyeRepGenConfFileName,originalConfFile);

    if (!QFile(originalFileName).exists()){
        error = "Could not create copy of original file " + originalFileName;
        outputFile.close();
        return;
    }

    if (!QFile(originalConfFile).exists()){
        error = "Could not create copy of original configuration file " + originalFileName;
        outputFile.close();
        return;
    }

    // Creating the frequency list
    int f = sweepParameters.startFrequency;
    while (f < fileFrequency){
        frequencyList << f;
        f = f + sweepParameters.frequencyStep;
    }

    if (frequencyList.isEmpty()){
        error = "Starting sweep frequency " +  QString::number(sweepParameters.startFrequency) + " larger than file frequency of " + QString::number(fileFrequency);
        outputFile.close();
        return;
    }

    // No matter the frequency step the last step is the actual frequency.
    frequencyList << fileFrequency;

    // Generating max dispersion list.
    maxDispersionList.clear();
    for (int i = sweepParameters.startMaxDispersionValue; i <= sweepParameters.endMaxDispersionValue; i++) maxDispersionList << i;

    // Total Number of files to process
    totalNumberOfSteps = maxDispersionList.size()*frequencyList.size();
    stepCounter = 0;

    QList<qint32> results;
    for (qint32 i = 0; i < frequencyList.size()-1; i++){

        // Do the downsampling.
        DataSetDownsampler dsd;
        if (!dsd.downSampleRawDataFile(sweepParameters.fileName,frequencyList.at(i))){
            error = "DOWNSAMPLING: " + dsd.getError();
            outputFile.close();
            return;
        }

        // Do the Max Dispersion Sweep.
        results = doMaxDispersionSweep(dsd.getOutputDownSampledFile(),sweepParameters.resolutionScaling);
        totalNumerOfFixations << results;

        // Saving to file
        for (qint32 j = 0; j < maxDispersionList.size(); j++){
            writer << frequencyList.at(i) << " " << maxDispersionList.at(j) << " " << results.at(j) << "\n";
        }

    }

    // Processing the original file last.
    results = doMaxDispersionSweep(originalFileName,sweepParameters.resolutionScaling);
    totalNumerOfFixations << results;

    // Saving to file
    for (qint32 j = 0; j < maxDispersionList.size(); j++){
        writer << frequencyList.last() << " " << maxDispersionList.at(j) << " " << results.at(j) << "\n";
    }

    outputFile.close();

}


QList<qint32> FrequencyAndDispersionSweeper::doMaxDispersionSweep(const QString &fileName, qreal resolutionScaling){

    QList<qint32> ans;

    DataSetExtractor::RawDataInfo rdi = DataSetExtractor::separateRawDataFile(fileName);
    if (!rdi.error.isEmpty()){
        error = "Separating Raw Data File: " + rdi.error;
        return ans;
    }

    // Setting the original file name and path
    QFileInfo info(fileName);
    QString workPath = info.path();
    QString csvBaseName = info.baseName();

    // Assuming that eye rep gen conf is in the selected file's directory.
    ConfigurationManager eyeRepGenConf;
    if (!eyeRepGenConf.loadConfiguration(workPath + QString("/") + FILE_EYE_REP_GEN_CONFIGURATION,COMMON_TEXT_CODEC)){
        error = "Loading Eye Rep Gen Conf: " + eyeRepGenConf.getError();
        return ans;
    }

    // Setting the resolution in the eye rep gen conf.
    eyeRepGenConf.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,rdi.resolution.width());
    eyeRepGenConf.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,rdi.resolution.height());
    eyeRepGenConf.addKeyValuePair(CONFIG_RESOLUTION_SCALING,resolutionScaling);

    // Required for configuring the processors
    MonitorGeometry mgeo;
    MovingWindowParameters mwp;

    if (eyeRepGenConf.getBool(CONFIG_VR_ENABLED) && eyeRepGenConf.containsKeyword(CONFIG_VR_ENABLED)){
        mgeo.distanceToMonitorInMilimiters = MonitorGeometry::VR_VIEW_DISTANCE_TO_MONITOR;
        mgeo.XmmToPxRatio                  = MonitorGeometry::VR_VIEW_WIDTH/eyeRepGenConf.getReal(CONFIG_RESOLUTION_WIDTH);
        mgeo.YmmToPxRatio                  = MonitorGeometry::VR_VIEW_HEIGHT/eyeRepGenConf.getReal(CONFIG_RESOLUTION_HEIGHT);
    }

    mgeo.resolutionWidth = eyeRepGenConf.getReal(CONFIG_RESOLUTION_WIDTH);
    mgeo.resolutionHeight = eyeRepGenConf.getReal(CONFIG_RESOLUTION_HEIGHT);

    EDPBase *processor;
    if (csvBaseName.startsWith(FILE_OUTPUT_READING)){
        processor = new EDPReading(&eyeRepGenConf);
    }
    else{
        error = "Unknown file type: " + fileName;
        return ans;
    }

    mgeo.distanceToMonitorInMilimiters = eyeRepGenConf.getReal(CONFIG_DISTANCE_2_MONITOR)*10;
    mgeo.XmmToPxRatio                  = eyeRepGenConf.getReal(CONFIG_XPX_2_MM);
    mgeo.YmmToPxRatio                  = eyeRepGenConf.getReal(CONFIG_YPX_2_MM);
    mwp.minimumFixationLength          = eyeRepGenConf.getReal(CONFIG_MIN_FIXATION_LENGTH);
    mwp.sampleFrequency                = eyeRepGenConf.getReal(CONFIG_SAMPLE_FREQUENCY);

    processor->setFieldingMargin(static_cast<qint32>(eyeRepGenConf.getReal(CONFIG_MARGIN_TARGET_HIT)));
    processor->setMonitorGeometry(mgeo);
    processor->setPixelsInSacadicLatency(eyeRepGenConf.getInt(CONFIG_LATENCY_ESCAPE_RAD));
    processor->configure(fileName,rdi.expDescription);

    FreqAnalysis::FreqAnalysisResult far = FreqAnalysis::doFrequencyAnalysis(&eyeRepGenConf,fileName);
    qDebug() << "Frequency of file used for MD Sweep" << far.averageFrequency;

    //for (int mdw = startValue; mdw <= endValue; mdw++){
    for (int j = 0; j < maxDispersionList.size(); j++){
        int mdw = maxDispersionList.at(j);
        mwp.maxDispersion = mdw;
        processor->setMovingWindowParameters(mwp);
        processor->calculateWindowSize();

        // Processing witht the specific max dispersion parameter.
        if (!processor->doEyeDataProcessing(rdi.expData)){
            error = "Error processing data: " + processor->getError() + " for MDW size of: " + QString::number(mdw);
            return ans;
        }
        QString destFile = workPath + QString("/") + csvBaseName + "-" + QString::number(mdw) + ".csv";
        QFile::remove(destFile);
        if (!QFile::copy(processor->getOuputMatrixFileName(),destFile)){
            error = "Could not copy " + processor->getOuputMatrixFileName() + " to " + destFile;
            return ans;
        }

        // Getting the total number of fixations.
        int acc = 0;
        for (qint32 i = 0; i < processor->getEyeFixations().left.size(); i++){
            acc = acc + processor->getEyeFixations().left.at(i).size();
        }

        for (qint32 i = 0; i < processor->getEyeFixations().right.size(); i++){
            acc = acc + processor->getEyeFixations().right.at(i).size();
        }

        ans << acc;

        stepCounter++;
        emit(updateProgress(100*stepCounter/totalNumberOfSteps));
    }

    return ans;
}
