#include "eyematrixprocessor.h"

EyeMatrixProcessor::EyeMatrixProcessor()
{
    bindingEye = -1;
}

QString EyeMatrixProcessor::processReading(const QString &csvFile){

    loadReadingCSV(csvFile);
    if (!error.isEmpty()) return "";

    csvReading.roundColumnValues(CSV_READING_WORD_INDEX_COL);
    csvReading.removeFirstNDifferent(READING_N_TO_FILTER_FROM_START,CSV_READING_SENTENCE_COL);
    csvReading.removeAllWithValueZero(CSV_READING_WORD_INDEX_COL);
    csvReading.filterRowsWithColOutsideRange(READING_MIN_VALID_FIXATION,READING_MAX_VALID_FIXATION,CSV_READING_FIXATION_LENGHT_COL);
    int eye = -1;
    results.unite(csvReading.getProcessingResults(EXP_READING,&eye));

    // Generating the report.
    QString report = "READING RESULTS:<br>";

    qreal total = results.value(STAT_ID_TOTAL_FIXATIONS);
    qreal first = results.value(STAT_ID_FIRST_STEP_FIXATIONS);
    qreal multi = results.value(STAT_ID_MULTIPLE_FIXATIONS);
    qreal single = results.value(STAT_ID_SINGLE_FIXATIONS);

    // Report text
    report = report + "Total number of fixations: " + QString::number(total)  + "<br>";
    report = report + "First Step Fixations: "  + QString::number(first) + " (" + QString::number(100.0*first/total)  + "%)<br>";
    report = report + "Multiple Step Fixations: "  + QString::number(multi) + " (" + QString::number(100.0*multi/total)  + "%)<br>";
    report = report + "Single Fixations: " + QString::number(single) + " (" + QString::number(100.0*single/total)  + "%)<br>";

    return report;

}

QString EyeMatrixProcessor::processBinding(const QString &csvFile, bool bound){

    loadBindingCSV(csvFile,bound);
    if (!error.isEmpty()) return "";

    int eye = -1;
    if (bindingEye != -1) eye = bindingEye;

    DataSet::ProcessingResults r;
    DataSet::ProcessingResults l;

    if (bound){
        r = csvBindingBC.getProcessingResults(EXP_BINDING_BC,&eye,EYE_R);
        l = csvBindingBC.getProcessingResults(EXP_BINDING_BC,&eye,EYE_L);
        //results.unite(csvBindingBC.getProcessingResults(EXP_BINDING_BC,&eye));
    }
    else{
        //results.unite(csvBindingUC.getProcessingResults(EXP_BINDING_UC,&eye));
        r = csvBindingUC.getProcessingResults(EXP_BINDING_UC,&eye,EYE_R);
        l = csvBindingUC.getProcessingResults(EXP_BINDING_UC,&eye,EYE_L);
    }

    // Generating the report.
    QString report = "RESULTS:<br>";

    if (bound) report = "BINDING BC " + report;
    else report = "BINDING UC " + report;

    // Report text
    if (bound){

        report = report + "Eye: LEFT<br>";
        report = report + "Number of correct answers : " + QString::number(l.value(STAT_ID_BC_CORRECT)) + "<br>";
        report = report + "Number of wrong answers : " + QString::number(l.value(STAT_ID_BC_WRONG)) + "<br>";
        report = report + "Average Pupil Size excluding Trial 0 : " + QString::number(l.value(STAT_ID_BC_PUPIL_L)) + "<br>";

        report = report + "Eye: RIGHT<br>";
        report = report + "Number of correct answers : " + QString::number(r.value(STAT_ID_BC_CORRECT)) + "<br>";
        report = report + "Number of wrong answers : " + QString::number(r.value(STAT_ID_BC_WRONG)) + "<br>";
        report = report + "Average Pupil Size excluding Trial 0 : " + QString::number(r.value(STAT_ID_BC_PUPIL_R)) + "<br>";

        results[STAT_ID_BC_PUPIL_L] = l.value(STAT_ID_BC_PUPIL_L);

    }
    else{
        report = report + "Eye: LEFT<br>";
        report = report + "Number of correct answers : " + QString::number(l.value(STAT_ID_UC_CORRECT)) + "<br>";
        report = report + "Number of wrong answers : " + QString::number(l.value(STAT_ID_UC_WRONG)) + "<br>";
        report = report + "Average Pupil Size excluding Trial 0 : " + QString::number(l.value(STAT_ID_UC_PUPIL_L)) + "<br>";

        report = report + "Eye: RIGHT<br>";
        report = report + "Number of correct answers : " + QString::number(r.value(STAT_ID_UC_CORRECT)) + "<br>";
        report = report + "Number of wrong answers : " + QString::number(r.value(STAT_ID_UC_WRONG)) + "<br>";
        report = report + "Average Pupil Size excluding Trial 0 : " + QString::number(r.value(STAT_ID_UC_PUPIL_R)) + "<br>";

        results[STAT_ID_UC_PUPIL_L] = l.value(STAT_ID_UC_PUPIL_L);
    }

    // Using the right eye, always for the final results.
    results.unite(r);


    // If bindingEye is not -1 then this is the Second Binding experiment and the encodig memory coefficient can be calculated.
    if (bindingEye != -1){
        qreal difference = results.value(STAT_ID_BC_PUPIL_R) - results.value(STAT_ID_UC_PUPIL_R);
        report = report + "<br>Encoding memory process R: " + QString::number(difference);
        results[STAT_ID_ENCODING_MEM_VALUE] = difference;
        difference = results.value(STAT_ID_BC_PUPIL_L) - results.value(STAT_ID_UC_PUPIL_L);
        report = report + "<br>Encoding memory process L: " + QString::number(difference);
    }
    else bindingEye = eye;

    return report;

}

