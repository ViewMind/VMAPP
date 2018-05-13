#include "settingsdialog.h"
#include "ui_settingsdialog.h"

SettingsDialog::SettingsDialog(QWidget *parent, ConfigurationManager *c) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    config = c;

    // Loading data or setting the default values.
    if (c->containsKeyword(CONFIG_DOCTOR_NAME)){
        ui->leDoctorName->setText(c->getString(CONFIG_DOCTOR_NAME));
    }
    else{
        ui->leDoctorName->setText("Consulting doctor");
    }

    // Report language.
    if (c->containsKeyword(CONFIG_REPORT_LANGUAGE)){
        QString lang = c->getString(CONFIG_REPORT_LANGUAGE);
        for (qint32 i = 0; i < ui->cbLangReport->count(); i++){
            if (ui->cbLangReport->itemText(i) == lang){
                ui->cbLangReport->setCurrentIndex(i);
                break;
            }
        }
    }

    ui->cbDemoMode->setChecked(c->getBool(CONFIG_DEMO_MODE));

    ui->cbEnableDualMonitor->setChecked(c->getBool(CONFIG_DUAL_MONITOR_MODE));

#ifdef ENABLE_EXPANDED_BINDING_OPTIONS
    ui->groupBox->setVisible(true);
    ui->cbBindingDefault->setChecked(c->getBool(CONFIG_BINDING_DEFAULT));
    ui->cbBindingUseNumbers->setChecked(c->getBool(CONFIG_BINDING_USE_NUMBERS));
    qint32 ntargets = c->getInt(CONFIG_BINDING_NUM_TARGETS);
    if ((ntargets >= 2) && (ntargets <= 4)) ui->cboxBindingNumberOfTargets->setCurrentIndex(ntargets-2);
    else ui->cboxBindingNumberOfTargets->setCurrentIndex(0);
#else
    ui->groupBox->setVisible(false);
#endif

}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_pbOk_clicked()
{

    // Saving all the data.
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_DOCTOR_NAME,ui->leDoctorName->text(),config);
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_REPORT_LANGUAGE,ui->cbLangReport->currentText(),config);
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_DEMO_MODE,bool2String(ui->cbDemoMode->isChecked()),config);
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_DUAL_MONITOR_MODE,bool2String(ui->cbEnableDualMonitor->isChecked()),config);
#ifdef ENABLE_EXPANDED_BINDING_OPTIONS
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_BINDING_DEFAULT,bool2String(ui->cbBindingDefault->isChecked()),config);
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_BINDING_NUM_TARGETS,QString::number(ui->cboxBindingNumberOfTargets->currentIndex() + 2),config);
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_BINDING_USE_NUMBERS,bool2String(ui->cbBindingUseNumbers->isChecked()),config);
#else
    ConfigurationManager::setValue(FILE_CONFIGURATION,COMMON_TEXT_CODEC,CONFIG_BINDING_DEFAULT,"true",config);
#endif

}

void SettingsDialog::on_cbBindingDefault_stateChanged(int arg1)
{
    if (Qt::Checked == arg1){
        ui->cbBindingUseNumbers->setEnabled(false);
        ui->cboxBindingNumberOfTargets->setEnabled(false);
    }
    else{
        ui->cbBindingUseNumbers->setEnabled(true);
        ui->cboxBindingNumberOfTargets->setEnabled(true);
    }
}
