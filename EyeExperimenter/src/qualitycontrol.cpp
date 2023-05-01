#include "qualitycontrol.h"

const char * QualityControl::GRAPH_DATA       = "GraphData";
const char * QualityControl::UPPER_REF_DATA   = "UpperREFData";
const char * QualityControl::LOWER_REF_DATA   = "LowerREFData";
const char * QualityControl::QC_INDEX         = "QCIndex";
const char * QualityControl::QC_OK            = "QCOk";
const char * QualityControl::QC_THRESHOLD     = "QCThreshold";

QualityControl::QualityControl()
{
    verifyFileIntegrity = true;
}

void QualityControl::disableCheckSumVerification(){
    verifyFileIntegrity = false;
}

bool QualityControl::checkFileIntegrity(){
    return rawdata.verifyChecksumHash();
}


void QualityControl::run(){

//    if (!rawdata.loadFromJSONFile(originalFileName)){
//        error = "Could not load the raw data file: " + originalFileName + ". Reason: " + rawdata.getError();
//        return;
//    }

//    if (DBUGBOOL(Debug::Options::FIX_QC_SHA_CHECKS)){
//        QString dbug = "DBUG FORCE FIX QC SHA CHECKS";
//        qDebug() << dbug;
//        StaticThreadLogger::warning("QualityControl::run",dbug);
//        rawdata.setChecksumHash();
//    }

//    if (!rawdata.verifyChecksumHash()){
//        error = "Checksum verification changed in the raw data file";
//        return;
//    }

//    // This allows to recompute QC factors if, online, those factors were updated.
//    rawdata.setQCParameters(updatedQCParameters);

//    availableStudies = rawdata.getStudies();
//    qualityControlData.clear();

//    listOfStudy3DFlag.clear();
//    for (qint32 i = 0; i < availableStudies.size(); i++){
//        QString study = availableStudies.at(i);
//        QString metaStudy = VMDC::MultiPartStudyBaseName::getMetaStudy(study);
//        if (!computeQualityControlVectors(study,metaStudy)) return;
//    }

//    return;

    QElapsedTimer t;
    t.start();
    while (t.elapsed() < 3000)

}

void QualityControl::setVMContainterFile(const QString &file, const QVariantMap &updateQC){
    originalFileName = file;
    updatedQCParameters = updateQC;
    this->start();
}

bool QualityControl::setDiscardReasonAndComment(const QString &discard_reson_code, const QString &comment){
    rawdata.addCustomMetadataFields(VMDC::MetadataField::DISCARD_REASON,discard_reson_code);
    rawdata.addCustomMetadataFields(VMDC::MetadataField::COMMENTS,comment);
    // Saving the info to HD:
    if (!rawdata.saveJSONFile(originalFileName,true)){
        error = "Could not save " + originalFileName + " after QC appending. Reason: " + rawdata.getError();
        return false;
    }
    return true;
}

QVariantMap QualityControl::getGraphDataAndReferenceDataForStudy(const QString &study){
    if (availableStudies.contains(study)) return qualityControlData.value(study).toMap();
    return QVariantMap();
}

QVariantList QualityControl::getStudyList() const {
    if (listOfStudy3DFlag.size() != availableStudies.size()){
        StaticThreadLogger::error("QualityControl::getStudyList","When getting study list, the list of available studies is of size: " + QString::number(availableStudies.size())
                                  + " while the list of 3D study flags is of size: " + QString::number(listOfStudy3DFlag.size()));
        return QVariantList();
    }

    QVariantList list;
    for (qint32 i = 0; i < availableStudies.size(); i++){
        QVariantMap t;
        t["name"] = availableStudies.at(i);
        t["3D"] = listOfStudy3DFlag.at(i);
        list << t;
    }

    return list;
}


QString QualityControl::getError() const {
    return error;
}

QString QualityControl::getSetFileName() const {
    return originalFileName;
}

