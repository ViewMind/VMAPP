#include "servercontrol.h"

ServerControl::ServerControl(QObject *parent) : QObject(parent)
{

    // Creating the configuration verifier
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_DATA_REQUEST_TIMEOUT] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_STRING;
    cv[CONFIG_ETDIR_PATH] = cmd;

    config.setupVerification(cv);

}

void ServerControl::startServer(){

    if (!QSslSocket::supportsSsl()){
        log.appendError("No support for SSL found. Cannot continue");
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    if (!config.loadConfiguration(CONFIG_FILE,COMMON_TEXT_CODEC)){
        log.appendError("Configuration file errors:<br>"+config.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    log.appendStandard("Starting update server...");
    updateServer.startServer(&config);
}
