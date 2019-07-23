#include "idtablediag.h"
#include "ui_idtablediag.h"

IDTableDiag::IDTableDiag(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::IDTableDiag)
{
    ui->setupUi(this);
}

IDTableDiag::~IDTableDiag()
{
    delete ui;
}

IDTableDiag::Columns IDTableDiag::getTableColumns() const {
    Columns c;
    c.displayID = ui->cbDisplayID->isChecked();
    c.vmHPid    = ui->cbHPID->isChecked();
    c.vmPid     = ui->cbPID->isChecked();
    c.dir = ui->lePath->text();
    return c;
}

void IDTableDiag::on_pushButton_clicked()
{
    ui->lePath->setText(QFileDialog::getExistingDirectory(this,"Select the directory to process","work"));
}

void IDTableDiag::on_pbOk_clicked()
{
    this->accept();
}

void IDTableDiag::on_pbCancel_clicked()
{
    this->reject();
}
