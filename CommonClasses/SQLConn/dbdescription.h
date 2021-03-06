#ifndef DBDESCRIPTION_H
#define DBDESCRIPTION_H

#include <QStringList>
#include <QDebug>

// The structure where the DB data is saved.
struct DBData{
    QStringList columns;
    QString error;
    QList<QStringList> rows;
    void clear(){
        columns.clear();
        rows.clear();
        error = "";
    }
    QString joinRowsAndCols(const QString &rowSep, const QString &colSep){
        QStringList compactedrows;
        for (qint32 i = 0; i < rows.size(); i++){
            compactedrows << rows.at(i).join(colSep);
        }
        return compactedrows.join(rowSep);
    }
    void fillRowsFromString(const QString &data, const QString &rowSep, const QString &colSep){
        QStringList crows = data.split(rowSep);
        for (qint32 i= 0; i < crows.size(); i++){
                rows << crows.at(i).split(colSep);
        }
    }
    void debugColumns() const{
        qWarning() << "COLUMNS" << columns;
        for (int i = 0; i < rows.size(); i++){
            qWarning() << " ROW:" << i << rows.at(i) << "Row Size: " << rows.size();
        }
    }
};

inline static void printDBDataList(const QList<DBData> &dbdata){
    for (int i = 0; i < dbdata.size(); i++){
        qWarning() << " --> DATUM: " << i;
        dbdata.at(i).debugColumns();
    }
}

// Separators used for both the data packet and inside a list of the table.
#define  DB_LIST_IN_COL_SEP                         "<||>"
#define  DB_COLUMN_SEP                              "<|=|>"
#define  DB_TRANSACTION_LIST_SEP                    "<|==|>"
#define  DB_ROW_SEP                                 "<|===|>"

// Table names
#define  TABLE_DOCTORS                           "tDoctors"
#define  TABLE_EYE_RESULTS                       "tEyeResults"
#define  TABLE_INSTITUTION                       "tInstitutions"
#define  TABLE_PLACEDPRODUCTS                    "tPlacedProducts"
#define  TABLE_FDATA                             "tFData"
#define  TABLE_PERFORMANCE                       "tPerformance"
#define  TABLE_EVALUATIONS                       "tEvaluations"

#define  TABLE_PATDATA                           "tPatientData"
#define  TABLE_MEDICAL_RECORDS                   "tMedicalRecords"
#define  TABLE_PATIENTD_IDS                      "tPatientIDs"

// Column names for the patient ids table
#define  TPATID_COL_KEYID                         "keyid"
#define  TPATID_COL_UID                           "uid"

// Column names for patient info
#define  TPATDATA_COL_KEYID                       "keyid"
#define  TPATDATA_COL_DOCTORID                    "doctorid"
#define  TPATDATA_COL_FIRSTNAME                   "firstname"
#define  TPATDATA_COL_LASTNAME                    "lastname"
#define  TPATDATA_COL_SEX                         "sex"
#define  TPATDATA_COL_BIRTHCOUNTRY                "birthcountry"
#define  TPATDATA_COL_BIRTHDATE                   "birthdate"
#define  TPATDATA_COL_DATE_INS                    "date_insertion"
#define  TPATDATA_COL_PUID                        "puid"
#define  TPATDATA_COL_FORMATIVE_YEARS             "formative_years"
#define  TPATDATA_NONCOL_DISPLAYID                "displayID"
#define  TPATDATA_NONCOL_PROTOCOL                 "patient_protocol"

// Column names for the patient medical information
#define  TPATMEDREC_COL_KEYID                    "keyid"
#define  TPATMEDREC_COL_PUID                     "puid"
#define  TPATMEDREC_COL_REC_INDEX                "record_index"
#define  TPATMEDREC_COL_DATE                     "date"
#define  TPATMEDREC_COL_MEDICATION               "medication"
#define  TPATMEDREC_COL_RNM                      "RNM"
#define  TPATMEDREC_COL_EVALS                    "evaluations"
#define  TPATMEDREC_COL_FORM_YEARS               "formative_years"
#define  TPATMEDREC_COL_PRESUMP_DIAGNOSIS        "presumptive_diagnosis"

