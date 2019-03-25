#include "loader.h"

Loader::Loader(QObject *parent, ConfigurationManager *c, CountryStruct *cs) : QObject(parent)
{

    // Loading the configuration file and checking for the must have configurations
    // The data is this file should NEVER change. Its values should be fixed.
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    loadingError = false;
    configuration = c;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_REAL;
    cv[CONFIG_XPX_2_MM] = cmd;
    cv[CONFIG_YPX_2_MM] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    // Eye Data processing parameters
    cv[CONFIG_READING_PX_TOL] = cmd;
    cv[CONFIG_MOVING_WINDOW_DISP] = cmd;
    cv[CONFIG_MIN_FIXATION_LENGTH] = cmd;
    cv[CONFIG_SAMPLE_FREQUENCY] = cmd;
    cv[CONFIG_DISTANCE_2_MONITOR] = cmd;
    cv[CONFIG_LATENCY_ESCAPE_RAD] = cmd;
    cv[CONFIG_MARGIN_TARGET_HIT] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_REAL;
    cv[CONFIG_TOL_MAX_FGLITECHES_IN_TRIAL] = cmd;
    cv[CONFIG_TOL_MAX_PERIOD_TOL] = cmd;
    cv[CONFIG_TOL_MIN_PERIOD_TOL] = cmd;
    cv[CONFIG_TOL_MAX_PERCENT_OF_INVALID_VALUES] = cmd;
    cv[CONFIG_TOL_MIN_NUMBER_OF_DATA_ITEMS_IN_TRIAL] = cmd;

    // Timeouts for connections.
    cv[CONFIG_CONNECTION_TIMEOUT] = cmd;
    cv[CONFIG_DATA_REQUEST_TIMEOUT] = cmd;
    cv[CONFIG_WAIT_DBDATA_TIMEOUT] = cmd;
    cv[CONFIG_WAIT_REPORT_TIMEOUT] = cmd;

    // Server address and the default country.
    cmd.clear();
    cv[CONFIG_SERVER_ADDRESS] = cmd;
    cv[CONFIG_EYEEXP_NUMBER] = cmd;
    cv[CONFIG_INST_NAME] = cmd;
    cv[CONFIG_INST_ETSERIAL] = cmd;
    cv[CONFIG_INST_UID] = cmd;
    cv[CONFIG_INST_PASSWORD] = cmd;

    cmd.clear();
    cmd.optional = true;
    cmd.type = ConfigurationManager::VT_BOOL;
    cv[CONFIG_TEST_MODE] = cmd;
    cv[CONFIG_ENABLE_GAZE_FOLLOWING] = cmd;

    // This cannot have ANY ERRORS
    configuration->setupVerification(cv);
    if (!configuration->loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        qWarning() << "Could not load config file";
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
    cv[CONFIG_REPORT_LANGUAGE] = cmd;
    cv[CONFIG_DEFAULT_COUNTRY] = cmd;

    // The booleans
    cmd.type = ConfigurationManager::VT_BOOL;
    cv[CONFIG_DUAL_MONITOR_MODE] = cmd;
    cv[CONFIG_DEMO_MODE] = cmd;
    cv[CONFIG_USE_MOUSE] = cmd;

    // Merging the settings or loading the default configuration.
    settings.setupVerification(cv);
    if (!settings.loadConfiguration(FILE_SETTINGS,COMMON_TEXT_CODEC)){
        logger.appendError("Errors loading the settings file: " + settings.getError());
        // Settings files should not have unwanted key words
        loadingError = true;
    }
    else{
        configuration->merge(settings);
    }
    loadDefaultConfigurations();

    countries = cs;
    changeLanguage();

    if (loadingError) return;

#ifdef USE_IVIEW
    configuration->addKeyValuePair(CONFIG_EYETRACKER_CONFIGURED,CONFIG_P_ET_REDM);
#else
    configuration->addKeyValuePair(CONFIG_EYETRACKER_CONFIGURED,CONFIG_P_ET_GP3HD);
#endif

    // Must make sure that the data directory exists
    QDir rawdata(DIRNAME_RAWDATA);
    if (!rawdata.exists()){
        if (!QDir(".").mkdir(DIRNAME_RAWDATA)){
            logger.appendError("Cannot create viewmind_data directory");
            loadingError = true;
            return;
        }
    }

    // Setting up de client for DB connection
    dbClient = new SSLDBClient(this,configuration);
    connect(dbClient,SIGNAL(transactionFinished()),this,SLOT(onDisconnectFromDB()));

    // Creating the local configuration manager, and loading the local DB.
    lim.setDirectory(QString(DIRNAME_RAWDATA), configuration->getString(CONFIG_EYEEXP_NUMBER), configuration->getString(CONFIG_INST_UID));

    // Creating the db backup directory if it does not exist.
    QDir(".").mkdir(DIRNAME_DBBKP);
    lim.enableBackups(DIRNAME_DBBKP);

    // Resetting the medical institution, just in case
    lim.resetMedicalInstitutionForAllDoctors(configuration->getString(CONFIG_INST_UID));

#ifdef USE_IVIEW
    QString expectedET = CONFIG_P_ET_REDM;
#else
    QString expectedET = CONFIG_P_ET_GP3HD;
#endif

    if (configuration->getString(CONFIG_EYETRACKER_CONFIGURED) != expectedET){
        logger.appendError("Wrong ET in the configuration file: Was expecting " + expectedET + " but found: " + configuration->getString(CONFIG_EYETRACKER_CONFIGURED));
        loadingError = true;
        return;
    }

    // Checking which ET should be used: Mouse or the one in the configuration file.
    if (configuration->getBool(CONFIG_USE_MOUSE)) configuration->addKeyValuePair(CONFIG_SELECTED_ET,CONFIG_P_ET_MOUSE);
    else configuration->addKeyValuePair(CONFIG_SELECTED_ET,configuration->getString(CONFIG_EYETRACKER_CONFIGURED));

    //    // TEST FOR FREQ CHECK
    //    Experiment *e = new Experiment();
    //    QString reading = "C:/Users/Viewmind/Documents/QtProjects/EyeExperimenter/exe/viewmind_etdata/AR123456789/CO123456788/reading_2_2019_03_16_11_54.dat";
    //    //QString binding = "C:/Users/Viewmind/Documents/ExperimenterOutputs/grace_worked/binding_bc_2_l_2_2019_03_14_17_56.dat";
    //    e->setupFreqCheck(configuration,reading);
    //    if (!e->doFrequencyCheck()){
    //        logger.appendError("Failed test freq check: " + e->getError());
    //    }
    //    else{
    //        logger.appendSuccess("Freq Check Passed");
    //    }

}

//*********************************************** UI Functions ******************************************************

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

bool Loader::checkETChange(){
    // Returns true if ET has been changed is required.
    if (configuration->getBool(CONFIG_USE_MOUSE) && (configuration->getString(CONFIG_SELECTED_ET) != CONFIG_P_ET_MOUSE)){
        // Mouse was configured, but mouse is not the selected ET. Switching to mouse.
        configuration->addKeyValuePair(CONFIG_SELECTED_ET,CONFIG_P_ET_MOUSE);
        return true;
    }
    else if (!configuration->getBool(CONFIG_USE_MOUSE) && (configuration->getString(CONFIG_SELECTED_ET) == CONFIG_P_ET_MOUSE)){
        // Mouse was NOT configured but it IS the selected ET. Switching to configured ET.
        configuration->addKeyValuePair(CONFIG_SELECTED_ET,configuration->getString(CONFIG_EYETRACKER_CONFIGURED));
        return true;
    }
    return false;
}

QRect Loader::frameSize(QObject *window)
{
    QQuickWindow *qw = qobject_cast<QQuickWindow *>(window);
    if(qw)
        return qw->frameGeometry();
    return QRect();
}

int Loader::getDefaultCountry(bool offset){
    QString countryCode = configuration->getString(CONFIG_DEFAULT_COUNTRY);
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

QString Loader::loadTextFile(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return "";
    QTextStream reader(&file);
    QString ans = reader.readAll();
    file.close();
    return ans;
}

QStringList Loader::getErrorMessageForCode(quint8 code){
    //qWarning() << "GETTING ERROR MESSAGE FOR CODE" << code;
    switch (code) {
    case RR_DB_ERROR:
        return language.getStringList("error_db_transaction");
    case RR_OUT_OF_EVALUATIONS:
        return language.getStringList("error_db_outofevals");
    case RR_WRONG_ET_SERIAL:
        return language.getStringList("error_db_wrongetserial");
    default:
        //logger.appendError("UNKNOWN Code when getting transaction error message: " + QString::number(code));
        break;
    }
    return QStringList();
}

QStringList Loader::getErrorMessageForDBCode(){
    quint8 code = dbClient->getErrorCode();
    //qWarning() << "GETTING ERROR MESSAGE FOR CODE" << code;
    switch (code) {
    case DBACK_DBCOMM_ERROR:
        return language.getStringList("error_db_transaction");
    case DBACK_UID_ERROR:
        return language.getStringList("error_db_wronginst");
    default:
        logger.appendError("UNKNOWN Code when getting transaction db error message: " + QString::number(code));
        break;
    }
    return QStringList();
}

QStringList Loader::getFileListForPatient(QString patuid, qint32 type){
    if (patuid.isEmpty()) patuid = configuration->getString(CONFIG_PATIENT_UID);
    return lim.getFileListForPatient(patuid,type);
}

QStringList Loader::getFileListCompatibleWithSelectedBC(QString patuid, qint32 selectedBC){
    if (patuid.isEmpty()) patuid = configuration->getString(CONFIG_PATIENT_UID);
    return lim.getBindingUCFileListCompatibleWithSelectedBC(patuid,selectedBC);
}

//*********************************************** Configuration Functions ******************************************************

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
    else return "";
}

void Loader::setSettingsValue(const QString &key, const QVariant &var){
    ConfigurationManager::setValue(FILE_SETTINGS,COMMON_TEXT_CODEC,key,var.toString(),configuration);
}

void Loader::setAgeForCurrentPatient(){
    QString birthDate = lim.getFieldForPatient(configuration->getString(CONFIG_PATIENT_UID),TPATDATA_COL_BIRTHDATE);
    QDate bdate = QDate::fromString(birthDate,"yyyy-MM-dd");
    QDate currentDate =  QDate::currentDate();
    int currentAge = currentDate.year() - bdate.year();
    if ( (bdate.month() > currentDate.month()) || ( (bdate.month() == currentDate.month()) && (bdate.day() > currentDate.day()) ) ) {
        currentAge--;
    }
    configuration->addKeyValuePair(CONFIG_PATIENT_AGE,currentAge);
}

//*********************************************** Local DB Functions ******************************************************

bool Loader::createPatientDirectory(){

    // Creating the doctor directory.
    QString patientuid = configuration->getString(CONFIG_PATIENT_UID);
    QString patdirPath = QString(DIRNAME_RAWDATA) + "/" + patientuid;
    if (!QDir(patdirPath).exists()){
        QDir baseDir(DIRNAME_RAWDATA);
        baseDir.mkdir(patientuid);
    }

    if (!QDir(patdirPath).exists()){
        logger.appendError("Could not create patient dir " + patientuid + " in directory");
        return false;
    }

    configuration->addKeyValuePair(CONFIG_PATIENT_DIRECTORY,patdirPath);

    return true;
}

QStringList Loader::generatePatientLists(const QString &filter, bool showAll){
    nameInfoList.clear();
    //qWarning() << "GENERATE PATIENT LIST " << showAll;
    if (showAll) nameInfoList = lim.getPatientListForDoctor("",filter);
    else nameInfoList = lim.getPatientListForDoctor(configuration->getString(CONFIG_DOCTOR_UID),filter);
    return nameInfoList.patientNames;
}

QString Loader::getDoctorUIDByIndex(qint32 selectedIndex){
    if ((selectedIndex > -1) && (selectedIndex < nameInfoList.doctorUIDs.size())){
        return nameInfoList.doctorUIDs.at(selectedIndex);
    }
    else return "";
}

qint32 Loader::getIndexOfDoctor(QString uid){
    if (uid.isEmpty()) uid = configuration->getString(CONFIG_DOCTOR_UID);
    return nameInfoList.doctorUIDs.indexOf(uid);
}

bool Loader::isDoctorValidated(qint32 selectedIndex){
    QString uid;
    if (selectedIndex == -1) uid = configuration->getString(CONFIG_DOCTOR_UID);
    else uid = getDoctorUIDByIndex(selectedIndex);
    return lim.isDoctorValid(uid);
}

bool Loader::isDoctorPasswordEmpty(qint32 selectedIndex){
    QString uid = getDoctorUIDByIndex(selectedIndex);
    //qWarning() << "Password for UID" << uid << " is " << lim.getDoctorPassword(uid) << ". Is empty: " <<  lim.getDoctorPassword(uid).isEmpty();
    return lim.getDoctorPassword(uid).isEmpty();
}

bool Loader::isDoctorPasswordCorrect(const QString &password){
    QString hashpass = QString(QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha256).toHex());
    QString storedPassword = lim.getDoctorPassword(configuration->getString(CONFIG_DOCTOR_UID));
    if (storedPassword.isEmpty()) return true;
    if (storedPassword == hashpass) return true;
    // Checking against the institution password
    return (hashpass == configuration->getString(CONFIG_INST_PASSWORD));
}

