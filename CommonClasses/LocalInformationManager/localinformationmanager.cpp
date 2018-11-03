#include "localinformationmanager.h"

const QString LocalInformationManager::PATIENT_DATA          = "PATIENT_DATA";
const QString LocalInformationManager::PATIENT_UPDATE        = "PATIENT_UPDATE";
const QString LocalInformationManager::DOCTOR_UPDATE         = "DOCTOR_UPDATE";
const QString LocalInformationManager::DOCTOR_PASSWORD       = "DOCTOR_PASSWORD";
const QString LocalInformationManager::DOCTOR_VALID          = "DOCTOR_VALID";
const QString LocalInformationManager::DOCTOR_HIDDEN         = "DOCTOR_HIDDEN";

LocalInformationManager::LocalInformationManager()
{
    workingDirectory = "";
}

void LocalInformationManager::setDirectory(const QString &workDir){
    workingDirectory = workDir;
    loadDB();
}

void LocalInformationManager::resetMedicalInstitutionForAllDoctors(const QString &inst_uid){
    QStringList druids = localDB.keys();
    for (qint32 i = 0; i < druids.size(); i++){
        QVariantMap drmap = localDB.value(druids.at(i)).toMap();
        drmap[TDOCTOR_COL_MEDICAL_INST] = inst_uid;
        localDB[druids.at(i)] = drmap;
    }
    backupDB();
}

QList<QStringList> LocalInformationManager::getAllPatientInfo() const {
    QList<QStringList> ans;
    QStringList druids = localDB.keys();
    for (qint32 i = 0; i < druids.size(); i++){
        QVariantMap patients = localDB.value(druids.at(i)).toMap().value(PATIENT_DATA).toMap();
        QStringList patuids = patients.keys();
        for (qint32 j =0; j < patuids.size(); j++){
            QStringList datum;
            datum << localDB.value(druids.at(i)).toMap().value(TDOCTOR_COL_FIRSTNAME).toString() + " "
                     + localDB.value(druids.at(i)).toMap().value(TDOCTOR_COL_LASTNAME).toString();
            datum << druids.at(i);
            datum << patients.value(patuids.at(j)).toMap().value(TPATREQ_COL_FIRSTNAME).toString() + " "
                     + patients.value(patuids.at(j)).toMap().value(TPATREQ_COL_LASTNAME).toString();
            datum << patuids.at(j);
            ans << datum;
        }
    }
    return ans;
}

QString LocalInformationManager::getFieldForPatient(const QString &druid, const QString &patuid, const QString &field) const{
    return localDB.value(druid).toMap().value(PATIENT_DATA).toMap().value(patuid).toMap().value(field).toString();
}

QString LocalInformationManager::getDoctorPassword(const QString &uid){
    return localDB.value(uid).toMap().value(DOCTOR_PASSWORD,"").toString();
}

QVariantMap LocalInformationManager::getPatientInfo(const QString &druid, const QString &patuid) const {
    return localDB.value(druid).toMap().value(PATIENT_DATA).toMap().value(patuid).toMap();
}

void LocalInformationManager::validateDoctor(const QString &dr_uid){
    //qWarning() << "Validating DR" << dr_uid;
    if (!localDB.contains(dr_uid)) return;
    QVariantMap drmap = localDB.value(dr_uid).toMap();
    drmap[DOCTOR_VALID] = true;
    // Since there were no updates while invalid, an update is forced.
    drmap[DOCTOR_UPDATE] = true;
    localDB[dr_uid] = drmap;
    backupDB();
}

void LocalInformationManager::deleteDoctor(const QString &uid){
    if (!localDB.contains(uid)) return;
    localDB.remove(uid);
    backupDB();
}

void LocalInformationManager::setDoctorData(const QString &uid, const QStringList &keys, const QVariantList &values){
    if (!localDB.contains(uid)) return;
    QVariantMap drmap = localDB.value(uid).toMap();
    for (qint32 i = 0; i < keys.size(); i++){
        drmap[keys.at(i)] = values.at(i);
    }
    localDB[uid] = drmap;
    backupDB();
}

bool LocalInformationManager::isHidden(const QString &uid){
    if (!localDB.contains(uid)) return false;
    if (!localDB.value(uid).toMap().contains(DOCTOR_HIDDEN)) return false;
    if (!localDB.value(uid).toMap().contains(DOCTOR_VALID)) return false;
    return (localDB.value(uid).toMap().value(DOCTOR_HIDDEN).toBool() && localDB.value(uid).toMap().value(DOCTOR_VALID).toBool());
}

bool LocalInformationManager::doesDoctorExist(const QString &uid) const{
    return localDB.contains(uid);
}

