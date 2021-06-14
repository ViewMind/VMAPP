#include "loader.h"


Loader::Loader(QObject *parent, ConfigurationManager *c, CountryStruct *cs) : QObject(parent)
{

    // Connecting the API Client slot.
    connect(&apiclient, &APIClient::requestFinish, this ,&Loader::receivedRequest);


    // Loading the configuration file and checking for the must have configurations
    // The data is this file should NEVER change. Its values should be fixed.
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    loadingError = false;
    configuration = c;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[Globals::VMConfig::INSTITUTION_ID] = cmd;
    cv[Globals::VMConfig::INSTANCE_NUMBER] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_STRING;
    cv[Globals::VMConfig::INSTANCE_KEY] = cmd;
    cv[Globals::VMConfig::INSTANCE_HASH_KEY] = cmd;
    cv[Globals::VMConfig::INSTITUTION_NAME] = cmd;

    // This cannot have ANY ERRORS
    configuration->setupVerification(cv);
    if (!configuration->loadConfiguration(Globals::Paths::CONFIGURATION,Globals::Share::TEXT_CODEC)){
        logger.appendError("Errors loading the configuration file: " + configuration->getError());
        // The program should not be able to continue after loading the language.
        loadingError = true;
    }

    // Loading the settings.
    ConfigurationManager settings;
    cv.clear();
    cmd.clear();
    cmd.optional = true; // All settings are optional.

    // The strings.
    cv[Globals::VMPreferences::DEFAULT_COUNTRY] = cmd;
    cv[Globals::VMPreferences::UI_LANGUAGE]     = cmd;
    cv[Globals::VMPreferences::LAST_SELECTED_PROTOCOL]     = cmd;

    // The booleans
    cmd.type = ConfigurationManager::VT_BOOL;
    cv[Globals::VMPreferences::DUAL_MONITOR_MODE] = cmd;
    cv[Globals::VMPreferences::DEMO_MODE] = cmd;
    cv[Globals::VMPreferences::USE_MOUSE] = cmd;

    // The ints
    cmd.type = ConfigurationManager::VT_INT;
    cv[Globals::VMPreferences::DEFAULT_READING_LANGUAGE] = cmd;

    // Merging the settings or loading the default configuration.
    settings.setupVerification(cv);
    if (QFile(Globals::Paths::SETTINGS).exists()){
        if (!settings.loadConfiguration(Globals::Paths::SETTINGS,Globals::Share::TEXT_CODEC)){
            logger.appendError("Errors loading the settings file: " + settings.getError());
            // Settings files should not have unwanted key words
            loadingError = true;
        }
        else{
            configuration->merge(settings);
        }
    }
    else{
        // We don't need to save the settings file as the function only loads those settings that weren't set.
        loadDefaultConfigurations();
    }

    // The country to to country code map.
    countries = cs;

    // Change the language array.
    changeLanguage();
    if (loadingError) return;

    // Must make sure that the data directory exists
    QDir rawdata(Globals::Paths::WORK_DIRECTORY);
    if (!rawdata.exists()){
        if (!QDir(".").mkdir(Globals::Paths::WORK_DIRECTORY)){
            logger.appendError("Cannot create viewmind_data directory");
            loadingError = true;
            return;
        }
    }

    if (!localDB.setDBFile(Globals::Paths::LOCALDB,Globals::Paths::DBBKPDIR,Globals::Debug::PRETTY_PRINT_JSON_DB,Globals::Debug::DISABLE_DB_CHECKSUM)){
        logger.appendError("Could not set local db file: " + localDB.getError());
        loadingError = true;
        return;
    }

    /// TODO send a request for processing parameters and doctors. Put up a sign.

    if (localDB.processingParametersPresent()){
        QVariantMap pp = localDB.getProcessingParameters();
        configuration->addKeyValuePair(Globals::Share::MAX_DISPERSION_SIZE,pp.value(LocalDB::PP_MAX_DISPERSION_SIZE).toInt());
        configuration->addKeyValuePair(Globals::Share::MINIMUM_FIX_DURATION,pp.value(LocalDB::PP_MINIMUM_FIX_DURATION).toInt());
        configuration->addKeyValuePair(Globals::Share::SAMPLE_FREQUENCY,pp.value(LocalDB::PP_SAMPLE_FREQUENCY).toInt());
    }
    else{
        logger.appendError("Processing parameters are not present in local database. Will not be able to do any studies");
    }

    // Configuring the API Client.
    apiclient.configure(configuration->getString(Globals::VMConfig::INSTITUTION_ID),
                        configuration->getString(Globals::VMConfig::INSTANCE_NUMBER),
                        configuration->getString(Globals::VMConfig::INSTANCE_KEY),
                        configuration->getString(Globals::VMConfig::INSTANCE_HASH_KEY));

    //Globals::Debug::prettpPrintQVariantMap(configuration->getMap());

    // For debugging
    this->setCurrentStudyFileForQC("viewmind_etdata/1_0_20210606090711350/nbackms_2021_06_14_08_32.json");

}

