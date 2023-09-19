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

//    logger->display("Some info");
//    logger->warning("Some info");
//    logger->error("Some info");

    connect(&maintainer,&MaintenanceManager::progressUpdate,this,&MainWindow::onProgressUpdate);
    connect(&maintainer,&MaintenanceManager::message,this,&MainWindow::onNewMessage);
    connect(&maintainer,&MaintenanceManager::finished,this,&MainWindow::onMaintenanceFinished);
    connect(&support,&SupportContact::finished,this,&MainWindow::onContactSupportReturs);

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
            QString workdir = Paths::Path(Paths::PI_CONTAINER_DIR); //maintainer.getEyeExpWorkDir();
            qint64 pid;
            QString program = Paths::Path(Paths::PI_CURRENT_EXE_FILE); //workdir + "/" + Globals::Paths::EYEEXP_EXECUTABLE;

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
    //maintainer.setAction(MaintenanceManager::ACTION_DIAGNOSE);
    //maintainer.start();
    ui->pbMainAction->setEnabled(false);
    this->logger->display(Langs::getString("notification_mail"));
    contactSupport("This is a test message");
}

void MainWindow::onContactSupportReturs(){
    QString error = support.getError();
    if (error != ""){
        this->logger->log("ERROR: Failed sending support email. Reason: " + error);
        this->logger->error(Langs::getString("error_mail_failed"));
        return;
    }

    this->logger->success(Langs::getString("success_mail"));
}

void MainWindow::contactSupport(const QString &message){

    // We need to try and read the vm configuration file.
    ConfigurationManager license;
    if (!license.loadConfiguration(Paths::Path(Paths::PI_CURRENT_VMCONFIG_FILE))){
        this->logger->error(Langs::getString("error_mail_failed"));
        this->logger->log("ERROR: Unable to read license file from : " + Paths::Path(Paths::PI_CURRENT_VMCONFIG_FILE) + ". Reason: " + license.getError());
        return;
    }

    QString institution = license.getString("institution_id");
    QString instance = license.getString("instance_number");
    QString key = license.getString("instance_key");
    QString hash = license.getString("instance_hash_key");

    // We configure the credentials.
    support.configure(institution,instance,key,hash);

    // Set the message to send and the path to the current logfiel to send as an attachment.
    support.setMessageAndLogFileName(message,this->logger->getLogFile());

    // We send the actual request.
    support.send();

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

