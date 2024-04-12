#include "localdb.h"

// Main fields.
const char * LocalDB::MAIN_EVALUATOR_DATA                    = "evaluators";
const char * LocalDB::MAIN_SUBJECT_DATA                      = "subjects";
const char * LocalDB::MAIN_PROTOCOL                          = "main_protocol";
const char * LocalDB::MAIN_CHECKSUM                          = "hash_checksum";
const char * LocalDB::MAIN_PROCESSING_PARAMETERS             = "processing_parameters";
const char * LocalDB::MAIN_MEDICS                            = "medics";
const char * LocalDB::MAIN_QC_PARAMETERS                     = "qc_parameters";
const char * LocalDB::MAIN_APP_VERSION                       = "app_version";
const char * LocalDB::MAIN_APP_UPDATE_DELAY_COUNTER          = "update_delay_counter";
const char * LocalDB::MAIN_DB_VERSION                        = "local_db_version";
const char * LocalDB::MAIN_RECOVERY_PASSWORD                 = "recovery_password";
const char * LocalDB::MAIN_LAST_LOG_UPLOAD                   = "last_log_upload";
const char * LocalDB::MAIN_PREFERENCES                       = "preferences";
const char * LocalDB::MAIN_LAST_APP_DOWNLOADED               = "last_downloaded_app";
const char * LocalDB::MAIN_INSTANCE_ENABLED                  = "instance_enabled";
const char * LocalDB::MAIN_HMD_CHANGE                        = "hmd_change_sn";
const char * LocalDB::MAIN_AVAILABLE_EVALS                   = "available_evals";
const char * LocalDB::MAIN_ONGOING_EVALUATIONS               = "ongoing_evaluations";

// Evaluator fields
const char * LocalDB::APPUSER_NAME                           = "name";
const char * LocalDB::APPUSER_LASTNAME                       = "lastname";
const char * LocalDB::APPUSER_PASSWORD                       = "password";
const char * LocalDB::APPUSER_EMAIL                          = "email";
const char * LocalDB::APPUSER_VIEWMIND_ID                    = "viewmind_id";

// Subject Fields
const char * LocalDB::SUBJECT_NAME                           = "name";
const char * LocalDB::SUBJECT_LASTNAME                       = "lastname";
const char * LocalDB::SUBJECT_INSTITUTION_ID                 = "supplied_institution_id";
const char * LocalDB::SUBJECT_BIRTHDATE                      = "birthdate";
const char * LocalDB::SUBJECT_BIRTHCOUNTRY                   = "birthcountry";
const char * LocalDB::SUBJECT_YEARS_FORMATION                = "years_formation";
const char * LocalDB::SUBJECT_CREATION_DATE                  = "creation_date";
const char * LocalDB::SUBJECT_CREATION_DATE_INDEX            = "creation_date_index";
const char * LocalDB::SUBJECT_BDATE_DISPLAY                  = "bdate_display";
const char * LocalDB::SUBJECT_SORTABLE_NAME                  = "sortable_name";
const char * LocalDB::SUBJECT_GENDER                         = "gender";
const char * LocalDB::SUBJECT_LOCAL_ID                       = "local_id";
const char * LocalDB::SUBJECT_ASSIGNED_MEDIC                 = "assigned_medic";
const char * LocalDB::SUBJECT_EMAIL                          = "email";
const char * LocalDB::SUBJECT_FIELD_IN_SERVER_RESPONSE       = "subjects";
const char * LocalDB::SUBJECT_MODIFIED_FLAG                  = "modified_but_not_sent";
const char * LocalDB::SUBJECT_UPDATED_IDS                    = "updated_subjects_ids";

// Protocol Fields
const char * LocalDB::PROTOCOL_NAME           = "protocol_name";
const char * LocalDB::PROTOCOL_CREATION_DATE  = "creation_date";
const char * LocalDB::PROTOCOL_ID             = "protocol_id";

// Preferences.
const char * LocalDB::PREF_UI_LANG            = "ui_language";
const char * LocalDB::PREF_EXP_LANG           = "explanation_language";
const char * LocalDB::PREF_LAST_SEL_PROTOCOL  = "last_selected_protocol";

// On Going Evaluations Field
const char * LocalDB::EVAL_ID                      = "id";
const char * LocalDB::EVAL_STATUS                  = "status";
const char * LocalDB::EVAL_CLINICIAN               = "clinician";
const char * LocalDB::EVAL_EVALUATOR               = "evaluator";
const char * LocalDB::EVAL_ISO_DATE                = "date";
const char * LocalDB::EVAL_TIME                    = "time";
const char * LocalDB::EVAL_TYPE                    = "eval_type";
const char * LocalDB::EVAL_TIMESTAMP               = "timestamp";
const char * LocalDB::EVAL_TASKS                   = "tasks";
const char * LocalDB::EVAL_SUBJECT                 = "subject";
const char * LocalDB::EVAL_PROTOCOL                = "protocol";
const char * LocalDB::EVAL_COMMENT                 = "comment";

// Tasks subfields for ongoing evaluations
const char * LocalDB::TASK_TYPE                    = "task_code";
const char * LocalDB::TASK_FILE                    = "file";
const char * LocalDB::TASK_QCI                     = "qci";
const char * LocalDB::TASK_QCI_OK                  = "qci_ok";
const char * LocalDB::TASK_UPLOADED                = "uploaded";
const char * LocalDB::TASK_DISCARDED               = "discarded";



LocalDB::LocalDB()
{
}

QString LocalDB::getError() const {
    return error;
}

bool LocalDB::setApplicationVersion(const QString &version){
    if (this->data.contains(MAIN_APP_VERSION)){
        if (this->data.value(MAIN_APP_VERSION).toString() != version){
            this->data[MAIN_APP_VERSION] = version;
            this->data[MAIN_APP_UPDATE_DELAY_COUNTER] = MAX_ALLOWED_UPDATE_DELAYS;
            saveAndBackup();
            return true;
        }
    }
    else{
        this->data[MAIN_APP_VERSION] = version;
        this->data[MAIN_APP_UPDATE_DELAY_COUNTER] = MAX_ALLOWED_UPDATE_DELAYS;
        saveAndBackup();
        return false;
    }
    return false;
}


qint32 LocalDB::getRemainingUpdateDelays() const{
    if (this->data.contains(MAIN_APP_UPDATE_DELAY_COUNTER)) return this->data.value(MAIN_APP_UPDATE_DELAY_COUNTER).toInt();
    else return 0;
}

void LocalDB::deniedUpdate(){
    if (this->data.contains(MAIN_APP_UPDATE_DELAY_COUNTER)){
        qint32 counter = this->data.value(MAIN_APP_UPDATE_DELAY_COUNTER).toInt();
        counter--;
        this->data[MAIN_APP_UPDATE_DELAY_COUNTER] = counter;
        saveAndBackup();
    }
}

void LocalDB::replaceMedicKeys(){

    QVariantMap medicMap = this->data.value(MAIN_MEDICS).toMap();
    QVariantMap subjects = this->data.value(MAIN_SUBJECT_DATA).toMap();
    QStringList oldkeys = medicMap.keys();
    QStringList allsubjects = subjects.keys();

    for (qint32 i = 0; i < oldkeys.size(); i++){

        QVariantMap medic = medicMap.value(oldkeys.at(i)).toMap();
        QString email = medic.value(APPUSER_EMAIL).toString();

        //qDebug() << email << oldkeys.at(i);
        if (email == oldkeys.at(i)) continue; // Otherwise this will remove all medics.
        medicMap[email] = medic;
        medicMap.remove(oldkeys.at(i));

        for (qint32 j = 0; j < allsubjects.size(); j++){
            QVariantMap subject = subjects.value(allsubjects.at(j)).toMap();
            if (subject.value(SUBJECT_ASSIGNED_MEDIC).toString() == oldkeys.at(i)){
                subject[SUBJECT_ASSIGNED_MEDIC] = email;
                subjects[allsubjects.at(j)] = subject;
            }
        }

    }

    this->data[MAIN_MEDICS] = medicMap;
    this->data[MAIN_SUBJECT_DATA] = subjects;

    saveAndBackup();

}

