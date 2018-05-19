#include "sslclientdev.h"
#include "ui_sslclientdev.h"

SSLClientDev::SSLClientDev(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SSLClientDev)
{
    ui->setupUi(this);
}

SSLClientDev::~SSLClientDev()
{
    delete ui;
}
