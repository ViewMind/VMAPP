#include "localinformationmanager.h"

const QString LocalInformationManager::PATIENT_DATA                          = "PATIENT_DATA";
const QString LocalInformationManager::PATIENT_CREATOR                       = "PATIENT_CREATOR";
const QString LocalInformationManager::PATIENT_UPDATE                        = "PATIENT_UPDATE";
const QString LocalInformationManager::PATIENT_COUNTER                       = "PATIENT_COUNTER";
const QString LocalInformationManager::DOCTOR_DATA                           = "DOCTOR_DATA";
const QString LocalInformationManager::DOCTOR_COUNTER                        = "DOCTOR_COUNTER";
const QString LocalInformationManager::DOCTOR_UPDATE                         = "DOCTOR_UPDATE";
const QString LocalInformationManager::DOCTOR_PASSWORD                       = "DOCTOR_PASSWORD";
const QString LocalInformationManager::DOCTOR_VALID                          = "DOCTOR_VALID";
const QString LocalInformationManager::DOCTOR_HIDDEN                         = "DOCTOR_HIDDEN";
const QString LocalInformationManager::EVALUATION_COUNTER                    = "EVALUATION_COUNTER";
const QString LocalInformationManager::FLAG_VIEWALL                          = "FLAG_VIEWALL";
const QString LocalInformationManager::PROTOCOLS                             = "PROTOCOLS";
const QString LocalInformationManager::PROTOCOL_VALID                        = "PROTOCOL_VALID";
const QString LocalInformationManager::REMAING_EVALS                         = "REMAING_EVALS";
const QString LocalInformationManager::PATIENT_MEDICAL_RECORDS               = "MEDICAL_RECORDS";
const QString LocalInformationManager::PATIENT_MEDICAL_RECORD_UP_TO_DATE     = "MED_RECS_UP_TO_DATE";

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

bool LocalInformationManager::createPatAndDrDBFiles(const QString &patid, const QString &drid){

    // Creating the doctor map
    QVariantMap drmap = localDB.value(DOCTOR_DATA).toMap().value(drid).toMap();
    ConfigurationManager drdata;
    QSet<QString> avoid;
    avoid << DOCTOR_UPDATE << DOCTOR_VALID << DOCTOR_PASSWORD << DOCTOR_HIDDEN;
    QStringList keys = drmap.keys();
    for (qint32 j = 0; j < keys.size(); j++){
        if (avoid.contains(keys.at(j))) continue;
        drdata.addKeyValuePair(keys.at(j),drmap.value(keys.at(j)));
    }

    // Creating the patient map
    ConfigurationManager patdata;
    QVariantMap patientMap = localDB.value(PATIENT_DATA).toMap().value(patid).toMap();
    avoid.clear();
    // The last two present problems ONLY for old local databases.
    avoid << PATIENT_UPDATE << TPATDATA_COL_PUID << PATIENT_CREATOR << TPATDATA_NONCOL_DISPLAYID
          << PATIENT_MEDICAL_RECORDS << PATIENT_MEDICAL_RECORD_UP_TO_DATE
          << TPATDATA_NONCOL_PROTOCOL << OLD_PUID_COLUMN << OLD_UID_COLUMN;
    QSet<QString> needToHash; needToHash << TPATDATA_COL_FIRSTNAME << TPATDATA_COL_LASTNAME;
    keys = patientMap.keys();
    for (qint32 k = 0; k < keys.size(); k++){
        if (avoid.contains(keys.at(k))) continue;
        QString value = patientMap.value(keys.at(k)).toString();
        if (needToHash.contains(keys.at(k))){
            value = QCryptographicHash::hash(value.toLatin1(),QCryptographicHash::Sha3_256).toHex();
        }
        patdata.addKeyValuePair(keys.at(k),value);
    }

    // Adding the Hashed patient ID.
    QString hash = QCryptographicHash::hash(patientMap.value(TPATDATA_COL_PUID).toString().toLatin1(),QCryptographicHash::Sha3_512).toHex();
    patdata.addKeyValuePair(TPATDATA_COL_PUID,hash);

    // Saving to the patient directory.
    QString patdir = DIRNAME_RAWDATA + QString("/") + patid + "/";
    if (!drdata.saveToFile(patdir + FILE_DOCDATA_DB,COMMON_TEXT_CODEC)){
        log.appendError("Could not create doctor data file: " + patdir + FILE_DOCDATA_DB);
        return false;
    }

    if (!patdata.saveToFile(patdir + FILE_PATDATA_DB,COMMON_TEXT_CODEC)){
        log.appendError("Could not create patient data file: " + patdir + FILE_PATDATA_DB);
        return false;
    }

    return true;
}