bool LocalDB::isVersionUpToDate() const{
    if (!this->data.contains(MAIN_DB_VERSION)) return false;
    if (this->data.value(MAIN_DB_VERSION) != LOCALDB_VERSION) return false;
    return true;
}

bool LocalDB::setDBFile(const QString &dbfile, const QString &bkp_dir, bool pretty_print_db, bool disable_checksum){

    dbBKPdir = bkp_dir;
    this->dbfile = ""; // This way if something goes wrong the db file won't be set

    // Checking the backup directory exists or that it can be created.
    if (!QDir(bkp_dir).exists()){
        QDir().mkpath(bkp_dir);
    }

    enablePrettyPrint = pretty_print_db;

    if (!QDir(bkp_dir).exists()){
        error = "Could not create backup directory " + bkp_dir;
        return false;
    }

    QFile file(dbfile);

    // If the file doesn't exist, it can be because it has not been created yet.
    if (!file.exists()) {
        this->dbfile = dbfile;
        return true;
    }

    if (!file.open(QFile::ReadOnly)){
        error = "Could not open DB file: " + file.fileName() + " for reading even though it exists";
        return false;
    }

    // Loading the data.
    QJsonParseError json_error;

    QString val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8(),&json_error);
    if (doc.isNull()){
        error = "Error loading DB file: " + json_error.errorString();
        return false;
    }
    data = doc.object().toVariantMap();


    // We now recheck on the hash.
    if (!disable_checksum) {
        QString checksum = data.value(MAIN_CHECKSUM).toString();
        QString hash = computeDataHash();
        if (checksum != hash){
            // Somehow data got corrupted.
            error = "Failed checksum verification when loading the database.";
            return false;
        }
    }

    this->dbfile = dbfile;

    return true;

}

bool LocalDB::checkForLogUpload(){

    if (!data.contains(MAIN_LAST_LOG_UPLOAD)) return true;

    QDateTime last = data.value(MAIN_LAST_LOG_UPLOAD).toDateTime();
    QDateTime now  = QDateTime::currentDateTime();
    qint64 msElapsed = last.msecsTo(now);

    if (msElapsed >= LOG_UPLOAD_FREQ_IN_MS) return true;
    return false;

}

void LocalDB::setLogUploadMark(){
    data[MAIN_LAST_LOG_UPLOAD] = QDateTime::currentDateTime();
}


bool LocalDB::checkEvaluatorExists(const QString &evaluator) const {
    return data.value(MAIN_EVALUATOR_DATA,QVariantMap()).toMap().contains(evaluator);
}

bool LocalDB::addOrModifySubject(const QString &subject_id, QVariantMap subject_data){
    QVariantMap subject_data_map = data.value(MAIN_SUBJECT_DATA).toMap();
    if (!subject_data_map.contains(subject_id)){
        subject_data[SUBJECT_CREATION_DATE] = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm");
    }
    else{
        subject_data[SUBJECT_CREATION_DATE] = subject_data_map.value(subject_id).toMap().value(SUBJECT_CREATION_DATE).toString();
    }
    subject_data[SUBJECT_LOCAL_ID] = subject_id;
    subject_data[SUBJECT_MODIFIED_FLAG] = true;
    subject_data_map[subject_id] = subject_data;
    data[MAIN_SUBJECT_DATA] = subject_data_map;
    // We save after ANY modification;
    return saveAndBackup();
}

bool LocalDB::markSubjectsAsUpdated(const QVariantList &subject_ids){

    QVariantMap subject_data_map = data.value(MAIN_SUBJECT_DATA).toMap();
    for (qint32 i = 0; i < subject_ids.size(); i++){

        QString subject_id = subject_ids.at(i).toString();

        if (subject_data_map.contains(subject_id)){
            QVariantMap subject = subject_data_map.value(subject_id).toMap();
            subject[SUBJECT_MODIFIED_FLAG] = false;
            subject_data_map[subject_id] = subject;
            //qDebug() << "MARKING " << subject_id << " AS UPDATED";
        }
    }
    data[MAIN_SUBJECT_DATA] = subject_data_map;
    return saveAndBackup();
}

bool LocalDB::modifyAssignedMedicToSubject(const QString &subject_id, const QString &medic){
    QVariantMap subject_data_map = data.value(MAIN_SUBJECT_DATA).toMap();
    if (!subject_data_map.contains(subject_id)) return false;
    QVariantMap subject_data = subject_data_map.value(subject_id).toMap();

    subject_data[SUBJECT_ASSIGNED_MEDIC] = medic;

    subject_data_map[subject_id] = subject_data;
    data[MAIN_SUBJECT_DATA] = subject_data_map;
    return saveAndBackup();
}

bool LocalDB::addOrModifyEvaluator(const QString &email, const QString &oldemail, QVariantMap evaluator_data){
    // Makign sure the password is NOT accidentally set here.
    if (evaluator_data.contains(APPUSER_PASSWORD)){
        evaluator_data.remove(APPUSER_PASSWORD);
    }
    // If the user exists making sure that the password is not overwritten.
    QVariantMap evaluator_map = data.value(MAIN_EVALUATOR_DATA).toMap();
    QString password;
    if (evaluator_map.contains(oldemail)){
        password = evaluator_map.value(oldemail).toMap().value(APPUSER_PASSWORD).toString();
        // We remove the old email user, just in case as this will avoid creating two users when changing email.
        evaluator_map.remove(oldemail);
    }
    evaluator_data[APPUSER_PASSWORD] = password;
    evaluator_data[APPUSER_EMAIL] = email;
    evaluator_map[email] = evaluator_data;
    data[MAIN_EVALUATOR_DATA] = evaluator_map;

    // We save after ANY modification;
    return saveAndBackup();
}

bool LocalDB::setPasswordForEvaluator(const QString &email, const QString &plaintext_password){
    if (!data.value(MAIN_EVALUATOR_DATA).toMap().contains(email)) return false;
    QVariantMap evaluator_map = data.value(MAIN_EVALUATOR_DATA).toMap();
    QVariantMap evaluator_data = evaluator_map.value(email).toMap();
    evaluator_data[APPUSER_PASSWORD] = QString(QCryptographicHash::hash(plaintext_password.toUtf8(),QCryptographicHash::Sha3_512).toHex());

    //qDebug() << "Saving email" << email << " and password" << plaintext_password;
    //qDebug() << evaluator_data.value(EVALUATOR_PASSWORD).toString();

    evaluator_map[email] = evaluator_data;
    data[MAIN_EVALUATOR_DATA] = evaluator_map;

    // We save after ANY modification;
    return saveAndBackup();
}

QString LocalDB::getSubjectFieldValue(const QString &subject_id,const QString &field) const {
    //qDebug() << "Searching for " << subject_id << "field" << field;
    if (data.value(MAIN_SUBJECT_DATA).toMap().contains(subject_id)){
        return data.value(MAIN_SUBJECT_DATA).toMap().value(subject_id).toMap().value(field).toString();
    }
    return "";
}

QVariantMap LocalDB::getEvaluatorData(const QString &evaluator) const{
    return data.value(MAIN_EVALUATOR_DATA,QVariantMap()).toMap().value(evaluator,QVariantMap()).toMap();
}

