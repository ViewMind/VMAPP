#include "massbindingfixationdrawer.h"
#include "ui_massbindingfixationdrawer.h"
#include <QMessageBox>

MassBindingFixationDrawer::MassBindingFixationDrawer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MassBindingFixationDrawer)
{
    ui->setupUi(this);
    fdrawer = new BindingFixationDrawer();

    connect(fdrawer,SIGNAL(finished()),this,SLOT(on_finished()));
    connect(fdrawer,SIGNAL(updatePercent(qint32)),ui->progressBar,SLOT(setValue(int)));

}

MassBindingFixationDrawer::~MassBindingFixationDrawer()
{
    delete ui;
}

void MassBindingFixationDrawer::on_finished(){
    QMessageBox::information(this,"Done","Done",QMessageBox::Ok);
}

void MassBindingFixationDrawer::on_pbGo_clicked()
{
    fdrawer->setCSVFile(ui->lineEdit->text());
    fdrawer->start();
}