//////////////////////////////////////////////////////// UI Functions ////////////////////////////////////////////////////////

QString Loader::getStringForKey(const QString &key){
    if (language.containsKeyword(key)){
        return language.getString(key);
    }
    else return "ERROR: NOT FOUND";
}

QStringList Loader::getStringListForKey(const QString &key){
    if (language.containsKeyword(key)){
        return language.getStringList(key);
    }
    else return QStringList();
}


int Loader::getDefaultCountry(bool offset){
    QString countryCode = configuration->getString(Globals::VMPreferences::DEFAULT_COUNTRY);
    if (countryCode.isEmpty()) return 0;
    int ans = countries->getIndexFromCode(countryCode);
    if (offset){
        // -1 Means that no selction is done, which means 0 code. All indexes are offseted by 1, due to this.
        return ans + 1;
    }
    else {
        if (ans == -1) return 0;
        else return ans;
    }
}

bool Loader::getLoaderError() const {
    return loadingError;
}

QString Loader::getWindowTilteVersion(){
    return Globals::Share::EXPERIMENTER_VERSION;
}

QStringList Loader::getCountryList() {
    return countries->getCountryList();
}

QStringList Loader::getCountryCodeList() {
    return countries->getCodeList();
}

QString Loader::getCountryCodeForCountry(const QString &country) {
    return countries->getCodeForCountry(country);
}

int Loader::getCountryIndexFromCode(const QString &code) {
    return countries->getIndexFromCode(code);
}

QString Loader::getVersionNumber() const {
    return Globals::Share::EXPERIMENTER_VERSION_NUMBER;
}

QString Loader::getManufactureDate() const {
    //return configuration->getString(Globals::VMPreferences::LABELLING_MANUFACTURE_DATE);
    return Globals::Labeling::MANUFACTURE_DATE;
}

QString Loader::getSerialNumber() const {
    //return configuration->getString(Globals::VMPreferences::LABELLING_SERIAL_NUMBER);
    return Globals::Labeling::SERIAL_NUMBER;
}

QString Loader::getUniqueAuthorizationNumber() const {
    //return configuration->getString(Globals::VMPreferences::LABELLING_AUTHORIZATION_UID);
    return Globals::Labeling::AUTHORIZATION_UID;
}

QString Loader::getInstitutionName() const {
    return configuration->getString(Globals::VMConfig::INSTITUTION_NAME);
}

////////////////////////////////////////////////////////  CONFIGURATION FUNCTIONS  ////////////////////////////////////////////////////////

QString Loader::getConfigurationString(const QString &key){
    if (configuration->containsKeyword(key)){
        return configuration->getString(key);
    }
    else return "";
}

bool Loader::getConfigurationBoolean(const QString &key){
    if (configuration->containsKeyword(key)){
        return configuration->getBool(key);
    }
    else return false;
}

void Loader::setSettingsValue(const QString &key, const QVariant &var){
    ConfigurationManager::setValue(Globals::Paths::SETTINGS,Globals::Share::TEXT_CODEC,key,var.toString(),configuration);
}

void Loader::setValueForConfiguration(const QString &key, const QVariant &var) {
    configuration->addKeyValuePair(key,var);
}

//////////////////////////////////////////////////////// FILE MANAGEMENT FUNCTIONS ////////////////////////////////////////////////////////