QVariantMap LocalDB::getSubjectData(const QString &subjectID) const{
    return data.value(MAIN_SUBJECT_DATA,QVariantMap()).toMap().value(subjectID,QVariantMap()).toMap();
}

qint32 LocalDB::getSubjectCount() const{
    return static_cast<qint32>(data.value(MAIN_SUBJECT_DATA,QVariantMap()).toMap().size());
}

QVariantMap LocalDB::getSubjectDataByInternalID(const QString &internalID) const{
    QVariantMap subjects = data.value(MAIN_SUBJECT_DATA,QVariantMap()).toMap();
    QStringList keys = subjects.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        QVariantMap subj = subjects.value(keys.at(i)).toMap();
        if (subj.value(SUBJECT_INSTITUTION_ID).toString() == internalID){
            return subj;
        }
    }
    return QVariantMap();
}

bool LocalDB::addProtocol(const QString &protocol_name, const QString &protocol_id, bool edit){
    QVariantMap protocols = data.value(MAIN_PROTOCOL,QVariantMap()).toMap();

    QVariantMap newProtocol;

    if (!edit) {
        // If edit is false the protocol is new. So it can't exist.
        if (protocols.contains(protocol_id)) return false;
    }
    else {
        // If edit is true, the protocols is being eedited and it MUST exist.
        if (!protocols.contains(protocol_id)) return false;
        newProtocol = protocols.value(protocol_id).toMap();
    }

    newProtocol[PROTOCOL_NAME] = protocol_name;

    // Creation date is ONLY set when being created.
    if (!edit) {
        newProtocol[PROTOCOL_CREATION_DATE] = QDate::currentDate().toString("yyyy-MM-dd");
        newProtocol[PROTOCOL_ID] = protocol_id;
    }

    // Storing and saving the new protocol information.
    protocols[protocol_id] = newProtocol;
    data[MAIN_PROTOCOL] = protocols;
    return saveAndBackup();
}

bool LocalDB::removeProtocol(const QString &protocol_id){
    QVariantMap protocols = data.value(MAIN_PROTOCOL,QVariantMap()).toMap();
    if (!protocols.contains(protocol_id)) return true;

    protocols.remove(protocol_id);

    data[MAIN_PROTOCOL] = protocols;
    return saveAndBackup();
}

QVariantMap LocalDB::getProtocolList() const {
    return data.value(MAIN_PROTOCOL,QVariantMap()).toMap();
}

bool LocalDB::processingParametersPresent() const {
    return data.contains(MAIN_PROCESSING_PARAMETERS);
}

bool LocalDB::setRecoveryPasswordFromServerResponse(const QVariantMap &response){
    if (!response.contains(APINames::ReturnDataFields::REC_PASSWD)){
        error = "Expected field: " + QString(APINames::ReturnDataFields::REC_PASSWD) + " but it wasn't found";
        return false;
    }

    QString hash = response.value(APINames::ReturnDataFields::REC_PASSWD).toString();
    if (hash != data.value(MAIN_RECOVERY_PASSWORD,"").toString()){
        data[MAIN_RECOVERY_PASSWORD] = response.value(APINames::ReturnDataFields::REC_PASSWD).toString();
        return saveAndBackup();
    }
    else return true;
}


bool LocalDB::setMedicInformationFromRemote(const QVariantMap &response){
    if (!response.contains(APINames::Medics::NAME)){
        error = "Expected medics field: " + QString(APINames::Medics::NAME) + " but it wasn't found";
        return false;
    }

    QVariantList allmedics = response.value(APINames::Medics::NAME).toList();

    QStringList tocheck; tocheck << APINames::Medics::EMAIL << APINames::Medics::FNAME << APINames::Medics::KEYID << APINames::Medics::LASTNAME;

    QVariantMap local_medics = data.value(MAIN_MEDICS).toMap();

    // We need to compute a checksum for medics to know when to save.
    QString checksum(QCryptographicHash::hash(Debug::QVariantMapToString(local_medics).toUtf8(),QCryptographicHash::Sha3_512).toHex());

    // Storing the server medics.
    QVariantMap serverMedics;

    for (qint32 i = 0; i < allmedics.size(); i++){

        QVariantMap temp = allmedics.at(i).toMap();

        for (qint32 i = 0; i < tocheck.size(); i++){
            if (!temp.contains(tocheck.at(i))){
                error = "Medical record " + Debug::QVariantMapToString(temp) + " is missing field: " + tocheck.at(i);
                return false;
            }
        }

        QVariantMap medic;
        QString keyid = temp.value(APINames::Medics::KEYID).toString();
        QString email = temp.value(APINames::Medics::EMAIL).toString();
        medic.insert(APPUSER_EMAIL,email);
        medic.insert(APPUSER_LASTNAME,temp.value(APINames::Medics::LASTNAME));
        medic.insert(APPUSER_NAME,temp.value(APINames::Medics::FNAME));
        medic.insert(APPUSER_PASSWORD,"");
        medic.insert(APPUSER_VIEWMIND_ID, keyid);
        //qDebug() << "Storing at keyid" << keyid;
        //std::cout << "##############################" << std::endl;
        //std::cout << "Keyid " << keyid.toStdString() << std::endl;
        //Globals::Debug::prettpPrintQVariantMap(temp);
        //std::cout << "------------------------------" << std::endl;
        serverMedics[email] = medic;

    }

    QString checksum2(QCryptographicHash::hash(Debug::QVariantMapToString(serverMedics).toUtf8(),QCryptographicHash::Sha3_512).toHex());

    if (checksum == checksum2) return true;  // No changes, nothing to do

    data[MAIN_MEDICS] = serverMedics;
    return saveAndBackup();
}

qint32 LocalDB::mergePatientDBFromRemote(const QVariantMap &response){

    if (!response.contains(SUBJECT_FIELD_IN_SERVER_RESPONSE)){
        error = "Expected subjects field but it wasn't found";
        return -1;
    }

    // We check subject by subject and we only add if the IS is not there.
    QVariantMap newSubjectMap = response.value(SUBJECT_FIELD_IN_SERVER_RESPONSE).toMap();
    QStringList subject_ids = newSubjectMap.keys();

    qint32 ret = 0;

    QVariantMap currentSubjects = data.value(MAIN_SUBJECT_DATA).toMap();
    for (qint32 i = 0; i < subject_ids.size(); i++){
        QString id = subject_ids.at(i);
        if (!currentSubjects.contains(id)){
            QVariantMap record = newSubjectMap.value(id).toMap();
            // Subject creation date is local. If we don't do this, it won't appear.
            record[SUBJECT_CREATION_DATE] = QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm");
            currentSubjects[id] = record;
            ret++;
        }
        else {
            // We need to check if we need to update it or not.
            QVariantMap patient = currentSubjects.value(id).toMap();
            if (!patient.value(SUBJECT_MODIFIED_FLAG,false).toBool()){
                // This can be false if the flag doesn't exist or it does exist and it is set to false.
                // We don't increase the counter as this is NOT a new record. But rather an update.
                // Finally we only replace the fields with the current ones. We don't overwrite

                QStringList keys_to_replace = newSubjectMap.value(id).toMap().keys();
                QVariantMap currentRecord = currentSubjects.value(id).toMap();
                for (qint32 i = 0; i < keys_to_replace.size(); i++){
                    currentRecord[keys_to_replace.at(i)] = newSubjectMap.value(id).toMap().value(keys_to_replace.at(i));
                }
                currentSubjects[id] = currentRecord;
            }
            else {
                qDebug() << "Skipping SUBJECT as it is marked to be updated";
            }
        }
    }

    data[MAIN_SUBJECT_DATA] = currentSubjects;
    if (!saveAndBackup()) return -1;
    return ret;

}

