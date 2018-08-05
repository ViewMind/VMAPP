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

    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_DBPORT] = cmd;

    config.setupVerification(cv);

    dataProcessingSSLServer.setDBConnection(dbSSLServer.getDBInterface());

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
