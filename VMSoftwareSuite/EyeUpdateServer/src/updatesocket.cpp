#include "updatesocket.h"

UpdateSocket::UpdateSocket(QSslSocket *newSocket, quint64 id):SSLIDSocket(newSocket,id)
{
    QString transactionID = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + "_TID" + QString::number(id);

    // Customized log file.
    log.setLogFileLocation(QString(DIRNAME_SERVER_LOGS) + "/" + transactionID);
    log.appendStandard("Created connection id: " + transactionID + " with address " + sslSocket->peerAddress().toString());
    finishedUpdatingProcess = false;
    timer.setInterval(CONNECTION_TIMEOUT);
    timer.start();


}

void UpdateSocket::on_timeout(){
    SSLIDSocket::on_timeout();
    log.appendError("Reached timeout and the information has not arrived. disconnecocting");
    on_disconnected();
}

void UpdateSocket::doDisconnects(){
    SSLIDSocket::doDisconnects();
}

void UpdateSocket::on_encryptedSuccess(){
    SSLIDSocket::on_encryptedSuccess();
    log.appendStandard("Established encrypted connections waiting for data to be sent");
    timer.start();
}

void UpdateSocket::on_readyRead(){
    QByteArray ba = socket()->readAll();
    quint8 ans = rx.bufferByteArray(ba);
    if (ans == DataPacket::DATABUFFER_RESULT_DONE){
        timer.stop();
        if (rx.hasInformationField(DataPacket::DPFI_UPDATE_EYEEXP_ID)) processUpdateRequest();
        else {
            log.appendError("Non update request reached the server, it is ignored");
            finishedUpdatingProcess = true;
            on_disconnected();
        }
    }
    else if (ans == DataPacket::DATABUFFER_RESULT_ERROR){
        timer.stop();
        log.appendError("Finshed receiving data but it was corrupted.");
        on_disconnected();
    }
}

void UpdateSocket::on_disconnected(){
    SSLIDSocket::on_disconnected();
    doDisconnects();
    if (!finishedUpdatingProcess){
        log.appendError("Updating was not finished correctly");
    }
    else{
        log.appendStandard("Finsihed update processing. Requesting socket deletion....");
    }
    emit(socketDone(ID));
}

void UpdateSocket::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    log.appendError(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
}

void UpdateSocket::on_socketStateChanged(QAbstractSocket::SocketState state){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    log.appendStandard(QString("SOCKET STATE: ") + metaEnum.valueToKey(state));
}

