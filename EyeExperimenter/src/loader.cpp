#include "loader.h"

Loader::Loader(QObject *parent, ConfigurationManager *c) : QObject(parent)
{

    // Connecting the API Client slot.
    connect(&apiclient, &APIClient::requestFinish, this ,&Loader::receivedAPIResponse);
    connect(&fileDownloader,&FileDownloader::downloadCompleted,this,&Loader::updateDownloadFinished);
    connect(&fileDownloader,&FileDownloader::downloadProgress,this,&Loader::onFileDownloaderUpdate);
    connect(&qcChecker,&StudyEndOperations::finished,this,&Loader::startUpSequenceCheck);

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
    isLicenceFilePresent = true;
    if (!configuration->loadConfiguration(Globals::Paths::CONFIGURATION)){
        StaticThreadLogger::error("Loader::Loader","Errors loading the configuration file: " + configuration->getError());
        // If the license file hast error or ist not present then we neeed to set up for autoconfiguration.
        isLicenceFilePresent = false;
        // loadingError = true;
    }


    institutionStringIdentification = "";

    if (isLicenceFilePresent){
        institutionStringIdentification = configuration->getString(Globals::VMConfig::INSTITUTION_NAME)
                + " (" + configuration->getString(Globals::VMConfig::INSTITUTION_ID) + "." + configuration->getString(Globals::VMConfig::INSTANCE_NUMBER) + ")";
    }

    // Now we load the local DB.
    if (!localDB.setDBFile(Globals::Paths::LOCALDB,Globals::Paths::DBBKPDIR,DBUGBOOL(Debug::Options::PRETTY_PRINT_DB),DBUGBOOL(Debug::Options::DISABLE_DB_CHECKSUM))){
        StaticThreadLogger::error("Loader::Loader","Could not set local db file: " + localDB.getError());
        loadingError = true;
        return;
    }


    changeLanguage();
    //qDebug() << "LOADER CONSTRUCTOR. Teh explanation language is " << configuration->getString(Globals::VMPreferences::EXPLANATION_LANGUAGE);
    setExplanationLanguage();

    // We now load the number of Slides that each explanation contains. This is independant of language, but we do need the language file loaded.
    StudyControl::FillNumberOfSlidesInExplanations(&explanationStrings);

    // Must make sure that the data directory exists
    QDir rawdata(Globals::Paths::WORK_DIRECTORY);
    if (!rawdata.exists()){
        if (!QDir(".").mkdir(Globals::Paths::WORK_DIRECTORY)){
            StaticThreadLogger::error("Loader::Loader","Cannot create viewmind_data directory");
            loadingError = true;
            return;
        }
    }

    if (!localDB.processingParametersPresent()){
        StaticThreadLogger::warning("Loader::Loader","Processing parameters are not present in local database. Will not be able to do any studies");
    }

    // Setting the current version and check if it changed. If it did this is a first time run.
    firstTimeRun = localDB.setApplicationVersion(Globals::Share::EXPERIMENTER_VERSION_NUMBER);

    // Configuring the API Client.
    apiclient.configure(configuration->getString(Globals::VMConfig::INSTITUTION_ID),
                        configuration->getString(Globals::VMConfig::INSTANCE_NUMBER),
                        Globals::Share::EXPERIMENTER_VERSION_NUMBER,
                        Globals::REGION,
                        configuration->getString(Globals::VMConfig::INSTANCE_KEY),
                        configuration->getString(Globals::VMConfig::INSTANCE_HASH_KEY));


    if (!localDB.isVersionUpToDate()){
        localDB.replaceMedicKeys();
    }

    // Getting the HW Data, and printing out for debugging.
    StaticThreadLogger::log("Loader::Loader","HW SPECS\n" + hwRecognizer.toString(true));

}

//////////////////////////////////////////////////////// UI Functions ////////////////////////////////////////////////////////
bool Loader::isVMConfigPresent() const {
    return isLicenceFilePresent;
}

bool Loader::instanceDisabled() const {
    return !this->localDB.isInstanceEnabled();
}

QVariantMap Loader::getInstIDFileInfo() const {

    // The inst ID file info is simply a file that identifies the intances (instance and institution number) when the license file is not present
    // It is used for both the functional verification screen and the activation screens.

    ConfigurationManager config_vminstid;
    if (!config_vminstid.loadConfiguration(Globals::Paths::VMINSTID)) return QVariantMap();
    return config_vminstid.getMap();

}

QVariantMap Loader::getHWInfo() const {
    return this->hwRecognizer.getHardwareSpecsAsVariantMap();
}

QVariantList Loader::findPossibleDupes(QString name, QString lname, QString personalID, QString birthDate){
    return localDB.possibleNewPatientMatches(name,lname,personalID,birthDate,getStringListForKey("viewpatlist_months"));
}

void Loader::storeNewStudySequence(const QString &name, const QVariantList &sequence){
    if (!localDB.storeStudySequence(name,sequence)){
        StaticThreadLogger::error("Loader::storeNewStudySequence","Failed to write db file to disk on storing new study sequence");
    }

    // Whenever we store a sequence, that sequence becomes the last selected sequence.
    if (!localDB.setLastSelectedSequence(name)){
        StaticThreadLogger::error("Loader::storeNewStudySequence","Failed to write db file to disk on storing last selected sequence");
    }

}

QVariantList Loader::getStudySequence(const QString &name){
    QVariantList list = localDB.getStudySequence(name);
    // Whenever a study sequece is selected that is a new study sequence. If it's the empty sequence then that is what it is.
    if (!localDB.setLastSelectedSequence(name)){
        StaticThreadLogger::error("Loader::getStudySequence","Failed to write db file to disk on storing last selected sequence");
    }
    return list;
}

void Loader::logUIMessage(const QString &message, bool isError){
    if (isError){
        StaticThreadLogger::error("Loader::logUIMessage",message);
    }
    else {
        StaticThreadLogger::log("Loader::logUIMessage",message);
    }
}

void Loader::setCurrentStudySequence(const QString &name){
    if (!localDB.setLastSelectedSequence(name)){
        StaticThreadLogger::error("Loader::setCurrentStudySequence","Failed to write db file to disk on storing last selected sequence");
    }
}

QVariantMap Loader::getStudySequenceListAndCurrentlySelected() const{

    QString seqname = localDB.getLastSelectedSequence();
    QStringList list = localDB.getStudySequenceList();
    QVariantMap map;
    map["current"] = seqname;
    map["list"] = list;
    return map;

}

void Loader::deleteStudySequence(const QString &name){
    if (!localDB.deleteStudySequence(name)){
        StaticThreadLogger::error("Loader::deleteStudySequence","Failed to write db file to disk on deleting sequence");
    }
}

void Loader::changeGetVMConfigScreenLanguage(const QString &var){
    // The language selection is stored in the preferences file.
    localDB.setPreference(LocalDB::PREF_UI_LANG,var);
    // And then it's loaded.
    changeLanguage();
}

QString Loader::getStringForKey(const QString &key, bool fromLangFile){
    //if (!fromLangFile) qDebug() << "Entering with key " << key << " but false from langfile";
    if (fromLangFile){
        if (language.containsKeyword(key)){
            return language.getString(key);
        }
        else {
            //qDebug() << "Could nto find" << key << "in lang file";
            return "ERROR: STRING KEY " + key +  " NOT FOUND";
        }
    }
    else {
        if (explanationStrings.containsKeyword(key)){
            return explanationStrings.getString(key);
        }
        else return "ERROR: EXPLANATION STRING KEY " + key +  " NOT FOUND";
    }
}

QStringList Loader::getStringListForKey(const QString &key, bool fromLangFile){
    if (fromLangFile){
        if (language.containsKeyword(key)){
            return language.getStringList(key);
        }
        else {
            //qDebug() << language.getAllKeys();
            return QStringList();
        }
    }
    else {
        if (explanationStrings.containsKeyword(key)){
            return explanationStrings.getStringList(key);
        }
        else {
            //qDebug() << language.getAllKeys();
            return QStringList();
        }
    }
}

