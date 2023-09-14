#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList args = QCoreApplication::arguments();

    qDebug() << args;

    logger = new MessageLogger(ui->listWidget);

    bool isUpdate = false;
    if (args.size() > 1){
        if (args.at(1) == "update"){ // zero is program name.
            isUpdate = true;
        }
    }

    if (isUpdate){
        logger->log("Started in UPDATE MODE");
        this->setDisplayMode(DM_UPDATE_MODE);
    }
    else {
        logger->log("Started in NORMAL MODE");
        this->setDisplayMode(DM_NORMAL_MODE);
    }

    connect(&drunner,&DirRunner::finished,this,&MainWindow::onDirRunFinished);
    connect(&dirComparer,&DirCompare::finished,this,&MainWindow::onDirCompareFinished);
    connect(&dirComparer,&DirCompare::updateProgress,this,&MainWindow::onDirCompareProgressUpdate);

    ui->progressBar->setValue(0);
    ui->labProgress->setText("");
    ui->labFile->setText("");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onDirCompareProgressUpdate(qreal p, QString filename){
    ui->labFile->setText(filename);
    ui->progressBar->setValue(qRound(p));
    ui->labProgress->setText(QString::number(qRound(p)) + "%");
}

void MainWindow::onDirRunFinished(){

}

void MainWindow::onDirCompareFinished(){

    //    ui->pbMainAction->setEnabled(true);
    //    ui->labProgress->setText("");
    //    ui->labFile->setText("");
    //    ui->progressBar->setValue(0);

    //    QMap<QString,DirCompare::FileListType> lists;
    //    lists["Files With Different CheckSum"] = DirCompare::FLT_BAD_CHECSUM;
    ////    lists["Files In Reference But Not IN Check Dir"] = DirCompare::FLT_NOT_IN_CHECK;
    ////    lists["Files Not In Reference"] = DirCompare::FLT_NOT_IN_REF;
    ////    lists["Identical Files"] = DirCompare::FLT_SAME;

    //    QStringList keys = lists.keys();
    //    for (qint32 i = 0; i < keys.size(); i++){
    //        qDebug() << keys.at(i);
    //        QStringList list = dirComparer.getFileList(lists.value(keys.at(i)));
    //        for (qint32 j = 0; j < list.size(); j++){
    //            qDebug() << "   " << list.at(j);
    //        }
    //    }

}

void MainWindow::on_pbMainAction_clicked(){

    ////    Start an application and quit.
    //    QString workdir = "C:/Users/ViewMind/Documents/VMAPP/EyeExperimenter/EyeExperimenter/";
    //    QString exe = "EyeExperimenter.exe";
    ////    qint64 pid;
    //    QString program = workdir + "/" + exe;
    ////    QProcess::startDetached(program,QStringList(),workdir,&pid);
    ////    this->close();

    //// Create a shortcut for the application.

    //    QStringList possiblePaths = QStandardPaths::standardLocations(QStandardPaths::DesktopLocation);
    //    QString pathToLink = possiblePaths.first() + "/EyeExperimenter.lnk";
    //    qDebug() << "From" << program << "To" << pathToLink;
    //    if (!QFile::link(program,pathToLink)){
    //        qDebug() << "Link failed";
    //    }
    //    else {
    //        qDebug() << "link success";
    //    }





    //    return;
    //    ui->pbMainAction->setEnabled(false);

    //    QString refdir = "C:/Users/ViewMind/Documents/temp/EyeExperimenter_";
    //    QString checkdir = "C:/Users/ViewMind/Documents/VMAPP/EyeExperimenter/EyeExperimenter";

    //    dirComparer.setDirs(refdir,checkdir);
    //    dirComparer.compare();
    //    ui->progressBar->setValue(0);

}

void MainWindow::setDisplayMode(DisplayMode dm){
    switch (dm){
    case DM_UPDATE_MODE:
        ui->labProgress->setVisible(false);
        ui->labFile->setVisible(false);
        ui->progressBar->setVisible(false);
        ui->pbMainAction->setVisible(false);
        break;
    case DM_NORMAL_MODE:
        ui->labProgress->setVisible(false);
        ui->labFile->setVisible(false);
        ui->progressBar->setVisible(false);
        ui->pbMainAction->setVisible(true);
        break;
    default:
        ui->labProgress->setVisible(true);
        ui->labFile->setVisible(true);
        ui->progressBar->setVisible(true);
        ui->pbMainAction->setVisible(true);
        break;
    }
}