void Loader::addNewDoctorToDB(QVariantMap dbdata, QString password, bool hide){

    // The data for the country must be changed as well as the UID must be generated.
    QString uid = dbdata.value(TDOCTOR_COL_UID).toString();

    if (uid.isEmpty()){
        // This is a new doctor
        if (configuration->getBool(CONFIG_TEST_MODE)){
            // CAN ONLY CREATE XXXX Doctor.
            uid = TEST_DOCTOR_ID;
        }
        else{
            uid = lim.newDoctorID();
            uid = configuration->getString(CONFIG_INST_UID) + "_" + configuration->getString(CONFIG_EYEEXP_NUMBER) + "_" + uid;
        }
    }

    QStringList columns;
    QStringList values;

    // The passoword for the doctor ONLY goes into the local DB, it needs to be removed from the data.
    if (password != ""){
        // The password needs to be overwritten.
        password = QString(QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha256).toHex());
    }

    // Converting the QVariantMap to a double string list
    columns = dbdata.keys();
    for (qint32 i = 0; i < columns.size(); i++){
        values << dbdata.value(columns.at(i)).toString();
    }

    // Adding the medical institution
    columns << TDOCTOR_COL_MEDICAL_INST;
    values << configuration->getString(CONFIG_INST_UID);

    // Saving data locally.
    lim.addDoctorData(uid,columns,values,password,hide);

}