bool LocalDB::setProcessingParametersFromServerResponse(const QVariantMap &response){

    if (!response.contains(APINames::ReturnDataFields::PROC_PARAMS)){
        error = "Expected processing parameters field: " + QString(APINames::ReturnDataFields::PROC_PARAMS) + " but it wasn't found";
        return false;
    }

    QStringList shouldBeThere;
    shouldBeThere << VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW << VMDC::ProcessingParameter::LATENCY_ESCAPE_RADIOUS
                  << VMDC::ProcessingParameter::SAMPLE_FREQUENCY
                  << VMDC::ProcessingParameter::GAZE_ON_CENTER_RADIOUS;

    QVariantMap pp = response.value(APINames::ReturnDataFields::PROC_PARAMS).toMap();

    QStringList serverkeys = pp.keys();
    for (qint32 i = 0; i < serverkeys.size(); i++){
        shouldBeThere.removeOne(serverkeys.at(i));
    }

    if (!shouldBeThere.isEmpty()){
        error = "The following processing parameters were missing from server response: " + shouldBeThere.join(",");
        return false;
    }

    data[MAIN_PROCESSING_PARAMETERS] = pp;
    return saveAndBackup();
}

bool LocalDB::setQCParametersFromServerResponse(const QVariantMap &response){

    if (!response.contains(APINames::ReturnDataFields::FREQ_PARAMS)){
        error = "Expected processing parameters field: " + QString(APINames::ReturnDataFields::FREQ_PARAMS) + " but it wasn't found";
        return false;
    }


    QStringList shouldBeThere;
    shouldBeThere << VMDC::QCGlobalParameters::DQI_THRESHOLD;

    QVariantMap qc_server = response.value(APINames::ReturnDataFields::FREQ_PARAMS).toMap();
    QVariantMap qc;

    //Debug::prettpPrintQVariantMap(qc);

    // We are only interested in the required fields. Any other are ignored.
    QStringList missing;
    for (qint32 i = 0; i < shouldBeThere.size(); i++){
        if (!qc_server.contains(shouldBeThere.at(i))){
            missing << shouldBeThere.at(i);
        }
        else {
            qc[shouldBeThere.at(i)] = qc_server[shouldBeThere.at(i)];
        }
    }

    // We make sure to print all missing fields.
    if (!missing.isEmpty()){
        error = "The following qc parameters were missing from server response: " + missing.join(",");
        return false;

    }

    // All checks out, we can save it as is.
    data[MAIN_QC_PARAMETERS] = qc;

    return saveAndBackup();
}


QVariantMap LocalDB::getProcessingParameters() const {
    return data.value(MAIN_PROCESSING_PARAMETERS).toMap();
}

bool LocalDB::passwordCheck(const QString &email, const QString &plaintext_password){
    //qDebug() << "Checking if data" << data;
    //qDebug() << "Has evaluator" << email;
    if (!data.value(MAIN_EVALUATOR_DATA).toMap().contains(email)) return false;
    QString reference = data.value(MAIN_EVALUATOR_DATA).toMap().value(email).toMap().value(APPUSER_PASSWORD).toString();
    QString recovery_password = data.value(MAIN_RECOVERY_PASSWORD).toString();

    //QString hash = QString(QCryptographicHash::hash(plaintext_password.toUtf8(),QCryptographicHash::Sha3_512).toHex());
    //qDebug() << "Checking for email " << email << plaintext_password;
    //qDebug() << "Hashed" << hash;
    //qDebug() << "Reference" << reference;
    //qDebug() << "Compare" << (reference == hash);
    //qDebug() << "Recovery password" << recovery_password;

    QString hash = QString(QCryptographicHash::hash(plaintext_password.toUtf8(),QCryptographicHash::Sha3_512).toHex());
    return (reference == hash) || (recovery_password == hash);

}


QVariantMap LocalDB::getDisplaySubjectList(QString filter, const QStringList &months){
    QVariantMap ans;

    // All matches are done in lower case.
    filter = filter.toLower();

    QVariantMap subdata = data.value(MAIN_SUBJECT_DATA,QVariantMap()).toMap();
    QStringList subject_ids = subdata.keys();

    for(qint32 i = 0; i < subject_ids.size(); i++){
        if (filterMatchSubject(subdata.value(subject_ids.at(i)).toMap(),filter)){
            QVariantMap map = subdata.value(subject_ids.at(i)).toMap();

            // Adding the creation date sorting index value. This is required due to the very very bad way in which I store the dates.
            //map[SUBJECT_CREATION_DATE_INDEX] = QDateTime::fromString(map.value(SUBJECT_CREATION_DATE).toString(),"dd/MM/yyyy HH:mm").toSecsSinceEpoch();

            // Building the display date: Day 3LetterMonth Year.
            QString bdate = buildDisplayBirthDate(map.value(SUBJECT_BIRTHDATE).toString(),months);
            map[SUBJECT_BDATE_DISPLAY] = bdate;

            QString lname = map.value(SUBJECT_LASTNAME).toString();
            QString fname = map.value(SUBJECT_NAME).toString();
            if ( (lname == "") && (fname == "") ){
                map[SUBJECT_SORTABLE_NAME] = "";
            }
            else if (lname == "") {
                map[SUBJECT_SORTABLE_NAME] = fname;
            }
            else if (fname == ""){
                map[SUBJECT_SORTABLE_NAME] = lname;
            }
            else {
                map[SUBJECT_SORTABLE_NAME] = lname + ", " + fname;
            }

            ans[subject_ids.at(i)] = map;
        }
    }

    return ans;
}

QString LocalDB::buildDisplayBirthDate(const QString &iso_bdate, const QStringList &months) const {
    QStringList bdate_parts = iso_bdate.split("-");
    QString bdate = "";
    if (bdate_parts.size() == 3){
        qint32 monthIndex = bdate_parts.at(1).toInt();
        monthIndex--;
        bdate = " UKN ";
        if ((monthIndex >= 0) && (monthIndex < months.size())){
            bdate = " " + months.at(monthIndex) + " ";
        }
        bdate = bdate_parts.last() + bdate + bdate_parts.first();
    }
    return bdate;
}

QVariantMap LocalDB::getMedicDisplayList() const {
    QVariantMap ans;
    QStringList keys = data.value(MAIN_MEDICS).toMap().keys();
    for (qint32 i = 0; i < keys.size(); i++){
        QVariantMap medicinfo = data.value(MAIN_MEDICS).toMap().value(keys.at(i)).toMap();
        ans[keys.at(i)] = medicinfo.value(APPUSER_LASTNAME).toString() + ", " + medicinfo.value(APPUSER_NAME).toString();
    }
    return ans;
}

QVariantMap LocalDB::getMedicData(const QString &key) const{

    //qDebug() << "Getting medic info " << key;
    //Debug::prettpPrintQVariantMap(data.value(MAIN_MEDICS).toMap());

    return data.value(MAIN_MEDICS).toMap().value(key).toMap();
}