QString EyeMatrixProcessor::processFielding(const QString &csvFile){

    loadFieldingCSV(csvFile);
    if (!error.isEmpty()) return "";
    int eye = -1;

    DataSet::ProcessingResults resL = csvFielding.getProcessingResults(EXP_FIELDNG,&eye,EYE_L);
    DataSet::ProcessingResults resR = csvFielding.getProcessingResults(EXP_FIELDNG,&eye,EYE_R);

    QString report = "RESULTS FIELDING:<br>";

    qreal hits = resL.value(STAT_ID_FIELDING_HIT_COUNT);
    qreal total = resL.value(STAT_ID_FIELDING_HIT_POSSIBLE);
    report = report + "Number of hits L: " + QString::number(hits) + " (" + QString::number(hits*100.0/total) + "%)" + "<br>";
    report = report + "Possible Hits L: " + QString::number(total) + "<br>";

    hits = resR.value(STAT_ID_FIELDING_HIT_COUNT);
    total = resR.value(STAT_ID_FIELDING_HIT_POSSIBLE);
    report = report + "Number of hits R: " + QString::number(hits) + " (" + QString::number(hits*100.0/total) + "%)" + "<br>";
    report = report + "Possible Hits R: " + QString::number(total) + "<br>";
    return report;

}

void EyeMatrixProcessor::loadBindingCSV(const QString &filename, bool boundColors){

    CSVCheckedReader::ColumnChecks cclist;
    CSVCheckedReader::ColumnChecker cc;

    // The subject name
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The trial id
    cc.clear();
    cc.type = CSVCheckedReader::CT_INT;
    cclist << cc;

    // The isTrial
    cc.clear();
    cc.type = CSVCheckedReader::CT_BOOL;
    cclist << cc;

    // The trial name
    cc.clear();
    cc.type = CSVCheckedReader::CT_STRING;
    cclist << cc;

    // The trial type
    cc.clear();
    cc.type = CSVCheckedReader::CT_STRING_SET;
    cc.set << "N/A" << "different" << "same";
    cclist << cc;

    // The response
    cc.clear();
    cc.type = CSVCheckedReader::CT_STRING_SET;
    cc.set << "N/A" << "D" << "S";
    cclist << cc;

    // The duration of fixation
    cc.clear();
    cc.type = CSVCheckedReader::CT_REAL;
    cclist << cc;

    // The eye ID
    cc.clear();
    cc.type = CSVCheckedReader::CT_BOOL;
    cclist << cc;

    // The blink
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The sacade amplitude
    cc.clear();
    cc.type = CSVCheckedReader::CT_REAL;
    cclist << cc;

    // The pupil size
    cc.clear();
    cc.type = CSVCheckedReader::CT_REAL;
    cclist << cc;

    if (boundColors) csvBindingBC = loadCSV(filename,cclist,CSV_BINDING_EYE_ID_COL);
    else csvBindingUC = loadCSV(filename,cclist,CSV_BINDING_EYE_ID_COL);

}

void EyeMatrixProcessor::loadReadingCSV(const QString &filename){

    CSVCheckedReader::ColumnChecks cclist;
    CSVCheckedReader::ColumnChecker cc;

    // The subject name
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The subject number
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The sentence number
    cc.clear();
    cc.type = CSVCheckedReader::CT_INT;
    cclist << cc;

    // The trial number
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The fixation number
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The screen x of the fixation
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The word number
    cc.clear();
    cc.type = CSVCheckedReader::CT_REAL;
    cclist << cc;

    // The char number
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The fixation length
    cc.clear();
    cc.type = CSVCheckedReader::CT_REAL;
    cclist << cc;

    // The number of words in sentence
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The eye ID
    cc.clear();
    cc.type = CSVCheckedReader::CT_BOOL;
    cclist << cc;

    // The pupil size
    cc.clear();
    cc.type = CSVCheckedReader::CT_REAL;
    cclist << cc;

    csvReading = loadCSV(filename,cclist,CSV_READING_EYE_ID_COL);

}

void EyeMatrixProcessor::loadFieldingCSV(const QString &filename){

    CSVCheckedReader::ColumnChecks cclist;
    CSVCheckedReader::ColumnChecker cc;

    // The subject name
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The trial number number
    cc.clear();
    cc.type = CSVCheckedReader::CT_INT;
    cclist << cc;

    // The image number
    cc.clear();
    cc.type = CSVCheckedReader::CT_INT;
    cclist << cc;

    // The target hit
    cc.clear();
    cc.type = CSVCheckedReader::CT_BOOL;
    cclist << cc;

    // The fixation length
    cc.clear();
    cc.type = CSVCheckedReader::CT_REAL;
    cclist << cc;

    // The eye ID
    cc.clear();
    cc.type = CSVCheckedReader::CT_BOOL;
    cclist << cc;

    // The sacadic latency
    cc.clear();
    cc.type = CSVCheckedReader::CT_REAL;
    cclist << cc;

    csvFielding = loadCSV(filename,cclist,CSV_FIELDING_EYE_ID_COL);

}

DataSet EyeMatrixProcessor::loadCSV(const QString &filename, CSVCheckedReader::ColumnChecks cc, qint32 eyeColumn, bool hasHeader){

    // First step is to load the matrix and make sure there is no error.
    CSVCheckedReader reader;
    error = "";
    CSVData data = reader.loadCSV(filename,cc,hasHeader);

    // Checking for errors
    if (!reader.getError().isEmpty()){
        error = "Loading CSV File: " + filename + ", found error: " + reader.getError();
        return DataSet();
    }

    // Loading the data into sets.
    DataSet dset;
    dset.setLeftAndRightData(data,eyeColumn);
    return dset;

}
