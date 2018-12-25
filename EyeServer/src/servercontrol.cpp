#include "servercontrol.h"

ServerControl::ServerControl(QObject *parent) : QObject(parent)
{

    // Creating the configuration verifier
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cv[CONFIG_RAW_DATA_REPO] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_NUMBER_OF_PARALLEL_PROCESSES] = cmd;

    cmd.clear();
    cv[CONFIG_EYEPROCESSOR_PATH] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_DATA_REQUEST_TIMEOUT] = cmd;

    // DB configuration is all strings.
    cmd.clear();
    cv[CONFIG_DBHOST] = cmd;
    cv[CONFIG_DBNAME] = cmd;
    cv[CONFIG_DBPASSWORD] = cmd;
    cv[CONFIG_DBUSER] = cmd;

    cv[CONFIG_ID_DBHOST] = cmd;
    cv[CONFIG_ID_DBNAME] = cmd;
    cv[CONFIG_ID_DBPASSWORD] = cmd;
    cv[CONFIG_ID_DBUSER] = cmd;

    cv[CONFIG_PATDATA_DBHOST] = cmd;
    cv[CONFIG_PATDATA_DBNAME] = cmd;
    cv[CONFIG_PATDATA_DBPASSWORD] = cmd;
    cv[CONFIG_PATDATA_DBUSER] = cmd;


    cv[CONFIG_S3_ADDRESS] = cmd;

    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_DBPORT] = cmd;
    cv[CONFIG_ID_DBPORT] = cmd;
    cv[CONFIG_PATDATA_DBPORT] = cmd;

    config.setupVerification(cv);

    // Initializing the database connections.
    QString host = config.getString(CONFIG_DBHOST);
    QString dbname = config.getString(CONFIG_DBNAME);
    QString user = config.getString(CONFIG_DBUSER);
    QString passwd = config.getString(CONFIG_DBPASSWORD);
    quint16 port = config.getInt(CONFIG_DBPORT);
    dbConnBase.setupDB(DB_NAME_BASE,host,dbname,user,passwd,port);

    host = config.getString(CONFIG_ID_DBHOST);
    dbname = config.getString(CONFIG_ID_DBNAME);
    user = config.getString(CONFIG_ID_DBUSER);
    passwd = config.getString(CONFIG_ID_DBPASSWORD);
    port = config.getInt(CONFIG_ID_DBPORT);
    dbConnID.setupDB(DB_NAME_ID,host,dbname,user,passwd,port);

    host = config.getString(CONFIG_PATDATA_DBHOST);
    dbname = config.getString(CONFIG_PATDATA_DBNAME);
    user = config.getString(CONFIG_PATDATA_DBUSER);
    passwd = config.getString(CONFIG_PATDATA_DBPASSWORD);
    port = config.getInt(CONFIG_PATDATA_DBPORT);
    dbConnPatData.setupDB(DB_NAME_PATDATA,host,dbname,user,passwd,port);

    dbSSLServer.setDBConnections(&dbConnBase,&dbConnID,&dbConnPatData);
    dataProcessingSSLServer.setDBConnections(&dbConnBase,&dbConnID,&dbConnPatData);

}

void ServerControl::startServer(){

    if (!config.loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        log.appendError("Configuration file errors:<br>"+config.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    if (!QSslSocket::supportsSsl()){
        log.appendError("No support for SSL found. Cannot continue");
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    log.appendStandard("Starting eye processing server...");
    dataProcessingSSLServer.startServer(&config);

    log.appendStandard("Starting sql server...");
    dbSSLServer.startServer(&config);
}