void Loader::setExplanationLanguage(){

    QString lang_code = localDB.getPreference(LocalDB::PREF_EXP_LANG,"en").toString();

    //qDebug() << "Switching language explanations to " << lang_code;

    QString exp_lang_file = ":/languages/exp_lang/" + lang_code + ".lang";

    if (!QFile::exists(exp_lang_file)){
        StaticThreadLogger::error("Loader::setExplanationLanguage","Cannot find lang file '" + exp_lang_file  +"'. Defaulting to English");
        exp_lang_file = ":/languages/exp_lang/en.lang";
    }

    if (!explanationStrings.loadConfiguration(exp_lang_file)){
        // In a stable program this should NEVER happen.
        StaticThreadLogger::error("Loader::setExplanationLanguage","CANNOT LOAD Explanation language file LANG FILE: '" + exp_lang_file + "'. Reason: " + language.getError());
    }

}

QVariantMap Loader::getExplanationLangMap() {
    QVariantMap map =  Globals::ExplanationLanguage::GetNameCodeMap();
    map["--"] = localDB.getPreference(LocalDB::PREF_EXP_LANG,"en").toString();
    return map;
}

QVariantMap Loader::getStudyNameMap() {

    QStringList word_list = getStringListForKey("viewQC_StudyNameMap");
    QVariantMap map;
    QString lastKey = "";

    // The map is structured as a list where the sequence is Study Name -> Language Name.
    for (qint32 i = 0; i < word_list.size(); i++){
        if ((i % 2) == 0){
            lastKey = word_list.at(i);
        }
        else {
            map[lastKey] = word_list.at(i);
        }
    }

    return map;

}

bool Loader::getLoaderError() const {
    return loadingError;
}

QString Loader::getWindowTilteVersion(){
    QString dbug_str = Debug::CreateDebugOptionSummary();
    QString version = Globals::Share::EXPERIMENTER_VERSION_NUMBER + " - " +
            eyeTrackerName + " - " +
            institutionStringIdentification;
    if (Globals::REGION != Globals::GLOBAL::REGION){
        version = version + " - " + Globals::REGION ;
    }

    if (!dbug_str.isEmpty()){
        version = version + " - " + dbug_str;
    }

    version = Globals::Share::APP_NAME + " - " + version;

    return version;
}

QString Loader::getVersionNumber() const {
    return Globals::Share::EXPERIMENTER_VERSION_NUMBER;
}

QString Loader::getInstitutionName() const {
    return configuration->getString(Globals::VMConfig::INSTITUTION_NAME);
}


void Loader::openUserManual(){

    QString lang = localDB.getPreference(LocalDB::PREF_UI_LANG,"English").toString();
    QString path = "";
    if (lang == Globals::UILanguage::ES){
        path = Globals::Paths::MANUAL_DIR + "/es.pdf";
    }
    else{
        path = Globals::Paths::MANUAL_DIR + "/en.pdf";
    }

    QString currentDirectory = QDir::currentPath();
    QString filePath = currentDirectory + "/" + path;
    if (!QFile(filePath).exists()){
        StaticThreadLogger::error("Loader::openUserManual","User manual could not be found at: " + filePath);
        return;
    }
    filePath = "file:///" + filePath;
    if (!QDesktopServices::openUrl(QUrl(filePath))){
        StaticThreadLogger::error("Loader::openUserManual","Could not open the user manual on file path: " + filePath);
    }
}

bool Loader::processingParametersArePresent() const {
    return localDB.processingParametersPresent();
}

void Loader::openURLInBrowser(const QString &url){
    QDesktopServices::openUrl(QUrl(url));
}

////////////////////////////////////////////////////////  UPDATE FUNCTIONS  ////////////////////////////////////////////////////////

QString Loader::getNewUpdateVersionAvailable() const{
    return newVersionAvailable;
}

qint32 Loader::getRemainingUpdateDenials() const{
    return localDB.getRemainingUpdateDelays();
}

void Loader::updateDenied(){
    localDB.deniedUpdate();
}

void Loader::startUpdate(){
    processingUploadError = FAIL_CODE_NONE;
    if (!apiclient.requestUpdate("../")){
        StaticThreadLogger::error("Loader::startUpdate","Request updated download error: "  + apiclient.getError());
    }
}


bool Loader::isFirstTimeRun() const{
    return firstTimeRun;
}

QStringList Loader::getLatestVersionChanges(){
    QString name = Globals::Paths::CHANGELOG_LOCATION + "/" + Globals::Paths::CHANGELOG_BASE;

    QString changeLogLangName = "English";
    if (localDB.getPreference(LocalDB::PREF_UI_LANG,"en").toString() == Globals::VMUILanguages::ES){
        changeLogLangName = "Spanish";
    }

    name = name  + changeLogLangName + ".txt";
    //qDebug() << "Opening file" << name;
    QFile changelogFile(name);
    if (!changelogFile.open(QFile::ReadOnly)){
        StaticThreadLogger::error("Loader::getLatestVersionChanges","Could not open changelog file " + changelogFile.fileName() + " for reading");
        return QStringList();
    }
    QTextStream reader(&changelogFile);
    reader.setEncoding(QStringConverter::Utf8);
    QString content = reader.readAll();
    changelogFile.close();
    QStringList allVersions = content.split("|",Qt::SkipEmptyParts);
    // The first version should be the latest.
    if (allVersions.size() < 1){
        StaticThreadLogger::error("Loader::getLatestVersionChanges","Something went wrong when parsing the changelog file " + changelogFile.fileName() + ". Could not split between versions using |");
        return QStringList();
    }
    QString lastVersion = allVersions.first();
    QStringList allLines = lastVersion.split("\n");

    // The first line is the title.
    QString title = allLines.first();
    allLines.removeFirst();

    // All the rest are the body.
    QString body = allLines.join("\n");

    QStringList ret; ret << title << body;
    return ret;
}

////////////////////////////////////////////////////////  CONFIGURATION FUNCTIONS  ////////////////////////////////////////////////////////

QString Loader::getSettingsString(const QString &key, const QString &defvalue){
    return localDB.getPreference(key,defvalue).toString();
}

QVariant Loader::getDebugOption(const QString &debugOption){
    // Since this is for use in the QML/JS part of the code the string literal must be used.
    QVariant option = Debug::DEBUG_OPTIONS.getVariant(debugOption);
    //Debug::prettpPrintQVariantMap(Debug::DEBUG_OPTIONS.getMap());
    if (!option.isValid()) return "";
    else return option;
}

void Loader::setSettingsValue(const QString &key, const QVariant &var){
    if (!localDB.setPreference(key,var)){
        StaticThreadLogger::error("Loader::setSettingsValue","Failed to store local DB file.");
    }
}

//////////////////////////////////////////////////////// FILE MANAGEMENT FUNCTIONS ////////////////////////////////////////////////////////

