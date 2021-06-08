#include "loader.h"

const char * Loader::FILENAME_BASE_READING = "reading";
const char * Loader::FILENAME_BASE_BINDING = "binding";
const char * Loader::FILENAME_BASE_NBACKVS = "nbackvs";
const char * Loader::FILENAME_BASE_NBACKRT = "nbackrt";
const char * Loader::FILENAME_BASE_NBACKMS = "nbackms";
const char * Loader::FILENAME_BASE_PERCEPTION = "perception";
const char * Loader::FILENAME_BASE_GONOGO = "gonogo";

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

    if (!localDB.setDBFile(Globals::Paths::LOCALDB,Globals::Paths::DBBKPDIR,Globals::Debug::PRETTY_PRINT_JSON,Globals::Debug::DISABLE_DB_CHECKSUM)){
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

bool Loader::createSubjectStudyFile(const QVariantMap &studyconfig){

    if (!studyconfig.contains(Globals::StudyConfiguration::UNIQUE_STUDY_ID)){
        logger.appendError("While creating a subject study file, Study Configuration Map does not contain " + Globals::StudyConfiguration::UNIQUE_STUDY_ID + " field. Cannot determine study");
        return false;
    }

    // The first part of the name is base on the study type.
    qint32 selectedStudy = studyconfig.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toInt();
    QString filename;

    bool new_file = true;
    QMap<QString,QString> ongoing_studies;
    QStringList trialtypelist;
    QString perception_part;

    QVariantMap sc;

    switch(selectedStudy){
    case Globals::StudyConfiguration::INDEX_BINDING_UC:
        filename = FILENAME_BASE_BINDING;
        // We need to check for ongoing studies with just BC.
        ongoing_studies = findOngoingStudyFileNames(studyconfig);
        if (ongoing_studies.contains(RDC::TrialListType::BOUND)){
            // An ongoing study requires unbound
            filename = ongoing_studies.value(RDC::TrialListType::UNBOUND);
            new_file = false;
        }
        break;
    case Globals::StudyConfiguration::INDEX_BINDING_BC:
        filename = FILENAME_BASE_BINDING;
        // We need to check for ongoing studies with just BC.
        ongoing_studies = findOngoingStudyFileNames(studyconfig);
        if (ongoing_studies.contains(RDC::TrialListType::BOUND)){
            // An ongoing study requires unbound
            filename = ongoing_studies.value(RDC::TrialListType::UNBOUND);
            new_file = false;
        }
        break;
    case Globals::StudyConfiguration::INDEX_GONOGO:
        filename = FILENAME_BASE_GONOGO;
        break;
    case Globals::StudyConfiguration::INDEX_NBACKMS:
        filename = FILENAME_BASE_NBACKMS;
        break;
    case Globals::StudyConfiguration::INDEX_NBACKRT:
        filename = FILENAME_BASE_NBACKRT;
        break;
    case Globals::StudyConfiguration::INDEX_NBACKVS:
        filename = FILENAME_BASE_NBACKVS;
        break;
    case Globals::StudyConfiguration::INDEX_PERCEPTION:
        filename = FILENAME_BASE_PERCEPTION;

        // This is the part of the perception study that we require.
        perception_part = studyconfig.value(RDC::StudyParameter::PERCEPTION_PART).toString();

        // We need to check for ongoing studies with just BC.
        ongoing_studies = findOngoingStudyFileNames(studyconfig);
        trialtypelist = ongoing_studies.keys();

        for (qint32 i = 0; i < trialtypelist.size(); i++){
            if (trialtypelist.at(i) == perception_part){
                filename = ongoing_studies.value(trialtypelist.at(i));
                new_file = false;
            }
        }

        break;
    case Globals::StudyConfiguration::INDEX_READING:
        filename = FILENAME_BASE_READING;
        break;
    default:
        logger.appendError("Trying to create study file for an unknown study: " + QString::number(selectedStudy));
        break;
    }

    if (!new_file){
        // All is done we just need to set it as the current one.
        logger.appendStandard("Continuging study in file " + filename);
        configuration->addKeyValuePair(Globals::Share::PATIENT_STUDY_FILE,filename);
        return true;
    }

    // Second part is the time stamp.
    QString date = QDateTime::currentDateTime().toString("dd/MM/yyyy");
    QString hour = QDateTime::currentDateTime().toString("HH:mm");
    filename = filename + "_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm") + ".json";

    // Creating the metadata.
    QVariantMap metadata;
    metadata.insert(RDC::MetadataField::DATE,date);
    metadata.insert(RDC::MetadataField::HOUR,hour);
    metadata.insert(RDC::MetadataField::INSTITUTION_ID,configuration->getString(Globals::VMConfig::INSTITUTION_ID));
    metadata.insert(RDC::MetadataField::INSTITUTION_INSTANCE,configuration->getString(Globals::VMConfig::INSTANCE_NUMBER));
    metadata.insert(RDC::MetadataField::INSTITUTION_KEY,configuration->getString(Globals::VMConfig::INSTANCE_KEY));
    metadata.insert(RDC::MetadataField::INSTITUTION_NAME,configuration->getString(Globals::VMConfig::INSTITUTION_NAME));
    metadata.insert(RDC::MetadataField::MOUSE_USED,configuration->getString(Globals::VMPreferences::USE_MOUSE));
    metadata.insert(RDC::MetadataField::PROC_PARAMETER_KEY,Globals::EyeTracker::PROCESSING_PARAMETER_KEY);

    // Creating the subject data
    QVariantMap subject_data;
    subject_data.insert(RDC::SubjectField::AGE,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_AGE));
    subject_data.insert(RDC::SubjectField::BIRTH_COUNTRY,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_BIRTHCOUNTRY));
    subject_data.insert(RDC::SubjectField::BIRTH_DATE,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_BIRTHDATE));
    subject_data.insert(RDC::SubjectField::GENDER,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_GENDER));
    subject_data.insert(RDC::SubjectField::INSTITUTION_PROVIDED_ID,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_INSTITUTION_ID));
    subject_data.insert(RDC::SubjectField::LASTNAME,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_LASTNAME));
    subject_data.insert(RDC::SubjectField::LOCAL_ID,configuration->getString(Globals::Share::PATIENT_UID));
    subject_data.insert(RDC::SubjectField::NAME,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_NAME));
    subject_data.insert(RDC::SubjectField::YEARS_FORMATION,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_YEARS_FORMATION));

    // We store this information the raw data container and leave it ready for the study to start.
    RawDataContainer rdc;
    if (!rdc.setSubjectData(subject_data)){
        logger.appendError("Failed setting subject data to new study. Reason: " + rdc.getError());
        return false;
    }
    if (!rdc.setMetadata(metadata)){
        logger.appendError("Failed setting study metadata to new study. Reason: " + rdc.getError());
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
                                const QString &gender, qint32 formative_years){

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
    configuration->addKeyValuePair(Globals::Share::CURRENTLY_LOGGED_EVALUATOR,suid);

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
    return localDB.getSubjectData(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR));
}