bool LocalInformationManager::createUpdateMedicalDBFile(const QString &patid){
    QVariantMap patientMap = localDB.value(PATIENT_DATA).toMap().value(patid).toMap();
    QVariantList recordsToUpdate = patientMap.value(PATIENT_MEDICAL_RECORD_UP_TO_DATE).toList();
    QVariantList medicalRecords = patientMap.value(PATIENT_MEDICAL_RECORDS).toList();

    if (recordsToUpdate.isEmpty()){
        log.appendError("No records to update for patient: " + patid);
        return false;
    }

    // Creating the structure to save to a configuration manager.
    QHash<QString,QStringList> valuesToAdd;
    QStringList normalColumns;
    QStringList jsonColumns;
    normalColumns   << TPATMEDREC_COL_DATE << TPATMEDREC_COL_PRESUMP_DIAGNOSIS << TPATMEDREC_COL_REC_INDEX;
    jsonColumns     << TPATMEDREC_COL_EVALS << TPATMEDREC_COL_MEDICATION << TPATMEDREC_COL_RNM;

    QSet<qint32> allreadyAdded;

    for (qint32 i = 0; i < recordsToUpdate.size(); i++){
        qint32 recordIndex = recordsToUpdate.at(i).toInt();

        // Due to the way the updating works several modifications to the same record
        // will add it multiple times to the update list. With this, the information is added only once.
        if (allreadyAdded.contains(recordIndex)) continue;

        if ((recordIndex < 0) || (recordIndex >= medicalRecords.size())){
            log.appendError("Attempting to get medical record: " + QString::number(recordIndex) + " from patient "
                            + patid + " but medical record list has a size of: " + QString::number(medicalRecords.size()));
            return false;
        }
        QVariantMap medRec = medicalRecords.at(recordIndex).toMap();
        //qWarning() << "Record Index" << recordIndex;
        //qWarning() << "   " << medRec;

        // Adding normal columns
        for (qint32 j = 0; j < normalColumns.size(); j++){
            QString col = normalColumns.at(j);
            if (medRec.contains(col)){
                valuesToAdd[col].append(medRec.value(col).toString());
            }
        }
        // Adding the record index itself.
        valuesToAdd[TPATMEDREC_COL_REC_INDEX].append(QString::number(recordIndex));

        // Adding json columns.
        for (qint32 j = 0; j < jsonColumns.size(); j++){
            QString col = jsonColumns.at(j);
            if (medRec.contains(col)){
                QVariantMap mapData = medRec.value(col).toMap();
                valuesToAdd[col].append(QVariantMap2JSONString(mapData));
            }
        }

        allreadyAdded << recordIndex;

    }

    // Adding everything to the configuration manager and then saving
    ConfigurationManager medRecordConf;
    QStringList keys; keys << normalColumns << jsonColumns;
    for (qint32 i = 0; i < keys.size(); i++){
        medRecordConf.addKeyValuePair(keys.at(i),valuesToAdd.value(keys.at(i)));
    }

    QString patdir = DIRNAME_RAWDATA + QString("/") + patid + "/";
    if (!medRecordConf.saveToFile(patdir + FILE_MEDRECORD_DB,COMMON_TEXT_CODEC)){
        log.appendError("Could save not medical record data file to " + patdir);
        return false;
    }

    return true;

}

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

