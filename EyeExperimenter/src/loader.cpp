#include "loader.h"

Loader::Loader(QObject *parent, ConfigurationManager *c, CountryStruct *cs) : QObject(parent)
{

    // Loading the configuration file and checking for the must have configuration
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    loadingError = false;
    configuration = c;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_REAL;
    cv[CONFIG_XPX_2_MM] = cmd;
    cv[CONFIG_YPX_2_MM] = cmd;
    cv[CONFIG_READING_PX_TOL] = cmd;

    cmd.clear();
    cv[CONFIG_SERVER_ADDRESS] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_BOOL;
    cmd.optional = true;
    cv[CONFIG_DUAL_MONITOR_MODE] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_CONNECTION_TIMEOUT] = cmd;
    cv[CONFIG_DATA_REQUEST_TIMEOUT] = cmd;
    cv[CONFIG_WAIT_DBDATA_TIMEOUT] = cmd;
    cv[CONFIG_WAIT_REPORT_TIMEOUT] = cmd;

    // Optional strings
    cmd.clear();
    cmd.optional = true;
    cv[CONFIG_OUTPUT_DIR] = cmd;
    cv[CONFIG_REPORT_LANGUAGE] = cmd;
    cv[CONFIG_SELECTED_ET] = cmd;
    cv[CONFIG_SSLSERVER_PATH] = cmd;
    cv[CONFIG_DEFAULT_COUNTRY] = cmd;

    cmd.clear();
    cmd.optional = true;
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_VALID_EYE] = cmd;

    // Optional booleans
    cmd.clear();
    cmd.optional = true;
    cmd.type = ConfigurationManager::VT_BOOL;
    cv[CONFIG_DEMO_MODE] = cmd;
    cv[CONFIG_OFFLINE_MODE] = cmd;

    cmd.clear();
    cmd.optional = true;
    cv[CONFIG_DOCTOR_UID] = cmd;
    cv[CONFIG_DOCTOR_NAME] = cmd;

    configuration->setupVerification(cv);


    if (!configuration->loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        logger.appendError("Errors loading the configuration file: " + configuration->getError());
        loadingError = true;
    }

    if (!loadingError){
        loadDefaultConfigurations();
    }

    countries = cs;
    changeLanguage();

    // Setting up de client for DB connection
    dbClient = new SSLDBClient(this,c);
    connect(dbClient,SIGNAL(transactionFinished(bool)),this,SLOT(onTransactionFinished(bool)));
    connect(dbClient,SIGNAL(diconnectionFinished()),this,SLOT(onDisconnectFromDB()));

    // Creating the local configuration manager, and loading the local DB.
    lim = new LocalInformationManager(configuration);
    //lim->printLocalDB();

}

//******************************************* DB Related Functions ***********************************************

void Loader::onTransactionFinished(bool isOk){

    QList<DBData> dbdata;
    QStringList locallist;

    switch (dbOperation){
    case DBO_NEW_DR:
        if (!isOk){
            logger.appendError("DB Add doctor operation failed.");
        }
        break;
    case DBO_LIST_DOCTORS:

        // Getting the dr info from the local DB,
        locallist = lim->getDoctorList(false);
        if (configuration->getBool(CONFIG_OFFLINE_MODE)){
            // There is nothing more to do.
            userDoctorInfo = locallist;
            emit(updatedDoctorList());
            return;
        }

        // Should now set the user doctor info
        userDoctorInfo.clear();
        dbdata = dbClient->getDBData();
        for (qint32 i = 0; i < dbdata.size(); i++){
            if (!dbdata.at(i).error.isEmpty()){
                logger.appendError("DB Error on list doctors:  " + dbdata.at(i).error);
                return;
            }
            // Assuming only one row per transaction so....
            if (dbdata.at(i).rows.size() != 1){
                logger.appendError("Expeting only 1 Row per transaction at " + QString::number(i) + " but got " + QString::number(dbdata.at(i).rows.size()));
            }
            else if (dbdata.at(i).rows.first().size() != 3){
                logger.appendError("Expeting 3 Columns in each row, but row " + QString::number(i) + " got " + QString::number(dbdata.at(i).rows.first().size()));
            }
            else userDoctorInfo << dbdata.at(i).rows.first().at(0) + " " + dbdata.at(i).rows.first().at(1) + " - (" + dbdata.at(i).rows.first().at(2) + ")";
        }

        // Comparing both lists and emitting the longest one:
        if (locallist.size() > userDoctorInfo.size()) userDoctorInfo = locallist;

        emit(updatedDoctorList());
        break;
    case DBO_NEW_PATIENT:
        // Nothing to do.
        break;
    case DBO_LIST_PATIENTS:

        locallist = lim->getPatientListForDoctor();
        if (configuration->getBool(CONFIG_OFFLINE_MODE)){
            // There is nothing more to do.
            patientListForDoctor = locallist;
            emit(updatedDoctorPatientList());
            return;
        }

        patientListForDoctor.clear();
        dbdata = dbClient->getDBData();
        // There should only be ONE query.
        if (dbdata.size() != 1){
            logger.appendError("Expeting only 1 Query Result for Patient List but got " + QString::number(dbdata.size()));
        }
        else if (!dbdata.first().error.isEmpty()){
            logger.appendError("DB Error on list patients:  " + dbdata.first().error);
        }
        else{
            // There can be multiple roews.
            for (qint32 i = 0; i < dbdata.first().rows.size(); i++){
                // There should be ONLY 3 columns.
                if (dbdata.first().rows.at(i).size() != 3){
                    logger.appendError("Expeting 3 Columns in each row, but row " + QString::number(i) + " got " + QString::number(dbdata.first().rows.at(i).size()));
                    break;
                }
                else patientListForDoctor << dbdata.first().rows.at(i).at(0) + " " + dbdata.first().rows.at(i).at(1) + " - (" + dbdata.first().rows.at(i).at(2) + ")";
            }

        }

        if (locallist.size() > patientListForDoctor.size()) patientListForDoctor = locallist;

        emit(updatedDoctorPatientList());
        break;
    default:
        break;
    }
}

