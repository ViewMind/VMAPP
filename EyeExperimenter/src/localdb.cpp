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


// Evaluator fields
const char * LocalDB::APPUSER_NAME          = "name";
const char * LocalDB::APPUSER_LASTNAME      = "lastname";
const char * LocalDB::APPUSER_PASSWORD      = "password";
const char * LocalDB::APPUSER_EMAIL         = "email";
const char * LocalDB::APPUSER_VIEWMIND_ID   = "viewmind_id";

// Subject Fields
const char * LocalDB::SUBJECT_NAME                  = "name";
const char * LocalDB::SUBJECT_LASTNAME              = "lastname";
const char * LocalDB::SUBJECT_INSTITUTION_ID        = "supplied_institution_id";
const char * LocalDB::SUBJECT_BIRTHDATE             = "birthdate";
const char * LocalDB::SUBJECT_BIRTHCOUNTRY          = "birthcountry";
const char * LocalDB::SUBJECT_YEARS_FORMATION       = "years_formation";
const char * LocalDB::SUBJECT_CREATION_DATE         = "creation_date";
const char * LocalDB::SUBJECT_CREATION_DATE_INDEX   = "creation_date_index";
const char * LocalDB::SUBJECT_BDATE_DISPLAY         = "bdate_display";
const char * LocalDB::SUBJECT_SORTABLE_NAME         = "sortable_name";
const char * LocalDB::SUBJECT_GENDER                = "gender";
const char * LocalDB::SUBJECT_STUDY_MARKERS         = "subject_study_markers";
const char * LocalDB::SUBJECT_LOCAL_ID              = "local_id";
const char * LocalDB::SUBJECT_ASSIGNED_MEDIC        = "assigned_medic";
const char * LocalDB::SUBJECT_EMAIL                 = "email";

// "Bookmark" fields
const char * LocalDB::MARKER_VALUE            = "marker_value";
const char * LocalDB::MARKER_TIME             = "marker_time";

// Protocol Fields
const char * LocalDB::PROTOCOL_NAME           = "protocol_name";
const char * LocalDB::PROTOCOL_CREATION_DATE  = "creation_date";
const char * LocalDB::PROTOCOL_ID             = "protocol_id";

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
    this->dbfile = dbfile;

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
    if (!file.exists()) return true;

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

    if (disable_checksum) return true;

    QString checksum = data.value(MAIN_CHECKSUM).toString();
    QString hash = computeDataHash();
    if (checksum != hash){
        // Somehow data got corrupted.
        error = "Failed checksum verification when loading the database.";
        return false;
    }

    return true;

}

bool LocalDB::addStudyMarkerForSubject(const QString &suid, const QString &study, const QString &value){

    if (!data.value(MAIN_SUBJECT_DATA).toMap().contains(suid)){
        error = "Trying to add a marker for subject " + suid + " but it doesn't exist";
        return false;
    }

    // Getting the marker or bookmark, empty if it doesn't exist.
    QVariantMap all_subject_data = data.value(MAIN_SUBJECT_DATA).toMap();
    QVariantMap suid_subject_data = all_subject_data.value(suid).toMap();
    QVariantMap study_markers = suid_subject_data.value(SUBJECT_STUDY_MARKERS).toMap();
    QVariantMap marker = study_markers.value(study).toMap();

    // Setting the values
    marker[MARKER_TIME]  = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
    marker[MARKER_VALUE] = value;

    // Setting it into the structure.
    study_markers[study] = marker;
    suid_subject_data[SUBJECT_STUDY_MARKERS] = study_markers;
    all_subject_data[suid] = suid_subject_data;
    data[MAIN_SUBJECT_DATA] = all_subject_data;

    return saveAndBackup();

}

QVariant LocalDB::getMarkerForStudy(const QString &suid, const QString &study) const{
    return data.value(MAIN_SUBJECT_DATA).toMap().value(suid).toMap().value(SUBJECT_STUDY_MARKERS).toMap().value(study);
}


