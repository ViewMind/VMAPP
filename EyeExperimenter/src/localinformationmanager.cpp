#include "localinformationmanager.h"

const QString LocalInformationManager::PATIENT_DATA = "PATIENT_DATA";

LocalInformationManager::LocalInformationManager(ConfigurationManager *c)
{
    config = c;
    loadDB();
}

bool LocalInformationManager::addDoctorData(const QString &dr_uid,const QStringList &cols, const QStringList &values){

    if (localDB.contains(dr_uid)){
        log.appendError("LOCAL DB: Cannot add UID: " + dr_uid + " as it already exists");
        return false;
    }

    QVariantMap drinfo;
    // Adding the dr information.
    for (qint32 i = 0; i < cols.size(); i++){
        drinfo[cols.at(i)] = values.at(i);
    }
    // Empty map for the patient.
    drinfo[PATIENT_DATA] = QVariantMap();
    localDB[dr_uid] = drinfo;

    return backupDB();
}

bool LocalInformationManager::addPatientData(const QString &patient_uid, const QStringList &cols, const QStringList &values){

    QVariantMap drinfo = localDB.value(currentDr).toMap();
    QVariantMap patients = drinfo.value(PATIENT_DATA).toMap();

    if (patients.contains(patient_uid)){
        log.appendError("LOCAL DB: Cannot patient to doctor because it already exists");
        return false;
    }

    // Patient information to variant map.
    QVariantMap thispatient;
    for (qint32 i = 0; i < cols.size(); i++){
        thispatient[cols.at(i)] = values.at(i);
    }

    // Storing the patient information in the patient map for the doctor
    patients[patient_uid] = thispatient;

    // Storing the new patient map in the doctor information
    drinfo[PATIENT_DATA] = patients;

    // Storing the new dr map in the local db.
    localDB[currentDr] = drinfo;

    // Finally saving the DB.
    return backupDB();

}

QStringList LocalInformationManager::getDoctorList(bool onlyUIDs){
    QStringList ans;
    QStringList uids = localDB.keys();
    if (onlyUIDs) return uids;
    for (qint32 i = 0; i < uids.size(); i++){
        QVariantMap drinfo = localDB.value(uids.at(i)).toMap();
        ans << drinfo.value(TDOCTOR_COL_FIRSTNAME).toString() + " " + drinfo.value(TDOCTOR_COL_LASTNAME).toString() + " - (" + uids.at(i) + ")";
    }
    return ans;
}

QStringList LocalInformationManager::getPatientListForDoctor(){
    QStringList ans;
    QVariantMap patients = localDB.value(currentDr).toMap().value(PATIENT_DATA).toMap();
    QStringList uids = patients.keys();
    for (qint32 i = 0; i < uids.size(); i++){
        QVariantMap patinfo = patients.value(uids.at(i)).toMap();
        ans << patinfo.value(TPATREQ_COL_FIRSTNAME).toString() + " " + patinfo.value(TPATREQ_COL_LASTNAME).toString() + " - (" + uids.at(i) + ")";
    }
    return ans;
}

void LocalInformationManager::loadDB(){

    // Checking to see if the directory structure exists.
    QDir basedir(config->getString(CONFIG_OUTPUT_DIR) + "/" + DIRNAME_RAWDATA);
    if (!basedir.exists()){
        log.appendError("LOCALDB LOADING: Base Data directory: " + basedir.path() + " does not exist and it should, at this point");
        return;
    }

    QString dbfile = basedir.path() + "/" + LOCAL_DB;

    QFile file(dbfile);
    if (!file.open(QFile::ReadOnly)){
        log.appendError("LOCALDB: Cannot open DB file " + dbfile  + " for reading");
        return;
    }

    QDataStream reader(&file);
    qint32 dummy;
    localDB.clear();
    reader >> dummy;
    reader >> localDB;
    file.close();

}

bool LocalInformationManager::backupDB(){

    // Checking to see if the directory structure exists.
    QDir basedir(config->getString(CONFIG_OUTPUT_DIR) + "/" + DIRNAME_RAWDATA);
    if (!basedir.exists()){
        log.appendError("LOCALDB: Base Data directory: " + basedir.path() + " does not exist and it should, at this point");
        return false;
    }

    QString dbfile = basedir.path() + "/" + LOCAL_DB;

    QFile file(dbfile);
    if (!file.open(QFile::WriteOnly)){
        log.appendError("LOCALDB: Cannot open DB file " + dbfile  + " for writing");
        return false;
    }

    QDataStream writer(&file);
    writer << LOCAL_DB_VERSION;
    writer << localDB;
    file.close();

    // Backing up the DB.
    QFile(LOCAL_DB).remove();
    QFile::copy(dbfile,LOCAL_DB);

    return true;

}

void LocalInformationManager::printLocalDB(){

    QStringList druids = localDB.keys();
    QString tab1 = "   ";
    QString tab2 = tab1 + tab1;
    QString tab3 = tab2 + tab1;

    for (qint32 i = 0; i < druids.size(); i++){
        qWarning() << "DR UID: " << druids.at(i);
        QVariantMap drinfo = localDB.value(druids.at(i)).toMap();
        QStringList drcols = drinfo.keys();
        for (qint32 j = 0; j < drcols.size(); j++){
            if (drcols.at(j) != PATIENT_DATA)
               qWarning() << tab1 << drcols.at(j) << " = " << drinfo.value(drcols.at(j)).toString();
        }
        QVariantMap patients = drinfo.value(PATIENT_DATA).toMap();
        qWarning() << tab1 << "PATIENT DATA:";
        QStringList patientUIDs = patients.keys();
        for (qint32 j = 0; j < patientUIDs.size(); j++){
            qWarning() << tab2 << patientUIDs.at(j);
            QVariantMap patientInfo = patients.value(patientUIDs.at(j)).toMap();
            QStringList patientCols = patientInfo.keys();
            for (qint32 k = 0; k < patientCols.size(); k++){
                qWarning() << tab3 << patientCols.at(k) << " = " << patientInfo.value(patientCols.at(k)).toString();
            }
        }
    }

}

