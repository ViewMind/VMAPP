#include "localdbreader.h"
#include "ui_localdbreader.h"

LocalDBReader::LocalDBReader(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LocalDBReader)
{
    ui->setupUi(this);
}

LocalDBReader::~LocalDBReader()
{
    delete ui;
}

void LocalDBReader::on_pbSelectAndGo_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select DAT DB File",".","DAT DB Files (*.dat)");
    if (fileName.isEmpty()){
        ui->pteLog->setPlainText("No file selected");
        return;
    }

    LocalInformationManager lim;
    lim.setWorkingFile(fileName);

    ui->pteLog->setPlainText(lim.printDBToString());

}