bool QualityControl::computeQualityControlVectors(const QString &studyType, const QString &metaStudyName){
    QVariantMap studyConfiguration = rawdata.getStudyConfiguration(studyType);
    if (studyConfiguration.contains(VMDC::StudyParameter::IS_3D_STUDY)){
        if (studyConfiguration.value(VMDC::StudyParameter::IS_3D_STUDY).toBool()){
            listOfStudy3DFlag << true;
            return computeQualityControlVectorsFor3DStudies(studyType,metaStudyName);
        }
        else {
            listOfStudy3DFlag << false;
            return computeQualityControlVectorsFor2DStudies(studyType,metaStudyName);
        }
    }
    else {
        listOfStudy3DFlag << false;
        return computeQualityControlVectorsFor2DStudies(studyType,metaStudyName);
    }
}

bool QualityControl::computeQualityControlVectorsFor2DStudies(const QString &studyType, const QString &metaStudyName){

    //qDebug() << "Quality control for" << studyType << metaStudyName;

    QVariantList trialList = rawdata.getStudyTrialList(studyType);
    QVariantMap studyConfiguration = rawdata.getStudyConfiguration(studyType);
    QString fixationEyeToUse = studyConfiguration.value(VMDC::StudyParameter::VALID_EYE).toString();

    if (fixationEyeToUse == VMDC::Eye::BOTH){
        if (studyConfiguration.contains(VMDC::StudyParameter::DEFAULT_EYE)){
            fixationEyeToUse = studyConfiguration.value(VMDC::StudyParameter::DEFAULT_EYE).toString();
        }
        else {
            // This should never happen. It is left for legacy reason in case QC Procesing is done on files that do not contains the Default Eye Study Parameter.
            fixationEyeToUse = VMDC::Eye::RIGHT;
        }
    }

    //qDebug() << "USING QC Eye" << fixationEyeToUse;

    QVariantMap qc = rawdata.getQCParameters();
    QVariantMap pp = rawdata.getProcessingParameters();
    qreal sample_freq = pp.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();

    // Computing values necessary for the information completion index.
    qreal sample_period = 1000.0/sample_freq; // In miliseconds.
    qreal max_allowed_over_period = sample_period*1.2; // 20% overperiod.
    qreal index_of_information_completion_threshold_in_trial = qc.value(VMDC::QCGlobalParameters::ICI_TRIAL_THRESHOLD).toReal()/100;
    qreal threshold_ici = qc.value(VMDC::QCGlobalParameters::ICI_VALID_N_TRIAL_THRESHOLD).toReal();

    // All related computations for frequency glitches are computed but never used, for now.
    qreal max_diff = qc.value(VMDC::QCGlobalParameters::MAX_TIMESTAMP_DIFF).toReal();
    qreal min_diff = qc.value(VMDC::QCGlobalParameters::MIN_TIMESTAMP_DIFF).toReal();
    qreal glitch_percent = qc.value(VMDC::QCGlobalParameters::MAX_GLITCHES).toReal()/100;

    // The frequency tolerance.
    qreal over_frequency  = qc.value(VMDC::QCGlobalParameters::ALLOWED_SAMPLING_FREQ_VARIATION_PLUS).toReal();
    qreal under_frequency = qc.value(VMDC::QCGlobalParameters::ALLOWED_SAMPLING_FREQ_VARIATION_MINUS).toReal();
    over_frequency = sample_freq*(1 + over_frequency/100);
    under_frequency = sample_freq*(1 - under_frequency/100);

    QVariantMap qcStudy = qc.value(metaStudyName).toMap();

    // The minimum number of points depends on the trial duration and the sampling frequency. The division by a thousand is because the time is in ms.
    qreal min_points_per_trial = qcStudy.value(VMDC::QCStudyParameters::MIN_TIME_DURATION_PER_TRIAL).toReal()*sample_freq/1000.0;
    //qDebug() << "Minimum number of points pre trial" << min_points_per_trial << sample_freq << qcStudy.value(VMDC::QCStudyParameters::MIN_TIME_DURATION_PER_TRIAL).toReal();
    //Debug::prettpPrintQVariantMap(qcStudy);
    qreal min_fix_per_trial    = qcStudy.value(VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL).toReal();

    // Getting the thresholds.
    qreal threshold_fix = qcStudy.value(VMDC::QCStudyParameters::THRESHOLD_VALID_NUM_FIXATIONS).toReal();
    qreal threshold_points = qcStudy.value(VMDC::QCStudyParameters::THRESHOLD_VALID_NUM_DATA_POINTS).toReal();
    qreal threshold_f = qc.value(VMDC::QCGlobalParameters::THRESHOLD_NUM_TRIALS_VALID_F).toReal();

    //qDebug() << "min points per trial" << min_points_per_trial;

    QVariantList pointsPerTrial;
    QVariantList refPointsPerTrial;

    QVariantList fixationsPerTrial;
    QVariantList refNFixations;

    QVariantList glitchesPerTrial;
    QVariantList refGlitches;

    QVariantList avgFreqPerTrial;
    QVariantList upperRefFreq;
    QVariantList lowerRefFreq;

    QVariantList indexOfInformationCompletionPerTrial;
    QVariantList refIndexOfInformationCompletion;

    qsizetype    ndatasets = 0;

    qreal number_of_trials_with_too_few_points = 0;
    qreal number_of_trials_with_too_few_fixations = 0;
    qreal number_of_trials_with_bad_avg_freq = 0;
    qreal number_of_trials_with_low_ici = 0;

    qreal qc_fix_index = 0;
    qreal qc_dpoint_index = 0;
    qreal qc_freq_index = 0;
    qreal qc_ici_index = 0;

    qreal number_of_trials = static_cast<qreal>(trialList.size());

    for (qint32 i = 0; i < trialList.size(); i++){

        QVariantMap trial = trialList.at(i).toMap();
        QVariantMap data = trial.value(VMDC::TrialField::DATA).toMap();
        QStringList dataset_names = data.keys();
        qsizetype pointsInTrial = 0;
        qreal     total_sampling_time = 0;
        qsizetype fixInTrial = 0;
        qint32 glitchesInTrial = 0;
        qreal  periodAcc = 0;
        qreal  timestamp = -1;
        qreal sum_of_missing_information_intervals = 0;
        qreal sum_of_the_duration_of_all_data_sets = 0;

        ndatasets = dataset_names.size();

        for (qint32 j = 0; j < dataset_names.size(); j++){

            QVariantMap dataSet = data.value(dataset_names.at(j)).toMap();
            QVariantList rawData = dataSet.value(VMDC::DataSetField::RAW_DATA).toList();
            QVariantList fixR = dataSet.value(VMDC::DataSetField::FIXATION_R).toList();
            QVariantList fixL = dataSet.value(VMDC::DataSetField::FIXATION_L).toList();

            // The easy stuff first: The number of data points and fixatons. The fixations shown will be those for the eye used in processing.
            pointsInTrial = pointsInTrial + rawData.size();
            if (fixationEyeToUse == VMDC::Eye::LEFT){
                fixInTrial = fixInTrial + fixL.size();
            }
            else {
                fixInTrial = fixInTrial + fixR.size();
            }

            // The actual frequency analysis.
            if (rawData.size() > 2){

                qreal startTimeOfDataSet = rawData.first().toMap().value(VMDC::DataVectorField::TIMESTAMP).toReal();
                qreal endTimeOfDataSet = rawData.last().toMap().value(VMDC::DataVectorField::TIMESTAMP).toReal();

                sum_of_the_duration_of_all_data_sets = sum_of_the_duration_of_all_data_sets + (endTimeOfDataSet - startTimeOfDataSet);

                timestamp = startTimeOfDataSet;

                total_sampling_time = total_sampling_time + (endTimeOfDataSet - startTimeOfDataSet);

                for (qint32 k = 1; k < rawData.size(); k++){
                    qreal ts = rawData.at(k).toMap().value(VMDC::DataVectorField::TIMESTAMP).toReal();
                    qreal diff = ts - timestamp;

                    sum_of_missing_information_intervals = sum_of_missing_information_intervals + qMax(diff - max_allowed_over_period,0.0);

                    timestamp = ts;

                    periodAcc = periodAcc + diff;

                    //qDebug() << min_diff << diff <<  max_diff;

                    if ((diff > max_diff) || (diff < min_diff)){
                        // This is a glitch.
                        glitchesInTrial++;
                    }
                }

            }



        }

        // For each dataset we use 1 less than the number of points in the raw data of that dataset to compute period.
        // Hence the number of datasets is subsctracted from the number of points in the trial.
        periodAcc = periodAcc / (static_cast<qreal>(pointsInTrial) - static_cast<qreal>(dataset_names.size()));        //qDebug() << periodAcc;

        // Data points computations.
        pointsPerTrial << pointsInTrial;
        refPointsPerTrial << min_points_per_trial;
        if (static_cast<qreal>(pointsInTrial) < min_points_per_trial){
            number_of_trials_with_too_few_points++;
        }
        qc_dpoint_index = ceil((number_of_trials-number_of_trials_with_too_few_points)*100/number_of_trials);

        // Fixation computations.
        fixationsPerTrial << fixInTrial;
        refNFixations << min_fix_per_trial;
        if (static_cast<qreal>(fixInTrial) < min_fix_per_trial){
            number_of_trials_with_too_few_fixations++;
        }
        qc_fix_index = ceil((number_of_trials-number_of_trials_with_too_few_fixations)*100/number_of_trials);

        // The reference for glitches is a percet of the number of points in the trial.
        glitchesPerTrial << glitchesInTrial;
        refGlitches << glitch_percent*(static_cast<qreal>(pointsInTrial)-static_cast<qreal>(dataset_names.size()));

        qreal avgFrequencyInTrial = 1000.0/periodAcc; // The 1000 is because the period is in ms.
        avgFreqPerTrial << avgFrequencyInTrial;
        lowerRefFreq << under_frequency;
        upperRefFreq << over_frequency;
        if ((avgFrequencyInTrial > over_frequency) || (avgFrequencyInTrial < under_frequency)){
            number_of_trials_with_bad_avg_freq++;
        }
        qc_freq_index = ceil((number_of_trials - number_of_trials_with_bad_avg_freq)*100/number_of_trials);

        // Computing the index of information completion for the trial.
        qreal index_of_information_completion = 1 - sum_of_missing_information_intervals/sum_of_the_duration_of_all_data_sets;
        indexOfInformationCompletionPerTrial << index_of_information_completion;
        refIndexOfInformationCompletion << index_of_information_completion_threshold_in_trial;
        if (static_cast<qreal>(index_of_information_completion) < index_of_information_completion_threshold_in_trial){
            number_of_trials_with_low_ici++;
        }
        qc_ici_index = ceil((number_of_trials - number_of_trials_with_low_ici)*100/number_of_trials);
    }

    // Saving the vector information in the raw data.
    if (!rawdata.setQCVector(studyType,VMDC::QCFields::AVG_FREQ,avgFreqPerTrial)){
        error = "Setting avg freq qc: " + rawdata.getError();
        return false;
    }

    if (!rawdata.setQCVector(studyType,VMDC::QCFields::POINTS,pointsPerTrial)){
        error = "Setting n points per trial qc: " + rawdata.getError();
        return false;
    }

    if (!rawdata.setQCVector(studyType,VMDC::QCFields::FIXATIONS,fixationsPerTrial)){
        error = "Setting fixations l qc: " + rawdata.getError();
        return false;
    }

    if (!rawdata.setQCVector(studyType,VMDC::QCFields::GLITCHES,glitchesPerTrial)){
        error = "Setting glitches qc: " + rawdata.getError();
        return false;
    }

    if (!rawdata.setQCVector(studyType,VMDC::QCFields::ICI,indexOfInformationCompletionPerTrial)){
        error = "Setting ICI qc: " + rawdata.getError();
        return false;
    }

    // Boolean flags for the OK status.
    bool qc_ok_fix = (qc_fix_index >= threshold_fix);
    bool qc_ok_f   = (qc_freq_index >= threshold_f);
    bool qc_ok_points = (qc_dpoint_index >= threshold_points);
    bool qc_ok_ici = (qc_ici_index >= threshold_ici);

    QVariantMap valuesToSet;
    valuesToSet[VMDC::QCFields::N_DATASETS] = ndatasets;
    valuesToSet[VMDC::QCFields::QC_FIX_INDEX] = qc_fix_index;
    valuesToSet[VMDC::QCFields::QC_FREQ_INDEX] = qc_freq_index;
    valuesToSet[VMDC::QCFields::QC_POINT_INDEX] = qc_dpoint_index;
    valuesToSet[VMDC::QCFields::QC_ICI_INDEX] = qc_ici_index;
    valuesToSet[VMDC::QCFields::QC_FIX_INDEX_OK] = qc_ok_fix;
    valuesToSet[VMDC::QCFields::QC_FREQ_INDEX_OK] = qc_ok_f;
    valuesToSet[VMDC::QCFields::QC_POINT_INDEX_OK] = qc_ok_points;
    valuesToSet[VMDC::QCFields::QC_ICI_INDEX_OK] = qc_ok_ici;

    QStringList keys = valuesToSet.keys();
    for (qint32 j = 0; j < keys.size(); j++){
        QString field = keys.at(j);
        QVariant value = valuesToSet.value(field);
        if (!rawdata.setQCValue(studyType,field,value)){
            error = "Setting " + field + " in qc: " + rawdata.getError();
            return false;
        }
    }

    // Saving the info to HD:
    if (!rawdata.saveJSONFile(originalFileName,true)){
        error = "Could not save " + originalFileName + " after QC appending. Reason: " + rawdata.getError();
        return false;
    }

    // Finally we save the data for graphs.
    QVariantMap studyQC;
    QVariantMap temp;

    temp.clear();
    temp[GRAPH_DATA] = pointsPerTrial;
    temp[UPPER_REF_DATA] = refPointsPerTrial;
    temp[LOWER_REF_DATA] = refPointsPerTrial;
    temp[QC_INDEX] = qc_dpoint_index;
    temp[QC_OK] = qc_ok_points;
    temp[QC_THRESHOLD] = threshold_points;
    studyQC.insert(VMDC::QCFields::POINTS,temp);

    temp[GRAPH_DATA] = fixationsPerTrial;
    temp[UPPER_REF_DATA] = refNFixations;
    temp[LOWER_REF_DATA] = refNFixations;
    temp[QC_INDEX] = qc_fix_index;
    temp[QC_OK] = qc_ok_fix;
    temp[QC_THRESHOLD] = threshold_fix;
    studyQC.insert(VMDC::QCFields::FIXATIONS,temp);

    temp.clear();
    temp[GRAPH_DATA] = avgFreqPerTrial;
    temp[UPPER_REF_DATA] = upperRefFreq;
    temp[LOWER_REF_DATA] = lowerRefFreq;
    temp[QC_INDEX] = qc_freq_index;
    temp[QC_OK] = qc_ok_f;
    temp[QC_THRESHOLD] = threshold_f;
    studyQC.insert(VMDC::QCFields::AVG_FREQ,temp);

    temp.clear();
    temp[GRAPH_DATA] = indexOfInformationCompletionPerTrial;
    temp[UPPER_REF_DATA] = refIndexOfInformationCompletion;
    temp[LOWER_REF_DATA] = refIndexOfInformationCompletion;
    temp[QC_INDEX] = qc_ici_index;
    temp[QC_OK] = qc_ok_ici;
    temp[QC_THRESHOLD] = threshold_ici;
    studyQC.insert(VMDC::QCFields::ICI,temp);

    qualityControlData.insert(studyType,studyQC);
    return true;

}

