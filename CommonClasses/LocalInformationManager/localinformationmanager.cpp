#include "localinformationmanager.h"

const QString LocalInformationManager::PATIENT_DATA          = "PATIENT_DATA";
const QString LocalInformationManager::PATIENT_CREATOR       = "PATIENT_CREATOR";
const QString LocalInformationManager::PATIENT_UPDATE        = "PATIENT_UPDATE";
const QString LocalInformationManager::PATIENT_COUNTER       = "PATIENT_COUNTER";
const QString LocalInformationManager::DOCTOR_DATA           = "DOCTOR_DATA";
const QString LocalInformationManager::DOCTOR_COUNTER        = "DOCTOR_COUNTER";
const QString LocalInformationManager::DOCTOR_UPDATE         = "DOCTOR_UPDATE";
const QString LocalInformationManager::DOCTOR_PASSWORD       = "DOCTOR_PASSWORD";
const QString LocalInformationManager::DOCTOR_VALID          = "DOCTOR_VALID";
const QString LocalInformationManager::DOCTOR_HIDDEN         = "DOCTOR_HIDDEN";
const QString LocalInformationManager::FLAG_VIEWALL          = "FLAG_VIEWALL";
const QString LocalInformationManager::PROTOCOLS             = "PROTOCOLS";
const QString LocalInformationManager::PROTOCOL_VALID        = "PROTOCOL_VALID";

LocalInformationManager::LocalInformationManager()
{
    workingDirectory = "";
    backupDirectory = "";
}

//************************************** Interface with Local DB *****************************************

void LocalInformationManager::setDirectory(const QString &workDir, const QString &eyeexpid, const QString &instUID){
    workingDirectory = workDir;
    loadDB(eyeexpid,instUID);
}

void LocalInformationManager::setViewAllFlag(bool flagValue){
    localDB[FLAG_VIEWALL] = flagValue;
    backupDB();
}

bool LocalInformationManager::getViewAllFlag() const{
    if (localDB.contains(FLAG_VIEWALL)) return localDB.value(FLAG_VIEWALL).toBool();
    else return false;
}

void LocalInformationManager::enableBackups(const QString &backupDir){
    backupDirectory = backupDir;
}

void LocalInformationManager::resetMedicalInstitutionForAllDoctors(const QString &inst_uid){
    QVariantMap drdb   = localDB.value(DOCTOR_DATA).toMap();
    QStringList druids = drdb.keys();
    for (qint32 i = 0; i < druids.size(); i++){
        QVariantMap drmap = drdb.value(druids.at(i)).toMap();
        drmap[TDOCTOR_COL_MEDICAL_INST] = inst_uid;
        drdb[druids.at(i)] = drmap;
    }
    localDB[DOCTOR_DATA] = drdb;
    backupDB();
}

QString LocalInformationManager::getFieldForPatient(const QString &patuid, const QString &field) const{
    return localDB.value(PATIENT_DATA).toMap().value(patuid).toMap().value(field).toString();
}

QString LocalInformationManager::getDoctorPassword(const QString &uid){
    return localDB.value(DOCTOR_DATA).toMap().value(uid).toMap().value(DOCTOR_PASSWORD,"").toString();
}

QVariantMap LocalInformationManager::getPatientInfo(const QString &patuid) const {
    //qWarning() << "Will return patient info for" << patuid;
    return localDB.value(PATIENT_DATA).toMap().value(patuid).toMap();
}

QVariantMap LocalInformationManager::getDoctorInfo(const QString &uid) const{
    return localDB.value(DOCTOR_DATA).toMap().value(uid).toMap();
}

QStringList LocalInformationManager::getFileListForPatient(const QString &patuid, qint32 whichList) const{
    switch(whichList){
    case LIST_INDEX_READING: return patientReportInformation.value(patuid).getReadingFileList();
    case LIST_INDEX_BINDING_BC: return patientReportInformation.value(patuid).getBindingBCFileList();
    case LIST_INDEX_BINDING_UC: return patientReportInformation.value(patuid).getBindingUCFileList();
    default: return QStringList();
    }
}