bool Loader::createSubjectStudyFile(const QVariantMap &studyconfig, qint32 medic, const QString &protocol){

    if (!studyconfig.contains(Globals::StudyConfiguration::UNIQUE_STUDY_ID)){
        logger.appendError("While creating a subject study file, Study Configuration Map does not contain " + Globals::StudyConfiguration::UNIQUE_STUDY_ID + " field. Cannot determine study");
        logger.appendError(Debug::QVariantMapToString(studyconfig));
        return false;
    }

    // The first part of the name is base on the study type.
    qint32 selectedStudy = studyconfig.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toInt();
    QString filename;

    bool new_file = true;
    QString incomplete_study;
    qint32 percepetion_part;

    SubjectDirScanner sdc;
    sdc.setup(configuration->getString(Globals::Share::PATIENT_DIRECTORY),configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR));

    switch(selectedStudy){
    case Globals::StudyConfiguration::INDEX_BINDING_UC:
        filename = Globals::BaseFileNames::BINDING;

        // We need to check for ongoing studies with just BC.
        incomplete_study = sdc.findIncompleteBindingStudies(VMDC::Study::BINDING_UC,studyconfig);
        if (sdc.getError() != ""){
            logger.appendError("While scanning for incomplete binding files: " + sdc.getError());
            return false;
        }

        if (incomplete_study != ""){
            // An ongoing study requires unbound
            filename = incomplete_study;
            new_file = false;
        }
        break;
    case Globals::StudyConfiguration::INDEX_BINDING_BC:
        filename = Globals::BaseFileNames::BINDING;

        // We need to check for ongoing studies with just BC.
        incomplete_study = sdc.findIncompleteBindingStudies(VMDC::Study::BINDING_BC,studyconfig);
        if (sdc.getError() != ""){
            logger.appendError("While scanning for incomplete binding files: " + sdc.getError());
            return false;
        }

        if (incomplete_study != ""){
            // An ongoing study requires unbound
            filename = incomplete_study;
            new_file = false;
        }

        break;
    case Globals::StudyConfiguration::INDEX_GONOGO:
        filename = Globals::BaseFileNames::GONOGO;
        break;
    case Globals::StudyConfiguration::INDEX_NBACKMS:
        filename = Globals::BaseFileNames::NBACKMS;
        break;
    case Globals::StudyConfiguration::INDEX_NBACKRT:
        filename = Globals::BaseFileNames::NBACKRT;
        break;
    case Globals::StudyConfiguration::INDEX_NBACKVS:
        filename = Globals::BaseFileNames::NBACKVS;
        break;
    case Globals::StudyConfiguration::INDEX_PERCEPTION:
        filename = Globals::BaseFileNames::PERCEPTION;

        // This is the part of the perception study that we require.
        percepetion_part = studyconfig.value(VMDC::StudyParameter::PERCEPTION_PART).toInt();

        // We need to check for ongoing studies with not all the parts.
        incomplete_study =  sdc.findIncompletedPerceptionStudy(percepetion_part,studyconfig);
        if (sdc.getError() != ""){
            logger.appendError("While scanning for incomplete binding files: " + sdc.getError());
            return false;
        }

        if (incomplete_study != ""){
            // An ongoing study requires unbound
            filename = incomplete_study;
            new_file = false;
        }

        break;
    case Globals::StudyConfiguration::INDEX_READING:
        filename = Globals::BaseFileNames::READING;
        break;
    default:
        logger.appendError("Trying to create study file for an unknown study: " + QString::number(selectedStudy));
        break;
    }

    // Adding the full path.
    filename = Globals::Paths::WORK_DIRECTORY + "/" + configuration->getString(Globals::Share::PATIENT_UID) + "/" + filename;

    if (!new_file){
        // All is done we just need to set it as the current one.                
        logger.appendStandard("Continuing study in file " + filename);
        configuration->addKeyValuePair(Globals::Share::PATIENT_STUDY_FILE,filename);
        return true;
    }

    // Second part of the actuall file name is the time stamp.
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString hour = QDateTime::currentDateTime().toString("HH:mm:ss");
    filename = filename + "_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm") + ".json";

    // Creating the metadata.
    QVariantMap metadata;
    metadata.insert(VMDC::MetadataField::DATE,date);
    metadata.insert(VMDC::MetadataField::HOUR,hour);
    metadata.insert(VMDC::MetadataField::INSTITUTION_ID,configuration->getString(Globals::VMConfig::INSTITUTION_ID));
    metadata.insert(VMDC::MetadataField::INSTITUTION_INSTANCE,configuration->getString(Globals::VMConfig::INSTANCE_NUMBER));
    metadata.insert(VMDC::MetadataField::INSTITUTION_NAME,configuration->getString(Globals::VMConfig::INSTITUTION_NAME));
    metadata.insert(VMDC::MetadataField::MOUSE_USED,configuration->getString(Globals::VMPreferences::USE_MOUSE));
    metadata.insert(VMDC::MetadataField::PROC_PARAMETER_KEY,Globals::EyeTracker::PROCESSING_PARAMETER_KEY);
    metadata.insert(VMDC::MetadataField::STATUS,VMDC::StatusType::ONGOING);
    metadata.insert(VMDC::MetadataField::PROTOCOL,protocol);


    // Creating the subject data
    QVariantMap subject_data;
    subject_data.insert(VMDC::SubjectField::AGE,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_AGE));
    subject_data.insert(VMDC::SubjectField::BIRTH_COUNTRY,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_BIRTHCOUNTRY));
    subject_data.insert(VMDC::SubjectField::BIRTH_DATE,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_BIRTHDATE));
    subject_data.insert(VMDC::SubjectField::GENDER,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_GENDER));
    subject_data.insert(VMDC::SubjectField::INSTITUTION_PROVIDED_ID,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_INSTITUTION_ID));
    subject_data.insert(VMDC::SubjectField::LASTNAME,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_LASTNAME));
    subject_data.insert(VMDC::SubjectField::LOCAL_ID,configuration->getString(Globals::Share::PATIENT_UID));
    subject_data.insert(VMDC::SubjectField::NAME,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_NAME));
    subject_data.insert(VMDC::SubjectField::YEARS_FORMATION,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_YEARS_FORMATION));

    // Setting the evaluator info. We do it here becuase it is required for the data file comparisons.
    QVariantMap evaluator;
    QVariantMap evaluator_local_data = localDB.getEvaluatorData(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR));
    evaluator.insert(VMDC::AppUserField::EMAIL,evaluator_local_data.value(LocalDB::APPUSER_EMAIL));
    evaluator.insert(VMDC::AppUserField::NAME,evaluator_local_data.value(LocalDB::APPUSER_NAME));
    evaluator.insert(VMDC::AppUserField::LASTNAME,evaluator_local_data.value(LocalDB::APPUSER_LASTNAME));
    evaluator.insert(VMDC::AppUserField::LOCAL_ID,"");
    evaluator.insert(VMDC::AppUserField::VIEWMIND_ID,"");

    // Setting the selected doctor info.
    QVariantMap medic_to_store;
    QVariantMap medic_local_data = localDB.getMedicData(QString::number(medic));
    medic_to_store.insert(VMDC::AppUserField::EMAIL,medic_local_data.value(LocalDB::APPUSER_EMAIL));
    medic_to_store.insert(VMDC::AppUserField::NAME,medic_local_data.value(LocalDB::APPUSER_NAME));
    medic_to_store.insert(VMDC::AppUserField::LASTNAME,medic_local_data.value(LocalDB::APPUSER_LASTNAME));
    medic_to_store.insert(VMDC::AppUserField::LOCAL_ID,"");
    medic_to_store.insert(VMDC::AppUserField::VIEWMIND_ID,medic_local_data.value(LocalDB::APPUSER_VIEWMIND_ID));

    // We store this information the raw data container and leave it ready for the study to start.
    ViewMindDataContainer rdc;
    if (!rdc.setSubjectData(subject_data)){
        logger.appendError("Failed setting subject data to new study. Reason: " + rdc.getError());
        return false;
    }
    if (!rdc.setMetadata(metadata)){
        logger.appendError("Failed setting study metadata to new study. Reason: " + rdc.getError());
        return false;
    }

    if (!rdc.setApplicationUserData(VMDC::AppUserType::EVALUATOR,evaluator)){
        logger.appendError("Failed to store evaluator data: " + rdc.getError());
        return false;
    }

    if (!rdc.setApplicationUserData(VMDC::AppUserType::MEDIC,medic_to_store)){
        logger.appendError("Failed to store medic data: " + rdc.getError());
        return false;
    }

    if (!rdc.saveJSONFile(filename,Globals::Debug::PRETTY_PRINT_JSON)){
        logger.appendError("Failed on createing new study file: " + filename + ". Reason: " + rdc.getError());
        return false;
    }

    // Finally we set this as the current work file
    configuration->addKeyValuePair(Globals::Share::PATIENT_STUDY_FILE,filename);

    return true;

}

