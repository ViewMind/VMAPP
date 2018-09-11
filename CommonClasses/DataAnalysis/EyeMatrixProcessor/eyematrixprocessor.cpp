#include "eyematrixprocessor.h"

EyeMatrixProcessor::EyeMatrixProcessor(quint8 eye)
{

    // Since eye both cannto be a final value it is used to identify that the data has not been processed.
    eyeForBC = EYE_BOTH;
    eyeForUC = EYE_BOTH;

    eyeForResults = eye;
    if (eye == EYE_L){
        eyeForResults = EYE_L;
    }
    else if (eye == EYE_R){
        eyeForResults = EYE_R;
    }
    else{
        eyeForResults = EYE_BOTH;
    }
}

void EyeMatrixProcessor::setSelectedEyeVariables(const DataSet &set){

    if (eyeForResults == EYE_L) selectedEye = EYE_L;
    else if (eyeForResults == EYE_R) selectedEye = EYE_R;
    else selectedEye = set.getEyeWithMostFixations();

    if (selectedEye == EYE_L) selectedEyeStr = "Left";
    else selectedEyeStr = "Right";
}

QString EyeMatrixProcessor::processReading(const QString &csvFile, DBHash *dbdata){

    loadReadingCSV(csvFile);
    if (!error.isEmpty()) return "";

    csvReading.roundColumnValues(CSV_READING_WORD_INDEX_COL);
    csvReading.removeFirstNDifferent(READING_N_TO_FILTER_FROM_START,CSV_READING_SENTENCE_COL);
    csvReading.removeAllWithValueZero(CSV_READING_WORD_INDEX_COL);
    csvReading.filterRowsWithColOutsideRange(READING_MIN_VALID_FIXATION,READING_MAX_VALID_FIXATION,CSV_READING_FIXATION_LENGHT_COL);

    setSelectedEyeVariables(csvReading);

    DataSet::ProcessingResults r = csvReading.getProcessingResults(EXP_READING,EYE_R);
    DataSet::ProcessingResults l = csvReading.getProcessingResults(EXP_READING,EYE_L);

    // Generating the report.

    QString report = "<br>READING RESULTS (Eye: Left):<br>";
    qreal total = l.value(STAT_ID_TOTAL_FIXATIONS);
    qreal first = l.value(STAT_ID_FIRST_STEP_FIXATIONS);
    qreal multi = l.value(STAT_ID_MULTIPLE_FIXATIONS);
    qreal single = l.value(STAT_ID_SINGLE_FIXATIONS);

    // Saving the DB Data
    if (dbdata != nullptr){
        dbdata->insert(TEYERES_COL_RDFSTEPFIXL,first);
        dbdata->insert(TEYERES_COL_RDMSTEPFIXL,multi);
        dbdata->insert(TEYERES_COL_RDSSTEPFIXL,single);
        dbdata->insert(TEYERES_COL_RDTOTALFIXL,total);
    }

    // Report text for Left Eye
    report = report + "Total number of fixations: " + QString::number(total)  + "<br>";
    report = report + "First Step Fixations: "  + QString::number(first) + " (" + QString::number(100.0*first/total)  + "%)<br>";
    report = report + "Multiple Step Fixations: "  + QString::number(multi) + " (" + QString::number(100.0*multi/total)  + "%)<br>";
    report = report + "Single Fixations: " + QString::number(single) + " (" + QString::number(100.0*single/total)  + "%)<br>";

    report = report + "<br>READING RESULTS (Eye: Right):<br>";
    total  = r.value(STAT_ID_TOTAL_FIXATIONS);
    first  = r.value(STAT_ID_FIRST_STEP_FIXATIONS);
    multi  = r.value(STAT_ID_MULTIPLE_FIXATIONS);
    single = r.value(STAT_ID_SINGLE_FIXATIONS);

    if (dbdata != nullptr){
        dbdata->insert(TEYERES_COL_RDFSTEPFIXR,first);
        dbdata->insert(TEYERES_COL_RDMSTEPFIXR,multi);
        dbdata->insert(TEYERES_COL_RDSSTEPFIXR,single);
        dbdata->insert(TEYERES_COL_RDTOTALFIXR,total);
    }

    // Report text for Right Eye
    report = report + "Total number of fixations: " + QString::number(total)  + "<br>";
    report = report + "First Step Fixations: "  + QString::number(first) + " (" + QString::number(100.0*first/total)  + "%)<br>";
    report = report + "Multiple Step Fixations: "  + QString::number(multi) + " (" + QString::number(100.0*multi/total)  + "%)<br>";
    report = report + "Single Fixations: " + QString::number(single) + " (" + QString::number(100.0*single/total)  + "%)<br>";

    if (selectedEye == EYE_L){
        results.unite(l);
        report = report + "<br>REPORT uses value from <b>LEFT</b> eye<br>" ;
    }
    else{
        results.unite(r);
        report = report + "<br>REPORT uses value from <b>RIGHT</b> eye<br>" ;
    }

    return report;

}