bool LocalInformationManager::doesPatientExist(const QString &druid, const QString &patuid) const{
    if (!localDB.contains(druid)) return false;
    return localDB.value(druid).toMap().value(PATIENT_DATA).toMap().contains(patuid);
}

bool LocalInformationManager::isDoctorValid(const QString &dr_uid){
    if (!localDB.contains(dr_uid)) {
        return false;
    }
    if (!localDB.value(dr_uid).toMap().contains(DOCTOR_VALID)) {
        return false;
    }
    return localDB.value(dr_uid).toMap().value(DOCTOR_VALID).toBool();
}

#ifdef USESSL
bool LocalInformationManager::setupDBSynch(SSLDBClient *client){

    client->setDBTransactionType(SQL_QUERY_TYPE_SET);
    bool ans = false;

    QStringList allDrUIDs = localDB.keys();
    for (qint32 i = 0; i < allDrUIDs.size(); i++){

        // Checking if the doctor needs to be synched up.
        QString druid = allDrUIDs.at(i);
        QVariantMap drmap = localDB.value(druid).toMap();

        bool addDoctor = false;
        if (!drmap.contains(DOCTOR_UPDATE)) addDoctor = true;
        else if (drmap.value(DOCTOR_UPDATE).toBool()) addDoctor = true;

        //qWarning() << "Flags: UPDATE" << drmap.value(DOCTOR_UPDATE).toBool() << " VALID " << drmap.value(DOCTOR_VALID).toBool() << druid;

        // Doctors who do not contain or whose valid flag is false do NOT update its information.
        if (!drmap.contains(DOCTOR_VALID)) continue;
        if (!drmap.value(DOCTOR_VALID).toBool()) continue;

        if (addDoctor){
            //qWarning() << "Adding the doctor";
            QStringList columns;
            QStringList values;
            QSet<QString> avoid;
            avoid << DOCTOR_UPDATE << PATIENT_DATA << DOCTOR_VALID << DOCTOR_PASSWORD << DOCTOR_HIDDEN;
            QStringList keys = drmap.keys();
            for (qint32 j = 0; j < keys.size(); j++){
                //Y esta bqWarning() << "Key " << j << " out of " << keys.size();
                if (avoid.contains(keys.at(j))) continue;
                columns << keys.at(j);
                values << drmap.value(keys.at(j)).toString();
            }
            ans = true;
            client->appendSET(TABLE_DOCTORS,columns,values);
        }

        // Checking if any of the doctor's patients need to be synched up.
        QVariantMap patientData = drmap.value(PATIENT_DATA).toMap();
        QStringList allPatUIDs = patientData.keys();

        for (qint32 j = 0; j < allPatUIDs.size(); j++){

            QString patID = allPatUIDs.at(j);
            QVariantMap patientMap = patientData.value(patID).toMap();

            bool addPatient = false;
            if (!patientMap.contains(PATIENT_UPDATE)) addPatient = true;
            else if (patientMap.value(PATIENT_UPDATE).toBool()) addPatient = true;

            if (addPatient){
                QStringList columns;
                QStringList values;
                QStringList columnsOpt;
                QStringList valuesOpt;
                QSet<QString> avoid; avoid << PATIENT_UPDATE;
                QSet<QString> required; required << TPATREQ_COL_BIRTHCOUNTRY << TPATREQ_COL_BIRTHDATE << TPATREQ_COL_FIRSTNAME << TPATREQ_COL_IDTYPE
                                                 << TPATREQ_COL_LASTNAME << TPATREQ_COL_SEX << TPATREQ_COL_UID << TPATREQ_COL_DOCTORID;
                QStringList keys = patientMap.keys();
                for (qint32 k = 0; k < keys.size(); k++){
                    if (avoid.contains(keys.at(k))) continue;
                    if (required.contains(keys.at(k))){
                        columns << keys.at(k);
                        values << patientMap.value(keys.at(k)).toString();
                    }
                    else{
                        columnsOpt << keys.at(k);
                        valuesOpt << patientMap.value(keys.at(k)).toString();
                    }
                }

                // Adding the DR ID.
                client->appendSET(TABLE_PATIENTS_REQ_DATA,columns,values);
                client->appendSET(TABLE_PATIENTS_OPT_DATA,columnsOpt,valuesOpt);
                ans = true;
            }
        }
    }

    return ans;

}
#endif

