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

void IDTableDiag::setInstitutions(const QHash<QString, QString> &imap){
    ui->comboBox->clear();
    QStringList keys = imap.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        ui->comboBox->addItem(imap.value(keys.at(i)) + " - " + keys.at(i),keys.at(i));
    }
}

IDTableDiag::Columns IDTableDiag::getTableColumns() const {
    Columns c;
    c.DBPUID   = ui->cbDBPUID->isChecked();
    c.DID      = ui->cbDID->isChecked();
    c.HPUID    = ui->cbHPID->isChecked();
    c.PUID     = ui->cbPID->isChecked();
    c.dir      = ui->comboBox->currentData().toString();
    return c;
}

void IDTableDiag::on_pbOk_clicked()
{
    this->accept();
}

void IDTableDiag::on_pbCancel_clicked()
{
    this->reject();
}