////////////////////////////////////////////////////////////////// EVALUATOR FUNCTIONS //////////////////////////////////////////////////////////////////
void Loader::logOut(){
    configuration->addKeyValuePair(Globals::Share::CURRENTLY_LOGGED_EVALUATOR,"");
}

bool Loader::isLoggedIn(){
    if (!configuration->containsKeyword(Globals::Share::CURRENTLY_LOGGED_EVALUATOR)) return false;
    return (configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR) != "");
}

bool Loader::checkIfEvaluatorEmailExists(const QString &username) const{
    return localDB.checkEvaluatorExists(username);
}

void Loader::addOrModifyEvaluator(const QString &email, const QString &oldemail ,const QString &password, const QString &name, const QString &lastname){

    // If it doesn't exist it will get an empty map. We check it with the current email as it is the the one that will change.
    QVariantMap map = localDB.getEvaluatorData(oldemail);

    map[LocalDB::APPUSER_LASTNAME] = lastname;
    map[LocalDB::APPUSER_NAME]     = name;

    // We store the data.
    if (!localDB.addOrModifyEvaluator(email,oldemail,map)){
        logger.appendError("While adding evaluator " + email + " with data " + name + " " + lastname + " (" + email + "): " + localDB.getError() );
        return;
    }

    if (password != ""){
        // Password is only updated when it's not empty. It's the responsiblity of the calling code to ensure a non empty password on creation.
        // qDebug() << "Setting password for " << email << " as " << password;
        localDB.setPasswordForEvaluator(email,password);
    }


}