bool Loader::createSubjectStudyFile(const QVariantMap &studyconfig, const QString &medic, const QString &protocol){

    if (!studyconfig.contains(Globals::StudyConfiguration::UNIQUE_STUDY_ID)){
        StaticThreadLogger::error("Loader::createSubjectStudyFile","While creating a subject study file, Study Configuration Map does not contain " + Globals::StudyConfiguration::UNIQUE_STUDY_ID + " field. Cannot determine study");
        StaticThreadLogger::error("Loader::createSubjectStudyFile",Debug::QVariantMapToString(studyconfig));
        return false;
    }

    // The first part of the name is base on the study type.
    qint32 selectedStudy = studyconfig.value(Globals::StudyConfiguration::UNIQUE_STUDY_ID).toInt();

    QString filename = "";
    bool new_file = true;

    switch(selectedStudy){
    case Globals::StudyConfiguration::INDEX_BINDING_UC:
        filename = Globals::BaseFileNames::BINDING;

        if (studyconfig.contains(Globals::StudyConfiguration::ONGOING_STUDY_FILE)){
            filename = studyconfig.value(Globals::StudyConfiguration::ONGOING_STUDY_FILE).toString();
            StaticThreadLogger::log("Loader::createSubjectStudyFile","Ongoing study file '" + filename + "'");
            new_file = false;
        }

        break;
    case Globals::StudyConfiguration::INDEX_BINDING_BC:
        filename = Globals::BaseFileNames::BINDING;

        // If there is an ongoing binding Study, then we can remove this
        if (studyconfig.contains(Globals::StudyConfiguration::ONGOING_STUDY_FILE)){
            filename = studyconfig.value(Globals::StudyConfiguration::ONGOING_STUDY_FILE).toString();
            StaticThreadLogger::log("Loader::createSubjectStudyFile","Ongoing study file '" + filename + "'");
            new_file = false;
        }

        break;
    case Globals::StudyConfiguration::INDEX_GONOGO:
        filename = Globals::BaseFileNames::GONOGO;
        break;
    case Globals::StudyConfiguration::INDEX_NBACKRT:
        filename = Globals::BaseFileNames::NBACKRT;
        break;
    case Globals::StudyConfiguration::INDEX_NBACK:
        filename = Globals::BaseFileNames::NBACK;
        break;
    case Globals::StudyConfiguration::INDEX_NBACKVS:
        filename = Globals::BaseFileNames::NBACKVS;
        break;
    case Globals::StudyConfiguration::INDEX_READING:
        filename = Globals::BaseFileNames::READING;
        break;
    case Globals::StudyConfiguration::INDEX_GNG_SPHERE:
        filename = Globals::BaseFileNames::GONOGO_3D;
        break;
    case Globals::StudyConfiguration::INDEX_PASSBALL:
        filename = Globals::BaseFileNames::PASSBALL;
        break;
    default:
        StaticThreadLogger::error("Loader::createSubjectStudyFile","Trying to create study file for an unknown study: " + QString::number(selectedStudy));
        break;
    }


    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    QString hour = QDateTime::currentDateTime().toString("HH:mm:ss");

    if (new_file){
        // Adding the full path.
        filename = Globals::Paths::WORK_DIRECTORY + "/" + configuration->getString(Globals::Share::PATIENT_UID) + "/" + filename;

        // Second part of the actuall file name is the time stamp.
        filename = filename + "_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm") + ".json";
    }

    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
        StaticThreadLogger::log("Loader::createSubjectStudyFile","DBUG: Using file for study at '" + filename + "'");
    }

    if (!new_file){
        // There is nothing more to do. We just return as everything is already set up
        configuration->addKeyValuePair(Globals::Share::PATIENT_STUDY_FILE,filename); // This is probably redudntant
        return true;
    }

    // Creating the metadata.
    QVariantMap metadata;
    metadata.insert(VMDC::MetadataField::DATE,date);
    metadata.insert(VMDC::MetadataField::HOUR,hour);
    metadata.insert(VMDC::MetadataField::INSTITUTION_ID,configuration->getString(Globals::VMConfig::INSTITUTION_ID));
    metadata.insert(VMDC::MetadataField::INSTITUTION_INSTANCE,configuration->getString(Globals::VMConfig::INSTANCE_NUMBER));
    metadata.insert(VMDC::MetadataField::INSTITUTION_NAME,configuration->getString(Globals::VMConfig::INSTITUTION_NAME));
    metadata.insert(VMDC::MetadataField::PROC_PARAMETER_KEY,apiclient.getEyeTrackerKey());
    metadata.insert(VMDC::MetadataField::STATUS,VMDC::StatusType::ONGOING);
    metadata.insert(VMDC::MetadataField::PROTOCOL,protocol);
    metadata.insert(VMDC::MetadataField::DISCARD_REASON,"");
    metadata.insert(VMDC::MetadataField::COMMENTS,"");
    metadata.insert(VMDC::MetadataField::APP_VERSION,this->getVersionNumber());

    // Creating the subject data
    QVariantMap subject_data;
    subject_data.insert(VMDC::SubjectField::BIRTH_COUNTRY,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_BIRTHCOUNTRY));
    subject_data.insert(VMDC::SubjectField::BIRTH_DATE,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_BIRTHDATE));
    subject_data.insert(VMDC::SubjectField::GENDER,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_GENDER));
    subject_data.insert(VMDC::SubjectField::INSTITUTION_PROVIDED_ID,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_INSTITUTION_ID));
    subject_data.insert(VMDC::SubjectField::LASTNAME,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_LASTNAME));
    subject_data.insert(VMDC::SubjectField::LOCAL_ID,configuration->getString(Globals::Share::PATIENT_UID));
    subject_data.insert(VMDC::SubjectField::NAME,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_NAME));
    subject_data.insert(VMDC::SubjectField::YEARS_FORMATION,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_YEARS_FORMATION));
    subject_data.insert(VMDC::SubjectField::EMAIL,localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_EMAIL));

    // Setting the evaluator info. We do it here becuase it is required for the data file comparisons.
    QVariantMap evaluator;
    //qDebug() << "Creating study file for evaluator" << configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR);
    QVariantMap evaluator_local_data = localDB.getEvaluatorData(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR));
    //Debug::prettpPrintQVariantMap(evaluator_local_data);
    evaluator.insert(VMDC::AppUserField::EMAIL,evaluator_local_data.value(LocalDB::APPUSER_EMAIL));
    evaluator.insert(VMDC::AppUserField::NAME,evaluator_local_data.value(LocalDB::APPUSER_NAME));
    evaluator.insert(VMDC::AppUserField::LASTNAME,evaluator_local_data.value(LocalDB::APPUSER_LASTNAME));
    evaluator.insert(VMDC::AppUserField::LOCAL_ID,"");
    evaluator.insert(VMDC::AppUserField::VIEWMIND_ID,"");

    // Setting the selected doctor info.
    QVariantMap medic_to_store;
    QVariantMap medic_local_data = localDB.getMedicData(medic);
    if (medic_local_data.empty()){
        StaticThreadLogger::error("Loader::createSubjectStudyFile","Failed to retrieve medic local data: " + medic);
        return false;
    }
    medic_to_store.insert(VMDC::AppUserField::EMAIL,medic_local_data.value(LocalDB::APPUSER_EMAIL));
    medic_to_store.insert(VMDC::AppUserField::NAME,medic_local_data.value(LocalDB::APPUSER_NAME));
    medic_to_store.insert(VMDC::AppUserField::LASTNAME,medic_local_data.value(LocalDB::APPUSER_LASTNAME));
    medic_to_store.insert(VMDC::AppUserField::LOCAL_ID,"");
    medic_to_store.insert(VMDC::AppUserField::VIEWMIND_ID,medic_local_data.value(LocalDB::APPUSER_VIEWMIND_ID));

    // Need to insert them as in in the file.
    QVariantMap pp = localDB.getProcessingParameters();

    // Check if we need to add the hand calibration data.
    if (configuration->containsKeyword(Globals::Share::HAND_CALIB_RES)){
        pp.insert(VMDC::ProcessingParameter::HAND_CALIB_RESULTS,configuration->getVariant(Globals::Share::HAND_CALIB_RES));
    }

    // Computing the actual maximum dispersion to use.
    qreal reference_for_md = qMax(pp.value(VMDC::ProcessingParameter::RESOLUTION_WIDTH).toReal(),pp.value(VMDC::ProcessingParameter::RESOLUTION_HEIGHT).toReal());
    qreal md_percent = pp.value(VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW).toReal();
    qint32 md_px = qRound(md_percent*reference_for_md/100.0);
    //qDebug() << "Setting the MD from" << reference_for_md << md_percent << " to " << md_px;
    pp.insert(VMDC::ProcessingParameter::MAX_DISPERSION_WINDOW_PX,md_px);

    // Computing the effective minimum fixation size to use.
    qreal sample_frequency = pp.value(VMDC::ProcessingParameter::SAMPLE_FREQUENCY).toReal();
    qreal sample_period = 1/sample_frequency;
    qreal minimum_fixation_length = qRound(sample_period*1000*MINIMUM_NUMBER_OF_DATAPOINTS_IN_FIXATION);
    pp.insert(VMDC::ProcessingParameter::MIN_FIXATION_DURATION,minimum_fixation_length);
    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
        QString dbug = "DBUG: Effective Minimum Fixation Computed At " + QString::number(minimum_fixation_length);
        qDebug() << dbug;
        StaticThreadLogger::warning("Loader::createSubjectStudyFile",dbug);
    }
    // These values will be properly set with the end study data.
    pp[VMDC::ProcessingParameter::RESOLUTION_HEIGHT] = 0;
    pp[VMDC::ProcessingParameter::RESOLUTION_WIDTH]  = 0;

    // Setting the QC Parameters that will be used.
    QVariantMap qc = localDB.getQCParameters();

    // We store this information the raw data container and leave it ready for the study to start.
    ViewMindDataContainer rdc;

    rdc.setQCParameters(qc);

    if (!rdc.setSubjectData(subject_data)){
        StaticThreadLogger::error("Loader::createSubjectStudyFile","Failed setting subject data to new study. Reason: " + rdc.getError());
        return false;
    }
    if (!rdc.setMetadata(metadata)){
        StaticThreadLogger::error("Loader::createSubjectStudyFile","Failed setting study metadata to new study. Reason: " + rdc.getError());
        return false;
    }

    if (!rdc.setApplicationUserData(VMDC::AppUserType::EVALUATOR,evaluator)){
        StaticThreadLogger::error("Loader::createSubjectStudyFile","Failed to store evaluator data: " + rdc.getError());
        return false;
    }

    if (!rdc.setApplicationUserData(VMDC::AppUserType::MEDIC,medic_to_store)){
        StaticThreadLogger::error("Loader::createSubjectStudyFile","Failed to store medic data: " + rdc.getError());
        return false;
    }

    if (!rdc.setProcessingParameters(pp)){
        StaticThreadLogger::error("Loader::createSubjectStudyFile","Failed to store processing parameters: " + rdc.getError());
        return false;
    }

    // We add the system specifications to the file.
    rdc.setSystemSpecs(hwRecognizer.getHardwareSpecsAsVariantMap());

    if (!rdc.saveJSONFile(filename)){
        StaticThreadLogger::error("Loader::createSubjectStudyFile","Failed on creating new study file: " + filename + ". Reason: " + rdc.getError());
        return false;
    }

    // Finally we set this as the current work file
    configuration->addKeyValuePair(Globals::Share::PATIENT_STUDY_FILE,filename);

    return true;

}

