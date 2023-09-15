#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Loading the debug options if any.
    DebugOptions::LoadDebugOptions();

    // Getting the arguments for this run.
    QStringList args = QCoreApplication::arguments();

    // Creating the log manager.
    logger = new MessageLogger(ui->listWidget);

    // Loading the language strings
    if (!Langs::LoadLanguageFile("en")){
        logger->log("Failed in loading the language file");
    }

    bool isUpdate = false;
    if (args.size() > 1){
        if (args.at(1) == "update"){ // zero is program name.
            isUpdate = true;
        }
    }

    if (isUpdate){
        logger->log("Started in UPDATE MODE");
        this->setDisplayMode(DM_UPDATE_MODE);
        // We show the welcome message.
        logger->success(Langs::getString("welcome_update"));
    }
    else {
        logger->log("Started in NORMAL MODE");
        this->setDisplayMode(DM_NORMAL_MODE);
        logger->success(Langs::getString("welcome_normal"));
    }

    connect(&maintainer,&MaintenanceManager::progressUpdate,this,&MainWindow::onProgressUpdate);
    connect(&maintainer,&MaintenanceManager::message,this,&MainWindow::onNewMessage);
    connect(&maintainer,&MaintenanceManager::finished,this,&MainWindow::onMaintenanceFinished);

    ui->progressBar->setValue(0);
    ui->labProgress->setText("");
    ui->labFile->setText("");

    if (isUpdate){
        // We need to start the updat.e
        maintainer.setAction(MaintenanceManager::ACTION_UPDATE);
        maintainer.start();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewMessage(qint32 type, QString message){
    if (type == MessageLogger::MSG_DISP){
        logger->display(message);
    }
    else if (type == MessageLogger::MSG_ERROR){
        logger->error(message);
    }
    else if (type == MessageLogger::MSG_SUCCESS){
        logger->success(message);
    }
    else if (type == MessageLogger::MSG_WARNING){
        logger->warning(message);
    }
    else{
        logger->log(message);
    }
}

void MainWindow::onMaintenanceFinished(){

    MaintenanceManager::Action action =  maintainer.getAction();
    if (action == MaintenanceManager::ACTION_UPDATE){
        // There was an update we need to start up the eye experimenter, if successfull
        if (maintainer.wasLastActionASuccess()){
            // We now run the EyeExperimenter.
            QString workdir = maintainer.getEyeExpWorkDir();
            qint64 pid;
            QString program = workdir + "/" + Globals::Paths::EYEEXP_EXECUTABLE;

            logger->log("Launching the EyeExplorer from: " + program);

            QStringList arguments;
            if (!QProcess::startDetached(program,arguments,workdir,&pid)){
                logger->error(Langs::getString("error_cant_start_ee"));
            }
            else {
                //this->close();
                QCoreApplication::quit(); // With this the application starts maximized instead of minimized. Go figure.
            }
        }
        else {
            logger->display(Langs::getString("unable_to_update"));
        }

    }
    else if (action == MaintenanceManager::ACTION_DIAGNOSE){
        logger->success("Diagnostics done");
    }

}

void MainWindow::onProgressUpdate(qreal p, QString message){
    ui->labFile->setText(message);
    ui->progressBar->setValue(qRound(p));
    ui->labProgress->setText(QString::number(qRound(p)) + "%");
}

void MainWindow::on_pbMainAction_clicked(){
    maintainer.setAction(MaintenanceManager::ACTION_DIAGNOSE);
    maintainer.start();
}

void MainWindow::setDisplayMode(DisplayMode dm){
    switch (dm){
    case DM_UPDATE_MODE:
        ui->labProgress->setVisible(true);
        ui->labFile->setVisible(true);
        ui->progressBar->setVisible(true);
        ui->pbMainAction->setVisible(false);
        break;
    case DM_NORMAL_MODE:
        ui->labProgress->setVisible(true);
        ui->labFile->setVisible(true);
        ui->progressBar->setVisible(true);
        ui->pbMainAction->setVisible(true);
        ui->pbMainAction->setText(Langs::getString("btn_main_action_diag"));
        break;
    default:
        ui->labProgress->setVisible(true);
        ui->labFile->setVisible(true);
        ui->progressBar->setVisible(true);
        ui->pbMainAction->setVisible(true);
        break;
    }
}