void Loader::onDisconnectFromDB(){
    if (dbOperation == DBO_NEW_DR){
        emit(newDoctorAdded());
    }
    else if (dbOperation == DBO_NEW_PATIENT){
        emit(newPatientAdded());
    }
}

void Loader::addNewDoctorToDB(QVariantMap dbdata){
    dbOperation = DBO_NEW_DR;
    dbClient->setDBTransactionType(SQL_QUERY_TYPE_SET);

    // The data for the country must be changed as well as the UID must be generated.
    QString countryCode = countries->getCodeForCountry(dbdata.value(TDOCTOR_COL_COUNTRYID).toString());
    dbdata[TDOCTOR_COL_COUNTRYID] = countryCode;
    dbdata[TDOCTOR_COL_UID] = countryCode + dbdata.value(TDOCTOR_COL_UID).toString();

    QStringList columns;
    QStringList values;

    // Converting the QVariantMap to a double string list
    columns = dbdata.keys();
    for (qint32 i = 0; i < columns.size(); i++){
        values << dbdata.value(columns.at(i)).toString();
    }

    // Saving data locally.
    lim->addDoctorData(dbdata.value(TDOCTOR_COL_UID).toString(),columns,values);

    // In offline mode, there is nothing more to do.
    if (configuration->getBool(CONFIG_OFFLINE_MODE)){
        emit(newDoctorAdded());
        return;
    }

    dbClient->appendSET(TABLE_DOCTORS,columns,values);
    dbClient->runDBTransaction();
}

void Loader::addNewPatientToDB(QVariantMap dbdatareq, QVariantMap dbdataopt){
    dbOperation = DBO_NEW_PATIENT;
    dbClient->setDBTransactionType(SQL_QUERY_TYPE_SET);

    // Making necessary adjustments
    QString countryCode = countries->getCodeForCountry(dbdatareq.value(TPATREQ_COL_BIRTHCOUNTRY).toString());
    dbdatareq[TPATREQ_COL_BIRTHCOUNTRY] = countryCode;
    dbdatareq[TPATREQ_COL_UID] = countryCode + dbdatareq.value(TPATREQ_COL_UID).toString();

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
    dbClient->appendSET(TABLE_PATIENTS_REQ_DATA,columns,values);
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
    dbClient->appendSET(TABLE_PATIENTS_OPT_DATA,columns,values);

    // Saving to the local DB.
    allColumns << columns;
    allValues << values;
    // qWarning() << "All Columns" << allColumns.size() << allColumns;
    // qWarning() << "All Values" << allValues.size() << allValues;

    lim->addPatientData(dbdatareq.value(TPATREQ_COL_UID).toString(),allColumns,allValues);

    if (configuration->getBool(CONFIG_OFFLINE_MODE)) {
        emit(newPatientAdded());
        return;
    }

    dbClient->runDBTransaction();
}

