#include "ssldataprocessingclient.h"

SSLDataProcessingClient::SSLDataProcessingClient(QObject *parent, ConfigurationManager *c):SSLClient(parent,c)
{
    if (!c->containsKeyword(CONFIG_WAIT_REPORT_TIMEOUT)){
        log.appendError("Wait for report timeout was not set in the configuration file");
        return;
    }

    if (!c->containsKeyword(CONFIG_DATA_REQUEST_TIMEOUT)){
        log.appendError("Data request timeout was not set in the configuration file");
        return;
    }

    clientConfigured = true;
    clientState = CS_CONNECTING;
}


void SSLDataProcessingClient::sendFinishedSignal(bool okvalue){
    if (!sentTransactionFinishedSignal){
        sentTransactionFinishedSignal = true;
        emit(transactionFinished(okvalue));
    }
}

void SSLDataProcessingClient::requestReport(){
    if (!clientConfigured){
        log.appendError("Cannot Report request process as client is not properly configured.");
        sendFinishedSignal(false);
        return;
    }
    sentTransactionFinishedSignal = false;
    connectToServer();
}

void SSLDataProcessingClient::connectToServer()
{
    QString directory = config->getString(CONFIG_PATIENT_DIRECTORY);

    if (!QDir(directory).exists()){
        log.appendError("Patient directory does not exist");
        sendFinishedSignal(false);
        return;
    }

    txDP.clearAll();

    bool reading, binding;
    reading = true;
    binding = true;

    QString readingfile;
    if (config->getBool(CONFIG_DEMO_MODE)) readingfile = ":/demo_data/reading_2018_06_03.dat";
    else readingfile = getNewestFile(directory,FILE_OUTPUT_READING);

    if (readingfile.isEmpty()){
        log.appendWarning("WARNING: No reading file found");
        reading = false;
    }
    else{
        txDP.addFile(readingfile,DataPacket::DPFI_READING);
    }

    QString bindingBC;
    if (config->getBool(CONFIG_DEMO_MODE)) bindingBC = ":/demo_data/binding_bc_2018_06_03.dat";
    else bindingBC = getNewestFile(directory,FILE_OUTPUT_BINDING_BC);

    if (bindingBC.isEmpty()){
        log.appendWarning("WARNING: No binding BC file found");
        binding = false;
    }
    else{
        txDP.addFile(bindingBC,DataPacket::DPFI_BINDING_BC);
    }

    QString bindingUC;
    if (config->getBool(CONFIG_DEMO_MODE)) bindingUC = ":/demo_data/binding_uc_2018_06_03.dat";
    else bindingUC = getNewestFile(directory,FILE_OUTPUT_BINDING_UC);

    if (bindingUC.isEmpty()){
        log.appendWarning("WARNING: No binding UC file found");
        binding = false;
    }
    else{
        txDP.addFile(bindingUC,DataPacket::DPFI_BINDING_UC);
    }
    if (!reading && !binding){
        log.appendError("ERROR: No processing is possible due to missing files.");
        sendFinishedSignal(false);
        return;
    }

    // Creating the packet to send to the server.
    txDP.addString(config->getString(CONFIG_DOCTOR_UID),DataPacket::DPFI_DOCTOR_ID);
    txDP.addString(config->getString(CONFIG_PATIENT_UID),DataPacket::DPFI_PATIENT_ID);
    txDP.addValue(config->getInt(CONFIG_VALID_EYE),DataPacket::DPFI_VALID_EYE);

    // Requesting connection and ack
    informationSent = false;
    socket->connectToHostEncrypted(config->getString(CONFIG_SERVER_ADDRESS),TCP_PORT_DATA_PROCESSING);

    // Starting timeout timer.
    clientState = CS_CONNECTING;
    startTimeoutTimer(config->getInt(CONFIG_CONNECTION_TIMEOUT)*1000);
}