QVariantMap LocalDB::getSubjectDataToUpdate() const {

    QVariantMap all_subject_data = data.value(MAIN_SUBJECT_DATA).toMap();
    QStringList subject_ids = all_subject_data.keys();
    QVariantMap to_update;

    for (qint32 i = 0; i < subject_ids.size(); i++){

        // Creating the subject data. Each record is created EXACTLY as it is for the study file, so the same methodology can be used to updated it in the server.
        QVariantMap record;
        QString subject_id = subject_ids.at(i);
        QVariantMap subject_data = all_subject_data.value(subject_id).toMap();

        // We only add it if it has been modified.
        if (subject_data.value(SUBJECT_MODIFIED_FLAG).toBool()){
            record.insert(VMDC::SubjectField::BIRTH_COUNTRY,subject_data.value(LocalDB::SUBJECT_BIRTHCOUNTRY));
            record.insert(VMDC::SubjectField::BIRTH_DATE,subject_data.value(LocalDB::SUBJECT_BIRTHDATE));
            record.insert(VMDC::SubjectField::GENDER,subject_data.value(LocalDB::SUBJECT_GENDER));
            record.insert(VMDC::SubjectField::INSTITUTION_PROVIDED_ID,subject_data.value(LocalDB::SUBJECT_INSTITUTION_ID));
            record.insert(VMDC::SubjectField::LASTNAME,subject_data.value(LocalDB::SUBJECT_LASTNAME));
            record.insert(VMDC::SubjectField::LOCAL_ID,subject_id);
            record.insert(VMDC::SubjectField::NAME,subject_data.value(LocalDB::SUBJECT_NAME));
            record.insert(VMDC::SubjectField::YEARS_FORMATION,subject_data.value(LocalDB::SUBJECT_YEARS_FORMATION));
            record.insert(VMDC::SubjectField::EMAIL,subject_data.value(LocalDB::SUBJECT_EMAIL));
            to_update[subject_id] = record;
            // qDebug() << "Adding subject " << subject_id << " to update";
        }

    }

    return to_update;

}

QStringList LocalDB::getUsernameEmails(bool withname) const {
    if (withname){

        QStringList emails = data.value(MAIN_EVALUATOR_DATA).toMap().keys();
        for (qint32 i = 0; i < emails.size(); i++){
            QString email = emails.at(i);
            QString name = data.value(MAIN_EVALUATOR_DATA).toMap().value(email).toMap().value(APPUSER_NAME).toString();
            QString lname = data.value(MAIN_EVALUATOR_DATA).toMap().value(email).toMap().value(APPUSER_LASTNAME).toString();
            emails[i] = lname + ", " + name + " (" + email + ")";
        }
        return emails;

    }
    else return data.value(MAIN_EVALUATOR_DATA).toMap().keys();
}

QVariantMap LocalDB::getQCParameters() const {
    return data.value(MAIN_QC_PARAMETERS).toMap();
}

bool LocalDB::filterMatchSubject(const QVariantMap &subject, const QString &filter){
    if (filter == "") return true;
    QString name = subject.value(SUBJECT_NAME).toString().toLower();
    QString lastname = subject.value(SUBJECT_LASTNAME).toString().toLower();
    QString instid = subject.value(SUBJECT_INSTITUTION_ID).toString().toLower();

    // If any of the identifiers contain the string this is a match
    return (name.contains(filter) || lastname.contains(filter) || instid.contains(filter));

}

bool LocalDB::saveAndBackup(){

    if (this->dbfile == ""){
        error = "Attempting to save the DB File withouth it being set";
        return false;
    }

    // Computing the hash with no checksum and then storing the checksum.
    QString previoushash = data.value(MAIN_CHECKSUM).toString();
    data[MAIN_DB_VERSION] = LOCALDB_VERSION; // This is done always, just in case.
    data[MAIN_CHECKSUM] = computeDataHash();

    // If hashes are the same, then nothing changed and there is no point in saving and or creating another backup.
    if (data.value(MAIN_CHECKSUM).toString() == previoushash) return true;

    // Opening the file for writing.
    QFile file (dbfile);
    if (!file.open(QFile::WriteOnly)){
        error = "Could not DB open " + file.fileName() + " for writing";
        return false;
    }

    // Converting to a compact JSON Array
    QJsonDocument json = QJsonDocument::fromVariant(data);

    QByteArray data_to_store;
    if (enablePrettyPrint){
        data_to_store = json.toJson(QJsonDocument::Indented);
    }
    else{
        data_to_store = json.toJson(QJsonDocument::Compact);
    }

    // Storing JSON Array in the db file.
    QTextStream writer(&file);
    writer << data_to_store;
    file.close();

    // Creating a bkp.
    QString bkpfile = dbBKPdir + "/bkp_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss") + ".dat";
    QFile::copy(dbfile,bkpfile);

    return true;

}


QString LocalDB::computeDataHash(){
    data[MAIN_CHECKSUM] = "";
    QJsonDocument json = QJsonDocument::fromVariant(data);
    QString hash;
    if (enablePrettyPrint){
        hash = QString(QCryptographicHash::hash(json.toJson(QJsonDocument::Indented),QCryptographicHash::Sha3_512).toHex());
    }
    else{
        hash = QString(QCryptographicHash::hash(json.toJson(QJsonDocument::Compact),QCryptographicHash::Sha3_512).toHex());
    }
    return hash;
}


void LocalDB::updatesToPreviousDBVersions(){

    if (data[MAIN_DB_VERSION].toInt() == 2){

        // DB version 2 requires reformatting the protocol list.
        QStringList protocolList = data[MAIN_PROTOCOL].toStringList();

        QVariantMap protocols;
        for (qint32 i = 0; i < protocolList.size(); i++){
            QVariantMap protocol_info;
            protocol_info[PROTOCOL_NAME] = protocolList.at(i);
            protocol_info[PROTOCOL_CREATION_DATE] = QDate::currentDate().toString("yyyy-MM-dd");
            protocol_info[PROTOCOL_ID] = protocolList.at(i);
            protocols[protocolList.at(i)] = protocol_info;
        }

        data[MAIN_PROTOCOL] = protocols;

    }



}

////////////////////////////// Update Help Check ///////////////////////////////

QString LocalDB::getLastDownloadedApp() const{
    if (data.contains(MAIN_LAST_APP_DOWNLOADED)) return data.value(MAIN_LAST_APP_DOWNLOADED).toString();
    else return "";
}

bool LocalDB::setLastDownloadedApp(const QString &version){
    data[MAIN_LAST_APP_DOWNLOADED] = version;
    return saveAndBackup();
}

////////////////////////////// HMD Change ///////////////////////////////
QString LocalDB::getHMDChangeSN() const {
    if (data.contains(MAIN_HMD_CHANGE)) return data.value(MAIN_HMD_CHANGE).toString();
    else return "";
}

bool LocalDB::setHMDChangeSN(const QString &sn){
    data[MAIN_HMD_CHANGE] = sn;
    return saveAndBackup();
}

////////////////////////////// Studies to Hide ///////////////////////////////
QVariantMap LocalDB::getAvailableEvals() const {
    if (data.contains(MAIN_AVAILABLE_EVALS)) return data.value(MAIN_AVAILABLE_EVALS).toMap();
    else return QVariantMap();
}

void LocalDB::setAvailableEvaluations(const QVariantMap &available) {
    data[MAIN_AVAILABLE_EVALS] = available;
    saveAndBackup();
}

////////////////////////////// Instance Enable Status ///////////////////////////////
void LocalDB::setInstanceEnableTo(bool enabled){
    data[MAIN_INSTANCE_ENABLED] = enabled;
    saveAndBackup();
}

bool LocalDB::isInstanceEnabled() const {
    if (data.contains(MAIN_INSTANCE_ENABLED)) return data.value(MAIN_INSTANCE_ENABLED).toBool();
    // The default here is true. Playing it safe.
    else return true;
}

////////////////////////////// PREFERENCES ///////////////////////////////
bool LocalDB::setPreference(const QString &preference, const QVariant &variant){
    QVariantMap map;
    if (data.contains(MAIN_PREFERENCES)){
        map = data.value(MAIN_PREFERENCES).toMap();
    }
    //qDebug() << "Setting preference" << preference << "to" << variant;
    map[preference] = variant;
    data[MAIN_PREFERENCES] = map;
    return saveAndBackup();
}