QString Loader::getCurrentSubjectStudyFile() const {
    return configuration->getString(Globals::Share::PATIENT_STUDY_FILE);
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
        StaticThreadLogger::error("Loader::addOrModifyEvaluator","While adding evaluator " + email + " with data " + name + " " + lastname + " (" + email + "): " + localDB.getError() );
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
    //Debug::prettpPrintQVariantMap(localDB.getEvaluatorData(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR)));
}

QVariantMap Loader::getCurrentEvaluatorInfo() const{
    return localDB.getEvaluatorData(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR));
}

QStringList Loader::getLoginEmails(bool with_name) const {
    return localDB.getUsernameEmails(with_name);
}

////////////////////////////////////////////////////////////////// SUBJECT FUNCTIONS //////////////////////////////////////////////////////////////////

QString Loader::addOrModifySubject(QString suid, const QString &name, const QString &lastname, const QString &institution_id,
                                   const QString &birthdate,
                                   const QString &gender, qint32 formative_years, const QString &email){

    //qDebug() << "Entering with suid" << suid;

    if (suid == ""){
        // We need to generate a new internal id.
        suid = configuration->getString(Globals::VMConfig::INSTITUTION_ID) + "_" + configuration->getString(Globals::VMConfig::INSTANCE_NUMBER) + "_"
                + QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");
        //qDebug() << "Computed new suid" << suid;
    }


    // Create a map for the data as is. The caller function is reponsible for data verification.
    QVariantMap map;
    map[LocalDB::SUBJECT_BIRTHDATE] = birthdate;
    map[LocalDB::SUBJECT_INSTITUTION_ID] = institution_id;
    map[LocalDB::SUBJECT_LASTNAME] = lastname;
    map[LocalDB::SUBJECT_NAME] = name;
    map[LocalDB::SUBJECT_YEARS_FORMATION] = formative_years;
    map[LocalDB::SUBJECT_GENDER] = gender;
    map[LocalDB::SUBJECT_EMAIL] = email;
    //map[LocalDB::SUBJECT_ASSIGNED_MEDIC] = selectedMedic;

    // Adding the data to the local database.
    if (!localDB.addOrModifySubject(suid,map)){
        StaticThreadLogger::error("Loader::addOrModifySubject","While adding subject " + suid + " with data " + name + " " + lastname + " (" + institution_id + "): " + localDB.getError() );
        return "";
    }
    return suid;
}

void Loader::modifySubjectSelectedMedic(const QString &suid, const QString &selectedMedic){
    if (!localDB.modifyAssignedMedicToSubject(suid,selectedMedic)){
        StaticThreadLogger::error("Loader::modifySubjectSelectedMedic","While modifiying subject " + suid + " by setting meddig " + selectedMedic + ": " + localDB.getError() );
    }
}

QVariantMap Loader::filterSubjectList(const QString &filter){
    return  localDB.getDisplaySubjectList(filter,getStringListForKey("viewpatlist_months"));
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
        StaticThreadLogger::error("Loader::setSelectedSubject","Could not create patient dir " + suid + " in directory");
        return false;
    }

    configuration->addKeyValuePair(Globals::Share::PATIENT_DIRECTORY,patdirPath);
    return true;

}

void Loader::setStudyMarkerFor(const QString &study, const QString &value){
    if (!localDB.addStudyMarkerForSubject(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR),study,value)){
        StaticThreadLogger::error("Loader::setStudyMarkerFor","Failed in setting study marker: " + localDB.getError());
    }
}

QString Loader::getStudyMarkerFor(const QString &study){
    return localDB.getMarkerForStudy(configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR),study).toString();
}

QVariantMap Loader::getCurrentSubjectInfo(){
    return localDB.getSubjectData(configuration->getString(Globals::Share::PATIENT_UID));
}

void Loader::clearSubjectSelection(){
    configuration->addKeyValuePair(Globals::Share::PATIENT_UID,"");
}

QString Loader::getCurrentlySelectedAssignedDoctor() const {
    return localDB.getSubjectFieldValue(configuration->getString(Globals::Share::PATIENT_UID),LocalDB::SUBJECT_ASSIGNED_MEDIC);
}

bool Loader::areThereAnySubjects() const {
    return localDB.getSubjectCount() > 0;
}


////////////////////////// MEDIC RELATED FUNCTIONS ////////////////////////////
QVariantMap Loader::getMedicList() const {
    return localDB.getMedicDisplayList();
}

////////////////////////// REPORT GENERATING FUNCTIONS ////////////////////////////
QVariantMap Loader::getReportsForLoggedEvaluator(){

    QStringList directory_list = QDir(Globals::Paths::WORK_DIRECTORY).entryList(QStringList(),QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);
    QString current_evaluator = configuration->getString(Globals::Share::CURRENTLY_LOGGED_EVALUATOR);

    // We need the name map in order to display proper, language senstitive names.
    QStringList name_map_as_list = getStringListForKey("viewQC_StudyNameMap");
    QMap<QString,QString> name_map;
    for (qint32 i = 0; i < name_map_as_list.size(); i++){
        // Name map is list where all names are listed at key,Name,key,Name ... So all even indexes are key for the next odd index.
        if ((i % 2) == 1){
            name_map[name_map_as_list.at(i-1)] = name_map_as_list.at(i);
        }
    }

    QList<QVariantMap> studiesToAnalyze;

    QStringList filters;
    filters << "*.qci";

    // We need to get the map from subject id to name.

    for (qint32 i = 0; i < directory_list.size(); i++){

        //sdc.setup(Globals::Paths::WORK_DIRECTORY + "/" + directory_list.at(i),current_evaluator);

        QString subject_id = directory_list.at(i);
        QString subject_dir_path = Globals::Paths::WORK_DIRECTORY + "/" + subject_id;

        // We need to get the subject data in order to get it's name.
        QVariantMap subject_data = localDB.getSubjectData(subject_id);

        QString fname = subject_data.value(LocalDB::SUBJECT_NAME).toString();
        QString lname = subject_data.value(LocalDB::SUBJECT_LASTNAME).toString();
        QString subject_name = "";
        if ((fname != "") || (lname != "")){
            subject_name = fname + " " + lname;
        }

        QString subject_inst_id = subject_data.value(LocalDB::SUBJECT_INSTITUTION_ID).toString();

        // We now scan all qci files in the subject dir.
        QDir subject_dir(subject_dir_path);

        QStringList qcifiles = subject_dir.entryList(filters,QDir::Files);
        QString error; // Required for error reporting while loading QCI Files.

        for (qint32 j = 0; j < qcifiles.size(); j++){
            error = "";

            QString full_path = subject_dir_path + "/" + qcifiles.at(j);
            QString study_file = qcifiles.at(j);
            study_file = study_file.replace(".qci",".zip");
            study_file = subject_dir_path + "/" + study_file;

            QVariantMap map = Globals::LoadJSONFileToVariantMap(full_path,&error);
            if (error != ""){
                StaticThreadLogger::error("Loader::getReportsForLoggedEvaluator","Error loading QCI File: " + error);
                continue;
            }

            //qDebug() << "Checking File" << subject_dir_path + "/" + qcifiles.at(i) << "for current evaluator" << current_evaluator;

            // First we check that the evaluator matches.
            if (map.value(Globals::QCIFields::EVALUATOR).toString() != current_evaluator) continue;

            // We translate the study name.
            QString report_type = map.value(Globals::QCIFields::STUDY_TYPE).toString();
            if (name_map.contains(report_type)){
                map[Globals::QCIFields::STUDY_TYPE] = name_map.value(report_type);
            }

            //Debug::prettpPrintQVariantMap(map);

            // Now we set the subject.
            map[Globals::QCIFields::SUBJECT]            = subject_name;

            // We need to check if the subject name is empty. If it is, we need to show something, so we show the ID. Either one or the other are not empty.
            if (subject_name == ""){
                map[Globals::QCIFields::SUBJECT]            = subject_inst_id;
            }

            map[Globals::QCIFields::SUBJECT_INST_ID]    = subject_inst_id;
            map[Globals::QCIFields::SUBJECT_VM_ID]      = subject_id;

            // And we need to set the study file.
            map[Globals::QCIFields::STUDY_FILE]         = study_file;

            studiesToAnalyze << map;

        }

    }

    // Sort the generated list by the order code before returning it (chronologically).

    QVariantMap returnmap = Globals::SortMapListByStringValue(studiesToAnalyze,Globals::QCIFields::DATE_ORDERCODE);

    //Debug::prettpPrintQVariantMap(returnmap);

    return returnmap;

}


