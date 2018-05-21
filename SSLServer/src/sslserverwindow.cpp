#include "sslserverwindow.h"
#include "ui_sslserverwindow.h"


SSLServerWindow::SSLServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SSLServerWindow)
{
    ui->setupUi(this);

    log.setLogInterface(ui->pteLog);

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

    config.setupVerification(cv);
    if (!config.loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        log.appendError("Configuration file errors:<br>"+config.getError());
        return;
    }

    connect(&sslManager,&SSLManager::newMessages,this,&SSLServerWindow::on_messagesAvailable);

    // Multiplying timeout times 1000 to turn into ms.
    config.addKeyValuePair(CONFIG_WAIT_DATA_TIMEOUT,config.getInt(CONFIG_WAIT_DATA_TIMEOUT)*1000);

    sslManager.startServer(&config);

    if (!QSslSocket::supportsSsl()){
        log.appendError("No support for SSL found. Cannot continue");
        return;
    }

    log.appendStandard("Starting server...");

}

void SSLServerWindow::on_messagesAvailable(){
    QStringList messages = sslManager.getMessages();
    for (qint32 i = 0; i < messages.size(); i++){
        log.appendStandard(messages.at(i));
    }
}

void SSLServerWindow::on_pbClearConsole_clicked()
{
    ui->pteLog->clear();
}

SSLServerWindow::~SSLServerWindow()
{
    delete ui;
}