QVariant LocalDB::getPreference(const QString &preference, const QString &retAndStoreIfDoenstExist) {
    QVariantMap map;
    if (data.contains(MAIN_PREFERENCES)){
        map = data.value(MAIN_PREFERENCES).toMap();
    }

    if (!map.contains(preference)){
        //qDebug() << "Map does not contain preference";
        if (retAndStoreIfDoenstExist != ""){
            setPreference(preference,retAndStoreIfDoenstExist);
            return retAndStoreIfDoenstExist;
        }
    }
    //else qDebug() << "Map contains" << preference << ". Will return" << map.value(preference).toString();

    return map.value(preference).toString();
}


////////////////////////////// ONGOING EVALUATIONS ///////////////////////////////

QString LocalDB::createNewEvaluation(const QString &subjectID,
                                     const QString &clinician,
                                     const QString &evaluator,
                                     const QString &protocol,
                                     const QString &instance_prefix,
                                     const QString &evalType){

    QDateTime now = QDateTime::currentDateTime();
    QString evaluationID = instance_prefix + "_" + now.toString("yyyyMMdd_hhmmss");


    QVariantMap newEvaluation;

    // We initialize the new evaluation object.
    newEvaluation[EVAL_CLINICIAN] = clinician;
    newEvaluation[EVAL_EVALUATOR] = evaluator;
    newEvaluation[EVAL_STATUS]    = EvaluationStatus::ONGOING;
    newEvaluation[EVAL_TIMESTAMP] = QDateTime::currentSecsSinceEpoch();
    newEvaluation[EVAL_ID]        = evaluationID;
    newEvaluation[EVAL_SUBJECT]   = subjectID;
    newEvaluation[EVAL_TIME]      = now.toString("h:mm");
    newEvaluation[EVAL_ISO_DATE]  = now.toString("yyyy-MM-dd");
    newEvaluation[EVAL_PROTOCOL]  = protocol;
    newEvaluation[EVAL_TYPE]      = evalType;
    newEvaluation[EVAL_COMMENT]   = "";

    // We can use the evaluation type to get a list of the tasks.
    QVariantMap availableEvaluations = this->data.value(MAIN_AVAILABLE_EVALS).toMap();

    //Debug::prettyPrintQVariantMap(availableEvaluations);

    //qDebug() << "Printing evaluation of Type" << evalType;

    QVariantMap evaluationStruct = availableEvaluations.value(evalType).toMap();

    //Debug::prettyPrintQVariantMap(evaluationStruct);

    QStringList tasks_list = evaluationStruct.value("list").toStringList();

    //qDebug() << "Resulting TASK LIST" << tasks_list;

    // We initialize each of the tasks object.
    QVariantMap tasks;
    for (qint32 i = 0; i < tasks_list.size(); i++){

        QString task = tasks_list.at(i);
        QString tarTaskFileName = "";
        QVariantMap taskMap = createNewTaskObjectForEvaluationTaskList(evaluationID,task,tasks.size(),&tarTaskFileName);
        // Store the configuration in the task object key-d by the file name.
        tasks[tarTaskFileName] = taskMap;

    }

    // Storing the task list.
    newEvaluation[EVAL_TASKS] = tasks;

    // Now we store in the DB.
    QVariantMap ongoing_evals = this->data.value(MAIN_ONGOING_EVALUATIONS).toMap();
    ongoing_evals[evaluationID] = newEvaluation;
    this->data[MAIN_ONGOING_EVALUATIONS] = ongoing_evals;

    saveAndBackup();

    return evaluationID;

}

QVariantList LocalDB::getRemainingTasksForEvaluation(const QString &evaluationID){

    QVariantMap ongoing_evals = this->data.value(MAIN_ONGOING_EVALUATIONS).toMap();
    if (!ongoing_evals.contains(evaluationID)) return QVariantList();
    QVariantMap tasks = ongoing_evals.value(evaluationID).toMap().value(EVAL_TASKS).toMap();

    QStringList filenames = tasks.keys();

    QVariantList remTasks;
    for (qint32 i = 0; i < filenames.size(); i++){
        QString filename = filenames.at(i);
        QVariantMap taskObj = tasks.value(filename).toMap();

        if (taskObj.value(TASK_QCI).toReal() < 0){
            QVariantMap obj;
            obj["type"] = taskObj.value(TASK_TYPE).toString();
            obj["file"] = taskObj.value(TASK_FILE).toString();
            remTasks << obj;
        }


    }

    return remTasks;

}

QVariantMap LocalDB::getEvaluation(const QString &evaluationID) const {
    QVariantMap ongoing_evals = this->data.value(MAIN_ONGOING_EVALUATIONS).toMap();
    return ongoing_evals.value(evaluationID).toMap();
}


QVariantMap LocalDB::getEvaluationDisplayMap(const QString &evaluationID, const QStringList &months, const QString &ui_lang_code, qlonglong override_time){

    QVariantMap ongoing_evals = this->data.value(MAIN_ONGOING_EVALUATIONS).toMap();
    if (!ongoing_evals.contains(evaluationID)) return QVariantMap();
    QVariantMap evaluation = ongoing_evals.value(evaluationID).toMap();

    // So in the display, the title is composed of the Patient name, the evaluation name and the date and time.
    QString date = evaluation.value(EVAL_ISO_DATE).toString();
    date = buildDisplayBirthDate(date,months);

    QString evalType = evaluation.value(EVAL_TYPE).toString();
    QString evalName = this->data.value(MAIN_AVAILABLE_EVALS).toMap().value(evalType).toMap().value("name").toMap().value(ui_lang_code).toString();

    QString subject_id = evaluation.value(EVAL_SUBJECT).toString();
    QString subject  = createSubjectTableDisplayText(subject_id);

    // Computing whether the evaluation is too old or not.
    qlonglong timeout_compare = EVALUATION_DISCARD_TIME; // A day in seconds.
    if (override_time > 0){
        timeout_compare = override_time;
    }
    qlonglong current_timestamp = QDateTime::currentSecsSinceEpoch();
    qlonglong timestamp = evaluation.value(EVAL_TIMESTAMP).toLongLong();
    qlonglong diff = (current_timestamp - timestamp);

    QVariantMap ret;
    ret["name"]       = subject;
    ret["subject_id"] = subject_id;
    ret["eval"]       = evalName;
    ret["date"]       = date + " " + evaluation.value(EVAL_TIME).toString();
    ret["eval_id"]    = evaluationID;
    ret["comment"]    = evaluation.value(EVAL_COMMENT,"").toString();

    // Now for each task, we return the type, the qci, if the qci has passed
    QVariantMap tasks = evaluation.value(EVAL_TASKS).toMap();
    QVariantList retTasks;

    QStringList taskKeys = tasks.keys();
    bool anyUploaded = false;
    for (qint32 i = 0; i < taskKeys.size(); i++){
        QString key = taskKeys.at(i);
        QVariantMap task = tasks.value(key).toMap();
        QVariantMap temp;
        temp["id"] = key;
        temp["qci"] = qRound(task.value(TASK_QCI).toReal());
        temp["qci_ok"] = task.value(TASK_QCI_OK).toBool();
        temp["name"] = task.value(TASK_TYPE).toString();
        temp["discarded"] = task.value(TASK_DISCARDED).toBool();

        bool uploaded = task.value(TASK_UPLOADED).toBool();
        temp["uploaded"] = uploaded;

        if (uploaded){
            anyUploaded = true;
        }
        retTasks << temp;
    }

    // The logic is that redo is false (i.e not possible) if either ANY tasks have been uploaded or if the study is too old.
    ret["redo"] = !(anyUploaded || (diff > timeout_compare));

    ret["tasks"] = retTasks;
    return ret;

}