// Column names for the doctor table
#define  TDOCTOR_COL_KEYID                       "keyid"
#define  TDOCTOR_COL_UID                         "uid"
#define  TDOCTOR_COL_TYPEID                      "typeid"
#define  TDOCTOR_COL_FIRSTNAME                   "firstname"
#define  TDOCTOR_COL_LASTNAME                    "lastname"
#define  TDOCTOR_COL_COUNTRYID                   "countryid"
#define  TDOCTOR_COL_STATE                       "state"
#define  TDOCTOR_COL_CITY                        "city"
#define  TDOCTOR_COL_MEDICAL_INST                "medicalinstitution"
#define  TDOCTOR_COL_TELEPHONE                   "telephone"
#define  TDOCTOR_COL_EMAIL                       "email"
#define  TDOCTOR_COL_ADDRESS                     "address"

// Column names for the EyeResult tables.
#define  TEYERES_COL_KEYID                       "keyid"
#define  TEYERES_COL_PUID                        "puid"
#define  TEYERES_COL_DOCTORID                    "doctorid"
#define  TEYERES_COL_STUDY_ID                    "study_id"
#define  TEYERES_COL_STUDY_DATE                  "study_date"
#define  TEYERES_COL_PROTOCOL                    "protocol"
#define  TEYERES_COL_FERROR                      "ferror"
#define  TEYERES_COL_RESULTS                     "results"
#define  TEYERES_COL_EVALUATION_ID               "evaluation_id"
#define  TEYERES_COL_CLIENT_STUDY_DATE           "client_study_date"

#define  TEYERES_JSON_EXP_GROUP_FIELDING               "fielding"
#define  TEYERES_JSON_EXP_GROUP_NBACKRT                "nbackrt"
#define  TEYERES_JSON_EXP_GROUP_NBACKVS                "nbackvs"
#define  TEYERES_JSON_EXP_GROUP_BINDING                "binding"
#define  TEYERES_JSON_EXP_GROUP_READING                "reading"
#define  TEYERES_JSON_EXP_GROUP_PARKINSON              "parkinson"
#define  TEYERES_JSON_EXP_GROUP_GONOGO                 "gonogo"
#define  TEYERES_JSON_EXP_GROUP_PERCEPTION             "perception"

