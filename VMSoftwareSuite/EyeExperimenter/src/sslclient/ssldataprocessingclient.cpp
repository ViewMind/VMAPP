#include "ssldataprocessingclient.h"

SSLDataProcessingClient::SSLDataProcessingClient(QObject *parent, ConfigurationManager *c):SSLClient(parent,c)
{
    clientState = CS_CONNECTING;
}

void SSLDataProcessingClient::requestReport(bool isDemoMode, const QString &oldRepFile){
    transactionIsOk = false;
    connectToServer(isDemoMode,oldRepFile);
}

void SSLDataProcessingClient::connectToServer(bool isDemoMode, const QString &oldRepFile)
{
    QString patientDirectory = config->getString(CONFIG_PATIENT_DIRECTORY);
    QString datDirectory = patientDirectory;
    QString confFile = patientDirectory + "/" + QString(FILE_EYE_REP_GEN_CONFIGURATION);

    // If oldRepFile is not empty, then this is a reprocessing request and therefore the files are in the processed_data folder.
    if (!oldRepFile.isEmpty()) {
        previousReportFile = oldRepFile;
        datDirectory = datDirectory + "/" + QString(DIRNAME_PROCESSED_DATA);
    }
    else previousReportFile = "";

    // -1 Is a timeout code.
    processingACKCode = -1;
    numberOfEvals = -2;

    ConfigurationManager eyeGenConf;
    if (!eyeGenConf.loadConfiguration(confFile,COMMON_TEXT_CODEC)){
        log.appendError("Failed to load eye report configuration file: " + eyeGenConf.getError());
        emit(transactionFinished());
        return;
    }

    txDP.clearAll();
    QString hash = QCryptographicHash::hash(config->getString(CONFIG_PATIENT_UID).toLatin1(),QCryptographicHash::Sha3_512).toHex();
    txDP.addString(hash,DataPacket::DPFI_PATIENT_ID);
    txDP.addValue(REQUEST_CODE_PROCESS_REPORT,DataPacket::DPFI_REQUEST_FIELD);
    txDP.addValue(config->getInt(CONFIG_INST_UID),DataPacket::DPFI_DB_INST_UID);
    txDP.addString(config->getString(CONFIG_INST_ETSERIAL),DataPacket::DPFI_DB_ET_SERIAL);
    txDP.addFile(confFile,DataPacket::DPFI_CONF_FILE);

    if (!txDP.addFile(patientDirectory + "/" + QString(FILE_PATDATA_DB),DataPacket::DPFI_PATIENT_FILE)){
        log.appendError("Failed to load patient data file: " + patientDirectory + "/" + QString(FILE_PATDATA_DB));
        emit(transactionFinished());
        return;
    }
    if (!txDP.addFile(patientDirectory + "/" + QString(FILE_DOCDATA_DB),DataPacket::DPFI_DOCTOR_FILE)){
        log.appendError("Failed to load doctor data file: " + patientDirectory + "/" + QString(FILE_DOCDATA_DB));
        emit(transactionFinished());
        return;
    }

    if (eyeGenConf.containsKeyword(CONFIG_FILE_BIDING_BC)){
        if (!isDemoMode) {
            if (!txDP.addFile(datDirectory + "/" + eyeGenConf.getString(CONFIG_FILE_BIDING_BC),DataPacket::DPFI_BINDING_BC)){
                log.appendError("On creating the DataPacket, could not get Binding BC File: " + patientDirectory + "/" + eyeGenConf.getString(CONFIG_FILE_BIDING_BC));
            }
        }
        else txDP.addFile(":/demo_data/binding_bc_2_l_2_2010_06_03_10_00.dat",DataPacket::DPFI_BINDING_BC);
    }
    if (eyeGenConf.containsKeyword(CONFIG_FILE_BIDING_UC)){
        if (!isDemoMode) {
            if (!txDP.addFile(datDirectory + "/" + eyeGenConf.getString(CONFIG_FILE_BIDING_UC),DataPacket::DPFI_BINDING_UC)){
                log.appendError("On creating the DataPacket, could not get Binding UC File: " + patientDirectory + "/" + eyeGenConf.getString(CONFIG_FILE_BIDING_UC));
            }
        }
        else txDP.addFile(":/demo_data/binding_uc_2_l_2_2010_03_06_10_03.dat",DataPacket::DPFI_BINDING_UC);
    }
    if (eyeGenConf.containsKeyword(CONFIG_FILE_READING)){
        if (!isDemoMode) {
            if(!txDP.addFile(datDirectory + "/" + eyeGenConf.getString(CONFIG_FILE_READING),DataPacket::DPFI_READING)){
                log.appendError("On creating the DataPacket, could not get Reading File: " + patientDirectory + "/" + eyeGenConf.getString(CONFIG_FILE_READING));
            }
        }
        else txDP.addFile(":/demo_data/reading_2_2010_06_03_10_15.dat",DataPacket::DPFI_READING);
    }
    if (eyeGenConf.containsKeyword(CONFIG_FILE_FIELDING)){
        if (!isDemoMode) {
            if(!txDP.addFile(datDirectory + "/" + eyeGenConf.getString(CONFIG_FILE_FIELDING),DataPacket::DPFI_FIELDING)){
                log.appendError("On creating the DataPacket, could not get Fielding File: " + patientDirectory + "/" + eyeGenConf.getString(CONFIG_FILE_FIELDING));
            }
        }
        // TODO Add Fielding DEMO!
        // else txDP.addFile(":/demo_data/reading_2_2010_06_03_10_15.dat",DataPacket::DPFI_READING);
    }

    // Requesting connection and ack
    socket->connectToHostEncrypted(config->getString(CONFIG_SERVER_ADDRESS),TCP_PORT_DATA_PROCESSING);

    // Starting timeout timer.
    clientState = CS_CONNECTING;
    startTimeoutTimer(config->getInt(CONFIG_CONNECTION_TIMEOUT)*1000);
}

