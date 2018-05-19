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
    cv[CONFIG_OUTPUT_DIR] = cmd;

    config.setupVerification(cv);
    if (!config.loadConfiguration(FILE_CONFIGURATION,COMMON_TEXT_CODEC)){
        log.appendError("Configuration file errors:<br>"+config.getError());
        return;
    }

    connect(&sslManager,&SSLManager::newMessages,this,&SSLServerWindow::on_messagesAvailable);

    sslManager.startServer((quint16) config.getInt(CONFIG_TCP_PORT));

    if (!QSslSocket::supportsSsl()){
        log.appendError("No support for SSL found. Cannot continue");
        return;
    }

    log.appendStandard("Starting server...");

    // Attempting to call eye processor
    QString program = "C:/Users/Viewmind/Documents/QtProjects/EyeReportGenerator/exe/release/EyeReportGen.exe";
    QStringList arguments;
    arguments << "argumento 1" << "argumento 2";

    QProcess *myProcess = new QProcess(parent);
    myProcess->start(program, arguments);

}

void SSLServerWindow::on_messagesAvailable(){
    QStringList messages = sslManager.getMessages();
    for (qint32 i = 0; i < messages.size(); i++){
        log.appendStandard(messages.at(i));
    }
}

SSLServerWindow::~SSLServerWindow()
{
    delete ui;
}
