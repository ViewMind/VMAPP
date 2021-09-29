#include "qualitycontrol.h"

const char * QualityControl::GRAPH_DATA       = "GraphData";
const char * QualityControl::UPPER_REF_DATA   = "UpperREFData";
const char * QualityControl::LOWER_REF_DATA   = "LowerREFData";
const char * QualityControl::QC_INDEX         = "QCIndex";
const char * QualityControl::QC_OK            = "QCOk";

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

    if (!rawdata.loadFromJSONFile(originalFileName)){
        error = "Could not load the raw data file: " + originalFileName + ". Reason: " + rawdata.getError();
        return;
    }

    if (DBUGBOOL(Debug::Options::FIX_QC_SHA_CHECKS)){
        QString dbug = "DBUG FORCE FIX QC SHA CHECKS";
        qDebug() << dbug;
        LogInterface logger;
        logger.appendWarning(dbug);
        rawdata.setChecksumHash();
    }

    if (!rawdata.verifyChecksumHash()){
        error = "Checksum verification changed in the raw data file";
        return;
    }

    // This allows to recompute QC factors if, online, those factors were updated.
    rawdata.setQCParameters(updatedQCParameters);

    availableStudies = rawdata.getStudies();
    qualityControlData.clear();

    for (qint32 i = 0; i < availableStudies.size(); i++){
        QString study = availableStudies.at(i);
        QString metaStudy = VMDC::MultiPartStudyBaseName::getMetaStudy(study);
        if (!computeQualityControlVectors(study,metaStudy)) return;
    }

    return;
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

QStringList QualityControl::getStudyList() const {
    return availableStudies;
}


QString QualityControl::getError() const {
    return error;
}

QString QualityControl::getSetFileName() const {
    return originalFileName;
}


bool QualityControl::computeQualityControlVectors(const QString &studyType, const QString &metaStudyName){

    //qDebug() << "Quality control for" << studyType << metaStudyName;

    QVariantList trialList = rawdata.getStudyTrialList(studyType);
    QVariantMap studyConfiguration = rawdata.getStudyConfiguration(studyType);
    QString validEye = studyConfiguration.value(VMDC::StudyParameter::VALID_EYE).toString();

    QVariantMap qc = rawdata.getQCParameters();
    QVariantMap pp = rawdata.getProcessingParameters();
    qreal sample_freq = pp.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();

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

    qsizetype    ndatasets = 0;

    qreal number_of_trials_with_too_few_points = 0;
    qreal number_of_trials_with_too_few_fixations = 0;
    qreal number_of_trials_with_bad_avg_freq = 0;

    qreal qc_fix_index = 0;
    qreal qc_dpoint_index = 0;
    qreal qc_freq_index = 0;

    qreal number_of_trials = static_cast<qreal>(trialList.size());

    for (qint32 i = 0; i < trialList.size(); i++){

        QVariantMap trial = trialList.at(i).toMap();
        QVariantMap data = trial.value(VMDC::TrialField::DATA).toMap();
        QStringList dataset_names = data.keys();
        qsizetype pointsInTrial = 0;
        qsizetype fixInTrial = 0;
        qint32 glitchesInTrial = 0;
        qreal  periodAcc = 0;
        qreal  timestamp = -1;

        ndatasets = dataset_names.size();

        for (qint32 j = 0; j < dataset_names.size(); j++){

            QVariantMap dataSet = data.value(dataset_names.at(j)).toMap();
            QVariantList rawData = dataSet.value(VMDC::DataSetField::RAW_DATA).toList();
            QVariantList fixR = dataSet.value(VMDC::DataSetField::FIXATION_R).toList();
            QVariantList fixL = dataSet.value(VMDC::DataSetField::FIXATION_L).toList();

            // The easy stuff first: The number of data points and fixatons. The fixations shown will be those for the eye used in processing.
            pointsInTrial = pointsInTrial + rawData.size();
            if (validEye == VMDC::Eye::LEFT){
                fixInTrial = fixInTrial + fixL.size();
            }
            else {
                fixInTrial = fixInTrial + fixR.size();
            }

            // The actual frequency analysis.
            if (rawData.size() > 2){
                timestamp = rawData.at(0).toMap().value(VMDC::DataVectorField::TIMESTAMP).toReal();

                for (qint32 k = 1; k < rawData.size(); k++){
                    qreal ts = rawData.at(k).toMap().value(VMDC::DataVectorField::TIMESTAMP).toReal();
                    qreal diff = ts - timestamp;
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

    // Boolean flags for the OK status.
    bool qc_ok_fix = (qc_fix_index >= threshold_fix);
    bool qc_ok_f   = (qc_freq_index >= threshold_f);
    bool qc_ok_points = (qc_dpoint_index >= threshold_points);

    QVariantMap valuesToSet;
    valuesToSet[VMDC::QCFields::N_DATASETS] = ndatasets;
    valuesToSet[VMDC::QCFields::QC_FIX_INDEX] = qc_fix_index;
    valuesToSet[VMDC::QCFields::QC_FREQ_INDEX] = qc_freq_index;
    valuesToSet[VMDC::QCFields::QC_POINT_INDEX] = qc_dpoint_index;
    valuesToSet[VMDC::QCFields::QC_FIX_INDEX_OK] = qc_ok_fix;
    valuesToSet[VMDC::QCFields::QC_FREQ_INDEX_OK] = qc_ok_f;
    valuesToSet[VMDC::QCFields::QC_POINT_INDEX_OK] = qc_ok_points;
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
    studyQC.insert(VMDC::QCFields::POINTS,temp);

    temp[GRAPH_DATA] = fixationsPerTrial;
    temp[UPPER_REF_DATA] = refNFixations;
    temp[LOWER_REF_DATA] = refNFixations;
    temp[QC_INDEX] = qc_fix_index;
    temp[QC_OK] = qc_ok_fix;
    studyQC.insert(VMDC::QCFields::FIXATIONS,temp);

    temp.clear();
    temp[GRAPH_DATA] = avgFreqPerTrial;
    temp[UPPER_REF_DATA] = upperRefFreq;
    temp[LOWER_REF_DATA] = lowerRefFreq;
    temp[QC_INDEX] = qc_freq_index;
    temp[QC_OK] = qc_ok_f;
    studyQC.insert(VMDC::QCFields::AVG_FREQ,temp);

    qualityControlData.insert(studyType,studyQC);
    return true;

}