void LocalInformationManager::addPatientMedicalRecord(const QString &patient_uid, const QVariantMap &medicalRecord, qint32 recordIndex){

    if (!localDB.value(PATIENT_DATA).toMap().contains(patient_uid)){
        log.appendError("Trying to append medical recordd to PUID: " + patient_uid + " that does not exists");
        return;
    }

    QVariantMap patientMap = localDB.value(PATIENT_DATA).toMap().value(patient_uid).toMap();
    QVariantList recordList;
    if (patientMap.contains(PATIENT_MEDICAL_RECORDS)){
        recordList = patientMap.value(PATIENT_MEDICAL_RECORDS).toList();
    }

    // Logic to actually saved the data and define the update.
    qint32 recordToUpdate = -1;
    if (recordIndex == -1) {
        recordList << medicalRecord; // -1 means that this is new record.
        recordToUpdate = recordList.size()-1;
    }
    else{
        // This is an update to an existing record.
        if ((recordIndex >= 0) && (recordIndex < recordList.size())){
            // We check if the records are different.
            if (recordList.at(recordIndex) != medicalRecord){
                recordList[recordIndex] = medicalRecord;
                recordToUpdate = recordIndex;
            }
        }
    }

    if (recordToUpdate != -1){
        // There was a change so the new list of records is saved.
        patientMap[PATIENT_MEDICAL_RECORDS] = recordList;

        // Then it's index is added to the update list to know which to send to the server on the next update.
        QVariantList toUpdateList;
        if (patientMap.contains(PATIENT_MEDICAL_RECORD_UP_TO_DATE)) toUpdateList = patientMap.value(PATIENT_MEDICAL_RECORD_UP_TO_DATE).toList();
        toUpdateList << recordToUpdate;
        patientMap[PATIENT_MEDICAL_RECORD_UP_TO_DATE] = toUpdateList;

        // Finally adding the modified patient map to the local DB
        QVariantMap patientData = localDB.value(PATIENT_DATA).toMap();
        patientData[patient_uid] = patientMap;
        localDB[PATIENT_DATA] = patientData;

        // And backing up said DB.
        backupDB();
    }

}

void LocalInformationManager::saveIDTable(const QString &fileName, const QStringList &tableHeaders){
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)){
        log.appendError("Could not open file " + fileName + " to save Table IDs");
        return;
    }

    QStringList rows;
    rows << tableHeaders.join(",");

    QVariantMap patdata = localDB.value(PATIENT_DATA).toMap();
    QStringList patids = patdata.keys();
    for (qint32 i = 0; i < patids.size(); i++){
        QString hash = QCryptographicHash::hash(patids.at(i).toLatin1(),QCryptographicHash::Sha3_512).toHex();
        QString did  = patdata.value(patids.at(i)).toMap().value(TPATDATA_NONCOL_DISPLAYID).toString();
        rows << did + "," + hash;
    }

    QTextStream writer(&file);
    writer << rows.join("\n");
    file.close();

    return;
}

void LocalInformationManager::cleanMedicalRecordUpdateFlag(const QString &patid){

    //qDebug() << "LIM: Cleaning update list for" << patid;

    QVariantMap allPatientData = localDB.value(PATIENT_DATA).toMap();
    if (!allPatientData.contains(patid)){
        log.appendError("Trying to clean the update flags of a non existing patient: " + patid);
        return;
    }
    QVariantMap patientMap = allPatientData.value(patid).toMap();
    patientMap[PATIENT_MEDICAL_RECORD_UP_TO_DATE] = QVariantList();
    allPatientData[patid] = patientMap;
    localDB[PATIENT_DATA] = allPatientData;

    // Changes were made so a backup is necessary
    backupDB();
}

qint32 LocalInformationManager::getRemainingEvals() const {
    if (localDB.contains(REMAING_EVALS)) return localDB.value(REMAING_EVALS).toInt();
    else return -1;
}