void Loader::setCurrentStudyFileToSendOrDiscard(const QString &file){
    configuration->addKeyValuePair(Globals::Share::SELECTED_STUDY,file);
    //qc.setVMContainterFile(file,localDB.getQCParameters()); // This will start processing in a separate thread.
}

////////////////////////////////////////////////////////////////// API FUNCTIONS //////////////////////////////////////////////////////////////////

void Loader::requestOperatingInfo(){

    newVersionAvailable = "";
    processingUploadError = FAIL_CODE_NONE;

    // We reset the startup sequence flag. In order to avoid unncessary processing we set the flag to one and disable the qcChecker at the start.
    startUpSequenceFlag = 1;

    // Now we set the work directory for generate any missing QCI files.
    // qcChecker.setFilesToProcessFromViewMindDataDirectory();

    // We start the qcChecker in the background.
    //qcChecker.start();

    // And now we request the operating info
    //qDebug() << "Requesting Operating Info";

    bool sendLog = localDB.checkForLogUpload();

    if (!sendLog){
        // We still, AT LEAST copy the RRS files.
        QString inst_id = configuration->getString(Globals::VMConfig::INSTITUTION_ID);
        QString inst_number = configuration->getString(Globals::VMConfig::INSTANCE_NUMBER);
        LogPrep lp(Globals::Paths::LOGFILE,inst_id,inst_number);
        lp.findAndCopyRRSLogs();
    }

    // We get the list of the subject updates.
    QVariantMap updated_subject_info = localDB.getSubjectDataToUpdate();

    if (!apiclient.requestOperatingInfo(hwRecognizer.toString(false),sendLog,updated_subject_info)){
        StaticThreadLogger::error("Loader::requestOperatingInfo","Request operating info error: "  + apiclient.getError());
        emit Loader::finishedRequest();
    }
}

void Loader::sendSupportEmail(const QString &subject, const QString &body, const QString &evaluator_name, const QString &evaluator_email){

    // We need to load the emaiil template.
    QFile file(":/resources/tempalte_support_email.html");
    if (!file.open(QFile::ReadOnly)){
        StaticThreadLogger::error("Loader::sendSupportEmail","Unable to read the support email template");
        return;
    }

    QTextStream reader(&file);
    QString email_template = reader.readAll();
    file.close();

    // Now we replace the placeholders with the actual values.
    QString issue_description = body;
    issue_description = issue_description.replace("\n","<br>\n"); // HTML P requrires <br> and not new lines.
    QString hardware = hwRecognizer.toString(true).replace("\n","<br>\n"); // HTML P requrires <br> and not new lines.

    QString instance = configuration->getString(Globals::VMConfig::INSTANCE_NUMBER);
    QString inst_id  = configuration->getString(Globals::VMConfig::INSTITUTION_ID);


    QVariantMap replacement_map;
    replacement_map[Globals::SupportEmailPlaceHolders::APPVERSION]  = this->getVersionNumber();
    replacement_map[Globals::SupportEmailPlaceHolders::EVALUATOR]   = evaluator_name;
    replacement_map[Globals::SupportEmailPlaceHolders::EVAL_EMAIL]  = evaluator_email;
    replacement_map[Globals::SupportEmailPlaceHolders::HWSPECS]     = hardware;
    replacement_map[Globals::SupportEmailPlaceHolders::INSTANCE]    = instance;
    replacement_map[Globals::SupportEmailPlaceHolders::INST_ID]     = inst_id;
    replacement_map[Globals::SupportEmailPlaceHolders::INST_NAME]   = configuration->getString(Globals::VMConfig::INSTITUTION_NAME);
    replacement_map[Globals::SupportEmailPlaceHolders::ISSUE]       = issue_description;

    QStringList keys = replacement_map.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        email_template = email_template.replace(keys.at(i),replacement_map.value(keys.at(i)).toString());
    }

    // Finally we save the support email where it's supposed to be saved
    QString email_filename = "support_email_" + inst_id + "_" + instance + "_" + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + ".html";
    email_filename = Globals::Paths::WORK_DIRECTORY + "/" + email_filename;

    QFile email_file(email_filename);
    if (!email_file.open(QFile::WriteOnly)){
        StaticThreadLogger::error("Loader::sendSupportEmail","Unable to open support email file: '" + email_filename + "', for writing");
        return;
    }

    QTextStream writer(&email_file);
    writer << email_template;
    email_file.close();

    StaticThreadLogger::log("Loader::sendSupportEmail","Successfully created email file: '" + email_filename + "'");


    if (!apiclient.requestSupportEmail(subject,email_filename)){
        StaticThreadLogger::error("Loader::sendSupportEmail","Request for sending support email error: "  + apiclient.getError());
    }
}

void Loader::requestActivation(int institution, int instance, const QString &key){
    processingUploadError = FAIL_CODE_NONE;
    if (!apiclient.requestActivation(institution,instance,key,hwRecognizer.toString(true))){
        StaticThreadLogger::error("Loader::requestActivation","Request activation error: "  + apiclient.getError());
        emit Loader::finishedRequest();
    }
}

void Loader::sendStudy(QString discard_reason, const QString &comment){
    processingUploadError = FAIL_FILE_CREATION;

    QString studyFileToSend = configuration->getString(Globals::Share::SELECTED_STUDY);

    // Since the file has allready been compresed. We need to create a second file and then tarball both of those files toghter in the final file.
    // So we take advantange of the QCI file and load that.

    // Now we store that map in a JSON file. For that we need the base name of the study to send.
    QFileInfo info(studyFileToSend);
    QString directory   = info.absolutePath();
    QString basename    = info.baseName();

    QString tarOutputFile = directory + "/" + basename + ".tar.gz";
    QString qcifile = directory + "/" + basename + ".qci";

    QString loadError = "";
    QVariantMap map = Globals::LoadJSONFileToVariantMap(qcifile, &loadError);
    map[VMDC::MetadataField::COMMENTS] = comment;
    map[VMDC::MetadataField::DISCARD_REASON] = discard_reason;

    // Now we create the JSON file.
    if (!Globals::SaveVariantMapToJSONFile(qcifile,map)){
        StaticThreadLogger::error("Loader::sendStudy","Failed in resaving the qci file as '" + qcifile + "'");
        emit Loader::finishedRequest();
    }

    // Now we create the tar output.
    QStringList arguments;
    arguments << "-c";
    arguments << "-z";
    arguments << "-f";
    arguments << tarOutputFile;
    arguments << basename + ".zip";
    arguments << basename + ".qci";

    QProcess tar;
    tar.setWorkingDirectory(directory);
    tar.start(Globals::Paths::TAR_EXE,arguments);
    tar.waitForFinished();

    qint32 exit_code = tar.exitCode();

    if (!QFile(tarOutputFile).exists()){
        StaticThreadLogger::error("Loader::sendStudy","Could not tar the .json and the .zip files for '" + basename + "'. Exit code for TAR.exe is: " + QString::number(exit_code));
        emit Loader::finishedRequest();
    }

    processingUploadError = FAIL_CODE_NONE;

    if (!apiclient.requestReportProcessing(tarOutputFile)){
        StaticThreadLogger::error("Loader::sendStudy","Requesting study report generation: " + apiclient.getError());
        emit Loader::finishedRequest();
    }

}

