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
#define  TABLE_PATIENTS_REQ_DATA                 "tPatientReqData"
#define  TABLE_PATIENTS_OPT_DATA                 "tPatientOptionalData"
#define  TABLE_DOCTORS                           "tDoctors"
#define  TABLE_EYE_RESULTS                       "tEyeResults"
#define  TABLE_INSTITUTION                       "tInstitution"

// Column names for required patient info
#define  TPATREQ_KEYID                           "keyid"           
#define  TPATREQ_COL_UID                         "uid"
#define  TPATREQ_COL_DOCTORID                    "doctorid"
#define  TPATREQ_COL_IDTYPE                      "idtype"
#define  TPATREQ_COL_FIRSTNAME                   "firstname"
#define  TPATREQ_COL_LASTNAME                    "lastname"
#define  TPATREQ_COL_BIRTHCOUNTRY                "birthcountry"
#define  TPATREQ_COL_SEX                         "sex"
#define  TPATREQ_COL_BIRTHDATE                   "birthdate"

// Column names for optional patient table
#define  TPATOPT_COL_KEYID                       "keyid"
#define  TPATOPT_COL_PATIENTID                   "patientid"
#define  TPATOPT_COL_DATE_INS                    "date_insertion"
#define  TPATOPT_COL_CITY                        "city"
#define  TPATOPT_COL_MAINACTIVITY                "mainactivity"
#define  TPATOPT_COL_HIGHESTDEGREE               "highestdegree"
#define  TPATOPT_COL_PHYSICALACTIVITY            "physicalactivity"
#define  TPATOPT_COL_NOFSPOKENLANG               "nofspokenlang"
#define  TPATOPT_COL_AGREESHAREDATA              "agreesharedata"
#define  TPATOPT_COL_FAMILYHISTORY               "familyhistory"
#define  TPATOPT_COL_PATIENTHISTORY              "patienthistory"
#define  TPATOPT_COL_REMEDIES                    "remedies"
#define  TPATOPT_COL_STATE                       "state"
#define  TPATOPT_COL_DIAGNOSTIC                  "diagnostic"

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

// Columnn names for the institituion table
#define  TINST_COL_KEYID                         "keyid"
#define  TINST_COL_UID                           "uid"
#define  TINST_COL_NAME                          "name"
#define  TINST_COL_HASHPASS                      "hashpass"
#define  TINST_COL_EVALS                         "evaluations"
#define  TINST_COL_ETSERIAL                      "etserial"
#define  TINST_COL_ETBRAND                       "etbrand"
#define  TINST_COL_ETMODEL                       "etmodel"

// Column names for the EyeResult tables.
#define  TEYERES_COL_KEYID                       "keyid"       
#define  TEYERES_COL_PATIENTID                   "patientid"   
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



#endif //DBDESCRIPTION_H