void SSLDataProcessingClient::on_encryptedSuccess(){
    if (clientState == CS_CONNECTING){
        log.appendSuccess("Established encrypted connection to the server. Waiting for acknowledge ... ");
        rxDP.clearAll();
        timer.stop();
        clientState = CS_WAIT_FOR_ACK;
        startTimeoutTimer(config->getInt(CONFIG_DATA_REQUEST_TIMEOUT)*1000);
    }
    else {
        log.appendError("Invalid state upon encrypted success closing off the connection");
        socket->disconnectFromHost();
        clientState = CS_CONNECTING;
        sendFinishedSignal(false);
        return;
    }
}

void SSLDataProcessingClient::on_readyRead(){

    if (clientState == CS_CONNECTING) return;

    quint8 errcode = rxDP.bufferByteArray(socket->readAll());

    if (errcode == DataPacket::DATABUFFER_RESULT_DONE){

        if (clientState == CS_WAIT_FOR_ACK){

            if (rxDP.hasInformationField(DataPacket::DPFI_SEND_INFORMATION)){
                // Since in the information was requested, it is now sent.
                QByteArray ba = txDP.toByteArray();
                qint64 num = socket->write(ba.constData(),ba.size());
                if (num != ba.size()){
                    log.appendError("ERROR: Unable to send the information. Please try again.");
                    socket->disconnectFromHost();
                    sendFinishedSignal(false);
                    return;
                }
                log.appendStandard("LOG: Sent requested information to server: " + QString::number(num) + " bytes");
                rxDP.clearBufferedData();
                clientState = CS_WAIT_FOR_REPORT;
                timer.stop();
                startTimeoutTimer(config->getInt(CONFIG_WAIT_REPORT_TIMEOUT)*1000);
            }
            else{
                // This is not it and it should start again.
                rxDP.clearBufferedData();
                return;
            }

        }
        else{ // We are waiting for a report

            if (!rxDP.isInformationFieldOfType(DataPacket::DPFI_REPORT,DataPacket::DPFT_FILE)){
                rxDP.clearBufferedData();
                return;
            }
            timer.stop();

            // At this point the report was received so it is saved
            QString reportPath = rxDP.saveFile(config->getString(CONFIG_PATIENT_DIRECTORY),DataPacket::DPFI_REPORT);
            if (reportPath.isEmpty()){
                log.appendError("ERROR: Could not save the report to the directory: " + reportPath + ". Please try again");
                sendFinishedSignal(false);
            }
            else{
                log.appendSuccess("Report saved to: " + reportPath);
                config->addKeyValuePair(CONFIG_REPORT_PATH,reportPath);
                sendFinishedSignal(true);
            }

            rxDP.clearAll();
            socket->disconnectFromHost();
        }

    }
    else if (errcode == DataPacket::DATABUFFER_RESULT_ERROR){
        log.appendError("ERROR: Buffering data from the receiver");
        rxDP.clearAll();
        socket->disconnectFromHost();
    }

}


//************************************* Socket and SSL Errors, Socket state changes ****************************************

void SSLDataProcessingClient::on_socketError(QAbstractSocket::SocketError error){
    SSLClient::on_socketError(error);
    sendFinishedSignal(false);
}

//*************************************************************************************************************************

void SSLDataProcessingClient::on_timeOut(){
    switch(clientState){
    case CS_CONNECTING:
        log.appendError("ERROR: Server connection notice did not arrive before the expected time. Closing connection. Please retry.");
        emit(transactionFinished(false));
        break;
    case CS_WAIT_FOR_REPORT:
        log.appendError("ERROR: Server generated report did not arrive before expected time. Closing connection. Please retry.");
        emit(transactionFinished(false));
        break;
    case CS_WAIT_FOR_ACK:
        log.appendError("ERROR: Server request for information did not arrive before expected time. Closing connection. Please retry.");
        emit(transactionFinished(false));
        break;
    }
    timer.stop();
    socket->disconnectFromHost();
}

SSLDataProcessingClient::~SSLDataProcessingClient()
{

}