qint32 Loader::getLastAPIRequest(){
    return apiclient.getLastRequestType();
}

void Loader::startUpSequenceCheck() {
    startUpSequenceFlag++;
    if (startUpSequenceFlag >= 2){
        startUpSequenceFlag = 0;
        StaticThreadLogger::log("Loader::startUpSequenceCheck","Finished start up sequence (Operating Info and QCI Regen Check)");
        emit Loader::finishedRequest();
    }
}

void Loader::onFileDownloaderUpdate(qreal progress, qreal hours, qreal minutes, qreal seconds, qint64 bytesDowloaded, qint64 bytesTotal){
    emit Loader::downloadProgressUpdate(progress,hours,minutes,seconds,bytesDowloaded,bytesTotal);
}

void Loader::onNotificationFromFlowControl(QVariantMap notification){
    if (notification.contains(Globals::FCL::HMD_KEY_RECEIVED)){
        // We now know which headset we are dealing with.
        QString key = notification.value(Globals::FCL::HMD_KEY_RECEIVED).toString();
        if (Globals::EyeTrackerKeys::HP == key){
            this->eyeTrackerName = Globals::EyeTrackerNames::HP;
        }
        else if (Globals::EyeTrackerKeys::VARJO == key){
            this->eyeTrackerName = Globals::EyeTrackerNames::VARJO;
        }
        else {
            StaticThreadLogger::error("Loader::onNotificationFromFlowControl","Got an unknown HMD Key: " + key);
            return;
        }

        apiclient.setEyeTrackerKey(key);
        emit Loader::titleBarUpdate();
    }
    else if (notification.contains(Globals::FCL::UPDATE_AVG_FREQ)){

        QString A = notification.value(Globals::FCL::UPDATE_AVG_FREQ).toString();
        QString M = notification.value(Globals::FCL::UPDATE_MAX_FREQ).toString();
        QString F = notification.value(Globals::FCL::UPDATE_SAMP_FREQ).toString();

        StaticThreadLogger::log("FREQUPDATE","Avg: " + A + " HZ. Freq: " + F + " Hz. Max: " + M  + " Hz");

    }
    else{
        StaticThreadLogger::warning("Loader::onNotificationFromFlowControl","Got an unknown notification: " + notification.keys().join(","));
    }
}

