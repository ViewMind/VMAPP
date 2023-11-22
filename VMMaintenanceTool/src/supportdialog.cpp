#include "supportdialog.h"
#include "ui_supportdialog.h"

SupportDialog::SupportDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SupportDialog)
{
    ui->setupUi(this);
    ui->labLoader->setVisible(false);
    ui->labWaitMsg->setVisible(false);
    ui->labWaitMsg->setText(Langs::getString("notification_mail"));
    QMovie *mv = new QMovie(":/images/loader.gif");
    mv->setScaledSize(QSize(200,200));
    mv->start();
    ui->labLoader->setAttribute(Qt::WA_NoSystemBackground);
    ui->labLoader->setMovie(mv);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    this->wasCanceled = false;

    connect(&sc,&SupportContact::finished,this,&SupportDialog::onSCContactFinished);

}


void SupportDialog::contactSupport(){

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
    sc.configure(institution,instance,key,hash);

    // Set the message to send and the path to the current logfiel to send as an attachment.
    sc.setMessageAndLogFileName(ui->plainTextEdit->toPlainText(),this->logger->getLogFile());

    // We send the actual request.
    sc.send();

}

QString SupportDialog::getSupportContactResult() const {
    return this->sc.getError();
}

void SupportDialog::setLoggger(MessageLogger *logger){
    this->logger = logger;
}

bool SupportDialog::wasItCanceled() const {
    return this->wasCanceled;
}

SupportDialog::~SupportDialog()
{
    delete ui;
}

void SupportDialog::onSCContactFinished(){
    this->accept();
}

void SupportDialog::on_pushButton_clicked() {

    ui->pushButton->setVisible(false);
    ui->plainTextEdit->setVisible(false);
    ui->label->setVisible(false);
    ui->labLoader->setVisible(true);
    ui->labWaitMsg->setVisible(true);
    ui->pbCancel->setVisible(false);

    contactSupport();

}


void SupportDialog::on_pbCancel_clicked(){
    this->wasCanceled = true;
    this->accept();
}