void LocalInformationManager::fillPatientDatInformation(const QString &druid){

    patientReportInformation.clear();

    // Creating the source directory path.
    QString baseDir = workingDirectory + "/" + druid;

    QDir doctorDir(baseDir);
    if (!doctorDir.exists()){
        // This is not necessarily an error because all new Doctors will not have a base directory created.
        // log.appendError("Base directory for the current Doctor: " + baseDir + " was not found");
        return;
    }

    // The Directory exists, so a list of all directories inside it, is generated.
    QStringList directories = doctorDir.entryList(QStringList(),QDir::Dirs | QDir::NoDotAndDotDot);
    QVariantMap patientData = localDB.value(druid).toMap().value(PATIENT_DATA).toMap();
    for (qint32 i = 0; i < directories.size(); i++){
        if (!patientData.contains(directories.at(i))) continue;

        // Separating all files by extension and prefix if necessary.
        QDir patientDir(baseDir + "/" + directories.at(i));

        //qWarning() << "DAT INFO TIME";
        DatFileInfoInDir datInfo;
        datInfo.setDatDirectory(patientDir.path());
        patientReportInformation[directories.at(i)] = datInfo;
        //qWarning() << "PATIENT REPORT INFO for" << directories.at(i);
        //datInfo.printData();
    }

}

void LocalInformationManager::addDoctorData(const QString &dr_uid, const QStringList &cols, const QStringList &values, const QString &password, bool hidden){

    // Doctor information needs to be checked against existing information to see if there is a change.
    QVariantMap drinfo;

    // Adding the dr information.
    for (qint32 i = 0; i < cols.size(); i++){
        drinfo[cols.at(i)] = values.at(i);
    }

    drinfo[DOCTOR_HIDDEN] = hidden;

    if (!localDB.contains(dr_uid)){
        // This is a new docotr.
        drinfo[DOCTOR_UPDATE] = true;
        drinfo[DOCTOR_VALID] = false; // Doctor is invalidated ONLY when created and when its hidden status is changed.
        drinfo[DOCTOR_HIDDEN] = false; // It should not be possible to CREATE a hidden Doctor. But just in case.
        drinfo[DOCTOR_PASSWORD] = password;
        // Empty map for the patient.
        drinfo[PATIENT_DATA] = QVariantMap();        
        localDB[dr_uid] = drinfo;
    }
    else {
        // Doctor is existing. Checking all information except patient data
        bool update = false;
        QVariantMap drmap = localDB.value(dr_uid).toMap();
        for (qint32 i = 0; i < cols.size(); i++){
            if (!drmap.contains(cols.at(i))){
                update = true;
                break;
            }
            else if (drmap.value(cols.at(i)).toString() != values.at(i)){
                update = true;
                break;
            }
        }
        drinfo[DOCTOR_UPDATE] = update;

        // Making sure the password and valid fields are copied.
        if (drmap.contains(DOCTOR_VALID)) drinfo[DOCTOR_VALID] = drmap.value(DOCTOR_VALID);
        else drinfo[DOCTOR_VALID] = false;
        if (drmap.contains(DOCTOR_PASSWORD)) drinfo[DOCTOR_PASSWORD] = drmap.value(DOCTOR_PASSWORD);
        else drinfo[DOCTOR_PASSWORD] = "";

        // Setting the new password if necessary, only if not empty.
        if (!password.isEmpty()){
            drinfo[DOCTOR_PASSWORD] = password;
        }

        // If the doctor was hidden, nothing else matters. It will not be updated and it is invalid.
        if (hidden){            
            drinfo[DOCTOR_UPDATE] = false;
            drinfo[DOCTOR_VALID] = false;
        }

        // Saving the existent patient data.
        drinfo[PATIENT_DATA] = drmap.value(PATIENT_DATA);
        localDB[dr_uid] = drinfo;
    }

    //qWarning() << "On Adding Dr Info UPDATE" << drinfo.value(DOCTOR_UPDATE).toBool() << " VALID " << drinfo.value(DOCTOR_VALID).toBool() << dr_uid;
    backupDB();
}

void LocalInformationManager::addPatientData(const QString &druid, const QString &patient_uid, const QStringList &cols, const QStringList &values){

    QVariantMap drinfo = localDB.value(druid).toMap();
    QVariantMap patients = drinfo.value(PATIENT_DATA).toMap();

    // Patient information to variant map.
    QVariantMap thispatient;
    for (qint32 i = 0; i < cols.size(); i++){
        thispatient[cols.at(i)] = values.at(i);
    }

    // Checking to see if updte is necessary
    if (patients.contains(patient_uid)){
        QVariantMap basePatient = patients.value(patient_uid).toMap();
        thispatient[PATIENT_UPDATE] = false;
        for (qint32 i = 0; i < cols.size(); i++){
            if (!basePatient.contains(cols.at(i))){
                thispatient[PATIENT_UPDATE] = true;
                break;
            }
            else if (basePatient.value(cols.at(i)).toString() != values.at(i)){
                thispatient[PATIENT_UPDATE] = true;
                break;
            }
        }
    }
    else{
        thispatient[PATIENT_UPDATE] = true;
    }

    // Storing the patient information in the patient map for the doctor
    patients[patient_uid] = thispatient;

    // Storing the new patient map in the doctor information
    drinfo[PATIENT_DATA] = patients;

    // Storing the new dr map in the local db.
    localDB[druid] = drinfo;

    // Finally saving the DB.
    return backupDB();

}

