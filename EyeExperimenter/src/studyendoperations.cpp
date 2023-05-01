#include "studyendoperations.h"

StudyEndOperations::StudyEndOperations() {

}

bool StudyEndOperations::wasOperationSuccssful() const {
    return operationsSuccess;
}

void StudyEndOperations::processFile(const QString &file, const QString &idx_file){
    dataFile = file;
    idxFile = file;
    operationsSuccess = false;
    this->start();
}

void StudyEndOperations::run(){

    // First we load the file and make sure it's ok.
    if (!vmdc.loadFromJSONFile(dataFile)){
        StaticThreadLogger::error("StudyEndOperations::run","Could not load the raw data file: '" + dataFile + "'. Reason: " + vmdc.getError());
        return;
    }

    // Now we need to know how to compute the QC Index for each study in the file. All studies need to approve individually.
    // If any of them fail then the QC Index should be set to bad.

    operationsSuccess = true;


}


void StudyEndOperations::computeBindingIndex(const QString &which_binding) const {

    qreal timeWithNoDataPerTrial = TIME_FINISH + TIME_START_CROSS + TIME_WHITE_TRANSITION;
    QVariantList trials = vmdc.getStudyTrialList(which_binding);
    qreal studyDuration =


}
