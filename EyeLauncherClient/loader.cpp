#include "loader.h"

Loader::Loader(QObject *parent) : QObject(parent)
{

    // Attempting to load the settings file to figure out the language.
    ConfigurationManager cmng;
    QString langfile = ":/languages/en.lang";
    if (cmng.loadConfiguration(FILE_EYEEXP_SETTINGS,COMMON_TEXT_CODEC)){
        if (cmng.getString(CONFIG_REPORT_LANGUAGE) == CONFIG_P_LANG_ES){
            langfile = ":/languages/es.lang";
        }
    }

    if (!language.loadConfiguration(langfile,COMMON_TEXT_CODEC)){
        logger.appendError("Could not load language file");
        return;
    }

    // Creating the socket and making the connections.
    serverConn = new QSslSocket(this);
    connect(serverConn,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(serverConn,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(serverConn,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(serverConn,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    connect(serverConn,SIGNAL(readyRead()),this,SLOT(on_readyRead()));

    // Timer connection
    connect(&timer,&QTimer::timeout,this,&Loader::on_timeOut);

    connectionState = CS_NONE;

}


QString Loader::getStringForKey(const QString &key){
    if (language.containsKeyword(key)){
        return language.getString(key);
    }
    else return "ERROR: NOT FOUND";
}

void Loader::checkForUpdates(){
    // Attempting to determine which EyeTracker branch is installed.
    ConfigurationManager cfg;
    if (!cfg.loadConfiguration(FILE_EYEEXP_SETTINGS,COMMON_TEXT_CODEC)){
        logger.appendError("Could not open EyeExperimenter configuration file: " + cfg.getError());
        startEyeExperimenter();
        return;
    }

    QString ettype = cfg.getString(CONFIG_EYETRACKER_CONFIGURED);
    if (ettype == CONFIG_P_ET_GP3HD) updateMessage = UPDATE_CHECK_GP_CODE;
    else if (ettype == CONFIG_P_ET_REDM) updateMessage = UPDATE_CHECK_SMI_CODE;
    else {
        logger.appendError("Could not determine EyeExperimenter type from " + ettype);
        startEyeExperimenter();
        return;
    }

    // Calculating the hash of the current executable
    QFile exe(FILE_EYEEXP_EXE);
    if (!exe.open(QFile::ReadOnly)){
        logger.appendError("Could not open the executable file for calculating the hash");
        startEyeExperimenter();
        return;
    }

    hashLocalExe = QString(QCryptographicHash::hash(exe.readAll(),QCryptographicHash::Sha3_256).toHex());
    exe.close();

    // Connecting to the server
    connectionState = CS_CONNECTING_FOR_UPDATE;
    serverConn->connectToHostEncrypted(SERVER_IP,TCP_PORT_DB_COMM);
    timer.setInterval(TIMEOUT_TO_CONNECT*1000); // 1 Minute timeout;

}

void Loader::requestExeUpdate(){
    // Connecting to the server
    connectionState = CS_CONNECTING_FOR_UPDATE;
    serverConn->connectToHostEncrypted(SERVER_IP,TCP_PORT_DB_COMM);
    timer.setInterval(TIMEOUT_TO_CONNECT*1000); // 1 Minute timeout;
}


//************************************* Starting the EyeExperimenter ****************************************
void Loader::startEyeExperimenter(){

    // Checking if the log file exists.
    QFile changelog(FILE_EYEEXP_CHANGELOG);
    if (!changelog.exists()){
        if (changelog.open(QFile::WriteOnly)){
            QTextStream writer(&changelog);
            writer << "--";
            changelog.close();
        }
        else logger.appendError("Could not create changelog file");
    }

    QProcess eyexp;
    eyexp.start(FILE_EYEEXP_EXE);
    connectionState = CS_STARTING_EYEEXP;
    timer.setInterval(TIMEOUT_CONN_READY_POLLING);
}


//************************************* Communication and timeout slots ****************************************
void Loader::on_encryptedSuccess(){

    timer.stop();
    DataPacket tx;

    if (connectionState == CS_CONNECTING_FOR_CHECK){
        tx.addString(updateMessage,DataPacket::DPFI_UPDATE_REQUEST);
        QByteArray ba = tx.toByteArray();
        qint64 num = serverConn->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logger.appendError("Could not send check update request.");
            startEyeExperimenter();
            return;
        }
        rx.clearAll();
        connectionState = CS_CHECK_UPDATE_SENT;
        timer.setInterval(TIMEOUT_TO_GET_UPDATE_CHECK*1000); // 1 Minute timeout;
    }
    else if (connectionState == CS_CONNECTING_FOR_UPDATE){
        QString msg;
        if (updateMessage == UPDATE_CHECK_GP_CODE) msg = UPDATE_GET_GP_CODE;
        else if (updateMessage == UPDATE_CHECK_SMI_CODE) msg = UPDATE_GET_SMI_CODE;
        DataPacket tx;
        tx.addString(msg,DataPacket::DPFI_UPDATE_REQUEST);
        QByteArray ba = tx.toByteArray();
        qint64 num = serverConn->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logger.appendError("Could not send get update request.");
            startEyeExperimenter();
            return;
        }
        rx.clearAll();
        connectionState = CS_GET_UPDATE_SENT;
        timer.setInterval(TIMEOUT_TO_GET_EXE*1000);
    }

}

void Loader::on_readyRead(){

    quint8 errcode = rx.bufferByteArray(serverConn->readAll());
    if (errcode == DataPacket::DATABUFFER_RESULT_DONE){

        timer.stop();

        if (connectionState == CS_CHECK_UPDATE_SENT){
            QString hash = rx.getField(DataPacket::DPFI_UPDATE_REQUEST).data.toString();
            if (hash != hashLocalExe){
                connectionState = CS_WAIT_FOR_CONNECTION_READY;
                logger.appendStandard("Update Found!: Remote Hash: " + hash + ". Local hash: " + hashLocalExe);
                isConnReady = false;
                hashLocalExe = hash; // Hash is saved for later comparison.
                timer.start(TIMEOUT_CONN_READY_POLLING*1000);
                return;
            }
            else {
                // No update.
                startEyeExperimenter();
                return;
            }
        }
        else if (connectionState == CS_GET_UPDATE_SENT){

            // First step is backing up the olde exe.
            QFile oldexe(FILE_EYEEXP_OLDEXE);
            if (oldexe.exists()) oldexe.remove();
            if (!QFile::copy(FILE_EYEEXP_EXE,FILE_EYEEXP_OLDEXE)){
                logger.appendError("Could not create backup of the executable file. Aborting update");
                startEyeExperimenter();
                return;
            }

            QString filename;
            filename = rx.saveFile(".",DataPacket::DPFI_UPDATE_EXE);
            QString logfile = rx.saveFile(".",DataPacket::DPFI_UPDATE_CHANGES);
            if (filename.isEmpty()){
                logger.appendError("Could not create new executable");
                startEyeExperimenter();
                return;
            }

            if (logfile.isEmpty()){
                logger.appendWarning("Could not save the change log. Continuing anyway");
            }

            // The hash of the file is checked.
            QFile newexe(filename);
            if (!newexe.open(QFile::ReadOnly)){
                logger.appendError("Could not open newly created exe for hash computation");
                newexe.remove();
                startEyeExperimenter();
                return;
            }

            QString hash = QString(QCryptographicHash::hash(newexe.readAll(),QCryptographicHash::Sha3_256).toHex());
            newexe.close();

            if (hash != hashLocalExe){
                logger.appendError("Hash error on update. Was expecting: " + hashLocalExe + ". But got: " + hash);
                newexe.remove();
                startEyeExperimenter();
                return;
            }

            // All checks passed so now the executable file is overwritten.
            // The file is saved in the current directory before being copied.
            QFile currentexe(FILE_EYEEXP_EXE);
            currentexe.remove();
            if (!QFile::copy(filename,FILE_EYEEXP_EXE)){
                logger.appendError("Failed to copy locally downoladed new executable to its final destination");
            }
            newexe.remove();
            startEyeExperimenter();
            return;
        }
    }
    else if (errcode == DataPacket::DATABUFFER_RESULT_ERROR){
        logger.appendError("Buffering data from the receiver");
        serverConn->disconnectFromHost();
        startEyeExperimenter();
        return;
    }
}

void Loader::on_timeOut(){
    if ((connectionState == CS_CONNECTING_FOR_CHECK) || (connectionState == CS_CONNECTING_FOR_UPDATE)){
        timer.stop();
        logger.appendError("Connection timeout");
        startEyeExperimenter();
        return;
    }
    else if (connectionState == CS_CHECK_UPDATE_SENT){
        timer.stop();
        logger.appendError("Check update information did not arrive in time");
        startEyeExperimenter();
        return;
    }
    else if (connectionState == CS_GET_UPDATE_SENT){
        timer.stop();
        logger.appendError("Update download took too long");
        startEyeExperimenter();
        return;
    }
    else if (connectionState == CS_WAIT_FOR_CONNECTION_READY){
        if (isConnReady){
            timer.stop();
            requestExeUpdate();
        }
    }
    else if (connectionState == CS_STARTING_EYEEXP){
        if (!QFile(FILE_EYEEXP_CHANGELOG).exists()){
            timer.stop();
            QCoreApplication::quit();
        }
    }
}


//************************************* Socket and SSL Errors, Socket state changes ****************************************

void Loader::on_sslErrors(const QList<QSslError> &errors){
    //    for (qint32 i = 0; i < errors.size(); i++){
    //        log.appendWarning("SSL ERROR: " + errors.at(i).errorString());
    //    }
    Q_UNUSED(errors);
    serverConn->ignoreSslErrors();
}

void Loader::on_socketStateChanged(QAbstractSocket::SocketState state){
    //    Q_UNUSED(state);
    //    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    //    log.appendStandard(QString("Log: Socket state - ") + metaEnum.valueToKey(state));
    if (state == QAbstractSocket::UnconnectedState){
        isConnReady = true;
    }
}

void Loader::on_socketError(QAbstractSocket::SocketError error){
    //QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    //log.appendError(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
    // If there is an error then the timer should be stopped.
    Q_UNUSED(error);
    if (timer.isActive()) timer.stop();
    serverConn->disconnectFromHost();
}
