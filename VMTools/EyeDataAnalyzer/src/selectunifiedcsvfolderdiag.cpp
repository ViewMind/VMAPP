#include "selectunifiedcsvfolderdiag.h"
#include "ui_selectunifiedcsvfolderdiag.h"

SelectUnifiedCSVFolderDiag::SelectUnifiedCSVFolderDiag(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectUnifiedCSVFolderDiag)
{
    ui->setupUi(this);
}

SelectUnifiedCSVFolderDiag::~SelectUnifiedCSVFolderDiag()
{
    delete ui;
}

void SelectUnifiedCSVFolderDiag::on_pbBrowse_clicked()
{
    ui->lePath->setText(QFileDialog::getExistingDirectory(this,"Select the directory to process","work"));
}

void SelectUnifiedCSVFolderDiag::on_pbOK_clicked()
{
    this->accept();
}

void SelectUnifiedCSVFolderDiag::on_pbCancel_clicked()
{
    this->reject();
}

qint32 SelectUnifiedCSVFolderDiag::getDisplayIDCode() const{
    return ui->cbIDToUse->currentIndex();
}

QString SelectUnifiedCSVFolderDiag::getDirectory() const{
    return ui->lePath->text();
}

qint64 SelectUnifiedCSVFolderDiag::getMaxDayDiffForMedRecs() const{
    return ui->leMedRecsDays->text().toInt();
}

qint32 SelectUnifiedCSVFolderDiag::getMaximumDispParameters() const{
    qint32 ans = ui->leMaxDispValue->text().toInt();
    if (ui->cbEnableMaxDispParameter->isChecked()) return ans;
    else return -1;
}

void SelectUnifiedCSVFolderDiag::on_cbEnableMaxDispParameter_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    ui->leMaxDispValue->setEnabled(ui->cbEnableMaxDispParameter->isChecked());
}