void SSLDataProcessingClient::sendMedicalRecordData(const QString &patuid){

    QString patientDirectory = QString(DIRNAME_RAWDATA) + "/" + patuid;
    QString confFile = patientDirectory + "/" + QString(FILE_EYE_REP_GEN_CONFIGURATION);

    // -1 Is a timeout code.
    processingACKCode = -1;
    transactionIsOk = false;

    txDP.clearAll();
    QString hash = QCryptographicHash::hash(patuid.toLatin1(),QCryptographicHash::Sha3_512).toHex();
    txDP.addValue(REQUEST_CODE_SAVE_PATIENT_DATA,DataPacket::DPFI_REQUEST_FIELD);
    txDP.addString(hash,DataPacket::DPFI_PATIENT_ID);
    txDP.addValue(config->getInt(CONFIG_INST_UID),DataPacket::DPFI_DB_INST_UID);
    txDP.addString(config->getString(CONFIG_INST_ETSERIAL),DataPacket::DPFI_DB_ET_SERIAL);
    txDP.addFile(confFile,DataPacket::DPFI_CONF_FILE);

    if (!txDP.addFile(patientDirectory + "/" + QString(FILE_PATDATA_DB),DataPacket::DPFI_PATIENT_FILE)){
        log.appendError("Failed to load patient data file: " + patientDirectory + "/" + QString(FILE_PATDATA_DB));
        emit(transactionFinished());
        return;
    }
    if (!txDP.addFile(patientDirectory + "/" + QString(FILE_DOCDATA_DB),DataPacket::DPFI_DOCTOR_FILE)){
        log.appendError("Failed to load doctor data file: " + patientDirectory + "/" + QString(FILE_DOCDATA_DB));
        emit(transactionFinished());
        return;
    }
    if (!txDP.addFile(patientDirectory + "/" + QString(FILE_MEDRECORD_DB),DataPacket::DPFI_DB_MEDICAL_RECORD_FILE)){
        log.appendError("Failed to load medical record data file: " + patientDirectory + "/" + QString(FILE_MEDRECORD_DB));
        emit(transactionFinished());
        return;
    }

    // Requesting connection and ack
    socket->connectToHostEncrypted(config->getString(CONFIG_SERVER_ADDRESS),TCP_PORT_DATA_PROCESSING);

    // Starting timeout timer.
    clientState = CS_CONNECTING_FOR_MEDREPORT;
    startTimeoutTimer(config->getInt(CONFIG_CONNECTION_TIMEOUT)*1000);

}

void SSLDataProcessingClient::on_encryptedSuccess(){
    timer.stop();
    if ((clientState == CS_CONNECTING) || (clientState == CS_CONNECTING_FOR_MEDREPORT)){
        log.appendStandard("Established encrypted connection to the server. Sending the information ");
        rxDP.clearAll();
        if (clientState == CS_CONNECTING) clientState = CS_WAIT_FOR_REPORT;
        else clientState = CS_WAIT_FOR_MEDREP_OK;
        QByteArray ba = txDP.toByteArray();
        qint64 num = socket->write(ba.constData(),ba.size());
        if (num != ba.size()){
            log.appendError("Unable to send the information to the server");
            socket->disconnectFromHost();
            return;
        }
        log.appendStandard("Sent requested information to server: " + QString::number(num) + " bytes");
        timer.stop();
        startTimeoutTimer(config->getInt(CONFIG_WAIT_REPORT_TIMEOUT)*1000);
    }
    else {
        log.appendError("Invalid state upon encrypted success closing off the connection");
        socket->disconnectFromHost();
        clientState = CS_CONNECTING;
        return;
    }
}

