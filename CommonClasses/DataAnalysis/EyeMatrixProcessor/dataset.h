#ifndef DATASET_H
#define DATASET_H

/*********************************************
 * All experiments are represented in one way
 * or another as trials. The data set is the
 * generic representation of that trials info
 * ********************************************/

#include "../../CommonClasses/common.h"
#include "csvcheckedreader.h"
#include <QtMath>

// Defines for the indexes of columns in the Reading CSV files
#define   CSV_READING_SUBJECT_COL                       0
#define   CSV_READING_SENTENCE_COL                      2
#define   CSV_READING_WORD_INDEX_COL                    6
#define   CSV_READING_FIXATION_LENGHT_COL               8
#define   CSV_READING_EYE_ID_COL                        10
#define   CSV_READING_SIZE_CHECK                        12

// Defines for the indexes of columns in the Binding CSV files
#define   CSV_BINDING_SUBJECT_COL                       0
#define   CSV_BINDING_TRIALID_COL                       1
#define   CSV_BINDING_ISTRIAL_COL                       2
#define   CSV_BINDING_TRIALNAME_COL                     3
#define   CSV_BINDING_FIXATION_LENGHT_COL               6
#define   CSV_BINDING_EYE_ID_COL                        7
#define   CSV_BINDING_TRIALTYPE_COL                     4
#define   CSV_BINDING_TRIALRESPONSE_COL                 5
#define   CSV_BINDING_SACADE_COL                        9
#define   CSV_BINDING_PUPIL_COL                         10
#define   CSV_BINDING_SIZE_CHECK                        11

// Defines for the indexes of columns in the Fielding CSV files
#define   CSV_FIELDING_SUBJECT_COL                      0
#define   CSV_FIELDING_TRIALID_COL                      1
#define   CSV_FIELDING_IMGNUMBER_COL                    2
#define   CSV_FIELDING_TARGET_HIT_COL                   3
#define   CSV_FIELDING_EYE_ID_COL                       5
#define   CSV_FIELDING_FIX_LENGTH_COL                   4
#define   CSV_FIELDING_TRIALRESPONSE_COL                5
#define   CSV_FIELDING_LAT_SAC_COL                      6
#define   CSV_FIELDING_ROW_SIZE                         7

// Defines that identify values in hash map and how to replace the string in the generated report
#define   STAT_ID_TOTAL_FIXATIONS                       0
#define   STAT_ID_FIRST_STEP_FIXATIONS                  1
#define   STAT_ID_MULTIPLE_FIXATIONS                    2
#define   STAT_ID_SINGLE_FIXATIONS                      3
#define   STAT_ID_UC_CORRECT                            4
#define   STAT_ID_UC_WRONG                              5
#define   STAT_ID_BC_WRONG                              6
#define   STAT_ID_BC_CORRECT                            7
#define   STAT_ID_BC_PUPIL_R                            8
#define   STAT_ID_UC_PUPIL_R                            9
#define   STAT_TEXT_DOCTOR                              12
#define   STAT_TEXT_DATE                                13
#define   STAT_TEXT_PATIENT                             14
#define   STAT_TEXT_OBSERVATIONS                        15
#define   STAT_TEXT_OUTPUTFILENAME                      16
#define   STAT_ID_FIELDING_HIT_COUNT                    17
#define   STAT_ID_FIELDING_HIT_POSSIBLE                 18
#define   STAT_ID_ENCODING_MEM_VALUE                    19
#define   STAT_ID_BC_PUPIL_L                            20
#define   STAT_ID_UC_PUPIL_L                            21

class DataSet
{
public:

    typedef QHash<qint32,qreal> ProcessingResults;

    DataSet();    
    void setLeftAndRightData(const CSVData &all, qint32 eyeCol);
    bool isEmpty() const {return (eyes.first().isEmpty() || eyes.last().isEmpty());}

    // Filtering functions functions
    void removeFirstNDifferent(qint32 N, qint32 idCol);
    void removeAllWithValueZero(qint32 idCol);
    void roundColumnValues(qint32 idCol);
    void filterRowsWithColOutsideRange(qreal min, qreal max, qint32 idCol);

    // Required for selecting which eye to use when Both Eyes were selected.
    quint8 getEyeWithMostFixations() const;

    // Processing functions.
    ProcessingResults getProcessingResults(qint32 experiment, qint32 eyeToUse);

private:

    // First is Left Eye and Second is Right eye.
    QList<CSVData> eyes;

    ProcessingResults getReadingResultsFor(qint32 eyeID) const;
    ProcessingResults getBindingResultsFor(qint32 eyeID, bool bound) const;
    ProcessingResults getFieldingResultsFor(qint32 eyeID) const;

};

#endif // DATASET_H
