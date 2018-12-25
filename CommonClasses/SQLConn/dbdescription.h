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
#define  TABLE_INSTITUTION                       "tInstitution"
#define  TABLE_PLACEDPRODUCTS                    "tPlacedProducts"

#define  TABLE_PATDATA                           "tPatientData"
#define  TABLE_PATIENTD_IDS                      "tPatientIDs"

// Column names for the patient ids table
#define  TPATID_COL_KEYID                         "keyid"
#define  TPATID_COL_UID                           "uid"

// Column names for patient info
#define  TPATDATA_COL_KEYID                       "keyid"
#define  TPATDATA_COL_DOCTORID                    "doctorid"
#define  TPATDATA_COL_FIRSTNAME                   "firstname"
#define  TPATDATA_COL_LASTNAME                    "lastname"
#define  TPATDATA_COL_IDTYPE                      "idtype"
#define  TPATDATA_COL_SEX                         "sex"
#define  TPATDATA_COL_BIRTHCOUNTRY                "birthcountry"
#define  TPATDATA_COL_BIRTHDATE                   "birthdate"
#define  TPATDATA_COL_DATE_INS                    "date_insertion"
#define  TPATDATA_COL_STATE                       "state"
#define  TPATDATA_COL_CITY                        "city"
#define  TPATDATA_COL_MAINACTIVITY                "mainactivity"
#define  TPATDATA_COL_HIGHESTDEGREE               "highestdegree"
#define  TPATDATA_COL_PHYSICALACTIVITY            "physicalactivity"
#define  TPATDATA_COL_NOFSPOKENLANG               "nofspokenlang"
#define  TPATDATA_COL_AGREESHAREDATA              "agreesharedata"
#define  TPATDATA_COL_FAMILYHISTORY               "familyhistory"
#define  TPATDATA_COL_PATIENTHISTORY              "patienthistory"
#define  TPATDATA_COL_REMEDIES                    "remedies"
#define  TPATDATA_COL_DIAGNOSTIC                  "diagnostic"
#define  TPATDATA_COL_PUID                        "puid"

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
#define  TEYERES_COL_RDTOTALFIXL                 "rdTotalFixL"
#define  TEYERES_COL_RDFSTEPFIXL                 "rdFStepFixL"
#define  TEYERES_COL_RDMSTEPFIXL                 "rdMStepFixL"
#define  TEYERES_COL_RDSSTEPFIXL                 "rdSStepFixL"
#define  TEYERES_COL_RDTOTALFIXR                 "rdTotalFixR"
#define  TEYERES_COL_RDFSTEPFIXR                 "rdFStepFixR"
#define  TEYERES_COL_RDMSTEPFIXR                 "rdMStepFixR"
#define  TEYERES_COL_RDSSTEPFIXR                 "rdSStepFixR"
#define  TEYERES_COL_BCAVGPUPILR                 "bcAvgPupilR"
#define  TEYERES_COL_UCAVGPUPILR                 "ucAvgPupilR"
#define  TEYERES_COL_BCAVGPUPILL                 "bcAvgPupilL"
#define  TEYERES_COL_UCAVGPUPILL                 "ucAvgPupilL"
#define  TEYERES_COL_BCCORRECT                   "bcCorrectAns"
#define  TEYERES_COL_UCCORRECT                   "ucCorrentAns"
#define  TEYERES_COL_BCGAZERESR                  "bcGazeResR"
#define  TEYERES_COL_UCGAZERESR                  "ucGazeResR"
#define  TEYERES_COL_BCGAZERESL                  "bcGazeResL"
#define  TEYERES_COL_UCGAZERESL                  "ucGazeResL"
#define  TEYERES_COL_BCWRONGANS                  "bcWrongAns"
#define  TEYERES_COL_UCWRONGANS                  "ucWrongAns"
#define  TEYERES_COL_BCTESTCORRECTANS            "bcTestCorrectAns"
#define  TEYERES_COL_UCTESTCORRECTANS            "ucTestCorrectAns"
#define  TEYERES_COL_BCTESTWRONGANS              "bcTestWrongAns"
#define  TEYERES_COL_UCTESTWRONGANS              "ucTestWrongAns"
#define  TEYERES_COL_FDHITR                      "fdHitR"
#define  TEYERES_COL_FDHITL                      "fdHitL"
#define  TEYERES_COL_FDHITTOTAL                  "fdHitTotal"

// Columnn names for the institituion table
#define  TINST_COL_KEYID                         "keyid"
#define  TINST_COL_UID                           "uid"
#define  TINST_COL_NAME                          "name"
#define  TINST_COL_HASHPASS                      "hashpass"
#define  TINST_COL_EVALS                         "evaluations"
#define  TINST_COL_FNAME                         "firstname"
#define  TINST_COL_LNAME                         "lastname"
#define  TINST_COL_ADDRESS                       "address"
#define  TINST_COL_PHONE                         "phone"
#define  TINST_COL_EMAIL                         "email"

// Column names for the placed product table.
#define  TPLACED_PROD_COL_KEYID                  "keyid"
#define  TPLACED_PROD_COL_INSTITUTION            "institution"
#define  TPLACED_PROD_COL_PRODUCT                "product"
#define  TPLACED_PROD_COL_PRODSN                 "product_serialnum"
#define  TPLACED_PROD_COL_SOFTVER                "software_version"
#define  TPLACED_PROD_COL_PCMODEL                "pc_model"
#define  TPLACED_PROD_COL_ETBRAND                "etbrand"
#define  TPLACED_PROD_COL_ETMODEL                "etmodel"
#define  TPLACED_PROD_COL_ETSERIAL               "etserial"
#define  TPLACED_PROD_COL_CHINRESTMODEL          "chinrest_model"
#define  TPLACED_PROD_COL_CHINRESTSN             "chinrest_serialnum"


#endif //DBDESCRIPTION_H
