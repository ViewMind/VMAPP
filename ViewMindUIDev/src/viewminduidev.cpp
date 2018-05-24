#include "viewminduidev.h"
#include "ui_viewminduidev.h"

ViewMindUiDev::ViewMindUiDev(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ViewMindUiDev)
{
    ui->setupUi(this);
}

ViewMindUiDev::~ViewMindUiDev()
{
    delete ui;
}