void Loader::addNewPatientToDB(QVariantMap dbdata){

    // Making necessary adjustments
    if (dbdata.value(TPATDATA_COL_PUID).toString().isEmpty()){
        QString uid = lim.newPatientID();
        uid = configuration->getString(CONFIG_INST_UID) + "_" + configuration->getString(CONFIG_EYEEXP_NUMBER) + "_" + uid;
        dbdata[TPATDATA_COL_PUID] = uid;
    }

    // Transforming the date format.
    QString date = dbdata.value(TPATDATA_COL_BIRTHDATE).toString();
    QStringList dateParts = date.split("/");
    // Since the date format is given by the program, I can trust that the split will have 3 parts. Setting the ISO Date.
    date = dateParts.at(2) + "-" + dateParts.at(1) + "-" + dateParts.at(0);
    dbdata[TPATDATA_COL_BIRTHDATE] = date;

    QString countryCode = countries->getCodeForCountry(dbdata.value(TPATDATA_COL_BIRTHCOUNTRY).toString());
    dbdata[TPATDATA_COL_BIRTHCOUNTRY] = countryCode;

    // Insertion date.
    dbdata[TPATDATA_COL_DATE_INS] = "TIMESTAMP(NOW())";

    QStringList columns;
    QStringList values;

    QStringList allColumns;
    QStringList allValues;

    columns = dbdata.keys();
    for (qint32 i = 0; i < columns.size(); i++){
        values << dbdata.value(columns.at(i)).toString();
    }

    //dbClient->appendSET(TABLE_PATIENTS_REQ_DATA,columns,values);
    allColumns = columns;
    allValues = values;

    // Adding the optional data, plus the mandaotory data.
    lim.addPatientData(dbdata.value(TPATDATA_COL_PUID).toString(),configuration->getString(CONFIG_DOCTOR_UID),allColumns,allValues);

}