void LocalInformationManager::setUpdateFlagTo(bool flag){

    QStringList druids = localDB.keys();
    for (qint32 i = 0; i < druids.size(); i++){
        QVariantMap drmap = localDB.value(druids.at(i)).toMap();
        drmap[DOCTOR_UPDATE] = flag;
        QVariantMap patients = drmap.value(PATIENT_DATA).toMap();
        QStringList patuids = patients.keys();
        for (qint32 j = 0; j < patuids.size(); j++){
            QVariantMap patmap = patients.value(patuids.at(j)).toMap();
            patmap[PATIENT_UPDATE] = flag;
            patients[patuids.at(j)] = patmap;
        }
        drmap[PATIENT_DATA] = patients;
        localDB[druids.at(i)] = drmap;
    }

    backupDB();
    //qWarning() << "AFTER SETTING THE FLAG TO" << flag;
    //printLocalDB();
}

void LocalInformationManager::preparePendingReports(const QString &uid){
    patientReportInformation[uid].prepareToInterateOverPendingReportFileSets();
}

QStringList LocalInformationManager::nextPendingReport(const QString &uid){
    return patientReportInformation[uid].nextPendingReportFileSet();
}

QList<QStringList> LocalInformationManager::getDoctorList(bool forceShow){
    QList<QStringList> ans;
    QStringList names;
    QStringList uids = localDB.keys();
    QStringList showUIds;
    for (qint32 i = 0; i < uids.size(); i++){
        QVariantMap drinfo = localDB.value(uids.at(i)).toMap();
        if (isHidden(uids.at(i)) && !forceShow) continue;
        names << drinfo.value(TDOCTOR_COL_FIRSTNAME).toString() + " " + drinfo.value(TDOCTOR_COL_LASTNAME).toString()
                 + " (" + uids.at(i) + ")";
        showUIds << uids.at(i);
    }
    if (!names.isEmpty()) ans << names << showUIds;
    return ans;
}

QList<QStringList> LocalInformationManager::getPatientListForDoctor(const QString &druid){
    QList<QStringList> ans;
    QStringList names;
    QVariantMap patients = localDB.value(druid).toMap().value(PATIENT_DATA).toMap();
    QStringList uids = patients.keys();
    //qWarning() << "PAT UIDS for" << config->getString(CONFIG_DOCTOR_UID) << " are " << uids;
    for (qint32 i = 0; i < uids.size(); i++){
        QVariantMap patinfo = patients.value(uids.at(i)).toMap();
        names << patinfo.value(TPATREQ_COL_FIRSTNAME).toString() + " " + patinfo.value(TPATREQ_COL_LASTNAME).toString();
    }
    if (!names.isEmpty()) {
        ans << names << uids;
        fillPatientDatInformation(druid);
        QStringList isoklist;
        for (qint32 i = 0; i < uids.size(); i++){
            if (patientReportInformation.value(uids.at(i)).hasPendingReports()) isoklist << "false";
            else isoklist << "true";
        }
        //qWarning() << "IS OK LIST" << isoklist;
        ans << isoklist;
    }
    return ans;
}

void LocalInformationManager::loadDB(){

    // Checking to see if the directory structure exists.
    QDir basedir(workingDirectory);
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

void LocalInformationManager::backupDB(){

    // Checking to see if the directory structure exists.
    QDir basedir(workingDirectory);
    if (!basedir.exists()){
        log.appendError("LOCALDB: Base Data directory: " + basedir.path() + " does not exist and it should, at this point");
    }

    QString dbfile = basedir.path() + "/" + LOCAL_DB;

    QFile file(dbfile);
    if (!file.open(QFile::WriteOnly)){
        log.appendError("LOCALDB: Cannot open DB file " + dbfile  + " for writing");
    }

    QDataStream writer(&file);
    writer << LOCAL_DB_VERSION;
    writer << localDB;
    file.close();

    // Backing up the DB.
    QFile(LOCAL_DB).remove();
    if (!QFile::copy(dbfile,LOCAL_DB)){
        log.appendError("LOCALDB: Failed to backup file " + dbfile);
    }

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

