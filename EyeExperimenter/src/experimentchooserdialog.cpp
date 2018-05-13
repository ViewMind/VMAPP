#include "experimentchooserdialog.h"
#include "ui_experimentchooserdialog.h"

ExperimentChooserDialog::ExperimentChooserDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExperimentChooserDialog)
{
    ui->setupUi(this);
    ui->lePatientAge->setValidator(new QIntValidator(6,130,this));
}

ExperimentChooserDialog::~ExperimentChooserDialog()
{
    delete ui;
}

void ExperimentChooserDialog::setPatientData(const QString &name, quint8 age, const QString &email){
    ui->lePatientAge->setText(QString::number(age));
    ui->lePatientName->setText(name);
    ui->leEmail->setText(email);
}

QList<qint32> ExperimentChooserDialog::getSelectedSequence() const{

    QList<qint32> seq;

    if (ui->cboxExperiments->currentIndex() == EXP_SEQ_READING_IMAGES_UC_BC){
        seq << EXP_READING << EXP_BINDING_UC << EXP_BINDING_BC;
    }
    else{
        seq << ui->cboxExperiments->currentIndex();
    }

    return seq;

}

QString ExperimentChooserDialog::getPatientName() const{
    return ui->lePatientName->text();
}

QString ExperimentChooserDialog::getPatientsAge() const{
    return ui->lePatientAge->text();
}

QString ExperimentChooserDialog::getEmail() const{
    return ui->leEmail->text();
}
