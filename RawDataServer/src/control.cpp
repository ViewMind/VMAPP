#include "control.h"

Control::Control(QObject *parent):QObject(parent)
{

    // Creating the configuration verifier
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cv[RAW_DATA_SERVER_PASSWORD] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_BOOL;
    cv[RAW_DATA_SERVER_DELWORKDIR] = cmd;

    config.setupVerification(cv);

    // Creating the the listner.
    listener = new SSLListener(this);

    idGen = 0;
}

void Control::startServer(){

    QString configurationFile = COMMON_PATH_FOR_DB_CONFIGURATIONS;
    qint32 definitions = 0;

    ConfigurationManager dbconfigs;

#ifdef SERVER_LOCALHOST
    configurationFile = configurationFile + "db_localhost_prod";
    definitions++;
#endif
#ifdef SERVER_DEVELOPMENT
    configurationFile = configurationFile + "db_development";
    definitions++;
#endif
#ifdef SERVER_PRODUCTION
    configurationFile = configurationFile + "db_production";
    definitions++;
#endif

    if (definitions != 1){
        logger.appendError("The number of DB Configuration files is " + QString::number(definitions) + " instead of 1");
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    // Creating the configuration verifier
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    // DB configuration is all strings.
    cmd.clear();
    cv[CONFIG_DBHOST] = cmd;
    cv[CONFIG_DBNAME] = cmd;
    cv[CONFIG_DBPASSWORD] = cmd;
    cv[CONFIG_DBUSER] = cmd;

    cv[CONFIG_ID_DBHOST] = cmd;
    cv[CONFIG_ID_DBNAME] = cmd;
    cv[CONFIG_ID_DBPASSWORD] = cmd;
    cv[CONFIG_ID_DBUSER] = cmd;

    cv[CONFIG_PATDATA_DBHOST] = cmd;
    cv[CONFIG_PATDATA_DBNAME] = cmd;
    cv[CONFIG_PATDATA_DBPASSWORD] = cmd;
    cv[CONFIG_PATDATA_DBUSER] = cmd;

    cv[CONFIG_S3_ADDRESS] = cmd;

    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_DBPORT] = cmd;
    cv[CONFIG_ID_DBPORT] = cmd;
    cv[CONFIG_PATDATA_DBPORT] = cmd;

    dbconfigs.setupVerification(cv);

    // Database configuration files
    if (!dbconfigs.loadConfiguration(configurationFile,COMMON_TEXT_CODEC)){
        logger.appendError("DB Configuration file errors:<br>"+dbconfigs.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    if (!config.loadConfiguration(FILE_FAW_DATA_SERVER_CONF,COMMON_TEXT_CODEC)){
        logger.appendError("Configuration file errors:<br>"+config.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    // Joining the two configuration files
    config.merge(dbconfigs);

    if (!QSslSocket::supportsSsl()){
        logger.appendError("No support for SSL found. Cannot continue");
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    // Initializing the database connections.
    QString host = config.getString(CONFIG_DBHOST);
    QString dbname = config.getString(CONFIG_DBNAME);
    QString user = config.getString(CONFIG_DBUSER);
    QString passwd = config.getString(CONFIG_DBPASSWORD);
    quint16 port = config.getInt(CONFIG_DBPORT);
    dbConnBase.setupDB(DB_NAME_BASE,host,dbname,user,passwd,port,"");
    //qWarning() << "Connection information: " + user + "@" + host + " with passwd " + passwd + ", port: " + QString::number(port) + " to db: " + dbname;

    host = config.getString(CONFIG_ID_DBHOST);
    dbname = config.getString(CONFIG_ID_DBNAME);
    user = config.getString(CONFIG_ID_DBUSER);
    passwd = config.getString(CONFIG_ID_DBPASSWORD);
    port = config.getInt(CONFIG_ID_DBPORT);
    dbConnID.setupDB(DB_NAME_ID,host,dbname,user,passwd,port,"");
    //qWarning() << "Connection information: " + user + "@" + host + " with passwd " + passwd + ", port: " + QString::number(port) + " to db: " + dbname;

    logger.appendStandard("Testing DB Connections...");
    if (!dbConnBase.open()){
        logger.appendError("FAIL TEST CONNECTION DB BASE: " + dbConnBase.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }
    dbConnBase.close();

    if (!dbConnID.open()){
        logger.appendError("FAIL TEST CONNECTION DB ID: " + dbConnID.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }
    dbConnID.close();

    // Listen for new connections.
    connect(listener,&SSLListener::newConnection,this,&Control::on_newConnection);

    if (!listener->listen(QHostAddress::Any,TCP_PORT_RAW_DATA_SERVER)){
        logger.appendError("ERROR : Could not start SSL Server: " + listener->errorString());
        emit(exitRequested());
        return;
    }

}

void Control::on_newConnection(){

    // logger.appendStandard("New connection received 0");

    // New connection is available.
    QSslSocket *sslsocket = (QSslSocket*)(listener->nextPendingConnection());
    SSLIDSocket *socket = new SSLIDSocket(sslsocket,idGen,"");

    //logger.appendStandard("New connection received 1");

    if (!socket->isValid()) {
        logger.appendError("ERROR: Could not cast incomming socket connection");
        return;
    }

    //logger.appendStandard("New connection received 2");

    // Doing the connections.
    connect(socket,&SSLIDSocket::sslSignal,this,&Control::on_newSSLSignal);
    sockets.addSocket(socket);

    logger.appendStandard("New connection received 3");

    // The SSL procedure.
    sslsocket->setPrivateKey(":/certificates/server.key");
    //logger.appendStandard("New connection received 3.1");
    sslsocket->setLocalCertificate(":/certificates/server.csr");
    //logger.appendStandard("New connection received 3.2");
    sslsocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    //logger.appendStandard("New connection received 3.2");
    sslsocket->startServerEncryption();

    //logger.appendStandard("New connection received 4");

}

void Control::on_newSSLSignal(quint64 socket, quint8 signaltype){

    //qWarning() << "DATA PROCESSING SSL SIGNAL" << signalType;
    QString where = "on_newSSLSignal: " + SSLIDSocket::SSLSignalToString(signaltype);
    SSLIDSocket *sslsocket = sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(socket,where);
        logger.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        return;
    }

    switch (signaltype){
    case SSLIDSocket::SSL_SIGNAL_DISCONNECTED:
        logger.appendStandard("Terminated connection: " + sslsocket->socket()->peerAddress().toString());
        sockets.deleteSocket(socket,where);
        break;
    case SSLIDSocket::SSL_SIGNAL_ENCRYPTED:
        logger.appendSuccess("SSL Handshake completed for address: " + sslsocket->socket()->peerAddress().toString() + ". Awaiting a request");
        sslsocket->startTimeoutTimer(50000);
        //requestProcessInformation();
        break;
    case SSLIDSocket::SSL_SIGNAL_SOCKET_ERROR:
        socketErrorFound(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_DATA_RX_DONE:
        // Information has arrived ok. Starting the process.
        sslsocket->stopTimer();
        finishedReceivingRequest(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_DATA_RX_ERROR:
        sslsocket->stopTimer();
        // Data is most likely corrupted.
        logger.appendError("Data from " + sslsocket->socket()->peerAddress().toString() + " was corrupted or contained errors");
        sockets.deleteSocket(socket,where);
        break;
    case SSLIDSocket::SSL_SIGNAL_TIMEOUT:
        // This means that the data did not arrive on time.
        sslsocket->stopTimer();
        logger.appendError("Timeout from " + sslsocket->socket()->peerAddress().toString() + " did not arrive in time");
        sockets.deleteSocket(socket,where);
        break;
    case SSLIDSocket::SSL_SIGNAL_PROCESS_DONE:
        logger.appendError("Unexpected Qt Signal SSL SIGNAL PROCESS DONE.");
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


/***************************************************** QTCP SOCKETS SLOTS ****************************************************/
void Control::sslErrorsFound(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    QString where = "sslErrorsFound";
    SSLIDSocket *sslsocket = sockets.getSocketLock(id,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(id,where);
        logger.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(id));
        return;
    }

    QList<QSslError> errorlist = sslsocket->socket()->sslErrors();
    QHostAddress addr = sslsocket->socket()->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        logger.appendError("SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }

    // Eliminating it from the list.
    sockets.releaseSocket(id,where);
    sockets.deleteSocket(id,where);

}

void Control::socketErrorFound(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    QString where = "socketErrorFound";
    SSLIDSocket *sslsocket = sockets.getSocketLock(id,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(id,where);
        logger.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(id));
        return;
    }

    QAbstractSocket::SocketError error = sslsocket->socket()->error();
    QHostAddress addr = sslsocket->socket()->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    if (error != QAbstractSocket::RemoteHostClosedError){
        logger.appendError(QString("Socket Error found: ") + metaEnum.valueToKey(error) + QString(" from Address: ") + addr.toString());
        // Eliminating it from the list.
        sockets.releaseSocket(id,where);
        sockets.deleteSocket(id,where);
    }

    sockets.releaseSocket(id,where);

}

void Control::changedState(quint64 id){
    Q_UNUSED(id)
    //    if (sockets.size() == 0) return;
    //    QHostAddress addr = socket->peerAddress();
    //    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    //    logger.appendStandard(addr.toString() + ": " + QString("New socket state, ") + metaEnum.valueToKey(state));
}

void Control::finishedReceivingRequest(quint64 socket){

    QString where = "finishedReceivingRequest";
    SSLIDSocket *sslsocket = sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(socket,where);
        logger.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        return;
    }

    if (sslsocket->getDataPacket().hasInformationField(DataPacket::DPFI_DB_INST_UID)){
        // This is a request for a local DB update
        sendLocalDB(socket);
    }
    if (sslsocket->getDataPacket().hasInformationField(DataPacket::DPFI_DB_INST_PASSWORD)){
        // Data query.
        processRequest(socket);
    }
    else {
        // Institution list query.
        QStringList instuids = getInstitutionUIDPair();
        DataPacket tx;
        tx.addString(instuids.join(DB_LIST_IN_COL_SEP),DataPacket::DPFI_INST_NAMES);
        QByteArray ba = tx.toByteArray();
        qint64 num = sslsocket->socket()->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logger.appendError("Failure sending then institution list information");
        }
    }

    sockets.releaseSocket(socket,where);
}

void Control::sendLocalDB(quint64 id){

    QString where = "sslErrorsFound";
    SSLIDSocket *sslsocket = sockets.getSocketLock(id,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(id,where);
        logger.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(id));
        return;
    }

    if (!verifyPassword(sslsocket->getDataPacket().getField(DataPacket::DPFI_DB_INST_PASSWORD).data.toString())){
        DataPacket tx;
        tx.addString("WRONG PASSWORD",DataPacket::DPFI_DB_ERROR);
        QByteArray ba = tx.toByteArray();
        qint64 num = sslsocket->socket()->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logger.appendError("Failure sending the wrong password message");
        }
        sockets.releaseSocket(id,where);
        return;
    }

    // Password checks out. Searching for the existance of the Backup.
    QString instUID = sslsocket->getDataPacket().getField(DataPacket::DPFI_DB_INST_UID).data.toString();
    QString instEDirname = ETDIR_PATH + QString("/") + instUID;
    QStringList dirsInInstPath = QDir(instEDirname).entryList(QStringList(),QDir::Dirs|QDir::NoDotAndDotDot);

    logger.appendStandard("Requested local DB for institution: " + instUID);

    QString serializedMap;
    serializedMap = "";

    QVariantMap patientIDmap;

    for (qint32 i = 0; i < dirsInInstPath.size(); i++){
        QString dbfilename = instEDirname + "/" + dirsInInstPath.at(i) + "/" + QString(FILE_LOCAL_DB_BKP);
        if (!QFile(dbfilename).exists()) {
            logger.appendWarning("Could not load local db file: " + dbfilename);
            continue;
        }

        logger.appendStandard("Loading local DB info for " + dbfilename);

        // Getting the local information and a list of the hashed patient ids.
        LocalInformationManager lim;
        lim.setWorkingFile(dbfilename);
        serializedMap = lim.serialDoctorPatientString(serializedMap);

        patientIDmap = lim.getHashedIDPatientMap(patientIDmap);
    }

    // Getting hashed puid to puid map.
    if (!dbConnID.open()){
        logger.appendError("Getting local db bkp, could not open DB CON ID");
        sockets.releaseSocket(id,where);
    }

    QStringList cols; cols << TPATID_COL_KEYID << TPATID_COL_UID;
    QString condition = QString(TPATID_COL_UID) +  " IN (" + patientIDmap.keys().join(",") + ")";
    if (!dbConnID.readFromDB(TABLE_PATIENTD_IDS,cols,condition)){
        logger.appendError("Getting the patiend ID hashes: " + dbConnID.getError());
        sockets.releaseSocket(id,where);
        return;
    }

    DBData res = dbConnID.getLastResult();
    for (qint32 i = 0; i < res.rows.size(); i++){
        if (res.rows.at(i).size() != 2){
            logger.appendError("Getting information from Patient Table IDs expected 2 column But got: " + QString::number(res.rows.at(i).size()));
            sockets.releaseSocket(id,where);
            return;
        }
        QVariantMap tmap = patientIDmap.value(res.rows.at(i).last()).toMap();
        tmap[IDMAP_ID_PUID] = res.rows.at(i).first();
        patientIDmap[res.rows.at(i).last()] = tmap;
    }

    QString serializedIDmap = VariantMapSerializer::serializeTwoLevelVariantMap("",patientIDmap);

    dbConnID.close();

    DataPacket tx;
    tx.addString(serializedMap,DataPacket::DPFI_SERIALIZED_DB);
    tx.addString(serializedIDmap,DataPacket::DPFI_PUID_LIST);
    QByteArray ba = tx.toByteArray();
    qint64 num = sslsocket->socket()->write(ba.constData(),ba.size());
    if (num != ba.size()){
        logger.appendError("Failure sending the wrong password message");
    }
    sockets.releaseSocket(id,where);
}

/*****************************************************************************************************************************/

bool Control::verifyPassword(const QString &password){
    QString hash =  QString(QCryptographicHash::hash(password.toLatin1(),QCryptographicHash::Sha256).toHex());
    if (hash == config.getString(RAW_DATA_SERVER_PASSWORD)) return true;
    else {
        logger.appendError("Wrong Passsword: " + password);
        logger.appendError("Expected hash: " + config.getString(RAW_DATA_SERVER_PASSWORD) + " but got hash " + hash);
        return false;
    }
}

void Control::processRequest(quint64 id){

    DataPacket tx;

    QString where = "processRequest";
    SSLIDSocket *sslsocket = sockets.getSocketLock(id,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(id,where);
        logger.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(id));
        return;
    }

    if (!dbConnBase.open()){
        logger.appendError("Getting requested information, could not open DB CON BASE");
        sockets.releaseSocket(id,where);
        return;
    }

    if (!dbConnID.open()){
        logger.appendError("Getting requested information, could not open DB CON BASE");
        sockets.releaseSocket(id,where);
        return;
    }

    DataPacket rx = sslsocket->getDataPacket();

    QString password = rx.getField(DataPacket::DPFI_DB_INST_PASSWORD).data.toString();
    QString instUID = rx.getField(DataPacket::DPFI_DB_INST_UID).data.toString();
    QString startDate = rx.getField(DataPacket::DPFI_DATE_START).data.toString();
    QString endDate = rx.getField(DataPacket::DPFI_DATE_END).data.toString();

    if (!verifyPassword(password)){
        tx.addString("WRONG PASSWORD",DataPacket::DPFI_DB_ERROR);
        QByteArray ba = tx.toByteArray();
        qint64 num = sslsocket->socket()->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logger.appendError("Failure sending the wrong password message");
        }
        sockets.releaseSocket(id,where);
        return;
    }

    ///////////////// Getting the puids in the time frame and from the institution rquestested
    QStringList columns;
    QString query;
    query = "SELECT DISTINCT r.puid FROM tEyeResults AS r, tDoctors AS d WHERE r.study_date >= '" + startDate + "' AND r.study_date <= '"
            + endDate + "' AND r.doctorid = d.uid AND d.medicalinstitution = '" + instUID + "'";
    columns << TEYERES_COL_PUID;

    if (!dbConnBase.specialQuery(query,columns)){
        logger.appendError("Getting information from the Eye Results table: " + dbConnBase.getError());
        sockets.releaseSocket(id,where);
        return;
    }

    DBData res = dbConnBase.getLastResult();
    QStringList puids;
    for (qint32 i = 0; i < res.rows.size(); i++){
        if (res.rows.at(i).size() != 1){
            logger.appendError("Getting information from Eye Result Table expected 1 column But got: " + QString::number(res.rows.at(i).size()));
            sockets.releaseSocket(id,where);
            return;
        }
        puids << res.rows.at(i).first();
    }

    if (puids.isEmpty()){
        tx.addString("No patients from institution " + instUID + " were found between " + startDate + " and " + endDate,DataPacket::DPFI_DB_ERROR);
        QByteArray ba = tx.toByteArray();
        qint64 num = sslsocket->socket()->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logger.appendError("Failure sending the no puids message");
        }
        sockets.releaseSocket(id,where);
        return;
    }

    ///////////////// Getting the hashes from the puid.
    columns.clear();
    columns << TPATID_COL_KEYID << TPATID_COL_UID;
    QString condition;
    condition = "keyid IN (" + puids.join(",") + ")";
    if (!dbConnID.readFromDB(TABLE_PATIENTD_IDS,columns,condition)){
        logger.appendError("Getting the patiend ID hashes: " + dbConnID.getError());
        sockets.releaseSocket(id,where);
        return;
    }

    res = dbConnID.getLastResult();
    QHash<QString,QString> puidHashMap;
    for (qint32 i = 0; i < res.rows.size(); i++){
        if (res.rows.at(i).size() != 2){
            logger.appendError("Getting information from Patient Table IDs expected 2 column But got: " + QString::number(res.rows.at(i).size()));
            sockets.releaseSocket(id,where);
            return;
        }
        puidHashMap[res.rows.at(i).last()] = res.rows.at(i).first();
        //qWarning() << "HASHMAP:" <<  res.rows.at(i).last() << res.rows.at(i).first();
    }


    ///////////////// Getting the data from S3
    QStringList hashes = puidHashMap.keys();
    QStringList downloadedDirectories;

    S3Interface s3;
    s3.setS3Bin(VIEWMIND_DATA_REPO);

    for (qint32 i = 0; i < hashes.size(); i++){
        logger.appendStandard("Downloading the directory: " + hashes.at(i));
        QString name = puidHashMap.value(hashes.at(i));
        while (name.size() < 10) name = "0" + name;
        downloadedDirectories << name;
        s3.copyRecursively(hashes.at(i), name);
    }

    dbConnID.close();
    dbConnBase.close();

    ///////////////// Processing the resulting files in order to send them.
    FileLister fileLister;
    QStringList fileNames;
    QStringList fileContents;

    for (qint32 i = 0; i < downloadedDirectories.size(); i++){
        QString dir = QString(SERVER_WORK_DIR) + "/" + downloadedDirectories.at(i);
        if (QDir(dir).exists()){
            fileLister.listFileInDirectory(dir);
            QStringList fileList = fileLister.getFileList();
            fileNames << fileList;
        }
    }

    bool couldNotRead = false;
    for (qint32 i = 0; i < fileNames.size(); i++){
        QFile file(fileNames.at(i));
        if (!file.open(QFile::ReadOnly)){
            couldNotRead = true;
            break;
        }
        QTextStream reader(&file);
        fileContents << reader.readAll();
        file.close();
    }

    if (couldNotRead){
        tx.addString("Error loading downlaoded files",DataPacket::DPFI_DB_ERROR);
        QByteArray ba = tx.toByteArray();
        qint64 num = sslsocket->socket()->write(ba.constData(),ba.size());
        if (num != ba.size()){
            logger.appendError("Failure sending the failed downloaded files error");
        }
        sockets.releaseSocket(id,where);
        return;
    }

    // If all was ok. The information is sent back.
    tx.addString(fileNames.join(DB_LIST_IN_COL_SEP),DataPacket::DPFI_RAW_DATA_NAMES);
    tx.addString(fileContents.join(DB_LIST_IN_COL_SEP),DataPacket::DPFI_RAW_DATA_CONTENT);
    QByteArray ba = tx.toByteArray();
    qint64 num = sslsocket->socket()->write(ba.constData(),ba.size());
    if (num != ba.size()){
        logger.appendError("Failure sending the failed downloaded files error");
    }

    sockets.releaseSocket(id,where);
}

QStringList Control::getInstitutionUIDPair(){
    QStringList ans;
    QStringList columns;

    if (!dbConnBase.open()){
        logger.appendError("Getting institutions names and uids, could not open DB CON BASE");
        return ans;
    }

    columns << TINST_COL_ENABLED << TINST_COL_UID << TINST_COL_NAME;

    if (!dbConnBase.readFromDB(TABLE_INSTITUTION,columns,"")){
        logger.appendError("Getting institutions names and uids: " + dbConnBase.getError());
        return ans;
    }

    DBData res = dbConnBase.getLastResult();

    for (qint32 i = 0; i < res.rows.size(); i++){

        if (res.rows.at(i).size() != 3){
            logger.appendError("Getting institutions expected three values per row. But got: " + QString::number(res.rows.at(i).size()));
            return QStringList();
        }
        ans << res.rows.at(i).first();
        ans << res.rows.at(i).at(1);
        ans << res.rows.at(i).last();
    }

    dbConnBase.close();

    return ans;

}