#define  TEYERES_JSON_NAME_RDTOTALFIXL                 "rdTotalFixL"
#define  TEYERES_JSON_NAME_RDFSTEPFIXL                 "rdFStepFixL"
#define  TEYERES_JSON_NAME_RDMSTEPFIXL                 "rdMStepFixL"
#define  TEYERES_JSON_NAME_RDSSTEPFIXL                 "rdSStepFixL"
#define  TEYERES_JSON_NAME_RDTOTALFIXR                 "rdTotalFixR"
#define  TEYERES_JSON_NAME_RDFSTEPFIXR                 "rdFStepFixR"
#define  TEYERES_JSON_NAME_RDMSTEPFIXR                 "rdMStepFixR"
#define  TEYERES_JSON_NAME_RDSSTEPFIXR                 "rdSStepFixR"
#define  TEYERES_JSON_NAME_BCAVGPUPILR                 "bcAvgPupilR"
#define  TEYERES_JSON_NAME_UCAVGPUPILR                 "ucAvgPupilR"
#define  TEYERES_JSON_NAME_BCAVGPUPILL                 "bcAvgPupilL"
#define  TEYERES_JSON_NAME_UCAVGPUPILL                 "ucAvgPupilL"
#define  TEYERES_JSON_NAME_BCCORRECT                   "bcCorrectAns"
#define  TEYERES_JSON_NAME_UCCORRECT                   "ucCorrentAns"
#define  TEYERES_JSON_NAME_BCGAZERESR                  "bcGazeResR"
#define  TEYERES_JSON_NAME_UCGAZERESR                  "ucGazeResR"
#define  TEYERES_JSON_NAME_BCGAZERESL                  "bcGazeResL"
#define  TEYERES_JSON_NAME_UCGAZERESL                  "ucGazeResL"
#define  TEYERES_JSON_NAME_BCWRONGANS                  "bcWrongAns"
#define  TEYERES_JSON_NAME_UCWRONGANS                  "ucWrongAns"
#define  TEYERES_JSON_NAME_BCTESTCORRECTANS            "bcTestCorrectAns"
#define  TEYERES_JSON_NAME_UCTESTCORRECTANS            "ucTestCorrectAns"
#define  TEYERES_JSON_NAME_BCTESTWRONGANS              "bcTestWrongAns"
#define  TEYERES_JSON_NAME_UCTESTWRONGANS              "ucTestWrongAns"
#define  TEYERES_JSON_NAME_BIND_INDEX_BC               "binding_index_bc"
#define  TEYERES_JSON_NAME_BIND_INDEX_UC               "binding_index_uc"
#define  TEYERES_JSON_NAME_READ_COG_IMPAIR             "cognitive_impairment_index"
#define  TEYERES_JSON_NAME_NBRT_FIX_ENC                "nbrt_fix_enc"
#define  TEYERES_JSON_NAME_NBRT_FIX_RET                "nbrt_fix_ret"
#define  TEYERES_JSON_NAME_NBRT_INHIB_PROB             "nbrt_inhib_prob"
#define  TEYERES_JSON_NAME_NBRT_SEQ_COMPLETE           "nbrt_seq_complete"
#define  TEYERES_JSON_NAME_NBRT_TARGET_HIT             "nbrt_target_hit"
#define  TEYERES_JSON_NAME_NBRT_MEAN_RESP_TIME         "nbrt_mean_resp_time"
#define  TEYERES_JSON_NAME_NBRT_MEAN_SAC_AMP           "nbrt_mean_sac_amp"
#define  TEYERES_JSON_NAME_GNG_SPEED_PROCESSING        "gng_speed_processing"
#define  TEYERES_JSON_NAME_GNG_DMT_INTERFERENCE        "gng_dmt_interference"
#define  TEYERES_JSON_NAME_GNG_DMT_FACILITATE          "gng_dmt_facilitate"
#define  TEYERES_JSON_NAME_GNG_PIP_FACILIATATE         "gng_pip_faciliatate"
#define  TEYERES_JSON_NAME_GNG_PIP_INTERFERENCE        "gng_pip_interference"

// Columnn names for the institituion table
#define  TINST_COL_KEYID                         "keyid"
#define  TINST_COL_UID                           "uid"
#define  TINST_COL_NAME                          "name"
#define  TINST_COL_FNAME                         "firstname"
#define  TINST_COL_LNAME                         "lastname"
#define  TINST_COL_ADDRESS                       "address"
#define  TINST_COL_PHONE                         "phone"
#define  TINST_COL_EMAIL                         "email"
#define  TINST_COL_ENABLED                       "enabled"

// Column names for the production table in the dashboard database.
#define  TPROD_COL_KEYID                         "keyid"
#define  TPROD_COL_INSTITUTION                   "linked_institution"
#define  TPROD_COL_PRODUCT_SN                    "product_sn"

// Column names for the FData Table.
#define  TFDATA_COL_KEYID                        "keyid"
#define  TFDATA_COL_FREQ_TOL_PARAMS              "freq_tol_params"
#define  TFDATA_COL_NODATA_SET_COUNT             "no_data_set_count"
#define  TFDATA_COL_GLITCHED_SET_COUNT           "glicthed_set_count"
#define  TFDATA_COL_FERROR                       "ferror"
#define  TFDATA_COL_EYE_FIX_COUNT                "eye_fixation_counts"
#define  TFDATA_COL_FILENAME                     "filename"
#define  TFDATA_COL_TEYERESULT                   "teye_result"

// Column names for the tPerformance Table
#define  TPERF_COL_KEYID                         "keyid"
#define  TPERF_COL_TIMESTRING                    "timestring"
#define  TPERF_COL_EYE_RESULT                    "teye_result"

// Column names for the tEvaluations Table
#define  TEVALS_COL_KEYID                        "keyid"
#define  TEVALS_COL_INSTITUTION                  "institution"
#define  TEVALS_COL_EVALUATIONS                  "evaluations"

#endif //DBDESCRIPTION_H