void LocalInformationManager::setRemainingEvals(qint32 remevals){
    localDB[REMAING_EVALS] = remevals;
    backupDB();
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
            QString patuid = ans.patientUIDs.at(i);
            if (patientReportInformation.value(patuid).hasPendingReports()) ans.patientISOKList << "false";
            else ans.patientISOKList << "true";

            // Getting if there were changes in the patient medical records.
            QVariantMap patmap = localDB.value(PATIENT_DATA).toMap().value(patuid).toMap();
            QString upToDate = "true";
            if (patmap.contains(PATIENT_MEDICAL_RECORD_UP_TO_DATE)){
                if (!patmap.value(PATIENT_MEDICAL_RECORD_UP_TO_DATE).toList().isEmpty()) {
                    //qDebug() << "Patient " << patuid << " has " << patmap.value(PATIENT_MEDICAL_RECORD_UP_TO_DATE).toList().size() << " to send";
                    upToDate = "false";
                }
            }

            ans.patientMedRecsUpToDateList << upToDate;

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

void LocalInformationManager::setWorkingFile(const QString &fileName){
    QFileInfo info(fileName);
    workingDirectory = info.path();
    loadDB("","",info.baseName() + "." + info.suffix());
}

void LocalInformationManager::loadDB(QString eyeexpid, QString instUID, QString fileName){

    Q_UNUSED(eyeexpid)
    Q_UNUSED(instUID)


    // Checking to see if the directory structure exists.
    QDir basedir(workingDirectory);
    if (!basedir.exists()){
        log.appendError("LOCALDB LOADING: Base Data directory: " + basedir.path() + " does not exist and it should, at this point");
        return;
    }

    QString dbfile = basedir.path() + "/" + fileName;

    QFile file(dbfile);

    if (file.exists()){
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

        if (!localDB.contains(EVALUATION_COUNTER)){
            localDB[EVALUATION_COUNTER] = 0;
        }

    }
    else{
        log.appendWarning("Set DB File: " + fileName + " does not exist");
        localDB[DOCTOR_COUNTER] = 0;
        localDB[PATIENT_COUNTER] = 0;
        localDB[EVALUATION_COUNTER] = 0;
        backupDB();
        return;
    }

    //printDBToConsole();

}

void LocalInformationManager::backupDB(){

    // Checking to see if the directory structure exists.
    QDir basedir(workingDirectory);
    if (!basedir.exists()){
        log.appendError("LOCALDB: Base Data directory: " + basedir.path() + " does not exist and it should, at this point");
    }

    QString dbfile = basedir.path() + "/" + FILE_LOCAL_DB;

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
        QString bkpfile = backupDirectory + "/" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm") + "_" + QString(FILE_LOCAL_DB);
        QFile(bkpfile).remove();
        if (!QFile::copy(dbfile,bkpfile)){
            log.appendError("LOCALDB: Failed to backup file " + dbfile);
        }
    }

}