QList<QVariantMap> LocalDB::getEvaluationTableInformation(const QString &evaluator,
                                                    qlonglong override_timeout,
                                                    const QStringList &months,
                                                    const QString &ui_lang_code){

    QList<QVariantMap> list;

    QVariantMap ongoing_evals = this->data.value(MAIN_ONGOING_EVALUATIONS).toMap();
    QStringList evalKeys = ongoing_evals.keys();

    QVariantMap medics = this->data.value(MAIN_MEDICS).toMap();

    qlonglong timeout_compare = EVALUATION_DISCARD_TIME; // A day in seconds.

    // This should only be non zero for debugging/testing.
    bool enable_debug = false;
    if (override_timeout > 0){
        enable_debug = true;
        timeout_compare = override_timeout;
    }

    qlonglong current_timestamp = QDateTime::currentSecsSinceEpoch();


    for (qint32 i = 0; i < evalKeys.size(); i++){

        QString evalID = evalKeys.at(i);

        QVariantMap evaluation = ongoing_evals.value(evalID).toMap();

        // We are only interested in the evaluation of this evaluator.
        if (evaluator != evaluation.value(EVAL_EVALUATOR).toString()) continue;

        qlonglong timestamp = evaluation.value(EVAL_TIMESTAMP).toLongLong();
        QString status = evaluation.value(EVAL_STATUS).toString();
        qlonglong diff = (current_timestamp - timestamp);

        if (enable_debug) {
            StaticThreadLogger::log("LocalDB::getEvaluationTableInformation","[DBUG] Evaluation '"
                                + evalID + "' has a time stamp difference of "
                                + QString::number(diff) + " compared against " + QString::number(timeout_compare) );
        }

        if (diff > timeout_compare){

            // The evaluation is too old. However if it's done is all ok.
            if (status != EvaluationStatus::READY_UPLOAD) continue; // Too late. All it's tasks will be discarded.
        }

        QString evalType = evaluation.value(EVAL_TYPE).toString();
        QString evalName = this->data.value(MAIN_AVAILABLE_EVALS).toMap().value(evalType).toMap().value("name").toMap().value(ui_lang_code).toString();

        QVariantMap clinician = medics.value(evaluation.value(EVAL_CLINICIAN).toString()).toMap();

        QString date = evaluation.value(EVAL_ISO_DATE).toString();
        date = buildDisplayBirthDate(date,months);

        QVariantMap obj;
        obj[EVAL_TIMESTAMP] = timestamp;
        obj[EVAL_SUBJECT]   = createSubjectTableDisplayText(evaluation.value(EVAL_SUBJECT).toString());
        obj[EVAL_TYPE]      = evalName;
        obj[EVAL_CLINICIAN] = clinician.value(APPUSER_LASTNAME).toString() + ", " + clinician.value(APPUSER_NAME).toString();
        obj[EVAL_TIME]      = date + " " + evaluation.value(EVAL_TIME).toString();
        obj[EVAL_STATUS]    = status;
        obj[EVAL_ID]        = evalID;

        list << obj;

    }

    return list;

}

bool LocalDB::updateEvaluation(const QString &evaluationID, const QString &taskFileName, qreal qci, bool qci_ok, bool requestRedo){

    QVariantMap ongoing_evals = this->data.value(MAIN_ONGOING_EVALUATIONS).toMap();
    QVariantMap evaluation = ongoing_evals.value(evaluationID).toMap();

    // Now we search for the task.
    QVariantMap tasks = evaluation.value(EVAL_TASKS).toMap();
    if (!tasks.contains(taskFileName)){
        this->error = "Updating evaluation '" + evaluationID + "', unable to file task file  '" + taskFileName + "'. Structure looks like\n" + Debug::QVariantMapToString(evaluation);
        return false;
    }

    // Updating the QCI values for the task.
    QVariantMap task = tasks.value(taskFileName).toMap();
    if (!requestRedo){
        task[TASK_QCI] = qci;
        task[TASK_QCI_OK] = qci_ok;
        if (!qci_ok) task[TASK_DISCARDED]  = true;
    }
    else {
        // If a redo is requested, this one is discarded.
        task[TASK_DISCARDED]  = true;
    }

    tasks[taskFileName] = task;

    // If the QCI values are bad we need to create a new file slot in the task list for the this task.
    if ((!qci_ok) || (requestRedo)){
        QString newFileName = "";
        QVariantMap newTask = createNewTaskObjectForEvaluationTaskList(evaluationID,task.value(TASK_TYPE).toString(),tasks.size(),&newFileName);
        tasks[newFileName] = newTask;

        // If we entered here due to a bad QCI the the status should reflect that.
        // However if a redo was requested the status WAS ready to upload and should go back to ongoing
        if (!requestRedo) evaluation[EVAL_STATUS] = EvaluationStatus::BAD_QCI;
        else evaluation[EVAL_STATUS] = EvaluationStatus::ONGOING;
    }
    else {

        // We need to determine if we are done or not. IN order to be done we need for all files in the task list to have a non zero QCI.
        QStringList allFiles = tasks.keys();
        bool done = true;
        for (qint32 i = 0; i < allFiles.size(); i++){
            QString key = allFiles.at(i);
            task = tasks.value(key).toMap();
            if (task.value(TASK_QCI).toReal() < 0){
                done = false;
                break;
            }
        }

        if (done) {
            // If we are done, we are ready to upload.
            evaluation[EVAL_STATUS] = EvaluationStatus::READY_UPLOAD;
        }

    }

    // We store everything back.
    evaluation[EVAL_TASKS] = tasks;
    ongoing_evals[evaluationID] = evaluation;
    this->data[MAIN_ONGOING_EVALUATIONS] = ongoing_evals;
    if (!saveAndBackup()) return false;
    return true;

}

void LocalDB::setEvaluationComment(const QString &evalID, const QString &comment){
    QVariantMap ongoing_evals = this->data.value(MAIN_ONGOING_EVALUATIONS).toMap();
    QVariantMap evaluation = ongoing_evals.value(evalID).toMap();

    evaluation[EVAL_COMMENT] = comment;

    ongoing_evals[evalID] = evaluation;
    this->data[MAIN_ONGOING_EVALUATIONS] = ongoing_evals;
    saveAndBackup();

}

QStringList LocalDB::getExpiredEvaluationID(qlonglong override_timeout, const QString &baseWorkDir){

    QVariantMap ongoing_evals = this->data.value(MAIN_ONGOING_EVALUATIONS).toMap();
    QStringList allEvalIDs = ongoing_evals.keys();

    qlonglong timeout_compare = EVALUATION_DISCARD_TIME; // A day in seconds.

    // This should only be non zero for debugging/testing.
    bool enable_debug = false;
    if (override_timeout > 0){
        enable_debug = true;
        timeout_compare = override_timeout;
    }

    qlonglong current_timestamp = QDateTime::currentSecsSinceEpoch();

    QStringList expired;

    for (qint32 i = 0; i < allEvalIDs.size(); i++){

        QString evalID = allEvalIDs.at(i);

        QVariantMap evaluation = ongoing_evals.value(evalID).toMap();

        QString baseDir = baseWorkDir + "/" + evaluation.value(EVAL_SUBJECT).toString();

        qlonglong timestamp = evaluation.value(EVAL_TIMESTAMP).toLongLong();
        QString status = evaluation.value(EVAL_STATUS).toString();
        qlonglong diff = (current_timestamp - timestamp);

        if (enable_debug) {
            StaticThreadLogger::log("LocalDB::getEvaluationTableInformation","[DBUG] Evaluation '"
                                + evalID + "' has a time stamp difference of "
                                + QString::number(diff) + " compared against " + QString::number(timeout_compare) );
        }

        if ( (diff > timeout_compare) && (status != EvaluationStatus::READY_UPLOAD) ){

            // Unfinished evaluation that has expired.

            // So we check if the files exist
            QVariantMap tasks = evaluation.value(EVAL_TASKS).toMap();
            tasks = cleanUpTasksForEval(tasks,baseDir);
            if (tasks.empty()){
                ongoing_evals.remove(evalID);
            }
            else {
                evaluation[EVAL_TASKS] = tasks;
                ongoing_evals[evalID] = evaluation;
                expired << evalID;
            }

        }
    }

    this->data[MAIN_ONGOING_EVALUATIONS] = ongoing_evals;
    saveAndBackup();

    return expired;

}

