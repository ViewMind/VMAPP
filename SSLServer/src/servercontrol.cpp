#include "servercontrol.h"

ServerControl::ServerControl(QObject *parent) : QObject(parent)
{

    // Creating the configuration verifier
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_TCP_PORT] = cmd;

    cmd.clear();
    cv[CONFIG_RAW_DATA_REPO] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_NUMBER_OF_PARALLEL_PROCESSES] = cmd;

    cmd.clear();
    cv[CONFIG_EYEPROCESSOR_PATH] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_WAIT_DATA_TIMEOUT] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_TCP_PORT_DBCOMM] = cmd;

    // DB configuration is all strings.
    cmd.clear();
    cv[CONFIG_DBHOST] = cmd;
    cv[CONFIG_DBNAME] = cmd;
    cv[CONFIG_DBPASSWORD] = cmd;
    cv[CONFIG_DBUSER] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_TCP_PORT_DBCOMM] = cmd;

    cmd.optional = true;
    cv[CONFIG_DBPORT] = cmd;

    config.setupVerification(cv);

    sslManager.setDBConnection(dbmng.getDBInterface());

}

void ServerControl::startServer(){

    if (!config.loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        log.appendError("Configuration file errors:<br>"+config.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    connect(&sslManager,&SSLManager::newMessages,this,&ServerControl::on_messagesAvailable);

    // Multiplying timeout times 1000 to turn into ms.
    config.addKeyValuePair(CONFIG_WAIT_DATA_TIMEOUT,config.getInt(CONFIG_WAIT_DATA_TIMEOUT)*1000);

    if (!QSslSocket::supportsSsl()){
        log.appendError("No support for SSL found. Cannot continue");
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    log.appendStandard("Starting eye processing server...");
    sslManager.startServer(&config);
    log.appendStandard("Starting sql server...");
    dbmng.startServer(config);
}

void ServerControl::on_messagesAvailable(){
    QStringList messages = sslManager.getMessages();
    for (qint32 i = 0; i < messages.size(); i++){
        log.appendStandard(messages.at(i));
    }
}