void Loader::receivedAPIResponse(){

    if (!apiclient.getError().isEmpty()){

        // We need to check if the instance is disabled or not.

        QVariantMap retdata = apiclient.getMapDataReturned();
        qint32 http_code = retdata.value(APINames::MAIN_HTTP_CODE).toInt();
        if (http_code == APINames::DISABLED_INSTANCE_HTTP_CODE){
            StaticThreadLogger::error("Loader::receivedRequest","Received instance disabled code");
            processingUploadError = FAIL_INSTANCE_DISABLED;
            this->localDB.setInstanceEnableTo(false);
        }
        else {
            this->localDB.setInstanceEnableTo(true);
            processingUploadError = FAIL_CODE_SERVER_ERROR;
            StaticThreadLogger::error("Loader::receivedRequest","Error Receiving Request :"  + apiclient.getError());
            if (apiclient.getLastRequestType() == APIClient::API_SENT_SUPPORT_EMAIL){
                emit Loader::sendSupportEmailDone(false);
            }
        }

    }
    else{

        // Disabled instance will ONLY come as an API error. Just a specific one. So if there was no error the instance is NOT disabled.
        // Unless comming from a support request. In that case we can't set it to true because the support request can be sent
        // with a disabled insance.
        if (apiclient.getLastRequestType() != APIClient::API_SENT_SUPPORT_EMAIL) this->localDB.setInstanceEnableTo(true);

        if (apiclient.getLastRequestType() == APIClient::API_OPERATING_INFO || apiclient.getLastRequestType() == APIClient::API_OPERATING_INFO_AND_LOG){

            apiclient.clearFileToSendHandles();

            if (apiclient.getLastRequestType() == APIClient::API_OPERATING_INFO_AND_LOG){
                StaticThreadLogger::log("Loader::receivedRequest","Returned from operating information call with log uplaod"); // This log line serves as the purpose of ensuring that the logfile.log will exist.

                //QFile::remove(apiclient.getLastGeneratedLogFileName()); // This will fail if no file exists. So we don't check for errors.

                // This ensures that we ONLY clean this when there is no error.
                QString instance_number = configuration->getString(Globals::VMConfig::INSTANCE_NUMBER);
                QString inst_id = configuration->getString(Globals::VMConfig::INSTITUTION_ID);
                LogPrep lp(Globals::Paths::LOGFILE,inst_id,instance_number);
                lp.cleanLogDir();

                // We mark now as the last log upload.
                localDB.setLogUploadMark();
            }
            else {
                StaticThreadLogger::log("Loader::receivedRequest","Got Operating Information. No Log Upload");
            }

            // Just in case we clear the calibration failed directory.
            cleanCalibrationDirectory();

            // If the subject update file was generated then, we need to delete it if it exists.
            bool expecting_subject_ids = false;
            if (apiclient.getLastGeneratedSubjectJSONFile() != ""){
                expecting_subject_ids = true;
                QFile deletefile(apiclient.getLastGeneratedSubjectJSONFile());
                if (deletefile.exists()){
                    if (deletefile.remove()){
                        StaticThreadLogger::log("Loader::receivedRequest","Deleted subject update JSON file of: "  + deletefile.fileName());
                    }
                    else {
                        StaticThreadLogger::warning("Loader::receivedRequest","Failed to delete subject update JSON file of: "  + deletefile.fileName());
                    }
                }
            }

            QVariantMap ret = apiclient.getMapDataReturned();
            QVariantMap mainData = ret.value(APINames::MAIN_DATA).toMap();

            if (expecting_subject_ids){
                if (mainData.contains(LocalDB::SUBJECT_UPDATED_IDS)){
                    localDB.markSubjectsAsUpdated(mainData.value(LocalDB::SUBJECT_UPDATED_IDS).toList());
                }
                else{
                    StaticThreadLogger::warning("Loader::receivedRequest","There was an existent subject update file, however not updated ids");
                }
            }

            if (DBUGBOOL(Debug::Options::PRINT_SERVER_RESP)){
                QString toprint = "DBUG: Received response:\n" + Debug::QVariantMapToString(mainData);
                StaticThreadLogger::warning("Loader::receivedRequest",toprint);
            }

            if (!localDB.setMedicInformationFromRemote(mainData)){
                StaticThreadLogger::error("Loader::receivedRequest","Failed to set medical professionals info from server: " + localDB.getError());
            }

            qint32 patients_added = localDB.mergePatientDBFromRemote(mainData);
            if (patients_added < 0){
                StaticThreadLogger::error("Loader::receivedRequest","Failed to merge the patient database. Reason: " + localDB.getError());
            }
            else {
                StaticThreadLogger::log("Loader::receiveRequest", "Added " + QString::number(patients_added) + " new subjects");
            }

            if (DBUGBOOL(Debug::Options::PRINT_PP)){
                QVariantMap pp = mainData.value(APINames::ProcParams::NAME).toMap();
                QString ppstr = Debug::QVariantMapToString(pp);
                ppstr = "DBUG: RECEIVED PROCESSING PARAMETERS:\n" + ppstr;
                qDebug() << ppstr;
                StaticThreadLogger::warning("Loader::receivedRequest",ppstr);
            }

            if (!localDB.setProcessingParametersFromServerResponse(mainData)){
                StaticThreadLogger::error("Loader::receivedRequest","Failed to set processing parameters from server: " + localDB.getError());
            }

            if (mainData.contains(APINames::Institution::INST_COUNTRY)){
                QString inst_country = mainData.value(APINames::Institution::INST_COUNTRY).toString();
                if (!localDB.setInstitutionCountryCode(inst_country)){
                    StaticThreadLogger::error("Loader::receivedRequest","Failed storing changes to country code. Reason: " + localDB.getError());
                }
                else {
                    StaticThreadLogger::log("Loader::receivedRequest","Setting the institution country to: " + inst_country);
                }
            }

            if (DBUGBOOL(Debug::Options::PRINT_QC)){
                QVariantMap qc = mainData.value(APINames::FreqParams::NAME).toMap();
                QString qcstr = Debug::QVariantMapToString(qc);
                qcstr = "DBUG: RECEIVED QC PARAMETERS:\n" + qcstr;
                qDebug() << qcstr;
                StaticThreadLogger::warning("Loader::receivedRequest",qcstr);

            }

            if (!localDB.setQCParametersFromServerResponse(mainData)){
                StaticThreadLogger::error("Loader::receivedRequest","Failed to set QC parameters from server: " + localDB.getError());
            }

            if (!localDB.setRecoveryPasswordFromServerResponse(mainData)){
                StaticThreadLogger::error("Loader::receivedRequest","Failed to set recovery password from server: " + localDB.getError());
            }

            // Checking for updates.
            if (!DBUGBOOL(Debug::Options::DISABLE_UPDATE_CHECK)){
                if (mainData.contains(APINames::UpdateParams::UPDATE_VERSION)) {
                    newVersionAvailable = mainData.value(APINames::UpdateParams::UPDATE_VERSION).toString();
                    if (mainData.contains(APINames::UpdateParams::UPDATE_ET_CHANGE)){
                        newVersionAvailable = newVersionAvailable + " - " + mainData.value(APINames::UpdateParams::UPDATE_ET_CHANGE).toString();
                    }
                }

                if (!newVersionAvailable.isEmpty()){
                    StaticThreadLogger::log("Loader::receivedRequest","Update Available: " + newVersionAvailable);
                }
            }
            else{
                QString dbug = "DBUG: Update check is disabled";
                qDebug() << dbug;
                StaticThreadLogger::error("Loader::receivedRequest",dbug);
            }

            // We straight up call the start up sequence checker and get out.
            startUpSequenceCheck();
            return;

        }
        else if (apiclient.getLastRequestType() == APIClient::API_REQUEST_REPORT){
            StaticThreadLogger::log("Loader::receivedRequest","Study file was successfully sent");

            apiclient.clearFileToSendHandles();

            if (!DBUGBOOL(Debug::Options::NO_RM_STUDIES)) {
                moveProcessedFiletToProcessedDirectory();
            }
            else{
                QString dbug = "DBUG: Studies are not moved to sent directory";
                qDebug() << dbug;
                StaticThreadLogger::error("Loader::receivedRequest",dbug);
            }
        }
        else if (apiclient.getLastRequestType() == APIClient::API_REQUEST_UPDATE){

            QVariantMap ret = apiclient.getMapDataReturned();
            QVariantMap mainData = ret.value(APINames::MAIN_DATA).toMap();

            if (DBUGBOOL(Debug::Options::PRINT_SERVER_RESP)){
                QString toprint = "DBUG: Received response to update request:\n" + Debug::QVariantMapToString(mainData);
                StaticThreadLogger::warning("Loader::receivedRequest",toprint);
            }

            // Before we actually download anything we need to check if the last download app coincides with the desired app.
            QString lastDownlodedApp = localDB.getLastDownloadedApp();
            updateDownloadSkipped = (lastDownlodedApp == newVersionAvailable);
            if (updateDownloadSkipped){
                updateDownloadSkipped = updateDownloadSkipped && QFile(Globals::Paths::VMTOOLEXE).exists();
                if (!updateDownloadSkipped){
                    StaticThreadLogger::log("Loader::receivedRequest", "The last downloaded app version and the new version are the same but the VM Maintenance Tool Executable could not be found. Download will ocurr");
                }
            }
            else {
                StaticThreadLogger::log("Loader::receivedRequest", "The last downloaded app version (" + lastDownlodedApp + ") differs from the new version (" + newVersionAvailable + "). Download will ocurr");
            }

            if (!updateDownloadSkipped){


                // URL should be present.
                QString dlurl = mainData.value(APINames::UpdateDLFields::URL).toString();

                // Starting the download. And the process must now wait until the download is finished.
                QString expectedPath = "../" + Globals::Paths::UPDATE_PACKAGE;

                QString override_url = DBUGSTR(Debug::Options::OVERRIDE_UPDATE_LINK);
                if (override_url != ""){
                    StaticThreadLogger::warning("Loader::receivedRequest","Overriding download URL with one provided in VMDebug File override_url");
                    dlurl = override_url;
                }

                // Before we download anything, we need to ensure that the current VM Tool is deleted.
                if (!QDir(Globals::Paths::VMTOOLDIR).removeRecursively()){
                    StaticThreadLogger::error("Loader::receivedRequest","Unable to delete current VMMaintenaceDir installation. Aborting update");
                    updateDownloadFinished(false);
                    return;
                }

                StaticThreadLogger::log("Loader::receivedRequest","Downloading the update package from '" + dlurl  + "'");
                fileDownloader.download(dlurl,expectedPath);

            }
            else {
                StaticThreadLogger::log("Loader::receivedRequest", "The last downloaded app version (" + lastDownlodedApp + ") is the same as the new version (" + newVersionAvailable + "). Skipping download");
                updateDownloadFinished(true);
            }

            return;


        }
        else if (apiclient.getLastRequestType() == APIClient::API_ACTIVATE){

            StaticThreadLogger::log("Loader::receivedRequest","Successfully finished an activation request");

            QVariantMap ret = apiclient.getMapDataReturned();
            QVariantMap mainData = ret.value(APINames::MAIN_DATA).toMap();

            // We need to create a file with the name of what should be the sole key.
            QStringList keys = mainData.keys();

            // processingUploadError = FAIL_BAD_ACTIVATION_RETURN;

            if (keys.empty()){
                StaticThreadLogger::error("Loader::receivedRequest","Received activation request with a map with no keys");
                processingUploadError = FAIL_BAD_ACTIVATION_RETURN;
                emit Loader::finishedRequest();
                return;
            }

            QString vmconfig_filename = keys.first();
            if (keys.size() > 1){
                StaticThreadLogger::warning("Loader::receivedRequest","More that one key recieved in the map resulting in activiation request. Proceeding with first key: " + vmconfig_filename);
            }

            // Now we save the contennts of the map to a file
            QString license_data = mainData.value(vmconfig_filename).toString();
            QFile vmconfig_file(vmconfig_filename);
            if (!vmconfig_file.open(QFile::WriteOnly)){
                StaticThreadLogger::error("Loader::receivedRequest","Unable to create license file '" + vmconfig_filename + "'");
                processingUploadError = FAIL_BAD_ACTIVATION_RETURN;
                emit Loader::finishedRequest();
                return;
            }

            QTextStream writer(&vmconfig_file);
            writer << license_data;
            vmconfig_file.close();

        }
        else if (apiclient.getLastRequestType() == APIClient::API_SENT_SUPPORT_EMAIL){
            // In this case there is nothing todo really other than notify the fron end.
            StaticThreadLogger::log("Loader::receivedRequest","Tech support email sent successfully");

            // We need to clear the sent files to avoid garbage build up.
            apiclient.clearFileToSendHandles();

            // This ensures that we ONLY clean this when there is no error.
            QString instance_number = configuration->getString(Globals::VMConfig::INSTANCE_NUMBER);
            QString inst_id = configuration->getString(Globals::VMConfig::INSTITUTION_ID);
            LogPrep lp(Globals::Paths::LOGFILE,inst_id,instance_number);
            lp.cleanLogDir();

            QFile::remove(apiclient.getLatestGeneratedSupportEmail());

            emit Loader::sendSupportEmailDone(true);
        }
        else {
            StaticThreadLogger::error("Loader::receivedRequest","Received unknown API Request Type finish:  " + QString::number(apiclient.getLastRequestType()));
        }
    }
    emit Loader::finishedRequest();
}