void LocalInformationManager::validateDoctor(const QString &dr_uid){
    //qWarning() << "Validating DR" << dr_uid;
    if (!localDB.value(DOCTOR_DATA).toMap().contains(dr_uid)) return;
    QVariantMap drdb = localDB.value(DOCTOR_DATA).toMap();
    QVariantMap drmap = drdb.value(dr_uid).toMap();

    drmap[DOCTOR_VALID] = true;
    // Since there were no updates while invalid, an update is forced.
    drmap[DOCTOR_UPDATE] = true;

    drdb[dr_uid] = drmap;
    localDB[DOCTOR_DATA] = drdb;

    backupDB();
}

bool LocalInformationManager::deleteDoctor(const QString &uid){
    QVariantMap drdb = localDB.value(DOCTOR_DATA).toMap();
    if (!drdb.contains(uid)) return false;

    // Checking if deletion is possible.
    QStringList patuids = localDB.value(PATIENT_DATA).toMap().keys();
    for (qint32 i = 0; i < patuids.size(); i++){
        QVariantMap patmap = localDB.value(PATIENT_DATA).toMap().value(patuids.at(i)).toMap();
        if ((patmap.value(PATIENT_CREATOR).toString() == uid) || (patmap.value(TPATDATA_COL_DOCTORID).toString() == uid)) return false;
    }

    drdb.remove(uid);
    localDB[DOCTOR_DATA] = drdb;
    backupDB();
    return true;
}

void LocalInformationManager::setDoctorData(const QString &uid, const QStringList &keys, const QVariantList &values){
    if (!localDB.value(DOCTOR_DATA).toMap().contains(uid)) return;
    QVariantMap drdb = localDB.value(DOCTOR_DATA).toMap();
    QVariantMap drmap = drdb.value(uid).toMap();
    for (qint32 i = 0; i < keys.size(); i++){
        drmap[keys.at(i)] = values.at(i);
    }
    drdb[uid] = drmap;
    localDB[DOCTOR_DATA] = drdb;
    backupDB();
}

bool LocalInformationManager::isHidden(const QString &uid){
    if (!localDB.value(DOCTOR_DATA).toMap().contains(uid)) return false;
    if (!localDB.value(DOCTOR_DATA).toMap().value(uid).toMap().contains(DOCTOR_HIDDEN)) return false;
    else return localDB.value(DOCTOR_DATA).toMap().value(uid).toMap().value(DOCTOR_HIDDEN).toBool();
}

bool LocalInformationManager::doesDoctorExist(const QString &uid) const{
    return localDB.value(DOCTOR_DATA).toMap().contains(uid);
}

bool LocalInformationManager::doesPatientExist(const QString &patuid) const{
    return localDB.value(PATIENT_DATA).toMap().contains(patuid);
}

bool LocalInformationManager::isDoctorValid(const QString &dr_uid){
    //qWarning() << "Checking if doctor is valid" << dr_uid;
    //printDBToConsole();
    if (!localDB.value(DOCTOR_DATA).toMap().contains(dr_uid)) return false;
    //qWarning() << "Existe";
    if (!localDB.value(DOCTOR_DATA).toMap().value(dr_uid).toMap().contains(DOCTOR_VALID)) return false;
    //qWarning() << "Returning the value of the flag in the database";
    return localDB.value(DOCTOR_DATA).toMap().value(dr_uid).toMap().value(DOCTOR_VALID).toBool();
}