QString EyeMatrixProcessor::processBinding(const QString &csvFile, bool bound, DBHash *dbdata){

    loadBindingCSV(csvFile,bound);
    if (!error.isEmpty()) return "";

    DataSet::ProcessingResults r;
    DataSet::ProcessingResults l;

    if (bound){
        r = csvBindingBC.getProcessingResults(EXP_BINDING_BC,EYE_R);
        l = csvBindingBC.getProcessingResults(EXP_BINDING_BC,EYE_L);
        setSelectedEyeVariables(csvBindingBC);
        eyeForBC = selectedEye;
    }
    else{
        r = csvBindingUC.getProcessingResults(EXP_BINDING_UC,EYE_R);
        l = csvBindingUC.getProcessingResults(EXP_BINDING_UC,EYE_L);
        setSelectedEyeVariables(csvBindingUC);
        eyeForUC = selectedEye;
    }

    // Generating the report.
    QString report = "RESULTS (Selcted/Best Eye " + selectedEyeStr + "):<br>";

    if (bound) report = "BINDING BC. " + report;
    else report = "BINDING UC " + report;

    // Report text
    if (bound){

        report = report + "Left EyeAverage Pupil Size excluding Trial 0 : " + QString::number(l.value(STAT_ID_BC_PUPIL_L)) + "<br>";
        report = report + "Right Eye Average Pupil Size excluding Trial 0 : " + QString::number(r.value(STAT_ID_BC_PUPIL_R)) + "<br>";

        results[STAT_ID_BC_PUPIL_R] = r.value(STAT_ID_BC_PUPIL_R);
        results[STAT_ID_BC_PUPIL_L] = l.value(STAT_ID_BC_PUPIL_L);

        if (dbdata != nullptr){
            dbdata->insert(TEYERES_COL_BCAVGPUPILL,l.value(STAT_ID_BC_PUPIL_L));
            dbdata->insert(TEYERES_COL_BCAVGPUPILR,r.value(STAT_ID_BC_PUPIL_R));
        }

    }
    else{
        report = report + "Left Eye Average Pupil Size excluding Trial 0 : " + QString::number(l.value(STAT_ID_UC_PUPIL_L)) + "<br>";
        report = report + "Right Eye Average Pupil Size excluding Trial 0 : " + QString::number(r.value(STAT_ID_UC_PUPIL_R)) + "<br>";

        results[STAT_ID_UC_PUPIL_R] = r.value(STAT_ID_UC_PUPIL_R);
        results[STAT_ID_UC_PUPIL_L] = l.value(STAT_ID_UC_PUPIL_L);

        if (dbdata != nullptr){
            dbdata->insert(TEYERES_COL_UCAVGPUPILL,l.value(STAT_ID_UC_PUPIL_L));
            dbdata->insert(TEYERES_COL_UCAVGPUPILR,r.value(STAT_ID_UC_PUPIL_R));
        }

    }

    // The encodig memory coefficient can be calculated only when both UC and BC studies have been processed.
    if ((eyeForBC != EYE_BOTH) && (eyeForUC != EYE_BOTH)){

        qreal differenceR = results.value(STAT_ID_BC_PUPIL_R) - results.value(STAT_ID_UC_PUPIL_R);
        report = report + "<br>Encoding memory process R: " + QString::number(differenceR);

        qreal differenceL = results.value(STAT_ID_BC_PUPIL_L) - results.value(STAT_ID_UC_PUPIL_L);
        report = report + "<br>Encoding memory process L: " + QString::number(differenceL);

        QString eyeused = "";

        if (eyeForBC == eyeForUC){
            if (eyeForBC == EYE_R){
                results[STAT_ID_ENCODING_MEM_VALUE] = differenceR;
                eyeused = "RIGHT";
            }
            else{
                results[STAT_ID_ENCODING_MEM_VALUE] = differenceL;
                eyeused = "LEFT";
            }
        }
        else{
            // The selected eye for each of the experiment is differente so the right one is used as default
            results[STAT_ID_ENCODING_MEM_VALUE] = differenceR;
            eyeused = "Right";
        }

        report = report + "<br><br> REPORT uses value from the <b>" + eyeused + "</b> eye <br>";

    }

    return report;

}