void Loader::getUserDoctorInfoFromDB(){

    dbOperation = DBO_LIST_DOCTORS;
    dbClient->setDBTransactionType(SQL_QUERY_TYPE_GET);

    if (configuration->getBool(CONFIG_OFFLINE_MODE)){
        onTransactionFinished(true);
        return;
    }

    QStringList list_uid = lim->getDoctorList(true);
    QStringList columns;
    columns << TDOCTOR_COL_FIRSTNAME << TDOCTOR_COL_LASTNAME << TDOCTOR_COL_UID;

    for (qint32 i = 0; i < list_uid.size(); i++){
        QString condition = QString(TDOCTOR_COL_UID) + " = '" + list_uid.at(i) + "'";
        dbClient->appendGET(TABLE_DOCTORS,columns,condition);
    }

    dbClient->runDBTransaction();
}

void Loader::getPatientListFromDB(){
    QString druid = configuration->getString(CONFIG_DOCTOR_UID);
    lim->setCurrentDoctor(druid);
    if (druid == "") return;

    dbOperation = DBO_LIST_PATIENTS;
    dbClient->setDBTransactionType(SQL_QUERY_TYPE_GET);

    // For offline mode the local db will be used.
    if (configuration->getBool(CONFIG_OFFLINE_MODE)){
        onTransactionFinished(true);
        return;
    }

    QStringList columns;
    columns << TPATREQ_COL_FIRSTNAME << TPATREQ_COL_LASTNAME << TPATREQ_COL_UID;

    QString condition = QString(TPATREQ_COL_DOCTORID) + " = '" + druid + "'";
    dbClient->appendGET(TABLE_PATIENTS_REQ_DATA,columns,condition);

    dbClient->runDBTransaction();
}

//****************************************************************************************************************

int Loader::getDefaultCountry(){
    QString countryCode = configuration->getString(CONFIG_DEFAULT_COUNTRY);
    if (countryCode.isEmpty()) return 0;
    int ans = countries->getIndexFromCode(countryCode);
    // -1 Means that no selction is done, which means 0 code. All indexes are offseted by 1, due to this.
    return ans + 1;
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

    QDir reports(dir.path() + "/" + QString(DIRNAME_REPORTS));
    QDir rawdata(dir.path() + "/" + QString(DIRNAME_RAWDATA));
    if (!reports.exists()){
        if (!dir.mkdir(DIRNAME_REPORTS)){
            logger.appendError("Cannot create reports directory in selected output directory");
            return "";
        }
    }
    if (!rawdata.exists()){
        if (!dir.mkdir(DIRNAME_RAWDATA)){
            logger.appendError("Cannot create etdata directory in selected output directory");
            return "";
        }
    }

    // Saving it to disk
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_OUTPUT_DIR,configuration->getString(CONFIG_OUTPUT_DIR));

    // Returning the new dir.
    return configuration->getString(CONFIG_OUTPUT_DIR);

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

void Loader::setConfigurationString(const QString &key, const QString &value){
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,key,value,configuration);
}

void Loader::setConfigurationBoolean(const QString &key, bool value){
    QString boolText = value ? "true" : "false";
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,key,boolText,configuration);
}

void Loader::setConfigurationInt(const QString &key, qint32 value){
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,key,QString::number(value),configuration);
}

bool Loader::createPatientDirectory(const QString &patientuid){

    // Creating the doctor directory.
    QString baseDir = configuration->getString(CONFIG_OUTPUT_DIR) + "/" + QString(DIRNAME_RAWDATA);
    QString repDir = configuration->getString(CONFIG_OUTPUT_DIR) + "/" + QString(DIRNAME_REPORTS);
    QString drname = configuration->getString(CONFIG_DOCTOR_UID);
    configuration->addKeyValuePair(CONFIG_PATIENT_UID,patientuid);

    if (!createDirectorySubstructure(drname,patientuid,baseDir,CONFIG_PATIENT_DIRECTORY)) return false;
    if (!createDirectorySubstructure(drname,patientuid,repDir,CONFIG_PATIENT_REPORT_DIR)) return false;

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
    if (!configuration->containsKeyword(CONFIG_DEMO_MODE)){
        configuration->addKeyValuePair(CONFIG_DEMO_MODE,false);
    }
    if (!configuration->containsKeyword(CONFIG_DUAL_MONITOR_MODE)){
        configuration->addKeyValuePair(CONFIG_DUAL_MONITOR_MODE,false);
    }
    if (!configuration->containsKeyword(CONFIG_SELECTED_ET)){
        configuration->addKeyValuePair(CONFIG_SELECTED_ET,CONFIG_P_ET_MOUSE);
    }
    if (!configuration->containsKeyword(CONFIG_REPORT_LANGUAGE)){
        configuration->addKeyValuePair(CONFIG_REPORT_LANGUAGE,CONFIG_P_LANG_EN);
    }
}
