#include "loader.h"

Loader::Loader(QObject *parent) : QObject(parent)
{

    // Attempting to load the settings file to figure out the language.
    ConfigurationManager cmng;
    QString langfile = ":/languages/en.lang";
    selectedLanguage = CONFIG_P_LANG_EN;
    if (cmng.loadConfiguration(FILE_EYEEXP_SETTINGS,COMMON_TEXT_CODEC)){
        if (cmng.getString(CONFIG_REPORT_LANGUAGE) == CONFIG_P_LANG_ES){
            selectedLanguage = CONFIG_P_LANG_ES;
            langfile = ":/languages/es.lang";
        }
    }

    changeLogFile = QString(FILE_CHANGELOG_UPDATER) + "_" + selectedLanguage;

    if (!language.loadConfiguration(langfile,COMMON_TEXT_CODEC)){
        logger.appendError("Could not load language file");
        return;
    }

    if (!QSslSocket::supportsSsl()){
        logger.appendError("NO SSL SUPPORT FOUND. Starting Eye Experimenter without any checks.");
        startEyeExperimenter();
    }

    // Creating the socket and making the connections.
    serverConn = new QSslSocket(this);
    connect(serverConn,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(serverConn,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(serverConn,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(serverConn,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    connect(serverConn,SIGNAL(readyRead()),this,SLOT(on_readyRead()));

    // Timer connection
    connect(&timeoutTimer,SIGNAL(timeout()),this,SLOT(on_timeOut()));

}

QString Loader::getTitleString(){
    return LAUNCHER_VERSION;
}

QString Loader::getStringForKey(const QString &key){
    if (language.containsKeyword(key)){
        return language.getString(key);
    }
    else return "ERROR: NOT FOUND";
}

void Loader::checkForUpdates(){

    emit(changeMessage(getStringForKey("msg_wait_update_check")));

    // Getting the institution and eyeexp id information.
    ConfigurationManager cfg;
    if (!cfg.loadConfiguration(FILE_EYEEXP_CONFIGURATION,COMMON_TEXT_CODEC)){
        logger.appendError("Could not open EyeExperimenter configuration file: " + cfg.getError());
        startEyeExperimenter();
        return;
    }

    instUID = cfg.getString(CONFIG_INST_UID);
    if (instUID.isEmpty()){
        logger.appendError("Could not determine institution id");
        startEyeExperimenter();
        return;
    }

    eyeexpID = cfg.getString(CONFIG_EYEEXP_NUMBER);
    if (eyeexpID.isEmpty()){
        logger.appendError("Could not determine institution id");
        startEyeExperimenter();
        return;
    }

    serverIP = cfg.getString(CONFIG_SERVER_ADDRESS);
    if (serverIP.isEmpty()){
        logger.appendError("Could not determine server address");
        startEyeExperimenter();
        return;
    }

    timeoutConnection = cfg.getInt(CONFIG_LAUCHER_CONNECT_TIMEOUT)*1000;
    if (timeoutConnection == 0){
        logger.appendError("Could not determine time out connection");
        startEyeExperimenter();
        return;
    }

    timeoutGetUpdateCheck = cfg.getInt(CONFIG_LAUCHER_DOWNLOAD_TIMEOUT)*1000;
    if (timeoutGetUpdateCheck == 0){
        logger.appendError("Could not determine time out for getting update check.");
        startEyeExperimenter();
        return;
    }

    bkpLocalDB = false;
    bkpLocalDB = cfg.getBool(CONFIG_ENABLE_LOCAL_DB_BKP);

    //qWarning() << "BKP LOCAL DB is" << bkpLocalDB;

    // Getting the language setting
    ConfigurationManager settings;
    if (!settings.loadConfiguration(FILE_EYEEXP_SETTINGS,COMMON_TEXT_CODEC)){
        logger.appendError("Could not load the settings file: " + settings.getError());
    }

    selectedLanguage = settings.getString(CONFIG_REPORT_LANGUAGE);
    if (selectedLanguage.isEmpty()){
        logger.appendError("Could not determine application language");
        startEyeExperimenter();
        return;
    }
    changeLogFile = QString(FILE_CHANGELOG_UPDATER) + "_" + selectedLanguage;

    // Calculating the hashes.
    eyeLauncherHash = DataPacket::getFileHash(FILE_EYEEXP_LAUNCHER);
    eyeExperimenterHash = DataPacket::getFileHash(FILE_EYEEXP_EXE);
    eyeConfigurationHash = DataPacket::getFileHash(FILE_EYEEXP_CONFIGURATION);

    if (eyeLauncherHash.isEmpty()){
        logger.appendError("Could not determine launcher hash");
    }
    if (eyeExperimenterHash.isEmpty()){
        logger.appendError("Could not determine EyeExperimenter hash");
    }
    if (eyeConfigurationHash.isEmpty()){
        logger.appendError("Could not determine Configuration hash");
    }

    // Connecting to the server
    connectionState = CS_CONNECTING;
    serverConn->connectToHostEncrypted(serverIP,TCP_PORT_UPDATE_SERVER);
    timeoutTimer.start(timeoutConnection);
}


//************************************* Starting the EyeExperimenter ****************************************
void Loader::startEyeExperimenter(){

    //qWarning() << "Starting eyeserver";
    emit(changeMessage(getStringForKey("msg_starting_the_appliation")));

    // Checking if the log file exists.
    QFile changelog(changeLogFile);
    if (!changelog.exists()){
        if (changelog.open(QFile::WriteOnly)){
            QTextStream writer(&changelog);
            writer << "--";
            changelog.close();
        }
        else logger.appendError("Could not create changelog file");
    }

    QProcess eyeExpProcess;
    connectionState = CS_POLLING;
    eyeExpProcess.setWorkingDirectory("../");
    eyeExpProcess.setProgram(FILE_EYEEXP_EXE);
    eyeExpProcess.startDetached();
    timeoutTimer.start(TIMEOUT_CONN_READY_POLLING);
}

//************************************* Communication and timeout slots ****************************************
void Loader::on_encryptedSuccess(){

    timeoutTimer.stop();
    DataPacket tx;
    if (!tx.addFile(FILE_EYEEXP_LOG,DataPacket::DPFI_UPDATE_LOGFILE)){
        logger.appendError("No log file to add to the package");
    }
    tx.addString(instUID,DataPacket::DPFI_DB_INST_UID);
    tx.addString(eyeexpID,DataPacket::DPFI_UPDATE_EYEEXP_ID);
    tx.addString(selectedLanguage,DataPacket::DPFI_UPDATE_LANG);
    tx.addString(eyeExperimenterHash,DataPacket::DPFI_UPDATE_EYEEXP);
    tx.addString(eyeLauncherHash,DataPacket::DPFI_UPDATE_EYELAUNCHER);
    tx.addString(eyeConfigurationHash,DataPacket::DPFI_UPDATE_CONFIG);
    tx.addString(eyeConfigurationHash,DataPacket::DPFI_UPDATE_CONFIG);
    if (bkpLocalDB){
        QString localdb = "../" + QString(DIRNAME_RAWDATA) + "/" + QString(FILE_LOCAL_DB);
        if (!tx.addFile(localdb,DataPacket::DPFI_LOCAL_DB_BKP)){
            logger.appendError("Could not add local db data file for backup: " + localdb);
        }
    }

    QByteArray ba = tx.toByteArray();
    qint64 num = serverConn->write(ba.constData(),ba.size());
    if (num != ba.size()){
        logger.appendError("Could not send check update request.");
        startEyeExperimenter();
        return;
    }
    rx.clearAll();
    connectionState = CS_GETTING_INFO;
    timeoutTimer.start(timeoutGetUpdateCheck);
}

void Loader::on_readyRead(){

    quint8 errcode = rx.bufferByteArray(serverConn->readAll());
    if (errcode == DataPacket::DATABUFFER_RESULT_DONE){
        timeoutTimer.stop();

        QString ans = rx.getField(DataPacket::DPFI_UPDATE_RESULT).data.toString();
        if (ans != "OK"){
            logger.appendError("There was a problem in the server with the update request");
            startEyeExperimenter();
            return;
        }


        // Renaming the EyeExperimenter log file.
        QString oldlog = QString(FILE_EYEEXP_LOG) + "." + QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm");
        if (!QFile::copy(FILE_EYEEXP_LOG,oldlog)){
            logger.appendError("Could not create the the log file backup: " + oldlog);
        }

        // Checking if there is a new version of the launcher.
        if (rx.hasInformationField(DataPacket::DPFI_UPDATE_EYELAUNCHER)){
            // Saved locally. Its the responsability of the EyeExperimenter to actually replace it.
            QString eyalauncher = rx.saveFile(".",DataPacket::DPFI_UPDATE_EYELAUNCHER);
            if (eyalauncher.isEmpty()) logger.appendError("Received UPDATE for EyeLauncherClient but was not able to save it");
            else logger.appendStandard("Downloaded update for EyeLauncherClient");
        }

        // Checking if there is a new version of the configuration file
        if (rx.hasInformationField(DataPacket::DPFI_UPDATE_CONFIG)){
            // Making a backup of the old file.
            QFile(FILE_EYEEXP_CONFIGURATION_BKP).remove();
            if (!QFile::copy(FILE_EYEEXP_CONFIGURATION,FILE_EYEEXP_CONFIGURATION_BKP)){
                logger.appendError("Could not create a backup of the configuration file. Not updating");
            }
            else {
                QString fname = rx.saveFile("../",DataPacket::DPFI_UPDATE_CONFIG);
                if (!fname.isEmpty()){
                    logger.appendStandard("Downloaded update for the configuration file");
                }
                else{
                    logger.appendError("Could not save new configuration file. Attempting to restore previous one.");
                    if (!QFile::copy(FILE_EYEEXP_CONFIGURATION_BKP,FILE_EYEEXP_CONFIGURATION)){
                        logger.appendError("Could not restore backup of the configuration file. Launch will fail");
                    }
                }
            }
        }

        // Checking if there is a new version of the actual eye experimenter file
        if (rx.hasInformationField(DataPacket::DPFI_UPDATE_EYEEXP)){
            // Making a backup of the old file.
            QFile(FILE_EYEEXP_OLDEXE).remove();
            if (!QFile::copy(FILE_EYEEXP_EXE,FILE_EYEEXP_OLDEXE)){
                logger.appendError("Could not create a backup of the EyeExperimenter file. Not updating");
            }
            else {
                if (!QFile(FILE_EYEEXP_EXE).remove()){
                    logger.appendError("Could not delete existing EyeExperimenter");;
                }
                else{
                    QString fname = rx.saveFile("../",DataPacket::DPFI_UPDATE_EYEEXP);
                    if (!fname.isEmpty()){
                        logger.appendStandard("Downloaded update for the EyeExperimenter file");
                    }
                    else{
                        logger.appendError("Could not save new the EyeExperimenter file. Attempting to restore previous one.");
                        if (!QFile::copy(FILE_EYEEXP_OLDEXE,FILE_EYEEXP_EXE)){
                            logger.appendError("Could not restore EyeExperimenter of the configuration file. Launch will fail if the original file was deleted");
                        }
                    }
                }
            }
        }

        // Saving the changelog file if present.
        if (rx.hasInformationField(DataPacket::DPFI_UPDATE_CHANGES)){
            QString fname = rx.saveFile(".",DataPacket::DPFI_UPDATE_CHANGES);
            if (!fname.isEmpty()) logger.appendError("Could not save changelog file");
            else logger.appendStandard("Saved changelog file: " + fname);
        }

        startEyeExperimenter();

    }

}

void Loader::on_timeOut(){
    //qWarning() << "Timeout conn state" << connectionState;
    if (connectionState == CS_CONNECTING){
        timeoutTimer.stop();
        logger.appendError("Connection timeout");
        startEyeExperimenter();
        return;
    }
    else if (connectionState == CS_GETTING_INFO) {
        timeoutTimer.stop();
        logger.appendError("Check update information did not arrive in time");
        startEyeExperimenter();
        return;
    }
    else if (connectionState == CS_POLLING) {
        if (!QFile(changeLogFile).exists()){
            timeoutTimer.stop();
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
    Q_UNUSED(state);
    //    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    //    log.appendStandard(QString("Log: Socket state - ") + metaEnum.valueToKey(state));
    //    if (state == QAbstractSocket::UnconnectedState){
    //        isConnReady = true;
    //        if (connectionState == CS_GET_UPDATE_SENT){
    //            // Connection was broken off while expecting the update.
    //            logger.appendError("Connection was broken off while wating for the updated executable");
    //            timeoutTimer.stop();
    //            startEyeExperimenter();
    //        }
    //    }
}

void Loader::on_socketError(QAbstractSocket::SocketError error){
    //QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    //log.appendError(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
    // If there is an error then the timer should be stopped.
    Q_UNUSED(error);
    serverConn->disconnectFromHost();
}