void LocalInformationManager::printDBToConsole(){
    qWarning() << "REMAINIG EVALS: " + localDB.value(REMAING_EVALS).toString();
    qWarning() << "DOCTOR COUNTER: " + localDB.value(DOCTOR_COUNTER).toString();
    qWarning() << "PATIENT COUNTER: " + localDB.value(PATIENT_COUNTER).toString();
    qWarning() << "EVALUATION COUNTER" + localDB.value(EVALUATION_COUNTER).toString();
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

QString LocalInformationManager::printDBToString() const{
    QString ans;
    ans = ans +   "REMAINIG EVALS: " + localDB.value(REMAING_EVALS).toString() + "\n";
    ans = ans +   "DOCTOR COUNTER: " + localDB.value(DOCTOR_COUNTER).toString() + "\n";
    ans = ans +   "PATIENT COUNTER: " + localDB.value(PATIENT_COUNTER).toString()+ "\n";
    ans = ans +   "EVALUATION COUNTER: " + localDB.value(EVALUATION_COUNTER).toString()+ "\n";
    if (localDB.value(FLAG_VIEWALL).toBool()) ans = ans +   "VIEW ALL FLAG: true\n";
    else ans = ans +   "VIEW ALL FLAG: false\n";
    ans = ans +   "PROTOCOLS: '" + localDB.value(PROTOCOLS).toStringList().join("','")+ "'\n";
    QStringList maps;
    maps << DOCTOR_DATA << PATIENT_DATA;
    for (qint32 i = 0; i < maps.size(); i++){
        ans = ans +   "   MAP " + maps.at(i) + "\n";
        QVariantMap map = localDB.value(maps.at(i)).toMap();
        QStringList uids = map.keys();
        for (qint32 j = 0; j < uids.size(); j++){
            ans = ans +   "      ENTRY:" + uids.at(j) + "\n";
            QStringList keys = map.value(uids.at(j)).toMap().keys();
            for (qint32 k = 0; k < keys.size(); k++){
                ans = ans +   "         " + keys.at(k) + ": " + map.value(uids.at(j)).toMap().value(keys.at(k)).toString()+ "\n";
            }
        }
    }
    return ans;
}

QString LocalInformationManager::serialDoctorPatientString(const QString &serialized_map) const{
    QVariantMap patmap = localDB.value(PATIENT_DATA).toMap();
    QString ans = VariantMapSerializer::serializeTwoLevelVariantMap(serialized_map,patmap);
    return VariantMapSerializer::serializeTwoLevelVariantMap(ans,localDB.value(DOCTOR_DATA).toMap());
}

QHash<QString, QString> LocalInformationManager::getPatientHashedIDMap() const {
    QVariantMap map = localDB.value(PATIENT_DATA).toMap();
    QStringList uids = map.keys();
    QHash<QString,QString> ans;
    for (qint32 j = 0; j < uids.size(); j++){
        QVariantMap patdata = map.value(uids.at(j)).toMap();
        QString hash = QCryptographicHash::hash(patdata.value(TPATDATA_COL_PUID).toString().toLatin1(),QCryptographicHash::Sha3_512).toHex();
        QString patient_name = patdata.value(TPATDATA_COL_FIRSTNAME).toString() + " " + patdata.value(TPATDATA_COL_LASTNAME).toString();
        //qWarning() << "PATHASHIDMAP: " << patient_name << patdata.value(TPATDATA_COL_PUID) << hash;
        ans[hash] = patient_name;
    }
    return ans;
}

QVariantMap LocalInformationManager::getHashedIDPatientMap(QVariantMap hidmap) const {
    QVariantMap map = localDB.value(PATIENT_DATA).toMap();
    QStringList uids = map.keys();
    for (qint32 j = 0; j < uids.size(); j++){
        QVariantMap patdata = map.value(uids.at(j)).toMap();
        QString hash = QCryptographicHash::hash(patdata.value(TPATDATA_COL_PUID).toString().toLatin1(),QCryptographicHash::Sha3_512).toHex();
        hidmap[hash] = patdata.value(TPATDATA_COL_PUID);
    }
    return hidmap;
}

QString LocalInformationManager::newDoctorID(){
    qint32 drcounter = localDB.value(DOCTOR_COUNTER).toInt();
    QString id = QString::number(drcounter);
    while (id.length() < DR_ID_LENGTH) id = "0" + id;
    drcounter++;
    id = "D" + id;
    localDB[DOCTOR_COUNTER] = drcounter;
    backupDB();
    return id;
}

QString LocalInformationManager::newPatientID(){
    qint32 drcounter = localDB.value(PATIENT_COUNTER).toInt();
    QString id = QString::number(drcounter);
    while (id.length() < PAT_ID_LENGTH) id = "0" + id;
    id = "P" + id;
    drcounter++;
    localDB[PATIENT_COUNTER] = drcounter;
    backupDB();
    return id;
}

QString LocalInformationManager::newEvaluationID(){
    quint64 evalCounter = localDB.value(EVALUATION_COUNTER).toInt();
    QString id = QString::number(evalCounter);
    while (id.length() < EVAL_ID_LENGTH) id = "0" + id;
    id = "E" + id;
    evalCounter++;
    localDB[EVALUATION_COUNTER] = evalCounter;
    backupDB();
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

QString LocalInformationManager::QVariantMap2JSONString(const QVariantMap &map){
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(map);
    return QString(jsonDoc.toJson(QJsonDocument::Compact));
}