void SSLDataProcessingClient::on_readyRead(){

    if ((clientState != CS_WAIT_FOR_REPORT) && (clientState != CS_WAIT_FOR_MEDREP_OK)) return;

    quint8 errcode = rxDP.bufferByteArray(socket->readAll());

    if (errcode == DataPacket::DATABUFFER_RESULT_DONE){

        timer.stop();
        if (!rxDP.hasInformationField(DataPacket::DPFI_PROCESSING_ACK)){
            log.appendWarning("Received packet with no information field");
            rxDP.clearBufferedData();
            return;
        }

        timer.stop();
        processingACKCode = rxDP.getField(DataPacket::DPFI_PROCESSING_ACK).data.toInt();

        if (processingACKCode != EYESERVER_RESULT_OK){
            log.appendError("Received a NOT Ok value from EyeServer: " + QString::number(processingACKCode));
            rxDP.clearAll();
            socket->disconnectFromHost();
            return;
        }

        if (clientState == CS_WAIT_FOR_MEDREP_OK){
            // There is nothing eles to do.
            clientState = CS_CONNECTING;
            rxDP.clearAll();
            transactionIsOk = true;
            socket->disconnectFromHost();
            return;
        }

        if (!rxDP.isInformationFieldOfType(DataPacket::DPFI_REPORT,DataPacket::DPFT_FILE)){
            log.appendError("Received packet with no Processing error and No Report field");
            rxDP.clearBufferedData();
            socket->disconnectFromHost();
            return;
        }

        if (rxDP.hasInformationField(DataPacket::DPFI_NUM_EVALS)){
            numberOfEvals = rxDP.getField(DataPacket::DPFI_NUM_EVALS).data.toInt();
            if (numberOfEvals < -1) numberOfEvals = -2;
        }
        else log.appendError("No information about number of evaluations received");

        // If this is a reprocessed file we need to move it first to the old reports folder
        if (!previousReportFile.isEmpty()){

            QString oldRepFile = config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + previousReportFile;

            //qWarning() << "OLDREPFILE ON RETURN" << "|" + previousReportFile + "|";

            if (!previousReportFile.isEmpty()){
                QString oldRepDir = config->getString(CONFIG_PATIENT_DIRECTORY) + "/" + QString(DIRNAME_OLD_REP);

                QDir patientDir(config->getString(CONFIG_PATIENT_DIRECTORY));
                if (!QDir(oldRepDir).exists()){
                    if (!patientDir.mkdir(DIRNAME_OLD_REP)){
                        log.appendError("Could not create old report directory: " + oldRepDir);
                        rxDP.clearAll();
                        socket->disconnectFromHost();
                        return;
                    }
                }

                // Checking that file exists
                if (QFile(oldRepFile).exists()){
                    // The file exist so it needs to me moved to the old rep dir
                    QString destinationFile = oldRepDir + "/" + previousReportFile + "." + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm")  ;
                    if (!QFile::copy(oldRepFile,destinationFile)){
                        log.appendError("Could not copy file " + oldRepFile + " to " + destinationFile + ". Cannto save reprocessed report");
                        rxDP.clearAll();
                        socket->disconnectFromHost();
                        return;
                    }
                }
                else{
                    // This should not happen. However as the report does not exists then there is nothing to do.
                    log.appendWarning("Report " + oldRepFile + " should be a reprocessed report, however the file does not exist");
                }
            }

        }

        // At this point the report was received and the processing code was ok.
        QString reportPath = rxDP.saveFile(config->getString(CONFIG_PATIENT_DIRECTORY),DataPacket::DPFI_REPORT);
        if (reportPath.isEmpty()){
            log.appendError("Could not save the report to the directory: " + reportPath + ". Please try again");
        }
        else{
            log.appendStandard("Report saved to: " + reportPath);
            config->addKeyValuePair(CONFIG_REPORT_PATH,reportPath);
            transactionIsOk = true;
        }

        rxDP.clearAll();

        // Sending OK to Server.
        txDP.clearAll();
        txDP.addString("OK",DataPacket::DPFI_PROCESSING_ACK);
        QByteArray ba = txDP.toByteArray();
        qint64 num = socket->write(ba.constData(),ba.size());
        if (num != ba.size()){
            log.appendError("Unable to send the report received to the server");
        }

        clientState = CS_CONNECTING;
        rxDP.clearAll();
        socket->disconnectFromHost();
    }
    else if (errcode == DataPacket::DATABUFFER_RESULT_ERROR){
        timer.stop();
        log.appendError("Buffering data from the receiver");
        clientState = CS_CONNECTING;
        rxDP.clearAll();
        socket->disconnectFromHost();
    }

}


//************************************* Socket and SSL Errors, Socket state changes ****************************************

void SSLDataProcessingClient::on_socketError(QAbstractSocket::SocketError error){
    SSLClient::on_socketError(error);
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    log.appendError("Disconnecting on socket error: " + QString(metaEnum.valueToKey(error)) + " Transaction incomplete.");
    socket->disconnectFromHost();
}

//*************************************************************************************************************************

void SSLDataProcessingClient::on_timeOut(){
    timer.stop();
    processingACKCode = -1;
    switch(clientState){
    case CS_CONNECTING:
        log.appendError("Server connection notice did not arrive before the expected time. Closing connection");
        break;
    case CS_WAIT_FOR_REPORT:
        log.appendError("Server generated report did not arrive before expected time. Closing connection");
        break;
    default:
        log.appendError("SSLDataProcessing Client: Time out called in unexpected state " + QString::number(clientState));
        break;
    }    
    socket->disconnectFromHost();
}

SSLDataProcessingClient::~SSLDataProcessingClient()
{

}

