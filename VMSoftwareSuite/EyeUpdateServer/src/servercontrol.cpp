#include "servercontrol.h"

ServerControl::ServerControl(QObject *parent) : QObject(parent)
{


}

void ServerControl::startServer(){

    if (!QSslSocket::supportsSsl()){
        log.appendError("No support for SSL found. Cannot continue");
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    bool shouldExit = false;
    config = LoadLocalConfiguration(&log,&shouldExit);
    if (shouldExit){
        exit(0);
    }

    if (config.getBool(CONFIG_PRODUCTION_FLAG)){
        log.appendStandard("Configured for Production");
    }
    else{
        log.appendStandard("Configured for Local Host");
    }
    log.appendStandard("Starting update server...");
    updateServer.startServer(&config);
}