bool Loader::evaluatorLogIn(const QString &username, const QString &password){
    if ( localDB.passwordCheck(username,password) ){
        configuration->addKeyValuePair(Globals::Share::CURRENTLY_LOGGED_EVALUATOR,username);
        return true;
    }
    return false;
}

void Loader::updateCurrentEvaluator(const QString &username){
    configuration->addKeyValuePair(Globals::Share::CURRENTLY_LOGGED_EVALUATOR,username);
}

QVariantMap Loader::getCurrentEvaluatorInfo() const{
    return localDB.getEvaluatorData(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR));
}

QStringList Loader::getLoginEmails() const {
    return localDB.getUsernameEmails();
}

////////////////////////////////////////////////////////////////// SUBJECT FUNCTIONS //////////////////////////////////////////////////////////////////

void Loader::addOrModifySubject(QString suid, const QString &name, const QString &lastname, const QString &institution_id,
                                const QString &age, const QString &birthdate, const QString &birthCountry,
                                const QString &gender, qint32 formative_years, qint32 selectedMedic){

    //qDebug() << "Entering with suid" << suid;

    if (suid == ""){
        // We need to generate a new internal id.
        suid = configuration->getString(Globals::VMConfig::INSTITUTION_ID) + "_" + configuration->getString(Globals::VMConfig::INSTANCE_NUMBER) + "_"
                + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
        //qDebug() << "Computed new suid" << suid;
    }

    // If a birthdate is provided, the age is computed
    QString saveage;
    if ((birthdate != "") && (age == "")){
        // We are expecting the ISO date.
        QDate bdate = QDate::fromString(birthdate,"yyyy-MM-dd");
        QDate currentDate= QDate::currentDate();    // gets the current date
        int currentAge = currentDate.year() - bdate.year();
        if ( (bdate.month() > currentDate.month()) || ( (bdate.month() == currentDate.month()) && (bdate.day() > currentDate.day()) ) ){
            currentAge--;
        }
        saveage = QString::number(currentAge);
    }
    else saveage = age;

    // Getting the country code.


    // Create a map for the data as is. The caller function is reponsible for data verification.
    QVariantMap map;
    map[LocalDB::SUBJECT_AGE] = saveage;
    map[LocalDB::SUBJECT_BIRTHCOUNTRY] = countries->getCodeForCountry(birthCountry);
    map[LocalDB::SUBJECT_BIRTHDATE] = birthdate;
    map[LocalDB::SUBJECT_INSTITUTION_ID] = institution_id;
    map[LocalDB::SUBJECT_LASTNAME] = lastname;
    map[LocalDB::SUBJECT_NAME] = name;
    map[LocalDB::SUBJECT_YEARS_FORMATION] = formative_years;
    map[LocalDB::SUBJECT_GENDER] = gender;
    map[LocalDB::SUBJECT_ASSIGNED_MEDIC] = selectedMedic;

    // Adding the data to the local database.
    if (!localDB.addOrModifySubject(suid,map)){
        logger.appendError("While adding subject " + suid + " with data " + name + " " + lastname + " (" + institution_id + "): " + localDB.getError() );
    }
}

