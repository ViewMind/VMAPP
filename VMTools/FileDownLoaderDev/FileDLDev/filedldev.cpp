#include "filedldev.h"
#include "ui_filedldev.h"

FileDLDev::FileDLDev(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FileDLDev)
{
    ui->setupUi(this);
    bool ans = connect(&dl,&FileDownloader::downloadCompleted,this,&FileDLDev::onDLComplete);
    ans = connect(&dl,&FileDownloader::downloadProgress,this,&FileDLDev::onDLProgress);
}

void FileDLDev::onDLProgress(qreal p, qreal hours, qreal minutes, qreal seconds){
    ui->plainTextEdit->appendPlainText("Progress " + QString::number(p) + ". ETA: " + QString::number(hours) + ":" + QString::number(minutes) + ":" + QString::number(seconds));
}

void FileDLDev::onDLComplete(bool ok){
    if (ok){
        ui->plainTextEdit->appendPlainText("Finished downloading with no issues");
    }
    else {
        ui->plainTextEdit->appendPlainText("ERROR: " + dl.getError());
    }
}

FileDLDev::~FileDLDev()
{
    delete ui;
}


void FileDLDev::on_pushButton_2_clicked()
{
    ui->plainTextEdit->clear();
    bool ans = dl.download("http://speedtest.ftp.otenet.gr/files/test1Mb.db","temp.bin");

    if (!ans){
        ui->plainTextEdit->appendPlainText("Failed to start download because: " + dl.getError());
    }
    else {
        ui->plainTextEdit->appendPlainText("Starting download...");
    }

}


void FileDLDev::on_pushButton_clicked()
{
    ui->plainTextEdit->clear();
    bool ans = dl.download("https://speed.hetzner.de/100MB.bin","temp.bin");

    if (!ans){
        ui->plainTextEdit->appendPlainText("Failed to start download because: " + dl.getError());
    }
    else {
        ui->plainTextEdit->appendPlainText("Starting download...");
    }
}

