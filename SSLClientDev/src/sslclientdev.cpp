#include "sslclientdev.h"
#include "ui_sslclientdev.h"

SSLClientDev::SSLClientDev(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SSLClientDev)
{
    ui->setupUi(this);
    config.addKeyValuePair(CONFIG_SERVER_ADDRESS,"192.168.1.10");
    config.addKeyValuePair(CONFIG_TCP_PORT,54912);
    config.addKeyValuePair(CONFIG_DOCTOR_NAME,"Dr Zito");
    client = new SSLClient(this,&config);
    client->show();
}

SSLClientDev::~SSLClientDev()
{
    delete ui;
}
