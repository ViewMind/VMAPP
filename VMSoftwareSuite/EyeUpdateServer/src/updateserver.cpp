#include "updateserver.h"

UpdateServer::UpdateServer(QObject *parent) : QObject(parent)
{

    // Creating the the listner.
    listener = new SSLListener(this);

    // Listen for new connections.
    connect(listener,&SSLListener::newConnection,this,&UpdateServer::on_newConnection);
    connect(listener,&SSLListener::lostConnection,this,&UpdateServer::on_lostConnection);

    idGen = 0;

}


bool UpdateServer::startServer(ConfigurationManager *c){

    config = c;
    if (!listener->listen(QHostAddress::Any,TCP_PORT_UPDATE_SERVER)){
        log.appendError("ERROR : Could not start SQL SSL Server: " + listener->errorString());
        return false;
    }

    return true;

}

// For now, this function is not used.
void UpdateServer::on_lostConnection(){
}

void UpdateServer::on_newConnection(){

    // New connection is available.
    QSslSocket *sslsocket = (QSslSocket*)(listener->nextPendingConnection());
    SSLIDSocket *socket = new SSLIDSocket(sslsocket,idGen,config->getString(CONFIG_S3_ADDRESS));

    //log.appendStandard("New connection with id " + QString::number(idGen));

    idGen++;

    if (!socket->isValid()) {
        log.appendError("ERROR: Could not cast incomming socket connection");
        return;
    }

    // Saving the socket.
    sockets.addSocket(socket);

    // Doing the connection SIGNAL-SLOT
    connect(socket,&SSLIDSocket::sslSignal,this,&UpdateServer::on_newSSLSignal);

    log.appendStandard("New update request from " + sslsocket->peerAddress().toString());

    // The SSL procedure.
    sslsocket->setPrivateKey(":/certificates/server.key");
    sslsocket->setLocalCertificate(":/certificates/server.csr");
    sslsocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslsocket->startServerEncryption();

}