void Loader::startDBSync(){
    // Adding all the data that needs to be sent.
    wasDBTransactionStarted = false;
    if (lim.setupDBSynch(dbClient)){
        // Running the transaction.
        wasDBTransactionStarted = true;
        dbClient->runDBTransaction();
    }
    else {
        onDisconnectFromDB();
    }
}

bool Loader::verifyInstitutionPassword(const QString &instPass){
    QString hasshedPassword = QString(QCryptographicHash::hash(instPass.toUtf8(),QCryptographicHash::Sha256).toHex());
    return (configuration->getString(CONFIG_INST_PASSWORD) == hasshedPassword);
}

bool Loader::requestDrValidation(const QString &instPassword, qint32 selectedDr){
    // Storing the hasshed password for later comparison
    QString drUIDtoValidate = getDoctorUIDByIndex(selectedDr);
    if (verifyInstitutionPassword(instPassword)){
        lim.validateDoctor(drUIDtoValidate);
        return true;
    }
    return false;
}


//******************************************* Report Realated Functions ***********************************************

void Loader::operateOnRepGenStruct(qint32 index, qint32 type){
    if ((type == -1) && (index == -1)) reportGenerationStruct.clear();
    else {
        switch(type){
        case LIST_INDEX_READING:
            reportGenerationStruct.readingFileIndex = index;
            break;
        case LIST_INDEX_BINDING_UC:
            reportGenerationStruct.bindingUCFileIndex = index;
            break;
        case LIST_INDEX_BINDING_BC:
            reportGenerationStruct.bindingBCFileIndex = index;
            break;
        }
    }
}