bool LocalDB::removeMarkerForSubject(const QString &suid, const QString &study){
    if (!data.value(MAIN_SUBJECT_DATA).toMap().contains(suid)){
        error = "Trying to delete marker for subject " + suid + " but it doesn't exist";
        return false;
    }

    // Getting the marker or bookmark, empty if it doesn't exist.
    QVariantMap all_subject_data = data.value(MAIN_SUBJECT_DATA).toMap();
    QVariantMap suid_subject_data = all_subject_data.value(suid).toMap();
    QVariantMap study_markers = suid_subject_data.value(SUBJECT_STUDY_MARKERS).toMap();
    if (study_markers.contains(study)){
        study_markers.remove(study);
    }

    // Storing everything in the structure.
    suid_subject_data[SUBJECT_STUDY_MARKERS] = study_markers;
    all_subject_data[suid] = suid_subject_data;
    data[MAIN_SUBJECT_DATA] = all_subject_data;

    return saveAndBackup();
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
    subject_data_map[subject_id] = subject_data;
    data[MAIN_SUBJECT_DATA] = subject_data_map;
    // We save after ANY modification;
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
    if (!response.contains(APINames::RecoveryPassword::NAME)){
        error = "Expected field: " + QString(APINames::RecoveryPassword::NAME) + " but it wasn't found";
        return false;
    }

    QString hash = response.value(APINames::RecoveryPassword::NAME).toString();
    if (hash != data.value(MAIN_RECOVERY_PASSWORD,"").toString()){
        data[MAIN_RECOVERY_PASSWORD] = response.value(APINames::RecoveryPassword::NAME).toString();
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

    qDebug() << "New Medic Data";

    data[MAIN_MEDICS] = serverMedics;
    return saveAndBackup();
}

bool LocalDB::setProcessingParametersFromServerResponse(const QVariantMap &response){

    if (!response.contains(APINames::ProcParams::NAME)){
        error = "Expected processing parameters field: " + QString(APINames::ProcParams::NAME) + " but it wasn't found";
        return false;
    }

    QStringList shouldBeThere;
    shouldBeThere << VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW << VMDC::ProcessingParameter::LATENCY_ESCAPE_RADIOUS
                  << VMDC::ProcessingParameter::MIN_FIXATION_DURATION << VMDC::ProcessingParameter::SAMPLE_FREQUENCY
                  << VMDC::ProcessingParameter::GAZE_ON_CENTER_RADIOUS;

    QVariantMap pp = response.value(APINames::ProcParams::NAME).toMap();

    QStringList serverkeys = pp.keys();
    for (qint32 i = 0; i < serverkeys.size(); i++){
        if (!shouldBeThere.contains(serverkeys.at(i))){
            error = "Unknown server side processing parameter " + serverkeys.at(i);
            return false;
        }
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

    if (!response.contains(APINames::FreqParams::NAME)){
        error = "Expected processing parameters field: " + QString(APINames::FreqParams::NAME) + " but it wasn't found";
        return false;
    }

    QStringList allStudies;
    allStudies << VMDC::MultiPartStudyBaseName::BINDING
               << VMDC::Study::NBACKRT
               << VMDC::Study::GONOGO
               << VMDC::Study::NBACKMS
               << VMDC::Study::NBACKVS
               << VMDC::Study::READING;


    QStringList shouldBeThere;
    shouldBeThere << VMDC::QCGlobalParameters::valid;
    shouldBeThere << allStudies; // There should be a field for each study.

    QStringList shouldBeTherePerStudy;

    QVariantMap qc = response.value(APINames::FreqParams::NAME).toMap();

    //Debug::prettpPrintQVariantMap(qc);

    // First we make sure that all fields are there.
    QStringList serverkeys = qc.keys();
    for (qint32 i = 0; i < serverkeys.size(); i++){
        if (!shouldBeThere.contains(serverkeys.at(i))){
            error = "Unknown server side qc parameter " + serverkeys.at(i);
            return false;
        }
        shouldBeThere.removeOne(serverkeys.at(i));
    }

    if (!shouldBeThere.isEmpty()){
        error = "The following qc parameters were missing from server response: " + shouldBeThere.join(",");
        return false;
    }


    // Now for each study we verify that all the keys for each study are there.
    for (qint32 i = 0; i < allStudies.size(); i++){
        shouldBeTherePerStudy << VMDC::QCStudyParameters::valid;

        serverkeys = qc.value(allStudies.at(i)).toMap().keys();

        for (qint32 i = 0; i < serverkeys.size(); i++){
            if (!shouldBeTherePerStudy.contains(serverkeys.at(i))){
                error = "Unknown server side qc study parameter " + serverkeys.at(i);
                return false;
            }
            shouldBeTherePerStudy.removeOne(serverkeys.at(i));
        }

        if (!shouldBeTherePerStudy.isEmpty()){
            error = "The following qc study parameters were missing from server response: " + shouldBeTherePerStudy.join(",") + ". For study: " + allStudies.at(i);
            return false;
        }
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
            QStringList bdate_parts = map.value(SUBJECT_BIRTHDATE).toString().split("-");
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
            map[SUBJECT_BDATE_DISPLAY] = bdate;
            map[SUBJECT_SORTABLE_NAME] = map.value(SUBJECT_LASTNAME).toString() + ", " + map.value(SUBJECT_NAME).toString();

            ans[subject_ids.at(i)] = map;
        }
    }

    return ans;
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

QStringList LocalDB::getUsernameEmails() const {
    return data.value(MAIN_EVALUATOR_DATA).toMap().keys();
}

QVariantMap LocalDB::getQCParameters() const {
    return data.value(MAIN_QC_PARAMETERS).toMap();
}

bool LocalDB::filterMatchSubject(const QVariantMap &subject, const QString &filter){
    if (filter == "") return true;
    QString name = subject.value(SUBJECT_NAME).toString().toLower();
    QString lastname = subject.value(SUBJECT_LASTNAME).toString().toLower();
    QString instid = subject.value(SUBJECT_INSTITUTION_ID).toString().toLower();

    // If any of the identifiers contain the string this is a match.
    return (name.contains(filter) || lastname.contains(filter) || instid.contains(filter));

}

bool LocalDB::saveAndBackup(){

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