// Handling all signals
void UpdateServer::on_newSSLSignal(quint64 socket, quint8 signaltype){

    QString where = "on_newSSLSignal: " + SSLIDSocket::SSLSignalToString(signaltype);
    SSLIDSocket *sslsocket = sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(socket,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        return;
    }

    switch (signaltype){
    case SSLIDSocket::SSL_SIGNAL_DISCONNECTED:
        log.appendStandard("Lost connection from: " + sslsocket->socket()->peerAddress().toString());
        sockets.deleteSocket(socket,where);
        break;
    case SSLIDSocket::SSL_SIGNAL_ENCRYPTED:
        log.appendSuccess("SSL Handshake completed for address: " + sslsocket->socket()->peerAddress().toString());
        sslsocket->startTimeoutTimer(config->getInt(CONFIG_DATA_REQUEST_TIMEOUT)*1000);
        break;
    case SSLIDSocket::SSL_SIGNAL_SOCKET_ERROR:
        socketErrorFound(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_DATA_RX_DONE:
        // Information has arrived ok. Starting the process.
        //log.appendStandard("Done buffering data for " + QString::number(socket));
        sslsocket->stopTimer();
        if (sslsocket->getDataPacket().hasInformationField(DataPacket::DPFI_UPDATE_EYEEXP_ID)) processUpdateRequest(socket);
        else {
            log.appendError("Non update request reached the server, it is ignored, from " + sslsocket->socket()->peerAddress().toString());
            sockets.deleteSocket(socket,where);
        }
        break;
    case SSLIDSocket::SSL_SIGNAL_DATA_RX_ERROR:
        sslsocket->stopTimer();
        // Data is most likely corrupted.
        sslsocket->stopTimer();
        log.appendError("Buffering data from: " + sslsocket->socket()->peerAddress().toString() + " gave an error");
        sockets.deleteSocket(socket,where);
        break;
    case SSLIDSocket::SSL_SIGNAL_TIMEOUT:
        // This means that the data did not arrive on time.
        sslsocket->stopTimer();
        log.appendError("Data from " + sslsocket->socket()->peerAddress().toString() + " did not arrive in time. SocketID: " + QString::number(socket));
        sockets.deleteSocket(socket,where);
        break;
    case SSLIDSocket::SSL_SIGNAL_PROCESS_DONE:
        //This signal should never be emitted here.
        log.appendError("Got a processing done signal. This should never happen in this server. From " + sslsocket->socket()->peerAddress().toString());
        sockets.deleteSocket(socket,where);
        break;
    case SSLIDSocket::SSL_SIGNAL_SSL_ERROR:
        sslErrorsFound(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_STATE_CHANGED:
        changedState(socket);
        break;
    }
    sockets.releaseSocket(socket,where);
}


void UpdateServer::socketErrorFound(quint64 id){

    QString where = "socketErrorFound: ";
    SSLIDSocket *sslsocket = sockets.getSocketLock(id,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(id,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(id));
        return;
    }

    QAbstractSocket::SocketError error = sslsocket->socket()->error();
    QHostAddress addr = sslsocket->socket()->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    if (error != QAbstractSocket::RemoteHostClosedError){
        log.appendError(QString("Socket Error found: ") + metaEnum.valueToKey(error) + QString(" from Address: ") + addr.toString());
        sockets.deleteSocket(id,where);
    }
    // Eliminating it from the list.
    sockets.releaseSocket(id,where);

}

void UpdateServer::sslErrorsFound(quint64 id){

    QString where = "sslErrorsFound: ";
    SSLIDSocket *sslsocket = sockets.getSocketLock(id,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(id,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(id));
        return;
    }

    QList<QSslError> errorlist = sslsocket->socket()->sslErrors();
    QHostAddress addr = sslsocket->socket()->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        log.appendError("SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }

    // Eliminating it from the list.
    sockets.deleteSocket(id,where);
    sockets.releaseSocket(id,where);

}

void UpdateServer::processUpdateRequest(quint64 socket){

    QString where = "processUpdateRequest: ";
    SSLIDSocket *sslsocket = sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(socket,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        return;
    }

    QString inst_uid = sslsocket->getDataPacket().getField(DataPacket::DPFI_DB_INST_UID).data.toString();
    QString eyeexp_number = sslsocket->getDataPacket().getField(DataPacket::DPFI_UPDATE_EYEEXP_ID).data.toString();

    // Verifying that the directory exists
    QString basePath = QString(DIRNAME_UPDATE_DIR) + "/" + inst_uid + "/" + eyeexp_number;
    if (!QDir(basePath).exists()){
        log.appendError("Could not find update directory: " + basePath);
        sendUpdateAns(DataPacket(),socket,"FAILED");
        sockets.releaseSocket(socket,where);
        return;
    }


    // Verifying that the log directories exists.
    QDir baseDir(basePath);
    QString logDirPath = basePath + "/" + QString(DIRNAME_UPDATE_DIR_LOG_SUBIDR);
    QString flogDirPath = basePath + "/" + QString(DIRNAME_UPDATE_DIR_FLOGS_SUBIDR);
    baseDir.mkdir(DIRNAME_UPDATE_DIR_LOG_SUBIDR);
    baseDir.mkdir(DIRNAME_UPDATE_DIR_FLOGS_SUBIDR);
    if (!QDir(logDirPath).exists()){
        log.appendError("Could not create LOG Directory: " + logDirPath);
        sendUpdateAns(DataPacket(),socket,"FAILED");
        sockets.releaseSocket(socket,where);
        return;
    }
    if (!QDir(flogDirPath).exists()){
        log.appendError("Could not create FLOG Directory: " + flogDirPath);
        sendUpdateAns(DataPacket(),socket,"FAILED");
        sockets.releaseSocket(socket,where);
        return;
    }


    // Saving the log files
    QString timestamp = "." + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss");
    sslsocket->getDataPacket().saveFile(logDirPath,DataPacket::DPFI_UPDATE_LOGFILE,timestamp);

    // Saving the local DB backup, if it exists
    if (sslsocket->getDataPacket().hasInformationField(DataPacket::DPFI_LOCAL_DB_BKP)){

        // Deleting the backup temp file if it exists
        QString tempFileName = basePath + "/" + QString(FILE_LOCAL_DB);
        QFile tempFile(tempFileName);
        tempFile.remove();
        if (tempFile.exists()){
            log.appendError("Could not delete de temporary local db backup: " + tempFileName);
        }
        else{
            // Temporary file was removed, saving the file locally.
            QString savedFile = sslsocket->getDataPacket().saveFile(basePath,DataPacket::DPFI_LOCAL_DB_BKP);
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
                }
            }
            else{
                log.appendError("Failed saving the temporary file: " + tempFileName);
            }
        }
    }

    // Checking the hashes sent.
    DataPacket tx;
    QString eyeexehash      = sslsocket->getDataPacket().getField(DataPacket::DPFI_UPDATE_EYEEXP).data.toString();
    QString confighash      = sslsocket->getDataPacket().getField(DataPacket::DPFI_UPDATE_CONFIG).data.toString();
    QString eyelauncherhash = sslsocket->getDataPacket().getField(DataPacket::DPFI_UPDATE_EYELAUNCHER).data.toString();

    QString hash = DataPacket::getFileHash(FILENAME_EYE_LAUNCHER);
    if (hash.isEmpty()){
        log.appendError("Could not compute hash for the local eyelauncher");
    }
    else if ((hash != eyelauncherhash) && (!eyelauncherhash.isEmpty())){
        if (!tx.addFile(FILENAME_EYE_LAUNCHER,DataPacket::DPFI_UPDATE_EYELAUNCHER)){
            log.appendError("Could not add local eyelauncher to send back");;
        }
    }

    QString hashFilePath = basePath + "/" + QString(FILENAME_CONFIGURATION);
    hash = DataPacket::getFileHash(hashFilePath);
    if (hash.isEmpty()){
        log.appendError("Could not compute hash for the local configuration file on path: " + hashFilePath);
    }
    else if ((hash != confighash) && !confighash.isEmpty()){
        if (!tx.addFile(hashFilePath,DataPacket::DPFI_UPDATE_CONFIG)){
            log.appendError("Could not add local configuration to send back: " + hashFilePath);
        }
    }

    hashFilePath = basePath + "/" + QString(FILENAME_EYE_EXPERIMENTER);
    hash = DataPacket::getFileHash(hashFilePath);
    if (hash.isEmpty()){
        log.appendError("Could not compute hash for the local eye experimetner file on path: " + hashFilePath);
    }
    else if ((hash != eyeexehash) && !eyeexehash.isEmpty()){
        if (!tx.addFile(hashFilePath,DataPacket::DPFI_UPDATE_EYEEXP)){
            log.appendError("Could not add local eye experimenter to send back: " + hashFilePath);
        }

        // Getting the laanguage in order to return the change log.
        QString lang = sslsocket->getDataPacket().getField(DataPacket::DPFI_UPDATE_LANG).data.toString();
        QString changeLogFilepath = basePath + "/" + QString(FILENAME_CHANGELOG) + "_" + lang;
        if (!tx.addFile(changeLogFilepath,DataPacket::DPFI_UPDATE_CHANGES)){
            log.appendError("Could not add local change log to send back: " + changeLogFilepath);
        }
    }

    sendUpdateAns(tx,socket,"OK");
    sockets.releaseSocket(socket,where);

}

void UpdateServer::sendUpdateAns(DataPacket tx, quint64 socket, const QString &ans){
    tx.addString(ans,DataPacket::DPFI_UPDATE_RESULT);
    QByteArray ba = tx.toByteArray();

    QString where = "DB sendUpdateAns: ";
    SSLIDSocket *sslsocket = sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(socket,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        return;
    }

    qint64 num = sslsocket->socket()->write(ba.constData(),ba.size());
    if (num != ba.size()){
        log.appendError("Failure sending update answer to: " + sslsocket->socket()->peerAddress().toString());
    }

    sockets.releaseSocket(socket,where);
}

void UpdateServer::changedState(quint64 id){

    Q_UNUSED(id)
    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    //    QAbstractSocket::SocketState state = sslsocket->socket()->state();
    //    QHostAddress addr = sslsocket->socket()->peerAddress();
    //    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    //    log.appendStandard(addr.toString() + ": " + QString("New socket state, ") + metaEnum.valueToKey(state));

}