QString EyeMatrixProcessor::processFielding(const QString &csvFile, qint32 numberOfTrials, DBHash *dbdata){

    loadFieldingCSV(csvFile);
    if (!error.isEmpty()) return "";

    DataSet::ProcessingResults resL = csvFielding.getProcessingResults(EXP_FIELDNG,EYE_L);
    DataSet::ProcessingResults resR = csvFielding.getProcessingResults(EXP_FIELDNG,EYE_R);

    QString report = "RESULTS FIELDING:<br>";

    qreal hits = resL.value(STAT_ID_FIELDING_HIT_COUNT);
    if (dbdata != nullptr){
        dbdata->insert(TEYERES_COL_FDHITL,hits);
    }

    report = report + "Number of hits L: " + QString::number(hits) + " (" + QString::number(hits*100.0/numberOfTrials) + "%)" + "<br>";
    report = report + "Possible Hits L: " + QString::number(numberOfTrials) + "<br>";

    hits = resR.value(STAT_ID_FIELDING_HIT_COUNT);
    if (dbdata != nullptr){
        dbdata->insert(TEYERES_COL_FDHITR,hits);
        dbdata->insert(TEYERES_COL_FDHITR,hits);
    }

    report = report + "Number of hits R: " + QString::number(hits) + " (" + QString::number(hits*100.0/numberOfTrials) + "%)" + "<br>";
    report = report + "Possible Hits R: " + QString::number(numberOfTrials) + "<br>";
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

    // The gaze column
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The nf column
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The Fixation X Position
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The Fixation Y Position
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The side in pixels of the targets
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The X coordinates of Target 1
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The Y coordinates of Target 1
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The X coordinates of Target 2
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The Y coordinates of Target 2
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The X coordinates of Target 3
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
    cclist << cc;

    // The Y coordinates of Target 3
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
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

    // The number of blinks
    cc.clear();
    cc.type = CSVCheckedReader::CT_INT;
    cclist << cc;

    // The amplitude of the sacade
    cc.clear();
    cc.type = CSVCheckedReader::CT_REAL;
    cclist << cc;

    // The gaze
    cc.clear();
    cc.type = CSVCheckedReader::CT_REAL;
    cclist << cc;

    // The number of fixations
    cc.clear();
    cc.type = CSVCheckedReader::CT_INT;
    cclist << cc;

    // The screen Y of the fixation
    cc.clear();
    cc.type = CSVCheckedReader::CT_IGNORE;
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
