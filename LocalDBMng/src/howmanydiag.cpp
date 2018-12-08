#include "howmanydiag.h"
#include "ui_howmanydiag.h"

HowManyDiag::HowManyDiag(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HowManyDiag)
{
    ui->setupUi(this);
}

HowManyDiag::~HowManyDiag()
{
    delete ui;
}

void HowManyDiag::on_buttonBox_accepted()
{
    bool ok;
    howMany = ui->leHowMany->text().toInt(&ok);
    if (!ok) howMany = 0;
}