QString Loader::getDatFileNameFromIndex(qint32 index, QString patuid, qint32 type){
    if (patuid.isEmpty()) patuid = configuration->getString(CONFIG_PATIENT_UID);
    return lim.getDatFileFromIndex(patuid,index,type);
}

void Loader::reloadPatientDatInformation(){
    lim.fillPatientDatInformation(configuration->getString(CONFIG_PATIENT_UID));
}


//******************************************* Updater Related Functions ***********************************************

QString Loader::checkForChangeLog(){
    QString filePath = "launcher/" + QString (FILE_CHANGELOG_UPDATER) + "_"  + configuration->getString(CONFIG_REPORT_LANGUAGE);
    qWarning() << "Searching for changelog" << filePath;
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly)) return "";
    QTextStream reader(&file);
    reader.setCodec(COMMON_TEXT_CODEC);
    QString content = reader.readAll();
    if (content.size() < 4) return "";
    else return content;
}

bool Loader::clearChangeLogFile(){
    QDir dir(DIRNAME_LAUNCHER);
    QStringList filter;
    filter << QString(FILE_CHANGELOG_UPDATER) + "*";
    QStringList allchangelogs = dir.entryList(filter,QDir::Files);
    for (qint32 i = 0; i < allchangelogs.size(); i++){
        QFile file(QString(DIRNAME_LAUNCHER) + "/" + allchangelogs.at(i));
        file.remove();
    }

    QFile newLauncher(QString(DIRNAME_LAUNCHER) + "/" + QString(FILE_NEW_LAUCHER_FILE));
    return newLauncher.exists();
}

