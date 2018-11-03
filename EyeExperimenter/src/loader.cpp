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

    // Timeouts for connections.
    cv[CONFIG_CONNECTION_TIMEOUT] = cmd;
    cv[CONFIG_DATA_REQUEST_TIMEOUT] = cmd;
    cv[CONFIG_WAIT_DBDATA_TIMEOUT] = cmd;
    cv[CONFIG_WAIT_REPORT_TIMEOUT] = cmd;

    // Server address and the default country.
    cmd.clear();
    cv[CONFIG_SERVER_ADDRESS] = cmd;
    cv[CONFIG_EYETRACKER_CONFIGURED] = cmd;
    cv[CONFIG_INST_NAME] = cmd;
    cv[CONFIG_INST_ETSERIAL] = cmd;
    cv[CONFIG_INST_UID] = cmd;
    cv[CONFIG_INST_PASSWORD] = cmd;

    cmd.clear();
    cmd.optional = true;
    cmd.type = ConfigurationManager::VT_BOOL;
    cv[CONFIG_TEST_MODE] = cmd;

    // This cannot have ANY ERRORS
    configuration->setupVerification(cv);
    if (!configuration->loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
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
    cv[CONFIG_OUTPUT_DIR] = cmd;
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
    }
    else{
        configuration->merge(settings);
    }
    loadDefaultConfigurations();

    countries = cs;
    changeLanguage();
    if (loadingError) return;

    // Setting up de client for DB connection
    dbClient = new SSLDBClient(this,configuration);
    connect(dbClient,SIGNAL(transactionFinished()),this,SLOT(onDisconnectFromDB()));

    // Creating the local configuration manager, and loading the local DB.
    lim.setDirectory(configuration->getString(CONFIG_OUTPUT_DIR) + "/" + QString(DIRNAME_RAWDATA));

    // Resetting the medical institution, just in case
    lim.resetMedicalInstitutionForAllDoctors(configuration->getString(CONFIG_INST_UID));

}

//******************************************* DB Related Functions ***********************************************

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

bool Loader::requestDrValidation(const QString &instPassword, qint32 selectedDr){
    // Storing the hasshed password for later comparison
    QString hasshedPassword = QString(QCryptographicHash::hash(instPassword.toUtf8(),QCryptographicHash::Sha256).toHex());
    QString drUIDtoValidate = getDoctorUIDByIndex(selectedDr);
    if (configuration->getString(CONFIG_INST_PASSWORD) == hasshedPassword){
        lim.validateDoctor(drUIDtoValidate);
        return true;
    }
    return false;
}

void Loader::onDisconnectFromDB(){
    if (dbClient->getTransactionStatus()){
        lim.setUpdateFlagTo(false);
    }
    emit(synchDone());
}

QString Loader::loadTextFile(const QString &fileName){
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) return "";
    QTextStream reader(&file);
    QString ans = reader.readAll();
    file.close();
    return ans;
}

void Loader::prepareAllPatientIteration(){
    allPatientList = lim.getAllPatientInfo();
    allPatientIndex = 0;
}