bool QualityControl::computeQualityControlVectorsFor3DStudies(const QString &studyType, const QString &metaStudyName){

    Q_UNUSED(metaStudyName)

    // QVariantMap studyConfiguration = rawdata.getStudyConfiguration(studyType);

    QVariantMap qc = rawdata.getQCParameters();
    QVariantMap pp = rawdata.getProcessingParameters();
    qreal sample_freq = pp.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();

    // Computing values necessary for the information completion index.
    qreal sample_period = 1000.0/sample_freq; // In miliseconds.
    qreal index_of_information_completion_threshold_in_trial = qc.value(VMDC::QCGlobalParameters::ICI_TRIAL_THRESHOLD).toReal();
    qreal threshold_ici = qc.value(VMDC::QCGlobalParameters::ICI_VALID_N_TRIAL_THRESHOLD).toReal();


    // We need to get the time vector and either split it into N bins or into a number that ensure at least M points per bin.
    qint32 targetNumberOfBins = 80;
    if (qc.contains(VMDC::QCGlobalParameters::ICI_3D_TARGET_NUMBER_OF_BINS)){
        targetNumberOfBins = qc.value(VMDC::QCGlobalParameters::ICI_3D_TARGET_NUMBER_OF_BINS).toInt();
    }
    else {
        StaticThreadLogger::warning("QualityControl::computeQualityControlVectorsFor3DStudies","Unavailable information for the Target Number Of Bins in a 3D Study. Using the default of 80");
    }
    qint32 minimumNumberOfDataPointsPerBin = 100;
    if (qc.contains(VMDC::QCGlobalParameters::ICI_3D_MIN_BIN_N_POINTS)){
        targetNumberOfBins = qc.value(VMDC::QCGlobalParameters::ICI_3D_MIN_BIN_N_POINTS).toInt();
    }
    else {
        StaticThreadLogger::warning("QualityControl::computeQualityControlVectorsFor3DStudies","Unavailable information for the Minimum Number of Points In a Bin For a 3D Study. Using the default of 100");
    }

    QVariant timevec = rawdata.get3DStudyData(studyType,VMDC::Study3DDataFields::TIME_VECTOR);
    QVariantList timeVector = timevec.toList();
    QList< QList<qreal> > timeVectorBins;

    qreal qc_ici_index = 0;

//    // Over all verification.
//    qreal npoints = timeVector.size();
//    qreal total_length = timeVector.last().toReal() - timeVector.first().toReal();
//    qreal total_expected = total_length/sample_period;

//    qDebug() << "Total n points in vector is " << npoints << " total vector time " << total_length << " and the expected number of pints in that legth is " << total_expected
//             << " in % it is " << npoints*100/total_expected;


    // Now we transform the time vector in to a list of lists. Each list is a "bin" akin to the datasets of 2D studies.
    if (timeVector.length() < minimumNumberOfDataPointsPerBin*targetNumberOfBins){

        // Time vector is too short, so we prioritize number of data points per bin, over the number of bins.
        QList<qreal> bin;
        for (qint32 i = 0; i < timeVector.size(); i++){
            bin << timeVector.at(i).toReal();
            if (bin.size() == minimumNumberOfDataPointsPerBin){
                timeVectorBins << bin;
                bin.clear();
            }
        }

        if (bin.size() > 0){
            timeVectorBins << bin; // Adding the last bin.
        }

    }
    else {

        // We have enough points, so we make the target number of bins.
        qreal N = static_cast<qreal>(timeVector.size());
        qreal M = static_cast<qreal>(targetNumberOfBins);
        qint32 nPointsPerBin = static_cast<qint32>(qFloor(N/M));

        QList<qreal> bin;
        for (qint32 i = 0; i < timeVector.size(); i++){
            bin << timeVector.at(i).toReal();

            // So we only check if we reached the target number of points per bin in all bins except the last one. The last one needs to be as long as it needs to
            if (timeVectorBins.size() < (targetNumberOfBins-1)){
                if (bin.size() == nPointsPerBin){
                    timeVectorBins << bin;
                    bin.clear();
                }
            }

        }
        timeVectorBins << bin; // Adding the last bin.
    }

//    /////////////////////// BIN FILTERING
//    /// It has been known for time stamps to be repeated. So we do filter the bins in order to property compute the ICI.
//    QList < QList<qreal> > filteredTimeVectorBins;

//    for (qint32 i = 0; i < timeVectorBins.size(); i++){

//        QList<qreal> filteredBin;

//        for (qint32 j = 0; j < timeVectorBins.at(i).size(); j++){
//            qreal value = timeVectorBins.at(i).at(j);
//            if (filteredBin.size() > 0){
//                // This filter assumes that duped time stamp values are actually consecutive. Which is why we only need to compared to the last.
//                if (value != filteredBin.last()){
//                    filteredBin << value;
//                }
//            }
//            else {
//                filteredBin << value;
//            }
//        }

//        filteredTimeVectorBins << filteredBin;

//    }

//    // The filtered bins replace the time vector bins. And we clear the excess variable.
//    timeVectorBins = filteredTimeVectorBins;
//    filteredTimeVectorBins.clear();

    //////////////////////////// Actual ICI Computation.

    //qDebug() << "min points per trial" << min_points_per_trial;

    QVariantList indexOfInformationCompletionPerTrial;
    QVariantList refIndexOfInformationCompletion;

    qreal number_of_trials_with_low_ici = 0;

    qreal number_of_bins = static_cast<qreal>(timeVectorBins.size());

    for (qint32 i = 0; i < timeVectorBins.size(); i++){

        // For each bin, we check the time stamp differnte between the beginning and the end. Assuming there is more than 2 data points.
        qreal index_of_information_completion = 0;

        if (timeVectorBins.size() > 2){
            qreal start = timeVectorBins.at(i).first();
            qreal end   = timeVectorBins.at(i).last();
            qreal diff = end - start;
            qreal expected_number_of_data_points = diff/sample_period; // We then compute how many data points should there be based on the time difference.
            index_of_information_completion = static_cast<qreal>(timeVectorBins.at(i).size())*100/expected_number_of_data_points; // Finally the ICI is the percent of the number of points, there actually are.
            if (index_of_information_completion > 100) index_of_information_completion = 100;

//            /// This entire section of code is for DEBUGGINING ONLY.
//            if (index_of_information_completion < 70){
//                qDebug() << "Printing Differences for bin" << i << ". Size" << timeVectorBins.at(i).size() << ". Expected size: " << expected_number_of_data_points;
//                qreal last = 0;
//                for (qint32 j = 0; j < timeVectorBins.at(i).size(); j++){
//                    qDebug() << timeVectorBins.at(i).at(j) << "diff" << timeVectorBins.at(i).at(j) - last;
//                    last = timeVectorBins.at(i).at(j);
//                }
//            }

        }
        indexOfInformationCompletionPerTrial << index_of_information_completion; // We store the result for this trial.
        refIndexOfInformationCompletion << index_of_information_completion_threshold_in_trial; // We store the reference yet again. This is required for plotting.
        if (index_of_information_completion < index_of_information_completion_threshold_in_trial){
            number_of_trials_with_low_ici++;
        }

    }

    qc_ici_index = ceil((number_of_bins - number_of_trials_with_low_ici)*100/number_of_bins);


    if (!rawdata.setQCVector(studyType,VMDC::QCFields::ICI,indexOfInformationCompletionPerTrial)){
        error = "Setting ICI qc: " + rawdata.getError();
        return false;
    }

    // Boolean flags for the OK status.
    bool qc_ok_ici = (qc_ici_index >= threshold_ici);

    QVariantMap valuesToSet;
    valuesToSet[VMDC::QCFields::QC_ICI_INDEX] = qc_ici_index;
    valuesToSet[VMDC::QCFields::QC_ICI_INDEX_OK] = qc_ok_ici;

    QStringList keys = valuesToSet.keys();
    for (qint32 j = 0; j < keys.size(); j++){
        QString field = keys.at(j);
        QVariant value = valuesToSet.value(field);
        if (!rawdata.setQCValue(studyType,field,value)){
            error = "Setting " + field + " in qc: " + rawdata.getError();
            return false;
        }
    }

    // Saving the info to HD:
    if (!rawdata.saveJSONFile(originalFileName,true)){
        error = "Could not save " + originalFileName + " after QC appending. Reason: " + rawdata.getError();
        return false;
    }

    // Finally we save the data for graphs.
    QVariantMap studyQC;
    QVariantMap temp;

    temp.clear();
    temp[GRAPH_DATA] = indexOfInformationCompletionPerTrial;
    temp[UPPER_REF_DATA] = refIndexOfInformationCompletion;
    temp[LOWER_REF_DATA] = refIndexOfInformationCompletion;
    temp[QC_INDEX] = qc_ici_index;
    temp[QC_OK] = qc_ok_ici;
    temp[QC_THRESHOLD] = threshold_ici;
    studyQC.insert(VMDC::QCFields::ICI,temp);

    qualityControlData.insert(studyType,studyQC);
    return true;


}