void Loader::replaceEyeLauncher(){
    QString newfile = QString(DIRNAME_LAUNCHER) + "/" + QString(FILE_NEW_LAUCHER_FILE);
    QString oldfile = QString(DIRNAME_LAUNCHER) + "/" + QString(FILE_OLD_LAUCHER_FILE);
    QString currentfile = QString(DIRNAME_LAUNCHER) + "/" + QString(FILE_EYE_LAUCHER_FILE);

    // Backing up current
    QFile(oldfile).remove();
    if (!QFile::copy(currentfile,oldfile)){
        logger.appendError("Failed to backup EyeLauncherClient from " + currentfile + " to " + oldfile);
        QCoreApplication::quit();
    }

    // Replacing the current with the new
    QFile(currentfile).remove();
    if (!QFile::copy(newfile,currentfile)){
        logger.appendError("Failed to update EyeLauncherClient from " + newfile + " to " + currentfile);
        QCoreApplication::quit();
    }

    // Removing the new file
    if (!QFile(newfile).remove()){
        logger.appendError("Could not remove new EyeLauncherClient file: " + newfile);
    }

    QCoreApplication::quit();
}

//******************************************* SLOTS ***********************************************

void Loader::onDisconnectFromDB(){
    //qWarning() << "onDisconnectFromDB wasDBTransactionStarted" << wasDBTransactionStarted;
    if (dbClient->getTransactionStatus() && wasDBTransactionStarted){
        lim.setUpdateFlagTo(false);
    }
    emit(synchDone());
}

void Loader::onFileSetRequested(const QStringList &fileList){
    QStringList fileSet;
    QString patuid = configuration->getString(CONFIG_PATIENT_UID);
    if (fileList.isEmpty()){
        fileSet = lim.getReportNameAndFileSet(patuid,reportGenerationStruct);
    }
    else{
        fileSet = lim.getReportNameAndFileSet(patuid,fileList);
        //qWarning() << "Getting the report name and file set from existing files" << fileList << "and they are" << fileSet;
    }
    emit(fileSetReady(fileSet));
}

//******************************************* Private Auxiliary Functions ***********************************************

void Loader::changeLanguage(){
    QString lang = configuration->getString(CONFIG_REPORT_LANGUAGE);
    if (lang == CONFIG_P_LANG_ES){
        if (!language.loadConfiguration(":/languages/es.lang",COMMON_TEXT_CODEC)){
            // In a stable program this should NEVER happen.
            logger.appendError("CANNOT LOAD ES LANG FILE: " + language.getError());
            loadingError = true;
        }
        else countries->fillCountryList(false);
    }
    else{
        // Defaults to english
        if (!language.loadConfiguration(":/languages/en.lang",COMMON_TEXT_CODEC)){
            // In a stable program this should NEVER happen.
            logger.appendError("CANNOT LOAD EN LANG FILE: " + language.getError());
            loadingError = true;
        }
        else countries->fillCountryList(true);
    }
}

void Loader::loadDefaultConfigurations(){

    if (!configuration->containsKeyword(CONFIG_DEFAULT_COUNTRY)) configuration->addKeyValuePair(CONFIG_DEFAULT_COUNTRY,"AR");
    if (!configuration->containsKeyword(CONFIG_DEMO_MODE)) configuration->addKeyValuePair(CONFIG_DEMO_MODE,false);
    if (!configuration->containsKeyword(CONFIG_DUAL_MONITOR_MODE)) configuration->addKeyValuePair(CONFIG_DUAL_MONITOR_MODE,false);
    if (!configuration->containsKeyword(CONFIG_USE_MOUSE)) configuration->addKeyValuePair(CONFIG_USE_MOUSE,true);
    if (!configuration->containsKeyword(CONFIG_SELECTED_ET)) configuration->addKeyValuePair(CONFIG_SELECTED_ET,CONFIG_P_ET_MOUSE);
    if (!configuration->containsKeyword(CONFIG_DUAL_MONITOR_MODE)) configuration->addKeyValuePair(CONFIG_DUAL_MONITOR_MODE,false);
    if (!configuration->containsKeyword(CONFIG_REPORT_LANGUAGE)) configuration->addKeyValuePair(CONFIG_REPORT_LANGUAGE,CONFIG_P_LANG_EN);

}

Loader::~Loader(){

}