bool LocalDB::updateTaskInEvaluationAsUploaded(const QString &evalID, const QString &taskID){

//    qDebug() << "WARNING SKIPPING MARKING AS UPLOADED";
//    return true;

    QVariantMap ongoing_evals = this->data.value(MAIN_ONGOING_EVALUATIONS).toMap();
    QVariantMap evaluation = ongoing_evals.value(evalID).toMap();

    // Now we search for the task.
    QVariantMap tasks = evaluation.value(EVAL_TASKS).toMap();
    if (!tasks.contains(taskID)){
        this->error = "Updating task to uploaded in '" + evalID + "', unable to file task file  '" + taskID + "'. Structure looks like\n" + Debug::QVariantMapToString(evaluation);
        return false;
    }

    // Change the value to uploaded.
    QVariantMap task = tasks.value(taskID).toMap();
    task[TASK_UPLOADED] = true;
    tasks[taskID] = task;

    // Now we need to check if any task is NOT uploaded.
    QStringList taskFileList = tasks.keys();
    bool allDone = true;
    for (qint32 i = 0; i < taskFileList.size(); i++){
        QString taskKey = taskFileList.value(i);
        task = tasks.value(taskKey).toMap();
        if (!task.value(TASK_UPLOADED).toBool()){
            allDone = false;
            break;
        }
    }

    if (!allDone){
        // We store everything as is.
        evaluation[EVAL_TASKS] = tasks;
        ongoing_evals[evalID] = evaluation;
    }
    else {
        // WE remove the entry from the db.
        // dontRemove can be true for debugging.
        ongoing_evals.remove(evalID);
    }

    this->data[MAIN_ONGOING_EVALUATIONS] = ongoing_evals;

    if (!saveAndBackup()) return false;
    return true;


}

QVariantMap LocalDB::createNewTaskObjectForEvaluationTaskList(const QString &evaluationID, const QString &task, qint32 counter , QString *tarTaskFileName){

    // So for each task we generate a file name.
    QDateTime now = QDateTime::currentDateTime();

    QString cntstring = QString::number(counter);
    while (cntstring.size() < 3) cntstring = "0" + cntstring; // It's VERY unlikely that we would get a number of tasks into de double digits, much less into 3. But JUST in case.

    QString baseTaskFileName = evaluationID + "_" + cntstring + "_" + task; //+ "_" + now.toString("yyyyMMddhhmmss");
    *tarTaskFileName  = baseTaskFileName + ".zip";
    QString jsonTaskFileName = baseTaskFileName + ".json";

    // And we initialize it's configuration.
    QVariantMap taskMap;
    taskMap[TASK_FILE]      = jsonTaskFileName;
    taskMap[TASK_TYPE]      = task;
    taskMap[TASK_QCI]       = -1;
    taskMap[TASK_QCI_OK]    = false;
    taskMap[TASK_UPLOADED]  = false;
    taskMap[TASK_DISCARDED] = false;

    return taskMap;
}

QString LocalDB::createSubjectTableDisplayText(const QString &subject_id){

    QVariantMap subjectData =  this->data.value(MAIN_SUBJECT_DATA).toMap().value(subject_id).toMap();
    QString lastName = subjectData.value(SUBJECT_LASTNAME).toString();
    QString firstName = subjectData.value(SUBJECT_NAME).toString();
    QString id = subjectData.value(SUBJECT_INSTITUTION_ID).toString();

    QString subject = subject_id;

    if (firstName == "") {
        if (lastName == "") subject = id;
        else {
            if (id != ""){
                subject = lastName + " (" + id + ")";
            }
            else {
                subject = lastName;
            }
        }
    }
    else {
        if (id != "") subject = lastName + "," + firstName + " (" + id + ")";
        else subject = lastName + "," + firstName;
    }

    return subject;

}

QVariantMap LocalDB::cleanUpTasksForEval(QVariantMap tasks, const QString &baseDir){

    QStringList zipFileNames = tasks.keys();

    QVariantMap filteredTasks;

    for (qint32 i = 0; i < zipFileNames.size(); i++){

        QString zipFile = zipFileNames.at(i);
        QString qci     = zipFile.replace(".zip",".qci");
        QVariantMap task = tasks.value(zipFile).toMap();

        if (!QFile(baseDir + "/" + zipFile).exists()) continue;
        if (!QFile(baseDir + "/" + qci).exists()) continue;

        filteredTasks[zipFile] = task;

    }

    return filteredTasks;

}

////////////////////////////// FUZZY String Compare ///////////////////////////////
QVariantList LocalDB::possibleNewPatientMatches(QString name, QString lastname, QString personalID ,QString iso_birthdate, const QStringList &months) const {

    QVariantMap patients = data.value(MAIN_SUBJECT_DATA).toMap();
    QStringList patient_ids = patients.keys();

    QVariantList list;
    FuzzyStringCompare comparator;

    for (qint32 i = 0; i < patient_ids.size(); i++){
        QString id = patient_ids.at(i);
        QVariantMap patient = patients.value(id).toMap();
        QString bdate = patient.value(SUBJECT_BIRTHDATE).toString();

        //qDebug() << "Comparing BDATE" << bdate << "with search BDATE" << iso_birthdate;

        // The very first filter is the year of birth.
        if (bdate == iso_birthdate){
            //if (iso_birthdate != ""){ // For testing purposes, so that the date always matches.

            // Now we do the fuzzy search.
            QString fname = patient.value(SUBJECT_NAME).toString();
            QString lname = patient.value(SUBJECT_LASTNAME).toString();
            QString id    = patient.value(SUBJECT_INSTITUTION_ID).toString();

            qreal match_val_fname = comparator.compare(fname,name);
            qreal match_val_lname = comparator.compare(lname,lastname);
            qreal match_val_id    = comparator.compare(id,personalID);

            //qDebug() << "FNAME:" << name << "->" << fname << ": " << match_val_fname << ". LNAME" << lastname << "->" << lname << ": " << match_val_lname;

            bool possible_match = false;

            // The comparison logic depends on the information provided.
            if (lastname == ""){
                // We only check for exact matches in personal ID, when no last name is provided.
                possible_match = (match_val_id > 0.99);
            }
            else {
                // If a last name is provided we only check the name thresholds
                possible_match = (match_val_fname > FNAME_FUZZY_MATCH_THRESHOLD) && (match_val_lname > LNAME_FUZZY_MATCH_THRESHOLD);
            }

            if (possible_match) {

                //                Debug::prettpPrintQVariantMap(patient);
                //                qDebug() << "==============================";

                // We have a fuzzy match. We add the record to the list.

                QString display_bdate = buildDisplayBirthDate(bdate,months);
                QVariantMap record;
                // The name of the fields for the record match the names required by the
                // QML Item that displays each matching record.

                record["vmName"] = lname + ", " + fname;
                record["vmBirthDate"] = display_bdate;
                record["vmSubjectID"] = id;
                record["vmPersonalID"] = patient.value(SUBJECT_INSTITUTION_ID).toString();

                //qDebug() << "   Adding it to record";

                list << record;
            }

        }

    }

    return list;

}