void Loader::updateDownloadFinished(bool allOk){

    if (!allOk){
        StaticThreadLogger::error("Loader::updateDownloadFinished","Failed to download update. Reason: " + fileDownloader.getError() + ". If error is empty it could have been called from another point with a false paraemter.");
        emit Loader::finishedRequest();
        return;
    }

    if (!updateDownloadSkipped){
        QString expectedPath = "../" + Globals::Paths::UPDATE_PACKAGE;
        if (QFile::exists(expectedPath)){
            StaticThreadLogger::log("Loader::updateDownloadFinished","Received update succesfully. Unzipping");

            QDir dir(".");
            dir.cdUp();

            // Untarring the exe file.
            QProcess process;
            QStringList arguments;
            arguments << "-xvzf" <<  dir.path() + "/" +  Globals::Paths::UPDATE_PACKAGE << "-C"  << dir.path();
            process.setProcessChannelMode(QProcess::MergedChannels);
            process.start(Globals::Paths::TAR_EXE,arguments);

            if (!process.waitForFinished()){
                QString output = QString::fromUtf8(process.readAllStandardOutput());
                StaticThreadLogger::error("Loader::updateDownloadFinished","Untarring failed with " + process.errorString() + ". Tar output:\n" + output);
            }

            // If the process finished correctly then the VMMaintenanceTool Directory should be present. And we have susccessfully downloaded the courrent version
            localDB.setLastDownloadedApp(newVersionAvailable); // This will prevent redownload incase that the application was already downloaded and the update process failed.
            StaticThreadLogger::error("Loader::updateDownloadFinished","We should now continue the update process");

            if (!QFile(Globals::Paths::VMTOOLEXE).exists()){
                StaticThreadLogger::error("Loader::updateDownloadFinished","Unable to find the VMMaintenanceTool Executable. Aborting update");
                emit Loader::finishedRequest();
                return;
            }

            // We need to detelet the zip file.
            if (!QFile("./" + Globals::Paths::UPDATE_PACKAGE).remove()){
                StaticThreadLogger::warning("Loader::updateDownloadFinished","Failed in cleaning the app.zip file which is the update package");
            }

        }
        else{
            StaticThreadLogger::error("Loader::updateDownloadFinished","The download apparently succeded but the file is not where it was expected: " + expectedPath);
        }
    }

    // At this point we know tha the VM Maintenance Tool Executable exists. So we need to do the following.
    // First we copy the vmconfiguration file for backup.
    if (!QFile(Globals::Paths::CONFIGURATION).copy(Globals::Paths::VMTOOLDIR + "/" + Globals::Paths::CONFIGURATION)){
        StaticThreadLogger::error("Loader::updateDownloadFinished","Failed in creating a backup of the vmconfiguration file to the tool maintenance directory");
    }

    // Now we create a Desktop ShortCut For the file.
    QStringList possiblePaths = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation); // the possible paths for the desktop.
    QString pathToLink = possiblePaths.first() + "/" + Globals::Paths::VM_UPDATE_LINK; // The name of the ShortCut
    QFileInfo info(Globals::Paths::VMTOOLEXE);
    if (!QFile::link(info.absoluteFilePath(),pathToLink)){
        StaticThreadLogger::error("Loader::updateDownloadFinished","Failed in creating a shortcut for tool app. The target path: '" + pathToLink + "'. The source path: '" + info.absoluteFilePath() +  "'");
    }

    // If we had no issues so far we call the VMMaintenanceTool with the update parameter.
    //    Start an application and quit.
    QString workdir = info.absoluteDir().absolutePath();
    qint64 pid;
    QString program = info.absoluteFilePath();
    QStringList arguments; arguments << "update";
    if (!QProcess::startDetached(program,arguments,workdir,&pid)){
        StaticThreadLogger::error("Loader::updateDownloadFinished","Failed to start the update application. EXE: '" + program + "'. Working Directory: '" + workdir +  "'");
    }

    // Since we starte detached sucessfully we should now be able to quit the app and let the
    QCoreApplication::exit();

    emit Loader::finishedRequest();

}

qint32 Loader::wasThereAnProcessingUploadError() const {
    return processingUploadError;
}

void Loader::cleanCalibrationDirectory() {

    QDir calib_dir(Globals::Paths::FAILED_CALIBRATION_DIR);
    if (calib_dir.exists()){

        QStringList filters; filters  << "*.tar.gz";
        QStringList tar_files = calib_dir.entryList(filters,QDir::Files|QDir::NoDotAndDotDot);

        for (qint32 i = 0; i < tar_files.size(); i++){
            QFile(Globals::Paths::FAILED_CALIBRATION_DIR + "/" + tar_files.at(i)).remove();
        }

    }

}

void Loader::moveProcessedFiletToProcessedDirectory(){

    QString sentFile = configuration->getString(Globals::Share::SELECTED_STUDY);

    // Getting the patient directory.
    QFileInfo info(sentFile);
    QString patientWorkingDirectory = info.path();
    QString baseFileName = info.baseName();

    // Creating the processed directory.
    QDir(patientWorkingDirectory).mkdir(ExperimentGlobals::SUBJECT_DIR_SENT);
    QString processedDir = patientWorkingDirectory + "/" + ExperimentGlobals::SUBJECT_DIR_SENT;
    if (!QDir(processedDir).exists()){
        StaticThreadLogger::error("Loader::moveProcessedFiletToProcessedDirectory","Failed to create patient sent directory at: " + processedDir);
        return;
    }

    QString tarGzFile = patientWorkingDirectory + "/" + baseFileName + ".tar.gz";
    QString qciFile = patientWorkingDirectory   + "/"   + baseFileName + ".qci";

    if (!QFile::exists(tarGzFile)){
        StaticThreadLogger::error("Loader::moveProcessedFiletToProcessedDirectory","Unable to locate the TAR.GZ file '" + tarGzFile + "'. Not Deleting Anything");
    }
    else {
        if (!QFile::remove(tarGzFile)){
            StaticThreadLogger::error("Loader::moveProcessedFiletToProcessedDirectory","Unable to delete the TAR.GZ file '" + tarGzFile + "'");
        }
    }

    // Moving the qci file
    if (!QFile::copy(qciFile,processedDir + "/" + baseFileName + ".qci")){
        StaticThreadLogger::error("Loader::moveProcessedFiletToProcessedDirectory","Failed to copy " + qciFile + " file from " + patientWorkingDirectory + " to " + processedDir);
        return;
    }

    // Deleting the qci file.
    if (!QFile::remove(qciFile)){
        StaticThreadLogger::error("Loader::moveProcessedFiletToProcessedDirectory","Failed to delete " + qciFile + " qci file from " + patientWorkingDirectory);
    }


}

////////////////////////////////////////////////////////////////// PROTOCOL FUNCTIONS //////////////////////////////////////////////////////////////////

bool Loader::addProtocol(const QString &name, const QString &id) {
    return localDB.addProtocol(name,id,false);
}

void Loader::editProtocol(const QString &id, const QString &newName){
    if (!localDB.addProtocol(newName,id,true)){
        StaticThreadLogger::error("Loader::editProtocol","Error while modifying protocol: " + id + " to new name: " + newName + ". Error was: " + localDB.getError());
    }
}

void Loader::deleteProtocol(const QString &id) {
    if (!localDB.removeProtocol(id)){
        StaticThreadLogger::error("Loader::deleteProtocol","Error while deleting protocol: " + localDB.getError());
    }
}

QVariantMap Loader::getProtocolList() {
    return localDB.getProtocolList();
}


////////////////////////////////////////////////////////////////// PRIVATE FUNCTIONS //////////////////////////////////////////////////////////////////

void Loader::changeLanguage(){
    QString lang = localDB.getPreference(LocalDB::PREF_UI_LANG,"English").toString();
    if (lang == Globals::UILanguage::ES){
        if (!language.loadConfiguration(":/languages/es.lang")){
            // In a stable program this should NEVER happen.
            StaticThreadLogger::error("Loader::changeLanguage","CANNOT LOAD ES LANG FILE: " + language.getError());
            loadingError = true;
        }
    }
    else{
        // Defaults to english
        if (!language.loadConfiguration(":/languages/en.lang")){
            // In a stable program this should NEVER happen.
            StaticThreadLogger::error("Loader::changeLanguage","CANNOT LOAD EN LANG FILE: " + language.getError());
            loadingError = true;
        }
    }

    // We now set the wait message for the studies.
    configuration->addKeyValuePair(Globals::Share::NBACK_WAIT_MSG,language.getString("viewevaluation_nback_wait_msg"));

}

Loader::~Loader(){

}
