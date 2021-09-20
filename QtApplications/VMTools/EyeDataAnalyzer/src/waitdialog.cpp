#include "waitdialog.h"
#include "ui_waitdialog.h"

WaitDialog::WaitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WaitDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowTitleHint);
    ui->progBar->setVisible(false);
}

void WaitDialog::setProgressBarVisibility(bool visibility){
    ui->progBar->setVisible(visibility);
}

void WaitDialog::setProgressBarValue(qreal value){
    ui->progBar->setValue(value);
}

void WaitDialog::setMessage(const QString &msg){
    ui->labMessage->setText(msg);
}

WaitDialog::~WaitDialog()
{
    delete ui;
}
