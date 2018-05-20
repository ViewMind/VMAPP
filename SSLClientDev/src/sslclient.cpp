#include "sslclient.h"
#include "ui_sslclient.h"

SSLClient::SSLClient(QWidget *parent, ConfigurationManager *c) :
    QDialog(parent),
    ui(new Ui::SSLClient)
{

    ui->setupUi(this);

    // Will enable the request button only if there are no errors.
    ui->pbRequestReport->setEnabled(false);

    // Setting up the log.
    log.setLogInterface(ui->plainTextEdit);
    if (!QSslSocket::supportsSsl()){
        log.appendError("There is no support for SSL. Please request help from ViewMind to solve this issue.");
        return;
    }

    // Creating the socket and making the connections.
    socket = new QSslSocket(this);
    connect(socket,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    connect(socket,SIGNAL(readyRead()),this,SLOT(on_readyRead()));

    // Timer connection
    connect(&timer,&QTimer::timeout,this,&SSLClient::on_timeOut);

    // Setting up the configuation and checking that everything is there.
    config = c;

    if (config == nullptr){
        log.appendError("BAD SSLCLIENT INITIALIZATION");
        return;
    }

    if (!c->containsKeyword(CONFIG_SERVER_ADDRESS)){
        log.appendError("Server address was not set in the configuration file");
        return;
    }

    if (!c->containsKeyword(CONFIG_TCP_PORT)){
        log.appendError("Server port was not set in the configuration file");
        return;
    }

    clientState = CS_CONNECTING;
    ui->pbRequestReport->setEnabled(true);
}


void SSLClient::on_pbRequestReport_clicked()
{
    // A configuration is created in order to get the patient information.
    QString directory = ui->lePatientDirectory->text();

    if (!QDir(directory).exists()){
        log.appendError("Patient directory does not exist");
        return;
    }

    txDP.clearAll();

    bool reading, binding;
    reading = true;
    binding = true;

    QString readingfile = getNewestFile(directory,FILE_OUTPUT_READING);
    if (readingfile.isEmpty()){
        log.appendWarning("WARNING: No reading file found");
        reading = false;
    }
    else{
        txDP.addFile(readingfile,DataPacket::DPFI_READING);
    }
    QString bindingBC = getNewestFile(directory,FILE_OUTPUT_BINDING_BC);
    if (bindingBC.isEmpty()){
        log.appendWarning("WARNING: No binding BC file found");
        binding = false;
        txDP.addFile(bindingBC,DataPacket::DPFI_BINDING_BC);
    }
    QString bindingUC = getNewestFile(directory,FILE_OUTPUT_BINDING_UC);
    if (bindingUC.isEmpty()){
        log.appendWarning("WARNING: No binding UC file found");
        binding = false;
        txDP.addFile(bindingUC,DataPacket::DPFI_BINDING_UC);
    }

    if (!reading && !binding){
        log.appendError("ERROR: No processing is possible due to missing files.");
        return;
    }

    QString patientInfoFile = directory + "/" + QString(FILE_PATIENT_INFO_FILE);
    QString pname = "A. Patient";
    qreal   p_age  = 0;
    QString dname = "The Doctor";

    if (!QFile(patientInfoFile).exists()){
        log.appendWarning("WARNING: No Patient information found. Using stock information");
    }
    else{
        ConfigurationManager m;
        if (!m.loadConfiguration(patientInfoFile,COMMON_TEXT_CODEC)){
            if (m.containsKeyword(CONFIG_PATIENT_NAME)){
                pname = m.getString(CONFIG_PATIENT_NAME);
            }
            else{
                log.appendWarning("WARNING: No patient name in info file. Using stock information");
            }
            if (m.containsKeyword(CONFIG_PATIENT_AGE)){
                p_age = m.getInt(CONFIG_PATIENT_AGE);
            }
            else{
                log.appendWarning("WARNING: No patient age in info file. Using stock information");
            }
        }
        else{
            log.appendWarning("WARNING: Error loading patient file. Using stock information");
        }
    }

    if (config->containsKeyword(CONFIG_DOCTOR_NAME)){
        dname = config->getString(CONFIG_DOCTOR_NAME);
    }
    else{
        log.appendWarning("WARNING: Doctor's name was not loaded. Using stock information");
    }

    txDP.addString(dname,DataPacket::DPFI_DOCTOR_ID);
    txDP.addString(pname,DataPacket::DPFI_PATIENT_ID);
    txDP.addValue(p_age,DataPacket::DPFI_AGE);

    // Requesting connection and ack
    informationSent = false;
    socket->connectToHostEncrypted(config->getString(CONFIG_SERVER_ADDRESS),(quint16)config->getInt(CONFIG_TCP_PORT));
    ui->pbRequestReport->setEnabled(false);
    ui->pbSearch->setEnabled(false);

    // Starting timeout timer.
    clientState = CS_CONNECTING;
    startTimeoutTimer();

}

void SSLClient::on_encryptedSuccess(){
    log.appendSuccess("Established encrypted connection to the server. Waiting for acknowledge ... ");
    rxDP.clearAll();
    timer.stop();
    clientState = CS_WAIT_FOR_ACK;
    startTimeoutTimer();
}

void SSLClient::on_readyRead(){

    if (clientState == CS_CONNECTING) return;

    if (rxDP.bufferByteArray(socket->readAll())){

        if (clientState == CS_WAIT_FOR_ACK){

            if (rxDP.hasInformationField(DataPacket::DPFI_SEND_INFORMATION)){
                // Since in the information was requested, it is now sent.
                QByteArray ba = txDP.toByteArray();
                qint64 num = socket->write(ba.constData(),ba.size());
                if (num != ba.size()){
                    log.appendError("ERROR: Unable to send the information. Please try again.");
                    socket->disconnectFromHost();
                    ui->pbRequestReport->setEnabled(true);
                    ui->pbSearch->setEnabled(true);
                    return;
                }

                rxDP.clearBufferedData();
                clientState = CS_WAIT_FOR_REPORT;
                startTimeoutTimer();

            }
            else{
                // This is not it and it should start again.
                rxDP.clearBufferedData();
                return;
            }

        }
        else{ // We are waiting for a report

            if (rxDP.isInformationFieldOfType(DataPacket::DPFI_REPORT,DataPacket::DPFT_FILE)){
                rxDP.clearBufferedData();
                return;
            }

            // At this point the report was received so it is saved
            QString reportPath = rxDP.saveFile(ui->lePatientDirectory->text(),DataPacket::DPFI_REPORT);
            if (reportPath.isEmpty()){
                log.appendError("ERROR: Could not save the report to the directory: " + ui->lePatientDirectory->text() + ". Please try again");
            }
            else{
                log.appendSuccess("Report saved to: " + reportPath);
            }

            rxDP.clearAll();
            socket->disconnectFromHost();
            ui->pbRequestReport->setEnabled(true);
            ui->pbSearch->setEnabled(false);
        }

    }

}



//************************************* Socket and SSL Errors, Socket state changes ****************************************

void SSLClient::on_sslErrors(const QList<QSslError> &errors){
    for (qint32 i = 0; i < errors.size(); i++){
        log.appendWarning("SSL ERROR: " + errors.at(i).errorString());
    }
    socket->ignoreSslErrors();
}

void SSLClient::on_socketStateChanged(QAbstractSocket::SocketState state){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    log.appendStandard(QString("Log: Socket state - ") + metaEnum.valueToKey(state));
}

void SSLClient::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    log.appendWarning(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
}

void SSLClient::on_pbSearch_clicked()
{
    QString location = QFileDialog::getExistingDirectory(this,"Select patient directory",ui->lePatientDirectory->text());
    if (!location.isEmpty()) ui->lePatientDirectory->setText(location);
}

//*************************************************************************************************************************

void SSLClient::on_timeOut(){
    switch(clientState){
    case CS_CONNECTING:
        log.appendError("ERROR: Server connection notice did not arrive before the expected time. Closing connection. Please retry.");
        break;
    case CS_WAIT_FOR_REPORT:
        log.appendError("ERROR: Server generated report did not arrive before expected time. Closing connection. Please retry.");
        break;
    case CS_WAIT_FOR_ACK:
        log.appendError("ERROR: Server request for information did not arrive before expected time. Closing connection. Please retry.");
        break;
    }

    socket->disconnectFromHost();
    ui->pbRequestReport->setEnabled(true);
    ui->pbSearch->setEnabled(true);
}

void SSLClient::startTimeoutTimer(){
    QString message = "WARNING: ";
    bool showMessage = false;
    qint32 timeout;

    switch(clientState){
    case CS_CONNECTING:
        timeout = DEFAULT_TIMEOUT_CONNECTION;
        if (config->containsKeyword(CONFIG_CONNECTION_TIMEOUT)){
            timeout = config->getInt(CONFIG_CONNECTION_TIMEOUT)*1000;
        }
        else{
            message = message + "Connection timeout was not configured.";
            showMessage = true;
        }
        break;
    case CS_WAIT_FOR_REPORT:
        timeout = DEFAULT_TIMEOUT_WAIT_ACK;
        if (config->containsKeyword(CONFIG_CONNECTION_TIMEOUT)){
            timeout = config->getInt(CONFIG_CONNECTION_TIMEOUT)*1000;
        }
        else{
            message = message + "Connection timeout was not configured.";
            showMessage = true;
        }
        break;
    case CS_WAIT_FOR_ACK:
        timeout = DEFAULT_TIMEOUT_WAIT_ACK;
        if (config->containsKeyword(CONFIG_DATA_REQUEST_TIMEOUT)){
            timeout = config->getInt(CONFIG_DATA_REQUEST_TIMEOUT)*1000;
        }
        else{
            message = message + "Data request timeout was not configured.";
            showMessage = true;
        }
        break;
    }

    if(showMessage){
        log.appendWarning(message + ". Using default timeout of " + QString::number(timeout/1000) + " seconds.");
    }

    // Starting waiting for data timer.
    timer.setInterval(timeout);
    timer.start();
}

SSLClient::~SSLClient()
{
    delete ui;
}