#ifdef USESSL
bool LocalInformationManager::setupDBSynch(SSLDBClient *client){

    client->setDBTransactionType(SQL_QUERY_TYPE_SET);
    bool ans = false;

    // Adding the doctor data that needs updating
    QStringList druids = localDB.value(DOCTOR_DATA).toMap().keys();
    for (qint32 i = 0; i < druids.size(); i++){
        QVariantMap drmap = localDB.value(DOCTOR_DATA).toMap().value(druids.at(i)).toMap();

        bool addDoctor = false;
        if (!drmap.contains(DOCTOR_UPDATE)) addDoctor = true;
        else if (drmap.value(DOCTOR_UPDATE).toBool()) addDoctor = true;

        // Doctors who do not contain or whose valid flag is false do NOT update its information.
        if (!drmap.contains(DOCTOR_VALID)) continue;
        if (!drmap.value(DOCTOR_VALID).toBool()) continue;

        if (addDoctor){
            //qWarning() << "Adding the doctor";
            QStringList columns;
            QStringList values;
            QSet<QString> avoid;
            avoid << DOCTOR_UPDATE << DOCTOR_VALID << DOCTOR_PASSWORD << DOCTOR_HIDDEN;
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
    }

    // Adding the patient data that needs updating.
    QStringList patuids = localDB.value(PATIENT_DATA).toMap().keys();
    for (qint32 j = 0; j < patuids.size(); j++){

        QVariantMap patientMap = localDB.value(PATIENT_DATA).toMap().value(patuids.at(j)).toMap();

        // Patient must be assiged to a VALIDATED doctor. Otherwise synchronization will not be possible as de Doctor is not in the DB.
        QString assignedDoctor = patientMap.value(TPATDATA_COL_DOCTORID).toString();
        if (!localDB.value(DOCTOR_DATA).toMap().value(assignedDoctor).toMap().value(DOCTOR_VALID).toBool()){
            continue;
        }

        bool addPatient = false;
        if (!patientMap.contains(PATIENT_UPDATE)) addPatient = true;
        else if (patientMap.value(PATIENT_UPDATE).toBool()) addPatient = true;

        if (addPatient){
            QStringList columns;
            QStringList values;
            QSet<QString> avoid; avoid << PATIENT_UPDATE << TPATDATA_COL_PUID << PATIENT_CREATOR << TPATDATA_NONCOL_DISPLAYID << TPATDATA_NONCOL_PROTOCOL;
            QSet<QString> needToHash; needToHash << TPATDATA_COL_FIRSTNAME << TPATDATA_COL_LASTNAME;
            QStringList keys = patientMap.keys();
            for (qint32 k = 0; k < keys.size(); k++){
                if (avoid.contains(keys.at(k))) continue;
                columns << keys.at(k);
                QString value = patientMap.value(keys.at(k)).toString();
                if (needToHash.contains(keys.at(k))){
                    value = QCryptographicHash::hash(value.toLatin1(),QCryptographicHash::Sha3_256).toHex();
                }
                values << value;
            }

            // Adding the Hashed patient ID.

            QString hash = QCryptographicHash::hash(patientMap.value(TPATDATA_COL_PUID).toString().toLatin1(),QCryptographicHash::Sha3_512).toHex();
            columns << TPATDATA_COL_PUID;
            log.appendStandard(patientMap.value(TPATDATA_COL_PUID).toString() + "==>" + hash);
            values << hash;

            client->appendSET(TABLE_PATDATA,columns,values);
            ans = true;
        }
    }

    return ans;

}
#endif

void LocalInformationManager::fillPatientDatInformation(const QString &patient){

    patientReportInformation.clear();

    QStringList patientDirs;
    if (patient.isEmpty()) patientDirs = QDir(workingDirectory).entryList(QStringList(),QDir::Dirs | QDir::NoDotAndDotDot);
    else patientDirs << patient;

    for (qint32 d = 0; d < patientDirs.size(); d++){
        // Creating the source directory path.
        QString patuid = patientDirs.at(d);
        QString patDirName = workingDirectory + "/" + patuid;
        QDir patientDir(patDirName);

        //qWarning() << "CHECKING FOR" << patDirName;

        if (!patientDir.exists()){
            // This is not necessarily an error because not all new patients willt have a patient directory created.
            continue;
        }

        DatFileInfoInDir datInfo;
        datInfo.setDatDirectory(patientDir.path());
        //qWarning() << "Adding information for patuid" << patuid;
        patientReportInformation[patuid] = datInfo;

    }

    //printDBToConsole();
}

void LocalInformationManager::addDoctorData(const QString &druid,
                                            const QStringList &cols, const QStringList &values,
                                            const QString &password, bool hidden){

    // Doctor information needs to be checked against existing information to see if there is a change.
    QVariantMap drinfo;

    // Adding the dr information.
    for (qint32 i = 0; i < cols.size(); i++){
        drinfo[cols.at(i)] = values.at(i);
    }

    drinfo[DOCTOR_HIDDEN] = hidden;

    if (!localDB.value(DOCTOR_DATA).toMap().contains(druid)){
        // This is a new docotr.

        drinfo[DOCTOR_UPDATE] = true;
        drinfo[DOCTOR_VALID] = false; // Doctor is invalidated ONLY when created and when its hidden status is changed.
        drinfo[DOCTOR_HIDDEN] = false; // It should not be possible to CREATE a hidden Doctor. But just in case.
        drinfo[DOCTOR_PASSWORD] = password;
        drinfo[TDOCTOR_COL_UID] = druid; // This will ensure that each user is UNIQUE GLOBALLY.
        QVariantMap drdb = localDB.value(DOCTOR_DATA).toMap();
        drdb[druid] = drinfo;
        localDB[DOCTOR_DATA] = drdb;
    }
    else {
        // Doctor is existing. Checking all information except patient data
        bool update = false;
        QVariantMap drdb = localDB.value(DOCTOR_DATA).toMap();
        QVariantMap drmap = drdb.value(druid).toMap();

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
        drdb[druid] = drinfo;
        localDB[DOCTOR_DATA] = drdb;
    }

    //printDBToConsole();

    //qWarning() << "On Adding Dr Info UPDATE" << drinfo.value(DOCTOR_UPDATE).toBool() << " VALID " << drinfo.value(DOCTOR_VALID).toBool() << dr_uid;
    backupDB();
}

void LocalInformationManager::addPatientData(const QString &patient_uid, const QString &creator_uid, const QStringList &cols, const QStringList &values){

    QVariantMap patdb = localDB.value(PATIENT_DATA).toMap();
    //QVariantMap patients = drinfo.value(PATIENT_DATA).toMap();

    // Patient information to variant map.
    QVariantMap thispatient;
    for (qint32 i = 0; i < cols.size(); i++){
        thispatient[cols.at(i)] = values.at(i);
    }

    // Checking to see if updte is necessary
    if (patdb.contains(patient_uid)){
        QVariantMap basePatient = patdb.value(patient_uid).toMap();

        // Data needs to be saved, if replaced.
        thispatient[PATIENT_CREATOR] = basePatient.value(PATIENT_CREATOR);

        //qWarning() << "addPatientData" << patient_uid << "setting update to false";
        thispatient[PATIENT_UPDATE] = false;
        for (qint32 i = 0; i < cols.size(); i++){
            if (!basePatient.contains(cols.at(i))){
                //qWarning() << "addPatientData" << patient_uid << "setting update to true because of new data";
                thispatient[PATIENT_UPDATE] = true;
                break;
            }
            else if (basePatient.value(cols.at(i)).toString() != values.at(i)){
                //qWarning() << "addPatientData" << patient_uid << "setting update to true because of value change";
                thispatient[PATIENT_UPDATE] = true;
                break;
            }
        }
    }
    else{
        // This is a new patient
        thispatient[PATIENT_UPDATE] = true;
        thispatient[PATIENT_CREATOR] = creator_uid;
    }

    // Storing the new dr map in the local db.
    patdb[patient_uid] = thispatient;
    localDB[PATIENT_DATA] = patdb;

    // Finally saving the DB.
    return backupDB();

}

void LocalInformationManager::setUpdateFlagTo(bool flag){

    QVariantMap db;
    QStringList uids;

    db = localDB.value(DOCTOR_DATA).toMap();
    uids = db.keys();
    for (qint32 i = 0; i < uids.size(); i++){
        QVariantMap map = db.value(uids.at(i)).toMap();
        map[DOCTOR_UPDATE] = flag;
        db[uids.at(i)] = map;
    }
    localDB[DOCTOR_DATA] = db;


    db = localDB.value(PATIENT_DATA).toMap();
    uids = db.keys();
    for (qint32 i = 0; i < uids.size(); i++){
        QVariantMap map = db.value(uids.at(i)).toMap();
        map[PATIENT_UPDATE] = flag;
        db[uids.at(i)] = map;
    }
    localDB[PATIENT_DATA] = db;

    backupDB();
    //qWarning() << "AFTER SETTING THE FLAG TO" << flag;
    //printLocalDB();
}

LocalInformationManager::DisplayLists LocalInformationManager::getDoctorList(bool forceShow){
    DisplayLists ans;
    QStringList uids = localDB.value(DOCTOR_DATA).toMap().keys();
    for (qint32 i = 0; i < uids.size(); i++){
        QVariantMap drinfo = localDB.value(DOCTOR_DATA).toMap().value(uids.at(i)).toMap();
        if (isHidden(uids.at(i)) && !forceShow) continue;

        QString displayUID = uids.at(i);
        QStringList parts = uids.at(i).split("_");
        if (parts.size() == 3) displayUID = parts.last();
        ans.doctorNames << drinfo.value(TDOCTOR_COL_FIRSTNAME).toString() + " " + drinfo.value(TDOCTOR_COL_LASTNAME).toString()
                           + " (" + displayUID + ")";
        ans.doctorUIDs << uids.at(i);
    }
    return ans;
}

LocalInformationManager::DisplayLists LocalInformationManager::getPatientListForDoctor(const QString &druid, const QString &filter){


    DisplayLists ans;

    // Each time the funciton is called, the directory structure is reviewed. Necessary to know which patiens have pending reports.
    fillPatientDatInformation("");

    QStringList uids = localDB.value(PATIENT_DATA).toMap().keys();

    for (qint32 i = 0; i < uids.size(); i++){
        QVariantMap patinfo = localDB.value(PATIENT_DATA).toMap().value(uids.at(i)).toMap();
        QString cuid = patinfo.value(PATIENT_CREATOR).toString();

        //qWarning() << "For patient" << uids.at(i) << "Creator is " << cuid;

        QString duid = patinfo.value(TPATDATA_COL_DOCTORID).toString();

        // Unless druid is empty (show all patients) then data is only added for the patients that were either created or assigned to the druid
        if (!druid.isEmpty() && (duid != druid) && (cuid != druid)) continue;

        QString entryText = patinfo.value(TPATDATA_COL_FIRSTNAME).toString() + " " + patinfo.value(TPATDATA_COL_LASTNAME).toString();
        QString displayID = patinfo.value(TPATDATA_NONCOL_DISPLAYID).toString();
        if ( filter.isEmpty() || entryText.contains(filter,Qt::CaseInsensitive) || displayID.contains(filter,Qt::CaseInsensitive) ){
            ans.patientNames << entryText;
            ans.patientUIDs << uids.at(i);
            ans.patientDisplayIDs << displayID;

            QVariantMap dmap = localDB.value(DOCTOR_DATA).toMap().value(duid).toMap();
            ans.doctorNames <<  dmap.value(TDOCTOR_COL_FIRSTNAME).toString() + " " + dmap.value(TDOCTOR_COL_LASTNAME).toString();
            ans.doctorUIDs << duid;

            dmap = localDB.value(DOCTOR_DATA).toMap().value(cuid).toMap();
            ans.creatorNames <<  dmap.value(TDOCTOR_COL_FIRSTNAME).toString() + " " + dmap.value(TDOCTOR_COL_LASTNAME).toString();
            ans.doctorUIDs << duid;

        }
    }

    if (!ans.patientNames.isEmpty()) {
        for (qint32 i = 0; i < ans.patientUIDs.size(); i++){
            //qWarning() << "Is Ok for PAT: " << ans.patientUIDs.at(i);
            if (patientReportInformation.value(ans.patientUIDs.at(i)).hasPendingReports()) ans.patientISOKList << "false";
            else ans.patientISOKList << "true";
        }
        //qWarning() << "IS OK LIST" << isoklist;
    }

    //    qWarning() << "RETURNING FROM getPatientListForDoctor with:";
    //    qWarning() << ans.patientNames;
    //    qWarning() << ans.patientUIDs;
    //    qWarning() << ans.patientISOKList;
    //    qWarning() << ans.doctorNames;
    //    qWarning() << ans.doctorUIDs;

    return ans;
}

void LocalInformationManager::loadDB(QString eyeexpid, QString instUID){

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
    qint32 localDBVersion;
    localDB.clear();
    reader >> localDBVersion;
    reader >> localDB;
    file.close();

    if (!localDB.contains(DOCTOR_COUNTER)){

        // Remaking the DB into a by Patient DB + Doctor DB. If a patient is repeated it is added to the assigned doctor field
        QVariantMap drdb;
        QVariantMap patdb;

        QVariantMap bkpLocalDb = localDB;
        localDB.clear();
        localDB[DOCTOR_COUNTER] = 0;
        localDB[PATIENT_COUNTER] = 0;
        QHash<QString,QString> existingPatients;

        QStringList druids = bkpLocalDb.keys();

        for (qint32 i = 0; i < druids.size(); i++){
            QString druid = druids.at(i);
            QVariantMap drmap = bkpLocalDb.value(druid).toMap();
            QVariantMap patmap = drmap.value(PATIENT_DATA).toMap();

            // Only relevant doctor information isKept
            QVariantMap newDRMap;
            newDRMap[TDOCTOR_COL_LASTNAME] = drmap.value(TDOCTOR_COL_LASTNAME).toString();
            newDRMap[TDOCTOR_COL_FIRSTNAME] = drmap.value(TDOCTOR_COL_FIRSTNAME).toString();
            newDRMap[TDOCTOR_COL_EMAIL] = drmap.value(TDOCTOR_COL_EMAIL).toString();

            // Generating the new UID.
            QString numid = newDoctorID();

            newDRMap[TDOCTOR_COL_MEDICAL_INST] = instUID;
            newDRMap[TDOCTOR_COL_UID] = instUID + "_" + eyeexpid + "_"  + numid;
            // Forcing updates.
            newDRMap[DOCTOR_UPDATE] = true;
            newDRMap[DOCTOR_PASSWORD] = drmap.value(DOCTOR_PASSWORD);
            newDRMap[DOCTOR_VALID] = drmap.value(DOCTOR_VALID);
            newDRMap[DOCTOR_HIDDEN] = drmap.value(DOCTOR_HIDDEN);

            // Adding the doctor data to the doctor DB
            drdb[instUID + "_" + eyeexpid + "_" + numid] = newDRMap;

            // Adding all of the doctor's patients to the patient db
            QStringList patuids = patmap.keys();
            for (qint32 j = 0; j < patuids.size(); j++){

                QString patuid = patuids.at(j);

                if (existingPatients.contains(patuid)){
                    QString nid = existingPatients.value(patuid);
                    QVariantMap pdb = patdb.value(nid).toMap();
                    pdb[PATIENT_CREATOR] = instUID + "_" + eyeexpid + "_"  + numid;
                    patdb[nid] = pdb;
                    continue;
                }

                QString pnumid = newPatientID();
                QString newpatuid = instUID + "_" + eyeexpid + "_"  + pnumid;
                log.appendStandard("MAPPING: " + patuid + " ==> " + newpatuid);
                existingPatients[patuid] = newpatuid;

                QVariantMap pdb = patmap.value(patuid).toMap();
                pdb[TPATDATA_COL_DOCTORID] = instUID + "_" + eyeexpid + "_"  + numid;
                pdb[PATIENT_CREATOR] = instUID + "_" + eyeexpid + "_"  + numid;
                pdb[TPATDATA_COL_PUID] = newpatuid;
                pdb[TPATDATA_NONCOL_DISPLAYID] = patuid;
                pdb[PATIENT_UPDATE] = true;

                // Removing non important data
                pdb.remove(TPATDATA_COL_STATE);
                pdb.remove(TPATDATA_COL_CITY);
                pdb.remove(TPATDATA_COL_IDTYPE);

                patdb[newpatuid] = pdb;

            }
        }

        // When all is said and done, the information is stored in the local DB.
        localDB[PATIENT_DATA] = patdb;
        localDB[DOCTOR_DATA]  = drdb;
        qWarning() << "NO DOCTOR COUNTER FOUND";
        printDBToConsole();

    }
    else if (!localDB.contains(PATIENT_COUNTER)){
        localDB[PATIENT_COUNTER] = 0;
        QVariantMap drdb = localDB.value(DOCTOR_DATA).toMap();
        QVariantMap patdb = localDB.value(PATIENT_DATA).toMap();

        // Changing doctor ids
        QStringList keys; keys = drdb.keys();
        QHash<QString,QString> drmap;

        for (qint32 i = 0; i < keys.size(); i++){
            QString oldid = keys.at(i);
            QVariantMap drdata = drdb.value(oldid).toMap();
            QStringList parts = oldid.split("_");
            QString newid;
            if (parts.size() != 2){
                log.appendError("OLD DB WITH UNKNOWN DR ÏD: " + oldid);
                newid = oldid;
            }
            else{
                newid = instUID + "_" + eyeexpid + "_D" + parts.last();
            }
            drdata[DOCTOR_UPDATE] = true;
            drdata[DOCTOR_VALID] = true;
            drdata[TDOCTOR_COL_MEDICAL_INST] = instUID;
            drdata[TDOCTOR_COL_UID] = newid;
            drdb[newid] = drdata;
            drdb.remove(oldid);
            drmap[oldid] = newid;
        }

        // Changing patient data.
        keys = patdb.keys();
        for (qint32 i = 0; i < keys.size(); i++){
            QString oldid = keys.at(i);
            QVariantMap patdata = patdb.value(oldid).toMap();
            QString newid = instUID + "_" + eyeexpid + "_" + newPatientID();
            QString drassinged = patdata.value(TPATDATA_COL_DOCTORID).toString();
            QString drcreator  = patdata.value(PATIENT_CREATOR).toString();
            if (!drmap.contains(drassinged)) log.appendError("DB REPURPOSE: Could not find assigned doctor: " + drassinged + " for pat " + oldid);
            if (!drmap.contains(drcreator)) log.appendError("DB REPURPOSE: Could not find creatro doctor: " + drcreator + "for pat " + oldid);
            patdata[TPATDATA_COL_DOCTORID] = drmap.value(drassinged);
            patdata[PATIENT_CREATOR] = drmap.value(drcreator);
            patdata[PATIENT_UPDATE] = true;
            patdata[TPATDATA_COL_PUID] = newid;
            patdata[TPATDATA_NONCOL_DISPLAYID] = oldid;
            patdb.remove(oldid);
            patdb[newid] = patdata;
            log.appendStandard("MAPPING: " + oldid + " ==> " + newid);
        }

        localDB[DOCTOR_DATA] = drdb;
        localDB[PATIENT_DATA] = patdb;
        qWarning() << "NO PATIENT COUNTER FOUND";
        printDBToConsole();
    }

    //printDBToConsole();

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
    //printDBToConsole();
    writer << LOCAL_DB_VERSION;
    writer << localDB;
    file.close();

    // Backing up the DB.
    if (!backupDirectory.isEmpty()){
        QString bkpfile = backupDirectory + "/" + QString(LOCAL_DB);
        QFile(bkpfile).remove();
        if (!QFile::copy(dbfile,bkpfile)){
            log.appendError("LOCALDB: Failed to backup file " + dbfile);
        }
    }

}

void LocalInformationManager::printDBToConsole(){
    qWarning() << "DOCTOR COUNTER: " + localDB.value(DOCTOR_COUNTER).toString();
    qWarning() << "PATIENT COUNTER: " + localDB.value(PATIENT_COUNTER).toString();
    qWarning() << "VIEW ALL FLAG" << localDB.value(FLAG_VIEWALL).toBool();
    qWarning() << "PROTOCOLS" << localDB.value(PROTOCOLS).toStringList();
    QStringList maps;
    maps << DOCTOR_DATA << PATIENT_DATA;
    for (qint32 i = 0; i < maps.size(); i++){
        qWarning() << "   MAP " + maps.at(i);
        QVariantMap map = localDB.value(maps.at(i)).toMap();
        QStringList uids = map.keys();
        for (qint32 j = 0; j < uids.size(); j++){
            qWarning() << "   ENTRY:" + uids.at(j);
            QStringList keys = map.value(uids.at(j)).toMap().keys();
            for (qint32 k = 0; k < keys.size(); k++){
                qWarning() << "      " + keys.at(k) + ": " + map.value(uids.at(j)).toMap().value(keys.at(k)).toString();
            }
        }
    }
}

QString LocalInformationManager::newDoctorID(){
    qint32 drcounter = localDB.value(DOCTOR_COUNTER).toInt();
    QString id = QString::number(drcounter);
    while (id.length() < DR_ID_LENGTH) id = "0" + id;
    drcounter++;
    id = "D" + id;
    localDB[DOCTOR_COUNTER] = drcounter;
    return id;
}

QString LocalInformationManager::newPatientID(){
    qint32 drcounter = localDB.value(PATIENT_COUNTER).toInt();
    QString id = QString::number(drcounter);
    while (id.length() < PAT_ID_LENGTH) id = "0" + id;
    id = "P" + id;
    drcounter++;
    localDB[PATIENT_COUNTER] = drcounter;
    return id;
}

//************************************** Interface with Patient Report Information *****************************************

QStringList LocalInformationManager::getBindingUCFileListCompatibleWithSelectedBC(const QString &patuid, qint32 selectedBC) {
    return patientReportInformation[patuid].getBindingUCFileListCompatibleWithSelectedBC(selectedBC);
}

QStringList LocalInformationManager::getReportNameAndFileSet(const QString &patuid, const DatFileInfoInDir::ReportGenerationStruct &repgen){
    return patientReportInformation.value(patuid).getFileSetAndReportName(repgen);
}

QStringList LocalInformationManager::getReportNameAndFileSet(const QString &patuid, const QStringList &fileList){
    return patientReportInformation[patuid].getFileSetAndReportName(fileList);
}

QString LocalInformationManager::getDatFileFromIndex(const QString &patuid, qint32 index, qint32 whichList) const{
    return patientReportInformation.value(patuid).getDatFileNameFromSelectionDialogIndex(index,whichList);
}

//************************************** Interface with Protocol List *****************************************
bool LocalInformationManager::addProtocol(const QString &protocol){
    if (!localDB.contains(PROTOCOLS)){
        QStringList list; list << protocol;
        QVariantList valid; valid << true;
        localDB[PROTOCOLS] = list;
        localDB[PROTOCOL_VALID] = valid;
        backupDB();
        return true;
    }
    else if (localDB.value(PROTOCOLS).toStringList().contains(protocol)) return false;
    else{
        QStringList protocollist = localDB.value(PROTOCOLS).toStringList();
        QVariantList valid = localDB.value(PROTOCOL_VALID).toList();
        protocollist << protocol;
        valid << true;
        localDB[PROTOCOLS] = protocollist;
        localDB[PROTOCOL_VALID] = valid;
        backupDB();
        return true;
    }    
}

void LocalInformationManager::deleteProtocol(const QString &protocol){
    if (!localDB.contains(PROTOCOLS)) return;
    QStringList protocollist = localDB.value(PROTOCOLS).toStringList();
    QVariantList valid = localDB.value(PROTOCOL_VALID).toList();
    qint32 index = protocollist.indexOf(protocol);
    if (index > -1) valid[index] = false;
    localDB[PROTOCOL_VALID] = valid;
    backupDB();
}

QStringList LocalInformationManager::getProtocolList(bool full) const {
    QStringList list;
    if (!localDB.contains(PROTOCOLS)) return list;
    QStringList fullist = localDB.value(PROTOCOLS).toStringList();
    if (full) return fullist;
    for (qint32 i = 0; i < fullist.size(); i++){
        if (localDB.value(PROTOCOL_VALID).toList().at(i).toBool()) list << fullist.at(i);
    }
    return list;
}