QVariantMap Loader::filterSubjectList(const QString &filter){
    return localDB.getDisplaySubjectList(filter);
}

bool Loader::setSelectedSubject(const QString &suid){

    // Set the id of the currently selected patient.
    configuration->addKeyValuePair(Globals::Share::PATIENT_UID,suid);

    // Creating the patient id
    QString patdirPath = QString(Globals::Paths::WORK_DIRECTORY) + "/" + suid;
    if (!QDir(patdirPath).exists()){
        QDir baseDir(Globals::Paths::WORK_DIRECTORY);
        baseDir.mkdir(suid);
    }

    if (!QDir(patdirPath).exists()){
        logger.appendError("Could not create patient dir " + suid + " in directory");
        return false;
    }

    configuration->addKeyValuePair(Globals::Share::PATIENT_DIRECTORY,patdirPath);
    return true;

}

void Loader::setStudyMarkerFor(const QString &study, const QString &value){
    if (!localDB.addStudyMarkerForSubject(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR),study,value)){
        logger.appendError("Failed in setting study marker: " + localDB.getError());
    }
}

QString Loader::getStudyMarkerFor(const QString &study){
    return localDB.getMarkerForStudy(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR),study).toString();
}

QVariantMap Loader::getCurrentSubjectInfo(){
    //    QVariantMap a = localDB.getSubjectData(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR));
    //    qDebug() << a;
    return localDB.getSubjectData(configuration->getString(Globals::Share::PATIENT_UID));
}

void Loader::clearSubjectSelection(){
    configuration->addKeyValuePair(Globals::Share::CURRENTLY_LOGGED_EVALUATOR,"");
}

QString Loader::getCurrentlySelectedAssignedDoctor() const {
    return localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_ASSIGNED_MEDIC);
}

////////////////////////// MEDIC RELATED FUNCTIONS ////////////////////////////
QVariantMap Loader::getMedicList() const {
    return localDB.getMedicDisplayList();
}

////////////////////////// REPORT GENERATING FUNCTIONS ////////////////////////////
QVariantMap Loader::getReportsForLoggedEvaluator(){
    QStringList directory_list = QDir(Globals::Paths::WORK_DIRECTORY).entryList(QStringList(),QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);
    QString current_evaluator = configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR);
    QList<QVariantMap> studiesToAnalyze;

    SubjectDirScanner sdc;

    //qDebug() << "Will analyze list of directories" << directory_list;

    for (qint32 i = 0; i < directory_list.size(); i++){

        sdc.setup(Globals::Paths::WORK_DIRECTORY + "/" + directory_list.at(i),current_evaluator);
        studiesToAnalyze << sdc.scanSubjectDirectoryForEvalutionsFrom();
        if (!sdc.getError().isEmpty()){
            logger.appendError("Error while analyzig " + sdc.getSetDirectory() + " for finished studies: " + sdc.getError());
        }
        //qDebug() << "Added " << sdc.getSetDirectory() << " and now we have " << studiesToAnalyze.size();
    }

    // Sort the generated list by the order code before returning it (chronologically).
    return SubjectDirScanner::sortSubjectDataListByOrder(studiesToAnalyze);

}


void Loader::setCurrentStudyFileForQC(const QString &file){
    configuration->addKeyValuePair(Globals::Share::SELECTED_STUDY,file);
    if (!qc.setVMContainterFile(file)){
        logger.appendError("Failed setting selected study to:  " + file + ". Reason: " + qc.getError());
    }
}

