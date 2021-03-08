#include "control.h"

Control::Control(QObject *parent) : QObject(parent)
{
    numberOfRemaingEvaluations = -3;
}

void Control::run(){

    // Loading the configuration file from the command line.
    QStringList arguments = QCoreApplication::arguments();
    if (arguments.size() != 2){
        // The only argument should be the full path of the working directory
        log.appendError("Wrong number of arguments: " + QString::number(arguments.size()) + ". The only argument should be the working directory");
        exitProgram(EYEDBMNG_ANS_PARAM_ERROR);
        return;
    }

    workingDirectory = arguments.last();

    if (!QDir(workingDirectory).exists()){
        log.appendError("Working directory " + workingDirectory +  " does not exist");
        exitProgram(EYEDBMNG_ANS_FILE_ERROR);
        return;
    }

    // Loading the input file
    commFile = workingDirectory + "/" + QString(FILE_DBMNG_COMM_FILE);
    if (!configuration.loadConfiguration(commFile,COMMON_TEXT_CODEC)){
        log.appendError("Could not load the communications file: " + configuration.getError());
        exitProgram(EYEDBMNG_ANS_FILE_ERROR);
        return;
    }

    QString log_file_path = QString(DIRNAME_SERVER_LOGS) + "/" + configuration.getString(CONFIG_TRANSACTION_ID);
    //qDebug() << "Log PATH" << log_file_path;
    log.setLogFileLocation(log_file_path);

    QString tID  = "DBMNG_" + configuration.getString(CONFIG_TRANSACTION_ID);
    if (tID.isEmpty()){
        log.appendWarning("No transaction ID found in comm file");
    }
    else log.setID(tID);

    // Loading the eye rep gen conf.
    ConfigurationManager eyeRepGenConf;
    if (!eyeRepGenConf.loadConfiguration(workingDirectory + "/" + QString(FILE_EYE_REP_GEN_CONFIGURATION),COMMON_TEXT_CODEC)){
        log.appendError("Could not load the eye gen conf file: " + eyeRepGenConf.getError());
        exitProgram(EYEDBMNG_ANS_FILE_ERROR);
        return;
    }

    // Joining the data.
    configuration.merge(eyeRepGenConf);

    // Getting the local configurations.
    bool shouldExit = false;
    ConfigurationManager dbconfigs = LoadLocalConfiguration(&log,&shouldExit);
    if (shouldExit){        
        exitProgram(EYEDBMNG_ANS_PARAM_ERROR);        
        return;
    }

    // Joining the two configuration files
    configuration.merge(dbconfigs);

    if (configuration.getBool(CONFIG_PRODUCTION_FLAG)){
        log.appendStandard("Configured for Production");
    }
    else{
        log.appendStandard("Configured for Local Host");
    }


    // Attempting to create/open the log file
    QString logfile = FILE_DB_LOG;
    QFile fileforlog(logfile);
    if (!fileforlog.exists()){
        if (!fileforlog.open(QFile::WriteOnly)){
            log.appendError("Could not create the log file at: " + logfile);
            std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
            exitProgram(EYEDBMNG_ANS_FILE_ERROR);
            return;
        }
        QTextStream writer(&fileforlog);
        writer << "";
        fileforlog.close();
    }
    else {
        if (!fileforlog.open(QFile::Append)){
            log.appendError("Could not open for appending the log file at: " + logfile);
            std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
            exitProgram(EYEDBMNG_ANS_FILE_ERROR);
            return;
        }
    }

    // Initializing the database connections.
    QString user = configuration.getString(CONFIG_DBUSER);
    QString passwd = configuration.getString(CONFIG_DBPASSWORD);

    QString host = configuration.getString(CONFIG_DATA_DBHOST);
    QString dbname = configuration.getString(CONFIG_DATA_DBNAME);
    quint16 port = configuration.getInt(CONFIG_DATA_DBPORT);
    dbConnBase.setupDB(DB_NAME_BASE,host,dbname,user,passwd,port,logfile);
    //qWarning() << "Connection information: " + user + "@" + host + " with passwd " + passwd + ", port: " + QString::number(port) + " to db: " + dbname;

    host = configuration.getString(CONFIG_ID_DBHOST);
    dbname = configuration.getString(CONFIG_ID_DBNAME);
    port = configuration.getInt(CONFIG_ID_DBPORT);
    dbConnID.setupDB(DB_NAME_ID,host,dbname,user,passwd,port,logfile);
    //qWarning() << "Connection information: " + user + "@" + host + " with passwd " + passwd + ", port: " + QString::number(port) + " to db: " + dbname;

    host = configuration.getString(CONFIG_PATDATA_DBHOST);
    dbname = configuration.getString(CONFIG_PATDATA_DBNAME);
    port = configuration.getInt(CONFIG_PATDATA_DBPORT);
    dbConnPatData.setupDB(DB_NAME_PATDATA,host,dbname,user,passwd,port,logfile);
    //qWarning() << "Connection information: " + user + "@" + host + " with passwd " + passwd + ", port: " + QString::number(port) + " to db: " + dbname;

    host = configuration.getString(CONFIG_DASH_DBHOST);
    dbname = configuration.getString(CONFIG_DASH_DBNAME);
    port = configuration.getInt(CONFIG_DASH_DBPORT);
    dbConnDash.setupDB(DB_NAME_DASHBOARD,host,dbname,user,passwd,port,logfile);

    log.appendStandard("Testing DB Connections...");
    if (!dbConnBase.open()){
        log.appendError("FAIL TEST CONNECTION DB BASE: " + dbConnBase.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        exitProgram(EYEDBMNG_ANS_DB_ERROR);
        return;
    }
    if (!dbConnBase.hasTransactions()){
        log.appendError("DB BASE does not support Transactions!");
        exitProgram(EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    if (!dbConnID.open()){
        log.appendError("FAIL TEST CONNECTION DB ID: " + dbConnID.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        exitProgram(EYEDBMNG_ANS_DB_ERROR);
        return;
    }
    if (!dbConnID.hasTransactions()){
        log.appendError("DB Pat ID does not support Transactions!");
        exitProgram(EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    if (!dbConnPatData.open()){
        log.appendError("FAIL TEST CONNECTION DB PATDATA: " + dbConnPatData.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        exitProgram(EYEDBMNG_ANS_DB_ERROR);
        return;
    }
    if (!dbConnPatData.hasTransactions()){
        log.appendError("DB Pat Data does not support Transactions!");
        exitProgram(EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    if (!dbConnDash.open()){
        log.appendError("FAIL TEST CONNECTION DB DASHBOARD: " + dbConnPatData.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        exitProgram(EYEDBMNG_ANS_DB_ERROR);
        return;
    }
    if (!dbConnDash.hasTransactions()){
        log.appendError("DB Dashboard does not support Transactions!");
        exitProgram(EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    if (configuration.getString(CONFIG_DBMNG_ACTION) == CONFIG_P_DMBNG_ACTION_STORE){
        log.appendSuccess("DBMNG in Store Mode");
        storeMode(CONFIG_P_DMBNG_ACTION_STORE);
    }
    else if (configuration.getString(CONFIG_DBMNG_ACTION) == CONFIG_P_DMBNG_ACTION_CHECK){
        log.appendSuccess("DBMNG in Check Mode");
        checkMode();
    }
    else if (configuration.getString(CONFIG_DBMNG_ACTION) == CONFIG_P_DBMNG_ACTION_PATDATA){
        log.appendSuccess("DBMNG in Patient Data Store Mode");
        storeMode(CONFIG_P_DBMNG_ACTION_PATDATA); // The first part of the process is identical. So storeMode is called.
    }

    else {
        log.appendError("Unrecognized DBMng Action: " + configuration.getString(CONFIG_DBMNG_ACTION));
        exitProgram(EYEDBMNG_ANS_PARAM_ERROR);
        return;
    }

}

/////////////////////////////////////////// CHECK MODE ///////////////////////////////////////////////////////////

void Control::checkMode(){

    QString etserial      = configuration.getString(CONFIG_INST_ETSERIAL);
    QString instID        = configuration.getString(CONFIG_INST_UID);
    bool reprocessRequest = configuration.getBool(CONFIG_REPROCESS_REQUEST);
    bool demoMode         = configuration.getBool(CONFIG_DEMO_MODE);

    QStringList columns;
    QString condition;
    DBData data;

    if (!dbConnDash.startTransaction()){
        log.appendError("Could not start DB Transaction for Dash DB");
        finishUp(DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    QString serial_check_query = "SELECT a.product_sn AS name\
            FROM tProduction a\
            INNER JOIN (\
                SELECT product_sn, MAX(keyid) keyid\
                FROM tProduction\
                GROUP BY product_sn\
                ) b ON a.product_sn = b.product_sn AND a.keyid = b.keyid AND a.linked_institution = ";

    serial_check_query = serial_check_query + instID;
    serial_check_query = serial_check_query + " AND a.product_sn = '" + etserial + "'";
    columns << "product_sn";

    if (!dbConnDash.specialQuery(serial_check_query,columns)){
        log.appendError("When querying serial number for institituion: " + dbConnDash.getError());
        finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
        return;

    }

    data = dbConnDash.getLastResult();

    if (data.rows.size() != 1){
        log.appendError("ProductSN |" + etserial + "| does not correspond to the serial registered for insitituion with UID " + instID);
        log.appendError("Numer of rows: " + QString::number(data.rows.size()));
        log.appendError("Query is: " + serial_check_query);
        finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_WRONG_SERIAL);
        return;
    }

    if (data.rows.first().size() != 1){
        log.appendError("ProductSN |" + etserial + "| does not correspond to the serial registered for insitituion with UID " + instID);
        log.appendError("Numer of columns: " + QString::number(data.rows.first().size()));
        log.appendError("Query is: " + serial_check_query);
        finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_WRONG_SERIAL);
        return;
    }

    if (data.rows.first().first() != etserial){
        log.appendError("ProductSN |" + etserial + "| does not correspond to the serial registered for insitituion with UID " + instID);
        finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_WRONG_SERIAL);
        return;
    }

//    if (!dbConnBase.startTransaction()){
//        log.appendError("Could not start DB Transaction for Base DB");
//        finishUp(DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
//        return;
//    }

    // Checking the number of evaluations
    columns.clear();
    columns << TEVALS_COL_EVALUATIONS;
    condition = QString(TEVALS_COL_INSTITUTION) + " = '" + instID + "'";
    if (!dbConnDash.readFromDB(TABLE_EVALUATIONS,columns,condition)){
        log.appendError("When querying institution and number of evaluations: " + dbConnDash.getError());
        finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    data = dbConnDash.getLastResult();

    if (data.rows.size() != 1){
        log.appendError("When querying the institution and number of evaluations: Number of returned rows was " + QString::number(data.rows.size()) + " instead of 1. UID: " + instID);
        finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    // Checking the number of evaluations
    if (data.rows.first().size() != 1){
        log.appendError("When querying the institution and number of evaluations: Number of returned columns was " + QString::number(data.rows.first().size()) + " instead of 1. INST UID: " + instID);
        finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    numberOfRemaingEvaluations = data.rows.first().last().toInt();

    if (!demoMode && !reprocessRequest){

        if (numberOfRemaingEvaluations == 0 ){
            log.appendError("No evaluations remaining forinsitituion with UID " + instID);
            finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_NOEVALS);
            return;
        }
    }

    finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_OK);

}

/////////////////////////////////////////// STORE MODE ///////////////////////////////////////////////////////////

void Control::storeMode(const QString &action){

    QElapsedTimer timer;
    timer.start();

    QString instID        = configuration.getString(CONFIG_INST_UID);
    bool reprocessRequest = configuration.getBool(CONFIG_REPROCESS_REQUEST);
    bool demoMode         = configuration.getBool(CONFIG_DEMO_MODE);

    QStringList columns;
    QString condition;
    QStringList temp;
    QStringList values;
    DBData data;


    // Loding the patient and doctor configuration.
    ConfigurationManager patdata;
    ConfigurationManager docdata;

    if (!patdata.loadConfiguration(workingDirectory + "/" + QString(FILE_PATDATA_DB),COMMON_TEXT_CODEC)){
        log.appendError("Could not load pat data file: " + patdata.getError());
        finishUp(DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_FILE_ERROR);
        return;
    }

    if (!docdata.loadConfiguration(workingDirectory + "/" + QString(FILE_DOCDATA_DB),COMMON_TEXT_CODEC)){
        log.appendError("Could not load doc data file: " + docdata.getError());
        finishUp(DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_FILE_ERROR);
        return;
    }

    QString pat_hashed_id = patdata.getString(TPATDATA_COL_PUID);
    QString puid;
    QString druid         = docdata.getString(TDOCTOR_COL_UID);

    // ----------------------------- Saving the data to S3 first -------------------------------------
    QString eyerepgenconf = workingDirectory + "/" + QString(FILE_EYE_REP_GEN_CONFIGURATION);
    QStringList filters;

    if ((!demoMode) && (action == CONFIG_P_DMBNG_ACTION_STORE)) {
        QStringList filesToSave;
        filters << "*.dat" << "*.datf";
        filesToSave = QDir(workingDirectory).entryList(filters,QDir::Files);

        // Data is saved ONLY if this is not a reprocess request.
        if (!reprocessRequest){
            if (filesToSave.isEmpty()){
                log.appendError("Could not load any dat or datf files to send to S3: ");
                finishUp(DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_FILE_ERROR);
                return;
            }

            filesToSave << QString(FILE_EYE_REP_GEN_CONFIGURATION);
            for (qint32 i = 0; i < filesToSave.size(); i++){
                QString cmd = S3_BASE_COMMAND;
                cmd = cmd + " " + workingDirectory + "/" + filesToSave.at(i) + " ";
                cmd = cmd + "s3://" + configuration.getString(CONFIG_S3_ADDRESS) + "/" + pat_hashed_id + "/" + configuration.getString(CONFIG_TIMESTAMP) + "/" + filesToSave.at(i) + " ";
                cmd = cmd + S3_PARMETERS;

                log.appendStandard("Running S3 Command: " + cmd);

                QProcess process;
                process.start("sh",QStringList() << "-c" << cmd);
                process.closeReadChannel(QProcess::StandardOutput);
                process.closeReadChannel(QProcess::StandardError);
                process.waitForFinished(60000000);
                qint32 result = process.exitCode();

                if (result != 0){
                   log.appendError("AWS S3 Copy Failed. Exited with code: " + QString::number(result));
                   finishUp(DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_FILE_ERROR);
                   return;
                }

            }
        }
    }
    else log.appendStandard("DEMO Transaction not saving information");

    // ----------------------------- Checking or adding the doctor data -----------------------------
    dbConnBase.startTransaction();
    columns.clear();
    columns = docdata.getAllKeys();
    for (qint32 i = 0; i < columns.size(); i++){
        values << docdata.getString(columns.at(i));
    }

    bool addDrInfo = false;
    condition = QString(TDOCTOR_COL_UID) + " = '" + docdata.getString(TDOCTOR_COL_UID) + "' ORDER BY " + QString(TDOCTOR_COL_KEYID) + " DESC";

    if (!dbConnBase.readFromDB(TABLE_DOCTORS,columns,condition)){
        log.appendError("When querying the doctor information: " + dbConnBase.getError());
        finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    data = dbConnBase.getLastResult();
    if (data.rows.isEmpty()) addDrInfo = true;
    else{
        // Using only the lastest dr info which should be the first.
        if (data.rows.first().size() != columns.size()){
            log.appendError("When getting doctor information " + QString::number(columns.size()) + " was requested. But got: " + QString::number(data.rows.first().size()));
            finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }
        for (qint32 i = 0; i < columns.size(); i++){
            if (docdata.getString(columns.at(i)) != data.rows.first().at(i)) {
                log.appendStandard("Doctor information updated: " + columns.at(i));
                addDrInfo = true;
                break;
            }
        }
    }

    if (addDrInfo){
        // Adding the doctor information, as it was either modified or not present in the DB.
        log.appendStandard("Adding information for doctor: " + druid);
        if (!dbConnBase.insertDB(TABLE_DOCTORS,columns,values,instID)){
            log.appendError("When adding doctor information: " + dbConnBase.getError());
            finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }
    }

    // ----------------------------- Checking if the patient exists. -----------------------------
    dbConnID.startTransaction();
    columns.clear();
    columns << TPATID_COL_KEYID;
    condition = QString(TPATID_COL_UID) + " = '" + pat_hashed_id + "'";
    if (!dbConnID.readFromDB(TABLE_PATIENTD_IDS,columns,condition)){
        log.appendError("When obtaining patient PUID: " + dbConnID.getError());
        finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    data = dbConnID.getLastResult();
    bool isNew = false;

    if (data.rows.size() == 0){
        // This a new patient and it needs to be added to the DB.
        log.appendStandard("Creating puid for patient: " + pat_hashed_id);
        isNew = true;
        columns.clear();
        values.clear();
        columns << TPATID_COL_UID;
        values << pat_hashed_id ;
        if (!dbConnID.insertDB(TABLE_PATIENTD_IDS,columns,values,druid)){
            log.appendError("When adding patient ID information: " + dbConnID.getError());
            finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }

        // Once added the keyid is obtained.
        columns.clear();
        columns << TPATID_COL_KEYID;
        condition = QString(TPATID_COL_UID) + "='" + pat_hashed_id + "'";

        if (!dbConnID.readFromDB(TABLE_PATIENTD_IDS,columns,condition)){
            log.appendError("When inserted a new hashed patient id: " + dbConnID.getError());
            finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }

        data = dbConnID.getLastResult();

        bool problem = false;
        if (data.rows.size() == 1){
            if (data.rows.first().size() == 1){
                puid  = data.rows.first().first();
            }
            else problem = true;
        }
        else problem = true;

        // If the code got here there was a problem with the query.
        if (problem){
            log.appendError("When obtaining newly created DB puid, unexpected dimensions for the query");
            finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }
    }
    else{
        if (data.rows.first().size() > 0){
            puid = data.rows.first().first();
        }
        else {
            log.appendError("When obtaining existing DB puid, Rows were returned but with not columns");
            finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }
    }

    // ----------------------------- Checking or adding patient data -----------------------------
    // By this point a PUID exists. So an check (or direct insertion is necesssary).
    dbConnPatData.startTransaction();

    columns.clear();
    values.clear();
    temp.clear();
    columns = patdata.getAllKeys();

    // Replacing the hashed pid with the database puid.
    patdata.addKeyValuePair(TPATDATA_COL_PUID,puid);
    for (qint32 i = 0; i < columns.size(); i++){
        values << patdata.getString(columns.at(i));
    }

    bool addPatData = false;
    if (!isNew){

        condition = QString(TPATDATA_COL_PUID) + " = '" + puid + "' ORDER BY " + QString(TPATDATA_COL_KEYID) + " DESC";

        if (!dbConnPatData.readFromDB(TABLE_PATDATA,columns,condition)){
            log.appendError("When querying the patient information: " + dbConnPatData.getError());
            finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }
        data = dbConnPatData.getLastResult();
        if (data.rows.isEmpty()) addPatData = true;
        else{

            if (data.rows.first().size() != columns.size()){
                log.appendError("When getting patient information " + QString::number(columns.size()) + " was requested. But got: " + QString::number(data.rows.first().size()));
                finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
                return;
            }

            for (qint32 i = 0; i < columns.size(); i++){
                if (columns.at(i) == TPATDATA_COL_DATE_INS) continue; // There is no point in comparing the insertion date.
                if (patdata.getString(columns.at(i)) != data.rows.first().at(i)) {
                    log.appendStandard("Patient information updated: " + columns.at(i) + " from " + data.rows.first().at(i) + " to " + patdata.getString(columns.at(i)));
                    addPatData = true;
                    break;
                }
            }
        }
    }

    if (isNew || addPatData){
        // Adding the patient information, as it was either modified or not present in the DB.
        log.appendStandard("Adding information for patient: " + pat_hashed_id);
        if (!dbConnPatData.insertDB(TABLE_PATDATA,columns,values,"DR: " + druid  + ", pat: " + pat_hashed_id)){
            log.appendError("When adding patient information: " + dbConnPatData.getError());
            finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }
    }

    // In DEMO mode there is no need to do anything else.
    if (demoMode){
        finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    // -------------- If this is a patient medical record store call, here the procedure changes. --------------------
    if (action == CONFIG_P_DBMNG_ACTION_PATDATA){
        patDataStoreMode(puid,instID);
        return;
    }

    // ----------------------------- Saving the data to the Database -----------------------------
    // All good so far. Loding files to save.
    ConfigurationManager eyeresultData;
    if (!eyeresultData.loadConfiguration(workingDirectory + "/" + QString(FILE_DBDATA_FILE),COMMON_TEXT_CODEC)){
        log.appendError("Loading DB Result data file: " + dbConnBase.getError());
        finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_FILE_ERROR);
        return;
    }

    columns.clear();
    values.clear();
    columns = eyeresultData.getAllKeys();
    for (qint32 i = 0; i < columns.size(); i++){
        values << eyeresultData.getString(columns.at(i));
    }

    columns << TEYERES_COL_STUDY_DATE << TEYERES_COL_PUID << TEYERES_COL_DOCTORID;
    values << "TIMESTAMP(NOW())" << puid  << druid;

    QString logid = druid + " for " + pat_hashed_id;

    if (!dbConnBase.insertDB(TABLE_EYE_RESULTS,columns,values,logid)){
        log.appendError("Loading DB Result data file: " + dbConnBase.getError());
        finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    qint32 resultKeyID = dbConnBase.getNewestKeyid(TEYERES_COL_KEYID,TABLE_EYE_RESULTS);
    if (resultKeyID == -1){
        log.appendError("Getting the keyid of the result that was recently inserted: " + dbConnBase.getError());
        finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
        return;
    }

    // ----------------------------- Saving the frequency related data -----------------------------
    // Getting all fdb files.
    filters.clear(); filters << "*.fdb";
    QStringList FDBFiles = QDir(workingDirectory).entryList(filters,QDir::Files);
    if (FDBFiles.isEmpty()){
        log.appendError("Could not load any FDB Files from directory: " + workingDirectory);
        finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_FILE_ERROR);
        return;
    }

    ConfigurationManager toDB;
    for (qint32 i = 0; i < FDBFiles.size(); i++){
        toDB.clear();
        if (!toDB.loadConfiguration(workingDirectory + "/" + FDBFiles.at(i),COMMON_TEXT_CODEC)){
            log.appendError("Could not load FDB File: " + toDB.getError());
            finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_FILE_ERROR);
            return;
        }
        values.clear();
        columns.clear();
        columns = toDB.getAllKeys();
        for (qint32 i = 0; i < columns.size(); i++){
            values << toDB.getString(columns.at(i));
        }
        columns << TFDATA_COL_TEYERESULT;
        values << QString::number(resultKeyID);
        if (!dbConnBase.insertDB(TABLE_FDATA,columns,values,logid)){
            log.appendError("Inserting FDB File data: " + dbConnBase.getError());
            finishUp(DB_FINISH_ACTION_ROLLBACK,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }
    }

    // ----------------------------- Saving the flag y the eyerepconf if all is good. And decreasing the number of evaluations available. -----------------------------

    if (!demoMode){
        QString err = ConfigurationManager::setValue(eyerepgenconf,COMMON_TEXT_CODEC,CONFIG_DATA_SAVED_TO_DB_FLAG,"true");
        if (err.isEmpty()){
            QString cmd = S3_BASE_COMMAND;
            cmd = cmd + " " + eyerepgenconf + " ";
            cmd = cmd + " s3://" + configuration.getString(CONFIG_S3_ADDRESS) + "/" + pat_hashed_id + "/" + configuration.getString(CONFIG_TIMESTAMP) + "/" + QString(FILE_EYE_REP_GEN_CONFIGURATION);
            cmd = cmd + " " + S3_PARMETERS;
            QProcess::execute(cmd,QStringList());
            log.appendStandard("Running S3 Command for flag update: " + cmd);
        }
        else{
            log.appendError("Could not update eye rep gen conf with saved to DB flag because: " + err);
        }
    }

    if (!reprocessRequest && !demoMode){

        // Starting the transaction.
        dbConnDash.startTransaction();

        columns.clear();
        columns << TEVALS_COL_EVALUATIONS;
        QString condition = QString(TEVALS_COL_INSTITUTION) + " = '" + instID + "'";
        if (!dbConnDash.readFromDB(TABLE_EVALUATIONS,columns,condition)){
            finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }

        DBData data = dbConnDash.getLastResult();

        if (data.rows.size() != 1){
            log.appendError("Decreasing report count: Number of returned rows was " + QString::number(data.rows.size()) + " instead of 1, for UID: " + instID );
            return;
        }
        qint32  numevals = data.rows.first().first().toInt();

        if (numevals == -1) {
            log.appendStandard("Institution " + instID + " has infinite number of evaluations");
        }
        else{

            // Checking the number of evaluations
            if ((numevals <= 0)){
                log.appendError("Asked to decrease number of evaluations for " + instID + " when it is " + QString(numevals));
                finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
                return;
            }

            // Decreasing and saving.
            numevals--;
            values.clear();
            values << QString::number(numevals);

            log.appendStandard("Decreased number of evaluations of instituition " + instID + " to " + QString::number(numevals));

            if (!dbConnDash.updateDB(TABLE_EVALUATIONS,columns,values,condition,logid)){
                log.appendError("When saving new number of evaluations " + instID + " when it is " + QString(numevals) + ": " + dbConnDash.getError());
                finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_ROLLBACK,EYEDBMNG_ANS_DB_ERROR);
                return;
            }

        }
    }
    else log.appendStandard("This is either reprocessing or demo mode. Number of evaluations is not decreased");

    quint64 timeSpent = timer.elapsed();

    // Adding the performance data.
    columns.clear();
    values.clear();
    QStringList perfTimes = configuration.getStringList(CONFIG_TIMESTRING);
    columns << TPERF_COL_TIMESTRING << TPERF_COL_EYE_RESULT;
    values << perfTimes.join("|") + "|STO:" + QString::number(timeSpent) << QString::number(resultKeyID);
    if (!dbConnBase.insertDB(TABLE_PERFORMANCE,columns,values,logid)){
        log.appendError("Could not add performance data " + dbConnBase.getError());
    }

    // Checking if mail file exists
    QFile mailBody(workingDirectory + "/" + FILE_MAIL_ERROR_LOG);
    if (mailBody.exists()){
        // Mail needs to be sent.

        log.appendStandard("Sending frequency error mail");

        columns.clear();
        columns << TINST_COL_NAME;
        QString condition = QString(TINST_COL_UID) + " = '" + instID + "'";
        QString instName = instID;
        if (!dbConnDash.readFromDB(TABLE_INSTITUTION,columns,condition)){
            log.appendError("Getting inst name for " + instName + ": " + dbConnDash.getError());
        }
        else{
            DBData data = dbConnDash.getLastResult();
            if (data.rows.size() != 1){
                log.appendError("Getting inst name for email, rows size " + QString::number(data.rows.size()) + " instead of 1, for UID: " + instID );
            }
            else {
                if (data.rows.first().size() < 1){
                    log.appendError("Getting inst name for email, col size is zero for UID: " + instID );
                }
                else instName = data.rows.first().first();
            }
        }

        if (!mailBody.open(QFile::ReadOnly)){
            log.appendError("Could not open mail body file: " + mailBody.fileName());
            return;
        }
        QTextStream reader(&mailBody);
        reader.setCodec(COMMON_TEXT_CODEC);
        QString body = reader.readAll();
        mailBody.close();

        // Path of the output PHP file
        QString outPHPFilename = workingDirectory + "/" + QString(FILE_PHP_MAIL);
        QString graphFile = workingDirectory + "/" + QString(FILE_GRAPHS_FILE);
        QFile phpFile(outPHPFilename);
        if (!phpFile.open(QFile::WriteOnly)){
            log.appendError("Could not open php mail file for writing " + outPHPFilename);
            return;
        }
        QTextStream writer(&phpFile);
        writer.setCodec(COMMON_TEXT_CODEC);

        writer << "<?php\n";
        if (configuration.getBool(CONFIG_PRODUCTION_FLAG)){
            writer << "require '/home/ec2-user/composer/vendor/autoload.php';\n";
            writer << "require '/home/ec2-user/composer/vendor/phpmailer/phpmailer/PHPMailerAutoload.php';\n";
        }
        else{
            writer << "use PHPMailer\\PHPMailer\\PHPMailer;\n";
            writer << "require '/home/ariela/repos/viewmind_projects/Scripts/php/vendor/autoload.php';\n";
        }

        writer << "$mail = new PHPMailer;\n";
        writer << "$mail->isSMTP();\n";
        writer << "$mail->setFrom('check.viewmind@gmail.com', 'ViewMind Administration');\n";
        writer << "$mail->Username = 'AKIARDLQA5AHRSTLPCYS';\n";
        writer << "$mail->Password = 'BHGWozyNwZoHjvUAhL8d7H9FC/H4RBNfh564MnKZRKj/';\n";
        writer << "$mail->Host = 'email-smtp.us-east-1.amazonaws.com';\n";
        writer << "$mail->Subject = 'ViewMind Frequency Check Alert From: " + instName + "';\n";
        writer << "$mail->addAddress('ariel.arelovich@viewmind.com.ar', 'Ariel Arelovich');\n";

        if (configuration.getBool(CONFIG_PRODUCTION_FLAG)){
            writer << "$mail->addAddress('matias.shulz@viewmind.com.ar', 'Matias Shulz');\n";
            writer << "$mail->addAddress('gerardofernandez480@gmail.com ', 'Gerardo Fernandez');\n";
        }

        // The HTML-formatted body of the email
        writer << "$mail->Body = '<h3>Frequency problems detected from Institituion: " + instName + "</h3>\n<h3>Details</h3>" + body + "';\n";
        writer << "$mail->addAttachment('" + graphFile  + "');\n";
        writer << "$mail->SMTPAuth = true;\n";
        writer << "$mail->SMTPSecure = 'tls';\n";
        writer << "$mail->Port = 587;\n";
        writer << "$mail->isHTML(true);\n";
        writer << "if(!$mail->send()) {\n";
        writer << "    echo 'Email error is: ' , $mail->ErrorInfo , PHP_EOL;\n";
        writer << "}\n";
        writer << "?>\n";

        // Actually sending the email.
        phpFile.close();
        QProcess p;
        p.setProgram("php");
        QStringList args; args << outPHPFilename;
        p.setArguments(args);
        p.start();
        p.waitForFinished();
        QString output(p.readAllStandardOutput());
        if (!output.trimmed().isEmpty()){
            log.appendError("EMAIL: " + output.trimmed());
        }
    }

    finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,EYEDBMNG_ANS_OK);
}


/////////////////////////////////////////// PATDATA STORE MODE ///////////////////////////////////////////////////
void Control::patDataStoreMode(const QString &puid, const QString &instUID){

    log.appendStandard("Storing medical records...");

    ConfigurationManager medRecords;

    if (!medRecords.loadConfiguration(workingDirectory + "/" + QString(FILE_MEDRECORD_DB),COMMON_TEXT_CODEC)){
        log.appendError("Could not load medical records db file file: " + medRecords.getError());
        finishUp(DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_FILE_ERROR);
        return;
    }

    QStringList columns;
    QList<QStringList> values;

    QStringList allkeys = medRecords.getAllKeys();

    // Verifying consistency in the number of records.
    qint32 numberOrRecords = -1;
    for (qint32 i = 0; i < allkeys.size(); i++){
        qint32 n = medRecords.getStringList(allkeys.at(i)).size();
        if (numberOrRecords == -1) numberOrRecords = n;
        else{
            if (numberOrRecords != n){
                log.appendError("Medical record number of records mismatch. Column : " + allkeys.at(i) + " has " +  QString::number(n) + " but previous columns had: " + QString::number(numberOrRecords));
                finishUp(DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_FILE_ERROR);
                return;
            }
        }
    }

    // Initializing the value lists
    for (qint32 j = 0; j < numberOrRecords; j++){
        values << QStringList();
    }

    // Adding the differet records.
    for (qint32 i = 0; i < allkeys.size(); i++){
        columns << allkeys.at(i);
        QStringList allvalues = medRecords.getStringList(allkeys.at(i));
        for (qint32 j = 0; j < allvalues.size(); j++){
            values[j].append("'" + allvalues.at(j) + "'");
        }
    }

    // Adding the patient ID to all of them.
    columns << TPATMEDREC_COL_PUID;
    for (qint32 j = 0; j < numberOrRecords; j++){
        values[j].append("'" + puid + "'");
    }

    // Adding the records to the database as they would not have been sent without them being changed or new.
    // The transaction was already started in the function that called this one, so it is not necessary to call it again.
    for (qint32 j = 0; j < numberOrRecords; j++){

        log.appendStandard("Adding medical record " + QString::number(j+1) + " of " + QString::number(numberOrRecords));
        if (!dbConnPatData.insertDB(TABLE_MEDICAL_RECORDS,columns,values.at(j),instUID,true)){
            log.appendError("When adding medical record information: " + dbConnPatData.getError());
            finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_DB_ERROR);
            return;
        }
    }

    // If there were not problems we need to finish everything right now.
    finishUp(DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_COMMIT,DB_FINISH_ACTION_CLOSE,EYEDBMNG_ANS_OK);

}



/////////////////////////////////////////// CLEAN UP ///////////////////////////////////////////////////
void Control::finishUp(quint8 commitBase,
                       quint8 commitID,
                       quint8 commitPatData,
                       quint8 commitDash,
                       qint32 code){
    if (commitBase == DB_FINISH_ACTION_COMMIT) {
        if (!dbConnBase.commit()) log.appendWarning("FAILED Commit for DB base transaction");
    }
    else if (commitBase == DB_FINISH_ACTION_ROLLBACK) {
        if (!dbConnBase.doRollBack()) log.appendWarning("FAILED Rollback for DB base transaction");
    }

    if (commitID == DB_FINISH_ACTION_COMMIT) {
        if (!dbConnID.commit()) log.appendWarning("FAILED Commit for DB id transaction");
    }
    else if (commitID == DB_FINISH_ACTION_ROLLBACK) {
        if (!dbConnID.doRollBack()) log.appendWarning("FAILED Rollback for DB id transaction");
    }

    if (commitPatData == DB_FINISH_ACTION_COMMIT){
        if (!dbConnPatData.commit()) log.appendWarning("FAILED Commit for DB pat data transaction");
    }
    else if (commitPatData == DB_FINISH_ACTION_ROLLBACK) {
        if (!dbConnID.doRollBack()) log.appendWarning("FAILED Rollback for DB pat data transaction");
    }

    if (commitDash == DB_FINISH_ACTION_COMMIT){
        if (!dbConnDash.commit()) log.appendWarning("FAILED Commit for DB dash transaction");
    }
    else if (commitDash == DB_FINISH_ACTION_ROLLBACK) {
        if (!dbConnDash.doRollBack()) log.appendWarning("FAILED Rollback for DB dash transaction");
    }

    dbConnBase.close();
    dbConnID.close();
    dbConnPatData.close();
    dbConnDash.close();

    exitProgram(code);
}

void Control::exitProgram(qint32 code){
    ConfigurationManager::setValue(commFile,COMMON_TEXT_CODEC,CONFIG_DBMNG_RESULT,QString::number(code));
    ConfigurationManager::setValue(commFile,COMMON_TEXT_CODEC,CONFIG_REMAINING_EVALUATIONS,QString::number(numberOfRemaingEvaluations));
    emit(done());
}
