#include "pngreportgenerator.h"
#include "ui_pngreportgenerator.h"

PNGReportGenerator::PNGReportGenerator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PNGReportGenerator)
{
    ui->setupUi(this);
    //ui->lePath->setText("C:/Users/Viewmind/Documents/ExperimenterOutputs/OtherSources/report_2018_06_20.rep");
}

PNGReportGenerator::~PNGReportGenerator()
{
    delete ui;
}

void PNGReportGenerator::on_pbBrowse_clicked()
{
    QString selected = QFileDialog::getOpenFileName(this,"Select rep file",".","REP Files (*.rep)");
    if (!selected.isEmpty()) ui->lePath->setText(selected);
}

void PNGReportGenerator::on_pbGenerate_clicked()
{

    ConfigurationManager config;
    config.addKeyValuePair(CONFIG_REPORT_LANGUAGE,ui->cbLang->currentText());
    if (ui->cboxUseLogo->isChecked())
        config.addKeyValuePair(CONFIG_REPORT_NO_LOGO,"false");
    else
        config.addKeyValuePair(CONFIG_REPORT_NO_LOGO,"false");

    QFileInfo info(ui->lePath->text());
    if (!info.exists()){
        QMessageBox::critical(this,"Error in rep file","File " + info.fileName() + " does not exist",QMessageBox::Ok);
        return;
    }

    config.addKeyValuePair(CONFIG_PATIENT_REPORT_DIR,info.absolutePath());

    ConfigurationManager dataSet;
    if (!dataSet.loadConfiguration(info.absoluteFilePath(),COMMON_TEXT_CODEC)){
        QMessageBox::critical(this,"Error in rep file",dataSet.getError(),QMessageBox::Ok);
        return;
    }

    ImageReportDrawer repdrawer;
    repdrawer.drawReport(&dataSet,&config);

    // Getting the file name
    QString outputlocation = config.getString(CONFIG_IMAGE_REPORT_PATH);
    QMessageBox::information(this,"Finished!","Report was generated to path: " + outputlocation,QMessageBox::Ok);

}