QStringList Loader::getStudyList() const {
    return qc.getStudyList();
}

QVariantMap Loader::getStudyGraphData(const QString &study, qint32 selectedGraph){

    //QString

}

////////////////////////////////////////////////////////////////// API FUNCTIONS //////////////////////////////////////////////////////////////////

void Loader::requestOperatingInfo(){
    if (!apiclient.requestOperatingInfo()){
        logger.appendError("Request operating info error: "  + apiclient.getError());
    }
}


void Loader::receivedRequest(){
    if (!apiclient.getError().isEmpty()){
        logger.appendError("Error Receiving Request :"  + apiclient.getError());
    }
    else{
        QVariantMap ret = apiclient.getMapDataReturned();
        //Globals::Debug::prettpPrintQVariantMap(ret);
        if (!localDB.setMedicInformationFromRemote(ret.value(APINames::MAIN_DATA).toMap())){
            logger.appendError("Failed to set medical professionals info from server: " + localDB.getError());
        }
        if (!localDB.setProcessingParametersFromServerResponse(ret.value(APINames::MAIN_DATA).toMap())){
            logger.appendError("Failed to set processing parameters from server: " + localDB.getError());
        }
        if (!localDB.setQCParametersFromServerResponse(ret.value(APINames::MAIN_DATA).toMap())){
            logger.appendError("Failed to set QC parameters from server: " + localDB.getError());
        }
    }
    emit(finishedRequest());
}

////////////////////////////////////////////////////////////////// PROTOCOL FUNCTIONS //////////////////////////////////////////////////////////////////

bool Loader::addProtocol(const QString &p) {
    return localDB.addProtocol(p);
}
void Loader::deleteProtocol(const QString &p) {
    if (!localDB.removeProtocol(p)){
        logger.appendError("Error while deleting protocol: " + localDB.getError());
    }
}

QStringList Loader::getProtocolList() {
    return localDB.getProtocolList();
}


////////////////////////////////////////////////////////////////// PRIVATE FUNCTIONS //////////////////////////////////////////////////////////////////

void Loader::changeLanguage(){
    QString lang = configuration->getString(Globals::VMPreferences::UI_LANGUAGE);
    if (lang == Globals::UILanguage::ES){
        if (!language.loadConfiguration(":/languages/es.lang",Globals::Share::TEXT_CODEC)){
            // In a stable program this should NEVER happen.
            logger.appendError("CANNOT LOAD ES LANG FILE: " + language.getError());
            loadingError = true;
        }
        else countries->fillCountryList(false);
    }
    else{
        // Defaults to english
        if (!language.loadConfiguration(":/languages/en.lang",Globals::Share::TEXT_CODEC)){
            // In a stable program this should NEVER happen.
            logger.appendError("CANNOT LOAD EN LANG FILE: " + language.getError());
            loadingError = true;
        }
        else countries->fillCountryList(true);
    }
}

void Loader::loadDefaultConfigurations(){

    if (!configuration->containsKeyword(Globals::VMPreferences::UI_LANGUAGE)) configuration->addKeyValuePair(Globals::VMPreferences::UI_LANGUAGE,Globals::UILanguage::EN);
    if (!configuration->containsKeyword(Globals::VMPreferences::DEFAULT_COUNTRY)) configuration->addKeyValuePair(Globals::VMPreferences::DEFAULT_COUNTRY,"AR");
    if (!configuration->containsKeyword(Globals::VMPreferences::DEMO_MODE)) configuration->addKeyValuePair(Globals::VMPreferences::DEMO_MODE,false);
    if (!configuration->containsKeyword(Globals::VMPreferences::USE_MOUSE)) configuration->addKeyValuePair(Globals::VMPreferences::USE_MOUSE,true);
    if (!configuration->containsKeyword(Globals::VMPreferences::DUAL_MONITOR_MODE)) configuration->addKeyValuePair(Globals::VMPreferences::DUAL_MONITOR_MODE,false);
    if (!configuration->containsKeyword(Globals::VMPreferences::DEFAULT_READING_LANGUAGE)) configuration->addKeyValuePair(Globals::VMPreferences::DEFAULT_READING_LANGUAGE,0);

}

Loader::~Loader(){

}