QStringList Loader::nextInAllPatientIteration(){
    if (allPatientIndex == allPatientList.size()) return QStringList();
    else{
        allPatientIndex++;
        return allPatientList.at(allPatientIndex-1);
    }
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
        logger.appendError("UNKNOWN Code when getting transaction error message: " + QString::number(code));
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
        logger.appendError("UNKNOWN Code when getting transaction error message: " + QString::number(code));
        break;
    }
    return QStringList();
}
bool Loader::addNewDoctorToDB(QVariantMap dbdata, QString password, bool hide, bool isNew){
    // The data for the country must be changed as well as the UID must be generated.
    QString countryCode = countries->getCodeForCountry(dbdata.value(TDOCTOR_COL_COUNTRYID).toString());
    dbdata[TDOCTOR_COL_COUNTRYID] = countryCode;
    QString uid = countryCode + dbdata.value(TDOCTOR_COL_UID).toString();
    dbdata[TDOCTOR_COL_UID] = uid;

    // This function returns false ONLY when it is attempting to create a new doctor with an existing UID.
    // qWarning() << "IsNew" << isNew << "Exists" << lim.doesDoctorExist(uid);
    if (isNew && lim.doesDoctorExist(uid)){
        return false;
    }

    // Checking for test mode doctor
    if (uid.contains(TEST_UID) && !configuration->getBool(CONFIG_TEST_MODE)){
        return false;
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
    lim.addDoctorData(dbdata.value(TDOCTOR_COL_UID).toString(),columns,values,password,hide);
    return true;
}

bool Loader::addNewPatientToDB(QVariantMap dbdatareq, QVariantMap dbdataopt, bool isNew){

    // Making necessary adjustments
    QString countryCode = countries->getCodeForCountry(dbdatareq.value(TPATREQ_COL_BIRTHCOUNTRY).toString());
    dbdatareq[TPATREQ_COL_BIRTHCOUNTRY] = countryCode;
    QString uid = countryCode + dbdatareq.value(TPATREQ_COL_UID).toString();
    dbdatareq[TPATREQ_COL_UID] = uid;

    if (lim.doesPatientExist(configuration->getString(CONFIG_DOCTOR_UID),uid) && isNew) return false;

    // Transforming the date format.
    QString date = dbdatareq.value(TPATREQ_COL_BIRTHDATE).toString();
    QStringList dateParts = date.split("/");
    // Since the date format is given by the program, I can trust that the split will have 3 parts. Setting the ISO Date.
    date = dateParts.at(2) + "-" + dateParts.at(1) + "-" + dateParts.at(0);
    dbdatareq[TPATREQ_COL_BIRTHDATE] = date;

    QStringList columns;
    QStringList values;

    QStringList allColumns;
    QStringList allValues;

    columns = dbdatareq.keys();
    for (qint32 i = 0; i < columns.size(); i++){
        values << dbdatareq.value(columns.at(i)).toString();
    }

    //dbClient->appendSET(TABLE_PATIENTS_REQ_DATA,columns,values);
    allColumns = columns;
    allValues = values;

    // Adding the optional data, plus the mandaotory data.
    dbdataopt[TPATOPT_COL_DATE_INS] = "TIMESTAMP(NOW())";
    dbdataopt[TPATOPT_COL_PATIENTID] = dbdatareq.value(TPATREQ_COL_UID).toString();
    columns = dbdataopt.keys();
    values.clear();
    for (qint32 i = 0; i < columns.size(); i++){
        values << dbdataopt.value(columns.at(i)).toString();
    }

    // Saving to the local DB.
    allColumns << columns;
    allValues << values;
    // qWarning() << "All Columns" << allColumns.size() << allColumns;
    // qWarning() << "All Values" << allValues.size() << allValues;

    lim.addPatientData(configuration->getString(CONFIG_DOCTOR_UID),dbdatareq.value(TPATREQ_COL_UID).toString(),allColumns,allValues);

    return true;

}

//****************************************************************************************************************

void Loader::prepareForRequestOfPendingReports(){
    lim.preparePendingReports(configuration->getString(CONFIG_PATIENT_UID));
}

void Loader::onRequestNextPendingReport(){
    emit(nextFileSet(lim.nextPendingReport(configuration->getString(CONFIG_PATIENT_UID))));
}

void Loader::setAgeForCurrentPatient(){
    QString birthDate = lim.getFieldForPatient(configuration->getString(CONFIG_DOCTOR_UID),configuration->getString(CONFIG_PATIENT_UID),TPATREQ_COL_BIRTHDATE);
    QDate bdate = QDate::fromString(birthDate,"yyyy-MM-dd");
    int currentAge = QDate::currentDate().year() - bdate.year();
    configuration->addKeyValuePair(CONFIG_PATIENT_AGE,currentAge);
}

QStringList Loader::getDoctorList() {
    nameInfoList.clear();
    nameInfoList = lim.getDoctorList();
    if (nameInfoList.isEmpty()) return QStringList();
    else return nameInfoList.at(0);
}

QStringList Loader::getPatientList(){
    nameInfoList.clear();
    nameInfoList = lim.getPatientListForDoctor(configuration->getString(CONFIG_DOCTOR_UID));
    if (nameInfoList.isEmpty()) return QStringList();
    else return nameInfoList.at(0);
}

QString Loader::getDoctorUIDByIndex(qint32 selectedIndex){
    if (nameInfoList.size() < 2) return "";
    if ((selectedIndex > -1) && (selectedIndex < nameInfoList.at(1).size())){
        return nameInfoList.at(1).at(selectedIndex);
    }
    else return "";
}

bool Loader::isDoctorPasswordCorrect(const QString &password){
    QString hashpass = QString(QCryptographicHash::hash(password.toUtf8(),QCryptographicHash::Sha256).toHex());
    QString storedPassword = lim.getDoctorPassword(configuration->getString(CONFIG_DOCTOR_UID));
    if (storedPassword.isEmpty()) return true;
    if (storedPassword == hashpass) return true;
    // Checking against the institution password
    return (hashpass == configuration->getString(CONFIG_INST_PASSWORD));
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

QStringList Loader::getUIDList() {
    if (nameInfoList.size() < 2) return QStringList();
    else return nameInfoList.at(1);
}

QStringList Loader::getPatientIsOKList() {
    if (nameInfoList.size() < 3) return QStringList();
    else return nameInfoList.at(2);
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

QRect Loader::frameSize(QObject *window)
{
    QQuickWindow *qw = qobject_cast<QQuickWindow *>(window);
    if(qw)
        return qw->frameGeometry();
    return QRect();
}

QString Loader::hasValidOutputRepo(const QString &dirToCheck){

    if (dirToCheck != ""){
        QUrl url(dirToCheck);
        QString fileloc = url.toLocalFile();

        if (fileloc.isEmpty()){
            // This means that it is a stright directroy path and not an URL.
            fileloc = dirToCheck;
        }

        configuration->addKeyValuePair(CONFIG_OUTPUT_DIR,fileloc);
    }

    // Checking if the directory for the outputs exists and is valid. If there are any problems the user is asked for a directory instead.
    if (!configuration->containsKeyword(CONFIG_OUTPUT_DIR)) return "";
    QDir dir(configuration->getString(CONFIG_OUTPUT_DIR));
    if (!dir.exists()) return "";

    QDir rawdata(dir.path() + "/" + QString(DIRNAME_RAWDATA));
    if (!rawdata.exists()){
        if (!dir.mkdir(DIRNAME_RAWDATA)){
            logger.appendError("Cannot create etdata directory in selected output directory");
            return "";
        }
    }

    // Returning the new dir.
    return configuration->getString(CONFIG_OUTPUT_DIR);

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

void Loader::setSettingsValue(const QString &key, const QVariant &var){
    ConfigurationManager::setValue(FILE_SETTINGS,COMMON_TEXT_CODEC,key,var.toString(),configuration);
}

bool Loader::createPatientDirectory(){

    // Creating the doctor directory.
    QString patientuid = configuration->getString(CONFIG_PATIENT_UID);
    QString baseDir = configuration->getString(CONFIG_OUTPUT_DIR) + "/" + QString(DIRNAME_RAWDATA);
    QString drname = configuration->getString(CONFIG_DOCTOR_UID);
    configuration->addKeyValuePair(CONFIG_PATIENT_UID,patientuid);

    if (!createDirectorySubstructure(drname,patientuid,baseDir,CONFIG_PATIENT_DIRECTORY)) return false;

    return true;
}

bool Loader::createDirectorySubstructure(QString drname, QString pname, QString baseDir, QString saveAs){

    // This is legacy code, should have no effect. Left just in case.
    drname = drname.replace(" ","_");
    drname = drname.replace(".","_");

    pname = pname.replace(" ","_");
    pname = pname.replace(".","_");

    QString drdir = baseDir + "/" + drname;
    QString pdir = drdir + "/" + pname;

    // Creating the the doctor's dir
    if (!QDir(drdir).exists()){
        QDir base(baseDir);
        if (!base.mkdir(drname)){
            logger.appendError("Could not create doctor dir: " + drdir + " in " + baseDir);
            return false;
        }
    }

    QDir dirdr(drdir);
    if (!QDir(pdir).exists()){
        if (!dirdr.mkdir(pname)){
            logger.appendError("Could not create patient dir: " + pname + " in " + drdir);
            return false;
        }
    }

    // Al is good and pdir is the final working directory so it is set in the configuration->
    configuration->addKeyValuePair(saveAs,pdir);
    return true;
}

void Loader::loadDefaultConfigurations(){

    if (!configuration->containsKeyword(CONFIG_OUTPUT_DIR)) configuration->addKeyValuePair(CONFIG_OUTPUT_DIR,""); // Reconfigure the settings.
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
