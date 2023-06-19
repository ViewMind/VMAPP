#include "loader.h"

Loader::Loader(QObject *parent, ConfigurationManager *c) : QObject(parent)
{

    // Connecting the API Client slot.
    connect(&apiclient, &APIClient::requestFinish, this ,&Loader::receivedRequest);
    //connect(&qc,&QualityControl::finished,this,&Loader::qualityControlFinished);
    connect(&fileDownloader,&FileDownloader::downloadCompleted,this,&Loader::updateDownloadFinished);
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


    QString titleString = "";

    if (isLicenceFilePresent){
        titleString = configuration->getString(Globals::VMConfig::INSTITUTION_NAME)
                + " (" + configuration->getString(Globals::VMConfig::INSTITUTION_ID) + "." + configuration->getString(Globals::VMConfig::INSTANCE_NUMBER) + ")";
    }

    Globals::SetExperimenterVersion(titleString);

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
    StudyControl::FillNumberOfSlidesInExplanations(&language);

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

    // Update cleanup.
    QFile::remove("../" + Globals::Paths::UPDATE_SCRIPT);

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
    return Globals::Share::EXPERIMENTER_VERSION;
}

QString Loader::getVersionNumber() const {
    return Globals::Share::EXPERIMENTER_VERSION_NUMBER;
}

QString Loader::getInstitutionName() const {
    return configuration->getString(Globals::VMConfig::INSTITUTION_NAME);
}


void Loader::openUserManual(){

    // Attempting to find the user manual (it is different when running from the application, versus running from the IDE).
    QString currentDirectory = QDir::currentPath();
    QString filePath = currentDirectory + "/" + Globals::Paths::USER_MANUAL;
    if (!QFile(filePath).exists()){
        //filePath = currentDirectory + "/EyeExperimenter/" + Globals::Paths::USER_MANUAL;
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
    metadata.insert(VMDC::MetadataField::PROC_PARAMETER_KEY,Globals::EyeTracker::PROCESSING_PARAMETER_KEY);
    metadata.insert(VMDC::MetadataField::STATUS,VMDC::StatusType::ONGOING);
    metadata.insert(VMDC::MetadataField::PROTOCOL,protocol);
    metadata.insert(VMDC::MetadataField::DISCARD_REASON,"");
    metadata.insert(VMDC::MetadataField::COMMENTS,"");
    metadata.insert(VMDC::MetadataField::APP_VERSION,this->getWindowTilteVersion());

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

    //qDebug() << "SAVING THE JSON FILE!!!!!!!!!!!";
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

QStringList Loader::getLoginEmails() const {
    return localDB.getUsernameEmails();
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

        QString subject_name = subject_data.value(LocalDB::SUBJECT_NAME).toString() + " " + subject_data.value(LocalDB::SUBJECT_LASTNAME).toString();
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

            //Debug::prettpPrintQVariantMap(map);

            // Now we set the subject.
            map[Globals::QCIFields::SUBJECT]            = subject_name;
            map[Globals::QCIFields::SUBJECT_INST_ID]    = subject_inst_id;

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

void Loader::requestOperatingInfo(bool logOnly){

    newVersionAvailable = "";
    processingUploadError = FAIL_CODE_NONE;

    // We reset the startup sequence flag.
    startUpSequenceFlag = 0;

    // Now we set the work directory for generate any missing QCI files.
    qcChecker.setFilesToProcessFromViewMindDataDirectory();

    // We start the qcChecker in the background.
    qcChecker.start();

    // And now we request the operating info
    //qDebug() << "Requesting Operating Info";

    bool sendLog = false;
    if (logOnly) sendLog = true; // The ONLY purpose of the call is to send the log.
    else {
        sendLog = localDB.checkForLogUpload();
    }

    if (!apiclient.requestOperatingInfo(hwRecognizer.toString(false),sendLog,logOnly)){
        StaticThreadLogger::error("Loader::requestOperatingInfo","Request operating info error: "  + apiclient.getError());
        emit Loader::finishedRequest();
    }
}

void Loader::requestActivation(int institution, int instance, const QString &key){
    processingUploadError = FAIL_CODE_NONE;
    if (!apiclient.requestActivation(institution,instance,key)){
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


void Loader::receivedRequest(){

    if (!apiclient.getError().isEmpty()){
        processingUploadError = FAIL_CODE_SERVER_ERROR;
        StaticThreadLogger::error("Loader::receivedRequest","Error Receiving Request :"  + apiclient.getError());
    }
    else{
        if (apiclient.getLastRequestType() == APIClient::API_OPERATING_INFO || apiclient.getLastRequestType() == APIClient::API_OPERATING_INFO_AND_LOG){

            apiclient.clearFileToSendHandles();

            if (apiclient.getLastRequestType() == APIClient::API_OPERATING_INFO_AND_LOG){
                StaticThreadLogger::log("Loader::receivedRequest","Returned from operating information call with log uplaod"); // This log line serves as the purpose of ensuring that the logfile.log will exist.

                QFile::remove(apiclient.getLastGeneratedLogFileName()); // This will fail if no file exists. So we don't check for errors.

                // We mark now as the last log upload.
                localDB.setLogUploadMark();
            }
            else {
                StaticThreadLogger::log("Loader::receivedRequest","Got Operating Information. No Log Upload");
            }

            // Just in case we clear the calibration failed directory.
            cleanCalibrationDirectory();

            QVariantMap ret = apiclient.getMapDataReturned();
            QVariantMap mainData = ret.value(APINames::MAIN_DATA).toMap();

            if (DBUGBOOL(Debug::Options::PRINT_SERVER_RESP)){
                QString toprint = "DBUG: Received response:\n" + Debug::QVariantMapToString(mainData);
                StaticThreadLogger::warning("Loader::receivedRequest",toprint);
            }

            if (!localDB.setMedicInformationFromRemote(mainData)){
                StaticThreadLogger::error("Loader::receivedRequest","Failed to set medical professionals info from server: " + localDB.getError());
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

            // URL should be present.
            QString dlurl = mainData.value(APINames::UpdateDLFields::URL).toString();

            // Starting the download. And the process must now wait until the download is finished.
            QString expectedPath = "../" + Globals::Paths::UPDATE_PACKAGE;
            fileDownloader.download(dlurl,expectedPath);
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
        else if (apiclient.getLastRequestType() == APIClient::API_OP_INFO_LOG_ONLY){
            // In this case there is nothing todo really other than notify the fron end.
            StaticThreadLogger::log("Loader::receivedRequest","Tech support log upload, successful");
            emit Loader::logUploadFinished();
        }
        else {
            StaticThreadLogger::error("Loader::receivedRequest","Received unknown API Request Type finish:  " + QString::number(apiclient.getLastRequestType()));
        }
    }
    emit Loader::finishedRequest();
}

void Loader::updateDownloadFinished(bool allOk){

    if (!allOk){
        StaticThreadLogger::error("Loader::updateDownloadFinished","Failed to download update. Reason: " + fileDownloader.getError());
        emit Loader::finishedRequest();
        return;
    }

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

        if (!QFile::exists("../" + Globals::Paths::UPDATE_SCRIPT)){
            QString output = QString::fromUtf8(process.readAllStandardOutput());
            StaticThreadLogger::error("Loader::updateDownloadFinished","Failed to to uncompressed update file. Tar output:\n" + output);
        }
        else{
            updater.start();
        }

    }
    else{
        StaticThreadLogger::error("Loader::updateDownloadFinished","The download apparently succeded but the file is not where it was expected: " + expectedPath);
    }

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
    QString lang = localDB.getPreference(LocalDB::PREF_UI_LANG,"en").toString();
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
