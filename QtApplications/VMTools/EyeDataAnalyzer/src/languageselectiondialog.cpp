#include "languageselectiondialog.h"
#include "ui_languageselectiondialog.h"

LanguageSelectionDialog::LanguageSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LanguageSelectionDialog)
{
    ui->setupUi(this);
}


LanguageSelectionDialog::~LanguageSelectionDialog()
{
    delete ui;
}

QString LanguageSelectionDialog::getLanguage() const{
    return ui->cbLangSelect->currentText();
}

void LanguageSelectionDialog::on_pbOk_clicked()
{
    this->accept();
}