void Loader::clearSubjectSelection(){
    configuration->addKeyValuePair(Globals::Share::CURRENTLY_LOGGED_EVALUATOR,"");
}


////////////////////////// REPORT GENERATING FUNCTIONS ////////////////////////////
QList<QVariantMap> Loader::getReportsForLoggedEvaluator(){
    QStringList directory_list = QDir(Globals::Paths::WORK_DIRECTORY).entryList(QStringList(),QDir::Dirs,QDir::Name);
    QString current_evaluator = configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR);
    QStringList errors;
    QList<QVariantMap> studiesToAnalyze;
    for (qint32 i = 0; i < directory_list.size(); i++){
        studiesToAnalyze << SubjectDirScanner::scanSubjectDirectoryForEvalutionsFrom(QString(Globals::Paths::WORK_DIRECTORY) + "/" + directory_list.at(i),current_evaluator,&errors);
    }
    if (!errors.isEmpty()){
        logger.appendError("Errors found scanning work directory for user " + current_evaluator);
        for (qint32 i = 0; i < errors.size(); i++){
            logger.appendError("   " + errors.at(i));
        }
    }

    // Sort the generated list by the order code before returning it (chronologically).
    SubjectDirScanner::sortSubjectDataListByOrder(&studiesToAnalyze);
    return studiesToAnalyze;

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
        Globals::Debug::prettpPrintQVariantMap(ret);
        if (!localDB.setMedicInformationFromRemote(ret.value(APINames::MAIN_DATA).toMap())){
            logger.appendError("Failed to set medical professionals info from server: " + localDB.getError());
        }
        if (!localDB.setProcessingParametersFromServerResponse(ret.value(APINames::MAIN_DATA).toMap())){
            logger.appendError("Failed to set processing parameters from server: " + localDB.getError());
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

QMap<QString, QString> Loader::findOngoingStudyFileNames(QVariantMap study_config){

    QStringList filters; filters << "*.json";
    QStringList filelist = QDir(configuration->getString(Globals::Share::PATIENT_DIRECTORY)).entryList(filters,QDir::Files,QDir::Name);

    QMap<QString,QString> ans;

    for (qint32 i = 0; i < filelist.size(); i++){

        // Only a few studies are multi part. So we check for one fo those before loading.
        if (filelist.at(i).contains(FILENAME_BASE_BINDING)){

            RawDataContainer rdc;
            if (!rdc.loadFromJSONFile(configuration->getString(Globals::Share::PATIENT_DIRECTORY) + "/" + filelist.at(i))){
                logger.appendError("Error loading JSON File for ongoing analysis: " + rdc.getError());
                continue;
            }

            // We can assume that, given the file name, the only study is a binding study.
            if (!rdc.isStudyOngoing(RDC::Study::BINDING)){
                // Check for errors just in case
                if (!rdc.getError().isEmpty()){
                    logger.appendError("Error getting binding study status from JSON file: " + rdc.getError() + " from file "
                                       + configuration->getString(Globals::Share::PATIENT_DIRECTORY) + "/" + filelist.at(i));
                }
                continue;
            }

            // The study is ongiong. Next we check that less than 24 hours have passed in order to make it viable.

            QStringList study_datetime = rdc.getMetaDataDateTime();
            QDateTime dt = QDateTime::fromString(study_datetime.first() + " " + study_datetime.last(), "yyyy-MM-dd HH:mm");

            // And now we check that the study configuration is the same.
            QVariantMap sc = rdc.getStudyConfiguration(RDC::Study::BINDING);
            bool aresame = true;
            aresame = aresame && (sc.value(RDC::StudyParameter::VALID_EYE).toString() == study_config.value(RDC::StudyParameter::VALID_EYE).toString());
            aresame = aresame && (sc.value(RDC::StudyParameter::NUMBER_TARGETS).toString() == study_config.value(RDC::StudyParameter::NUMBER_TARGETS).toString());
            aresame = aresame && (sc.value(RDC::StudyParameter::TARGET_SIZE).toString() == study_config.value(RDC::StudyParameter::TARGET_SIZE).toString());

            // They were configured differently.
            if (!aresame) continue;

            if (dt.secsTo(QDateTime::currentDateTime()) < NUMBER_SECONDS_IN_A_DAY){
                QStringList list = rdc.getTrialListTypesForStudy(RDC::Study::BINDING);

                // If it's an ongoing trial, as it should be, then from the list either bound or unboudn is missing.
                if (list.size() != 1){
                    logger.appendError("Error getting the trial list types for binding in ongoing analysis. Got a list of size " + QString::number(list.size())
                                       +  ". Ongoing binding should not have more than 1.  RawDatacontainer error: " + rdc.getError());
                    continue;
                }

                if (list.contains(RDC::TrialListType::BOUND)){
                    ans[RDC::TrialListType::UNBOUND] = filelist.at(i);
                }
                else if (list.contains(RDC::TrialListType::UNBOUND)){
                    ans[RDC::TrialListType::BOUND] = filelist.at(i);
                }
                else {
                    logger.appendError("Error on ongoing analysis for binding got a trial list tyep that was neither bound or unbound: " + list.first());
                }

            }

        }
        else if (filelist.at(i).contains(FILENAME_BASE_PERCEPTION)){

            RawDataContainer rdc;
            if (!rdc.loadFromJSONFile(configuration->getString(Globals::Share::PATIENT_DIRECTORY) + "/" + filelist.at(i))){
                logger.appendError("Error loading JSON File for ongoing analysis: " + rdc.getError());
                continue;
            }

            // We can assume that, given the file name, the only study is a binding study.
            if (!rdc.isStudyOngoing(RDC::Study::PERCEPTION)){
                // Check for errors just in case
                if (!rdc.getError().isEmpty()){
                    logger.appendError("Error getting perception study status from JSON file: " + rdc.getError() + " from file "
                                       + configuration->getString(Globals::Share::PATIENT_DIRECTORY) + "/" + filelist.at(i));
                }
                continue;
            }

            // The study is ongiong. Next we check that less than 24 hours have passed in order to make it viable.

            QStringList study_datetime = rdc.getMetaDataDateTime();
            QDateTime dt = QDateTime::fromString(study_datetime.first() + " " + study_datetime.last(), "yyyy-MM-dd HH:mm");

            // And now we check that the study configuration is the same.
            QVariantMap sc = rdc.getStudyConfiguration(RDC::Study::PERCEPTION);
            bool aresame = true;
            aresame = aresame && (sc.value(RDC::StudyParameter::VALID_EYE) == study_config.value(RDC::StudyParameter::VALID_EYE));
            aresame = aresame && (sc.value(RDC::StudyParameter::PERCEPTION_TYPE) == study_config.value(RDC::StudyParameter::PERCEPTION_TYPE));

            // If they are not configured the same, then we move on.
            if (!aresame) continue;

            if (dt.secsTo(QDateTime::currentDateTime()) < NUMBER_SECONDS_IN_A_DAY){
                QStringList list = rdc.getTrialListTypesForStudy(RDC::Study::PERCEPTION);

                // If it's an ongoing trial, as it should be, then from the list either bound or unboudn is missing.
                if ((list.size() < 1) || (list.size() >= NUMBER_OF_PERCEPTION_PARTS)){
                    logger.appendError("Error getting the trial list types for perception in ongoing analysis. Got a list of size " + QString::number(list.size())
                                       +  ". Ongoing  perception should have between 1 and 7.  RawDatacontainer error: " + rdc.getError());
                    continue;
                }

                QStringList perception_parts = RDC::PerceptionPart::valid;
                for (qint32 j = 0; j < perception_parts.size();  j++){
                    if (!list.contains(perception_parts.at(j))){
                        ans[perception_parts.at(j)] = filelist.at(i);
                    }
                }

            }

        }
    }
    return ans;
}


Loader::~Loader(){

}





//******************************************* Updater Related Functions ***********************************************

//QString Loader::checkForChangeLog(){
//    QString filePath = "launcher/" + QString (FILE_CHANGELOG_UPDATER) + "_"  + configuration->getString(Globals::VMPreferences::REPORT_LANGUAGE);
//    //qWarning() << "Searching for changelog" << filePath;
//    QFile file(filePath);
//    if (!file.open(QFile::ReadOnly)) return "";
//    QTextStream reader(&file);
//    reader.setCodec(COMMON_TEXT_CODEC);
//    QString content = reader.readAll();
//    if (content.size() < 4) return "";

//    // Getting only the last N Updates
//    // MAX_UPDATES_TO_SHOW
//    int updateCounter = 0;
//    bool enableCounter = false;
//    QStringList lines = content.split("\n");
//    content = "";
//    while (!lines.isEmpty()){

//        QString line = lines.first();
//        lines.removeFirst();
//        line = line.trimmed();

//        if (!line.startsWith("-")){
//            // The logic is like this: Lines that do not start with "-" are update titles.
//            // It's necessary to add everything that comes after the line until another line
//            // that does not start with "-" to actually "add" an update to the variable.
//            if (enableCounter) {
//                enableCounter = false;
//                updateCounter++;
//            }
//            else{
//                enableCounter = true;
//            }
//        }

//        if (updateCounter == MAX_UPDATES_TO_SHOW){
//            break;
//        }
//        else{
//            //qDebug() << updateCounter << ": ->" << line;
//            content = content + line + "\n";
//        }
//    }

//    return content;
//}

//bool Loader::clearChangeLogFile(){
//    // return false;  // FOR DEBUGGING CHANGELOG WINDOW
//    QDir dir(DIRNAME_LAUNCHER);
//    QStringList filter;
//    filter << QString(FILE_CHANGELOG_UPDATER) + "*";
//    QStringList allchangelogs = dir.entryList(filter,QDir::Files);
//    for (qint32 i = 0; i < allchangelogs.size(); i++){
//        QFile file(QString(DIRNAME_LAUNCHER) + "/" + allchangelogs.at(i));
//        file.remove();
//    }

//    QFile newLauncher(QString(DIRNAME_LAUNCHER) + "/" + QString(FILE_NEW_LAUCHER_FILE));
//    return newLauncher.exists();
//}

//void Loader::replaceEyeLauncher(){
//    QString newfile = QString(DIRNAME_LAUNCHER) + "/" + QString(FILE_NEW_LAUCHER_FILE);
//    QString oldfile = QString(DIRNAME_LAUNCHER) + "/" + QString(FILE_OLD_LAUCHER_FILE);
//    QString currentfile = QString(DIRNAME_LAUNCHER) + "/" + QString(FILE_EYE_LAUCHER_FILE);

//    // Backing up current
//    QFile(oldfile).remove();
//    if (!QFile::copy(currentfile,oldfile)){
//        logger.appendError("Failed to backup EyeLauncherClient from " + currentfile + " to " + oldfile);
//        QCoreApplication::quit();
//    }

//    // Replacing the current with the new
//    QFile(currentfile).remove();
//    if (!QFile::copy(newfile,currentfile)){
//        logger.appendError("Failed to update EyeLauncherClient from " + newfile + " to " + currentfile);
//        QCoreApplication::quit();
//    }

//    // Removing the new file
//    if (!QFile(newfile).remove()){
//        logger.appendError("Could not remove new EyeLauncherClient file: " + newfile);
//    }

//    QCoreApplication::quit();
//}


