#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Loading the debug options if any.
    DebugOptions::LoadDebugOptions();

    // The first thing we need to do is fill out the paths.
    Paths::FillPaths();

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

    ui->pbMainAction->setText(Langs::getString("btn_main_action_diag"));

    if (isUpdate){
        logger->log("Started in UPDATE MODE");
        ui->pbMainAction->setVisible(false);
        logger->success(Langs::getString("welcome_update"));
    }
    else {
        logger->log("Started in NORMAL MODE");
        maintainer.setRecommendedAction(MaintenanceManager::ACTION_DIAGNOSE);
        logger->success(Langs::getString("welcome_normal"));
    }

    //    logger->success("Some info");
    //    logger->display("Some info");
    //    logger->display("Some info");
    //    logger->warning("Some info");
    //    logger->warning("Some info");
    //    logger->error("Some info");
    //    logger->error("Some info");

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

    this->setWindowTitle(this->windowTitle() + " - " + Globals::VERSION);

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

    MaintenanceManager::Action action =  maintainer.getLastPerformedAction();

    if (action == MaintenanceManager::ACTION_UPDATE){
        // There was an update we need to start up the eye experimenter, if successfull
        if (maintainer.wasLastActionASuccess()){
            // We now run the EyeExperimenter.
            QString workdir = Paths::Path(Paths::PI_INSTALL_DIR);
            qint64 pid;
            QString program = Paths::Path(Paths::PI_CURRENT_EXE_FILE);

            logger->log("Launching the EyeExplorer from: " + program);
            //logger->warning("Will not launch"); return;

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

        // If the suggested action is none, then there is nothing else to do.
        if (maintainer.getRecommendedAction() == MaintenanceManager::ACTION_NONE){
            ui->pbMainAction->setEnabled(false);
            logger->display(Langs::getString("notification_no_issues"));
            return;
        }
        else if (maintainer.getRecommendedAction() == MaintenanceManager::ACTION_CORRECT){
            ui->pbMainAction->setEnabled(true);
            ui->pbMainAction->setText(Langs::getString("btn_main_action_fix"));
            logger->display(Langs::getString("notification_corrective_actions"));
        }
        else if (maintainer.getRecommendedAction() == MaintenanceManager::ACTION_CONTACT_SUPPORT){
            ui->pbMainAction->setEnabled(true);
            ui->pbMainAction->setText(Langs::getString("btn_main_action_contact"));
            logger->error(Langs::getString("error_vmupdate_failed"));
        }
        else {
            logger->error(Langs::getString("error_vmupdate_failed"));
        }

    }
    else if (action == MaintenanceManager::ACTION_CORRECT){
        if (maintainer.wasLastActionASuccess()){
            ui->pbMainAction->setEnabled(false);
            logger->success(Langs::getString("success_maintenace"));
        }
        else {
            ui->pbMainAction->setEnabled(true);
            ui->pbMainAction->setText(Langs::getString("btn_main_action_contact"));
            maintainer.setRecommendedAction(MaintenanceManager::ACTION_CONTACT_SUPPORT);
            logger->error(Langs::getString("error_vmupdate_failed"));
        }
    }
    else {
        logger->log("ERROR: Finihsed Action is neither CORRECT, DIAGNOSE or UPDATE");
    }

}

void MainWindow::onProgressUpdate(qreal p, QString message){
    ui->labFile->setText(message);
    ui->progressBar->setValue(qRound(p));
    ui->labProgress->setText(QString::number(qRound(p)) + "%");
}

void MainWindow::on_pbMainAction_clicked(){

    if (maintainer.getRecommendedAction() == MaintenanceManager::ACTION_DIAGNOSE){
        ui->pbMainAction->setEnabled(false);
        maintainer.setAction(MaintenanceManager::ACTION_DIAGNOSE);
        maintainer.start();
    }
    else if (maintainer.getRecommendedAction() == MaintenanceManager::ACTION_CONTACT_SUPPORT){
        SupportDialog diag;
        diag.setLoggger(logger);
        diag.exec();
        QString error = diag.getSupportContactResult();
        if (error == ""){
            if (diag.wasItCanceled()){
                logger->display(Langs::getString("contact_canceled"));
            }
            else {
                logger->success(Langs::getString("success_mail"));
            }
        }
        else {
            logger->log("ERROR: Failed sending support mail. Reason: " + error);
            logger->error(Langs::getString("error_mail_failed"));
        }
    }
    else if (maintainer.getRecommendedAction() == MaintenanceManager::ACTION_CORRECT){
        ui->pbMainAction->setEnabled(false);
        maintainer.setAction(MaintenanceManager::ACTION_CORRECT);
        maintainer.start();
    }
    else {
        ui->pbMainAction->setEnabled(false);
        logger->log("ERROR: On Action Triggered, but Action is neither DIAGNOSE, SUPPORT or CORRECT.");
    }

}

