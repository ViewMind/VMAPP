#include "ssldataprocessingclient.h"

SSLDataProcessingClient::SSLDataProcessingClient(QObject *parent, ConfigurationManager *c):SSLClient(parent,c)
{
    clientState = CS_CONNECTING;
}


void SSLDataProcessingClient::sendFinishedSignal(bool okvalue){
    if (!sentTransactionFinishedSignal){
        sentTransactionFinishedSignal = true;
        emit(transactionFinished(okvalue));
    }
}

void SSLDataProcessingClient::requestReport(){
    sentTransactionFinishedSignal = false;
    connectToServer();
}

void SSLDataProcessingClient::connectToServer()
{
    QString directory = config->getString(CONFIG_PATIENT_DIRECTORY);
    QString confFile = directory + "/" + QString(FILE_EYE_REP_GEN_CONFIGURATION);

    ConfigurationManager eyeGenConf;
    if (!eyeGenConf.loadConfiguration(confFile,COMMON_TEXT_CODEC)){
        log.appendError("Failed to load eye report configuration file: " + eyeGenConf.getError());
        sendFinishedSignal(false);
        return;
    }

    txDP.clearAll();
    txDP.addString(config->getString(CONFIG_DOCTOR_UID),DataPacket::DPFI_DOCTOR_ID);
    txDP.addString(config->getString(CONFIG_PATIENT_UID),DataPacket::DPFI_PATIENT_ID);
    txDP.addFile(confFile,DataPacket::DPFI_PATIENT_FILE);

    if (eyeGenConf.containsKeyword(CONFIG_FILE_BIDING_BC))
        txDP.addFile(directory + "/" + eyeGenConf.containsKeyword(CONFIG_FILE_BIDING_BC),DataPacket::DPFI_BINDING_BC);
    if (eyeGenConf.containsKeyword(CONFIG_FILE_BIDING_UC))
        txDP.addFile(directory + "/" + eyeGenConf.containsKeyword(CONFIG_FILE_BIDING_UC),DataPacket::DPFI_BINDING_UC);
    if (eyeGenConf.containsKeyword(CONFIG_FILE_READING))
        txDP.addFile(directory + "/" + eyeGenConf.containsKeyword(CONFIG_FILE_READING),DataPacket::DPFI_READING);

    // adding the the demo mode.
    qreal demo;
    if (config->getBool(CONFIG_DEMO_MODE)) demo = 1;
    else demo = 0;

    /// TODO: FOR NOW NOTHING IS DEMO MODE. Change.
    txDP.addValue(1,DataPacket::DPFI_DEMO_MODE);

    // Requesting connection and ack
    informationSent = false;
    socket->connectToHostEncrypted(config->getString(CONFIG_SERVER_ADDRESS),TCP_PORT_DATA_PROCESSING);

    // Starting timeout timer.
    clientState = CS_CONNECTING;
    startTimeoutTimer(config->getInt(CONFIG_CONNECTION_TIMEOUT)*1000);
}

void SSLDataProcessingClient::on_encryptedSuccess(){
    if (clientState == CS_CONNECTING){
        log.appendStandard("LOG: Established encrypted connection to the server. Waiting for acknowledge ... ");
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
                    log.appendError("Unable to send the information. Please try again.");
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
                log.appendError("Could not save the report to the directory: " + reportPath + ". Please try again");
                sendFinishedSignal(false);
            }
            else{
                //log.appendSuccess("Report saved to: " + reportPath);
                config->addKeyValuePair(CONFIG_REPORT_PATH,reportPath);
                sendFinishedSignal(true);
            }

            rxDP.clearAll();
            socket->disconnectFromHost();
        }

    }
    else if (errcode == DataPacket::DATABUFFER_RESULT_ERROR){
        log.appendError("Buffering data from the receiver");
        rxDP.clearAll();
        socket->disconnectFromHost();
    }

}


//************************************* Socket and SSL Errors, Socket state changes ****************************************

void SSLDataProcessingClient::on_socketError(QAbstractSocket::SocketError error){
    SSLClient::on_socketError(error);
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    log.appendError("Disconnecting on socket error: " + QString(metaEnum.valueToKey(error)) + " Transaction incomplete.");
    sendFinishedSignal(false);
}

//*************************************************************************************************************************

void SSLDataProcessingClient::on_timeOut(){
    switch(clientState){
    case CS_CONNECTING:
        log.appendError("Server connection notice did not arrive before the expected time. Closing connection. Please retry.");
        emit(transactionFinished(false));
        break;
    case CS_WAIT_FOR_REPORT:
        log.appendError("Server generated report did not arrive before expected time. Closing connection. Please retry.");
        emit(transactionFinished(false));
        break;
    case CS_WAIT_FOR_ACK:
        log.appendError("Server request for information did not arrive before expected time. Closing connection. Please retry.");
        emit(transactionFinished(false));
        break;
    }
    timer.stop();
    socket->disconnectFromHost();
}

SSLDataProcessingClient::~SSLDataProcessingClient()
{

}