void UpdateSocket::on_sslErrors(const QList<QSslError> &errorlist){
    QHostAddress addr = sslSocket->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        log.appendWarning("SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }
}

UpdateSocket::~UpdateSocket(){
    timer.stop();
    if (sslSocket != nullptr) delete sslSocket;
}

void UpdateSocket::processUpdateRequest(){

    QString inst_uid = rx.getField(DataPacket::DPFI_DB_INST_UID).data.toString();
    QString eyeexp_number = rx.getField(DataPacket::DPFI_UPDATE_EYEEXP_ID).data.toString();

    // Verifying that the directory exists
    QString basePath = QString(DIRNAME_UPDATE_DIR) + "/" + inst_uid + "/" + eyeexp_number;
    if (!QDir(basePath).exists()){
        log.appendError("Could not find update directory: " + basePath);
        sendUpdateAns(DataPacket(),"FAILED");
        return;
    }

    log.appendStandard("Update request received for Institution: " + inst_uid + " Instance Number: " + eyeexp_number);

    // Verifying that the log directories exists.
    QDir baseDir(basePath);
    QString logDirPath = basePath + "/" + QString(DIRNAME_UPDATE_DIR_LOG_SUBIDR);
    QString flogDirPath = basePath + "/" + QString(DIRNAME_UPDATE_DIR_FLOGS_SUBIDR);
    baseDir.mkdir(DIRNAME_UPDATE_DIR_LOG_SUBIDR);
    baseDir.mkdir(DIRNAME_UPDATE_DIR_FLOGS_SUBIDR);
    if (!QDir(logDirPath).exists()){
        log.appendError("Could not create LOG Directory: " + logDirPath);
        sendUpdateAns(DataPacket(),"FAILED");
        return;
    }
    if (!QDir(flogDirPath).exists()){
        log.appendError("Could not create FLOG Directory: " + flogDirPath);
        sendUpdateAns(DataPacket(),"FAILED");
        return;
    }


    // Saving the log files
    QString timestamp = "." + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    rx.saveFile(logDirPath,DataPacket::DPFI_UPDATE_LOGFILE,timestamp);

    // Saving the local DB backup, if it exists
    if (rx.hasInformationField(DataPacket::DPFI_LOCAL_DB_BKP)){

        log.appendStandard("Local DB BKP Enabled");

        // Deleting the backup temp file if it exists
        QString tempFileName = basePath + "/" + QString(FILE_LOCAL_DB);
        QFile tempFile(tempFileName);
        tempFile.remove();
        if (tempFile.exists()){
            log.appendError("Could not delete de temporary local db backup: " + tempFileName);
        }
        else{
            // Temporary file was removed, saving the file locally.
            QString savedFile = rx.saveFile(basePath,DataPacket::DPFI_LOCAL_DB_BKP);
            if (savedFile != ""){
                // Removing the actual bkp file
                QString bkpFileName = basePath + "/" + QString(FILE_LOCAL_DB_BKP);
                QFile bkpFile(bkpFileName);
                bkpFile.remove();
                if (bkpFile.exists()){
                    log.appendError("Failed to remove the local DB bkp file: " + bkpFileName);
                }
                else{
                    if (!QFile::copy(tempFileName,bkpFileName)){
                        log.appendError("Could not copy temporary bkp file " + tempFileName + " to actual local db backup: " + bkpFileName);
                    }
                    else log.appendStandard("Local BKP saved sucessfully to: " + savedFile);
                }
            }
            else{
                log.appendError("Failed saving the temporary file: " + tempFileName);
            }
        }
    }

    // Checking the hashes sent.
    DataPacket tx;
    QString eyeexehash      = rx.getField(DataPacket::DPFI_UPDATE_EYEEXP).data.toString();
    QString confighash      = rx.getField(DataPacket::DPFI_UPDATE_CONFIG).data.toString();
    QString eyelauncherhash = rx.getField(DataPacket::DPFI_UPDATE_EYELAUNCHER).data.toString();

    QString err = "";
    QString hash = DataPacket::getFileHash(FILENAME_EYE_LAUNCHER,&err);
    if (hash.isEmpty()){
        log.appendError("Could not compute hash for the launcher.exe: " + err);
    }
    else if ((hash != eyelauncherhash) && (!eyelauncherhash.isEmpty())){
        if (!tx.addFile(FILENAME_EYE_LAUNCHER,DataPacket::DPFI_UPDATE_EYELAUNCHER)){
            log.appendError("Could not add local eyelauncher to send back");;
        }
        else log.appendStandard("Added launcher.exe to response");
    }

    err = "";
    QString hashFilePath = basePath + "/" + QString(FILENAME_CONFIGURATION);
    hash = DataPacket::getFileHash(hashFilePath,&err);
    if (hash.isEmpty()){
        log.appendError("Could not compute hash for the local configuration file on path: " + hashFilePath + ": " + err);
    }
    else if ((hash != confighash) && !confighash.isEmpty()){
        if (!tx.addFile(hashFilePath,DataPacket::DPFI_UPDATE_CONFIG)){
            log.appendError("Could not add local configuration to send back: " + hashFilePath);
        }
        else log.appendStandard("Added configuration to response");
    }

    err = "";
    hashFilePath = basePath + "/" + QString(FILENAME_EYE_EXPERIMENTER);
    hash = DataPacket::getFileHash(hashFilePath,&err);
    if (hash.isEmpty()){
        log.appendError("Could not compute hash for the local eye experimetner file on path: " + hashFilePath + ": " + err);
    }
    else if ((hash != eyeexehash) && !eyeexehash.isEmpty()){
        if (!tx.addFile(hashFilePath,DataPacket::DPFI_UPDATE_EYEEXP)){
            log.appendError("Could not add local eye experimenter to send back: " + hashFilePath);
        }
        else log.appendStandard("Added EyeExperimenter.exe to response");

        // Getting the laanguage in order to return the change log.
        QString lang = rx.getField(DataPacket::DPFI_UPDATE_LANG).data.toString();
        QString changeLogFilepath = basePath + "/" + QString(FILENAME_CHANGELOG) + "_" + lang;
        if (!tx.addFile(changeLogFilepath,DataPacket::DPFI_UPDATE_CHANGES)){
            log.appendError("Could not add local change log to send back: " + changeLogFilepath);
        }
        else log.appendStandard("Added changelog to response");
    }

    sendUpdateAns(tx,"OK");

}

void UpdateSocket::sendUpdateAns(DataPacket tx, const QString &ans){
    tx.addString(ans,DataPacket::DPFI_UPDATE_RESULT);
    QByteArray ba = tx.toByteArray();
    qint64 num = sslSocket->write(ba.constData(),ba.size());
    if (num != ba.size()){
        log.appendError("Failure sending update");
    }
    else log.appendStandard("Added updated result and packet was sent");
    if (ans == "OK") finishedUpdatingProcess = true;
    on_disconnected();
}

