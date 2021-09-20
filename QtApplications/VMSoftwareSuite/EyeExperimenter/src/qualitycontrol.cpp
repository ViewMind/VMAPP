#include "qualitycontrol.h"

const char * QualityControl::GRAPH_DATA = "GraphData";
const char * QualityControl::REF_DATA   = "REFData";

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

    //rawdata.setChecksumHash();

    if (!rawdata.verifyChecksumHash()){
        error = "Checksum verification changed in the raw data file";
        return;
    }

    availableStudies = rawdata.getStudies();
    qualityControlData.clear();

    for (qint32 i = 0; i < availableStudies.size(); i++){
        QString study = availableStudies.at(i);
        QString metaStudy = VMDC::MultiPartStudyBaseName::getMetaStudy(study);
        if (!computeQualityControlVectors(study,metaStudy)) return;
    }

    return;
}

void QualityControl::setVMContainterFile(const QString &file){
    originalFileName = file;
    this->start();
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

    QVariantMap qc = rawdata.getQCParameters();
    QVariantMap pp = rawdata.getProcessingParameters();
    qreal sample_freq = pp.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();

    qreal max_diff = qc.value(VMDC::QCGlobalParameters::MAX_TIMESTAMP_DIFF).toReal();
    qreal min_diff = qc.value(VMDC::QCGlobalParameters::MIN_TIMESTAMP_DIFF).toReal();
    qreal glitch_percent = qc.value(VMDC::QCGlobalParameters::MAX_GLITCHES).toReal()/100;

    QVariantMap qcStudy = qc.value(metaStudyName).toMap();
    //qDebug() << "QC Values for" << metaStudyName;
    //Debug::prettpPrintQVariantMap(qc);
    qreal min_points_per_trial = qcStudy.value(VMDC::QCStudyParameters::MIN_POINTS_PER_TRIAL).toReal();
    qreal min_fix_per_trial    = qcStudy.value(VMDC::QCStudyParameters::MIN_FIXS_PER_TRIAL).toReal();

    //qDebug() << "min points per trial" << min_points_per_trial;

    QVariantList pointsPerTrial;
    QVariantList refPointsPerTrial;
    QVariantList fixationsPerTrialL;
    QVariantList fixationsPerTrialR;
    QVariantList refNFixations;
    QVariantList glitchesPerTrial;
    QVariantList refGlitches;
    QVariantList avgFreqPerTrial;
    QVariantList refFreq;
    qint32 ndatasets;

    for (qint32 i = 0; i < trialList.size(); i++){

        QVariantMap trial = trialList.at(i).toMap();
        QVariantMap data = trial.value(VMDC::TrialField::DATA).toMap();
        QStringList dataset_names = data.keys();
        qint32 pointsInTrial = 0;
        qint32 fixRInTrial = 0;
        qint32 fixLInTrial = 0;
        qint32 glitchesInTrial = 0;
        qreal  periodAcc = 0;
        qreal  timestamp = -1;

        ndatasets = dataset_names.size();

        for (qint32 j = 0; j < dataset_names.size(); j++){

            QVariantMap dataSet = data.value(dataset_names.at(j)).toMap();
            QVariantList rawData = dataSet.value(VMDC::DataSetField::RAW_DATA).toList();
            QVariantList fixR = dataSet.value(VMDC::DataSetField::FIXATION_R).toList();
            QVariantList fixL = dataSet.value(VMDC::DataSetField::FIXATION_L).toList();

            // The easy stuff first: The number of data points and fixatons.
            pointsInTrial = pointsInTrial + rawData.size();
            fixRInTrial = fixRInTrial + fixR.size();
            fixLInTrial = fixLInTrial + fixL.size();

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
        //qDebug() << periodAcc;
        periodAcc = periodAcc / (pointsInTrial - dataset_names.size());
        //qDebug() << periodAcc;

        pointsPerTrial << pointsInTrial;
        refPointsPerTrial << min_points_per_trial;

        fixationsPerTrialL << fixLInTrial;
        fixationsPerTrialR << fixRInTrial;
        refNFixations << min_fix_per_trial;

        // The reference for glitches is a percet of the number of points in the trial.
        glitchesPerTrial << glitchesInTrial;
        refGlitches << glitch_percent*(pointsInTrial-dataset_names.size());

        avgFreqPerTrial << 1000.0/periodAcc; // The 1000 is because the period is in ms.
        //qDebug() << "Sample freq" << sample_freq;
        refFreq << sample_freq;
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

    if (!rawdata.setQCVector(studyType,VMDC::QCFields::FIXATIONS_L,fixationsPerTrialL)){
        error = "Setting fixations l qc: " + rawdata.getError();
        return false;
    }

    if (!rawdata.setQCVector(studyType,VMDC::QCFields::FIXATIONS_R,fixationsPerTrialR)){
        error = "Setting fixations r qc: " + rawdata.getError();
        return false;
    }

    if (!rawdata.setQCVector(studyType,VMDC::QCFields::GLITCHES,glitchesPerTrial)){
        error = "Setting glitches qc: " + rawdata.getError();
        return false;
    }

    QVariantList tempndata; tempndata << ndatasets;
    if (!rawdata.setQCVector(studyType,VMDC::QCFields::N_DATASETS,tempndata)){
        error = "Setting n datasets qc: " + rawdata.getError();
        return false;
    }

    // Saving the info to HD:
    if (!rawdata.saveJSONFile(originalFileName,ExperimentGlobals::PRETTY_PRINT_OUTPUT_FILES)){
        error = "Could not save " + originalFileName + " after QC appending. Reason: " + rawdata.getError();
    }

    // Finally we save the data for graphs.
    QVariantMap studyQC;
    QVariantMap temp;

    temp.clear();
    temp[GRAPH_DATA] = pointsPerTrial;
    temp[REF_DATA] = refPointsPerTrial;
    studyQC.insert(VMDC::QCFields::POINTS,temp);

    temp.clear();
    temp[GRAPH_DATA] = fixationsPerTrialL;
    temp[REF_DATA] = refNFixations;
    studyQC.insert(VMDC::QCFields::FIXATIONS_L,temp);

    temp.clear();
    temp[GRAPH_DATA] = fixationsPerTrialR;
    temp[REF_DATA] = refNFixations;
    studyQC.insert(VMDC::QCFields::FIXATIONS_R,temp);

    temp.clear();
    temp[GRAPH_DATA] = glitchesPerTrial;
    temp[REF_DATA] = refGlitches;
    studyQC.insert(VMDC::QCFields::GLITCHES,temp);

    temp.clear();
    temp[GRAPH_DATA] = avgFreqPerTrial;
    temp[REF_DATA] = refFreq;
    studyQC.insert(VMDC::QCFields::AVG_FREQ,temp);

    qualityControlData.insert(studyType,studyQC);
    return true;

}