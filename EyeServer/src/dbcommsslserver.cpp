#include "dbcommsslserver.h"

DBCommSSLServer::DBCommSSLServer(QObject *parent) : QObject(parent)
{

    // Creating the the listner.
    listener = new SSLListener(this);

    // Listen for new connections.
    connect(listener,&SSLListener::newConnection,this,&DBCommSSLServer::on_newConnection);
    connect(listener,&SSLListener::lostConnection,this,&DBCommSSLServer::on_lostConnection);

    idGen = 0;

}


bool DBCommSSLServer::startServer(ConfigurationManager *c){

    config = c;
    if (!listener->listen(QHostAddress::Any,TCP_PORT_DB_COMM)){
        log.appendError("ERROR : Could not start SQL SSL Server: " + listener->errorString());
        return false;
    }

    return true;

}

// For now, this function is not used.
void DBCommSSLServer::on_lostConnection(){
}

void DBCommSSLServer::on_newConnection(){

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
    connect(socket,&SSLIDSocket::sslSignal,this,&DBCommSSLServer::on_newSSLSignal);

    log.appendStandard("New connection");

    // The SSL procedure.
    sslsocket->setPrivateKey(":/certificates/server.key");
    sslsocket->setLocalCertificate(":/certificates/server.csr");
    sslsocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslsocket->startServerEncryption();

}


// Handling all signals
void DBCommSSLServer::on_newSSLSignal(quint64 socket, quint8 signaltype){

    QString where = "DB on_newSSLSignal: " + SSLIDSocket::SSLSignalToString(signaltype);
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
        else processSQLRequest(socket);
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


void DBCommSSLServer::socketErrorFound(quint64 id){

    QString where = "DB socketErrorFound: ";
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

void DBCommSSLServer::sslErrorsFound(quint64 id){

    QString where = "DB sslErrorsFound: ";
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

DBCommSSLServer::VerifyDBRetStruct DBCommSSLServer::verifyDoctor(const QStringList &columns, const QStringList &values){
    VerifyDBRetStruct ret;
    qint32 instIndex = columns.indexOf(TDOCTOR_COL_MEDICAL_INST);
    if ((instIndex == -1) || (instIndex >= values.size())) {
        log.appendError("On VERIFY DOCTOR: No column found for Medical Institution. Index: " + QString::number(instIndex) + " Column List: " + columns.join(", "));
        ret.errorCode = DBACK_UID_ERROR;
        return ret;
    }
    QString inst_uid = values.at(instIndex);

    QStringList cols;
    cols << TINST_COL_UID;
    QString cond = QString(TINST_COL_UID) + "='" + inst_uid + "'";

    if (!dbConnBase->readFromDB(TABLE_INSTITUTION,cols,cond)){
        log.appendError("On VERIFY DOCTOR, DB Error: " + dbConnBase->getError());
        ret.errorCode = DBACK_DBCOMM_ERROR;
        return ret;
    }

    DBData data = dbConnBase->getLastResult();
    if (data.rows.size() != 1){
        log.appendError("On VERIFY DOCTOR, Expecting only 1 row from the DB, but got " + QString::number(data.rows.size()));
        ret.errorCode = DBACK_UID_ERROR;
        return ret;
    }

    if (data.rows.first().size() != 1){
        log.appendError("On VERIFY DOCTOR, Expecting only 1 column from the DB, but got " + QString::number(data.rows.first().size()));
        ret.errorCode = DBACK_UID_ERROR;
        return ret;
    }

    // If it matched, then the UID is ok.
    ret.errorCode = DBACK_ALL_OK;
    ret.logid = inst_uid;
    return ret;

}


DBCommSSLServer::VerifyDBRetStruct DBCommSSLServer::verifyPatient(const QStringList &columns, const QStringList &values){

    VerifyDBRetStruct ret;
    ret.errorCode = DBACK_ALL_OK;
    ret.puid = -1;
    ret.indexOfPatUid = -1;

    qint32 drIndex = columns.indexOf(TPATDATA_COL_DOCTORID);
    if ((drIndex == -1) || (drIndex >= values.size())) {
        log.appendError("On VERIFY PATIENT: No column found for Doctor UID found. Index: " + QString::number(drIndex) + " Column List: " + columns.join(", "));
        ret.errorCode = DBACK_UID_ERROR;
        return ret;
    }
    QString druid = values.at(drIndex);

    QStringList cols;
    cols << TDOCTOR_COL_UID;
    QString cond = QString(TDOCTOR_COL_UID) + "='" + druid + "'";

    if (!dbConnBase->readFromDB(TABLE_DOCTORS,cols,cond)){
        log.appendError("On VERIFY PATIENT, DB Error: " + dbConnBase->getError());
        ret.errorCode = DBACK_DBCOMM_ERROR;
        return ret;
    }

    DBData data = dbConnBase->getLastResult();
    if (data.rows.size() < 1){
        log.appendError("On VERIFY PATIENT, Expecting only 1 row from the DB, but got " + QString::number(data.rows.size()));
        ret.errorCode = DBACK_UID_ERROR;
        return ret;
    }

    //////////////////////////// TABLE ID Operations

    // If it matched, then the UID is ok. This means that the doctor exists. Now the patient needs to be checked.
    ret.indexOfPatUid = columns.indexOf(TPATDATA_COL_PUID);
    if ((ret.indexOfPatUid  == -1) || (ret.indexOfPatUid  >= values.size())) {
        log.appendError("On VERIFY PATIENT: No column found for PUID found. Index: " + QString::number(ret.indexOfPatUid) + " Column List: " + columns.join(", "));
        ret.errorCode = DBACK_UID_ERROR;
        return ret;
    }
    QString patid = values.at(ret.indexOfPatUid);
    cols.clear();
    cols << TPATID_COL_KEYID;

    // Transforming the patid via the SHA3-512.
    // qWarning() << "Searching for patid" << patid;

    //qWarning() << "PATID: " << patid;

    cond = QString(TPATID_COL_UID) + "='" + patid +"'";

    if (!dbConnID->readFromDB(TABLE_PATIENTD_IDS,cols,cond)){
        log.appendError("On VERIFY PATIENT, DB Error: " + dbConnID->getError());
        ret.errorCode = DBACK_DBCOMM_ERROR;
        return ret;
    }
    data = dbConnID->getLastResult();

    if (data.rows.size() == 0){

        // This a new patient and it needs to be added to the DB.
        // qWarning() << "Adding the new patient to the DB with uid" << patid;
        cols.clear();
        QStringList vals;
        cols << TPATID_COL_UID;
        vals << patid;
        if (!dbConnID->insertDB(TABLE_PATIENTD_IDS,cols,vals,druid)){
            log.appendError("On VERIFY PATIENT: Adding a new patient: " + dbConnID->getError());
            ret.errorCode = DBACK_DBCOMM_ERROR;
            return ret;
        }

        // Once added the keyid is obtained.
        cols.clear();
        cols << TPATID_COL_KEYID;
        cond = QString(TPATID_COL_UID) + "='" + patid +"'";

        if (!dbConnID->readFromDB(TABLE_PATIENTD_IDS,cols,cond)){
            log.appendError("On VERIFY PATIENT, DB Error: " + dbConnID->getError());
            ret.errorCode = DBACK_DBCOMM_ERROR;
            return ret;
        }

        data = dbConnID->getLastResult();

        if (data.rows.size() == 1){
            if (data.rows.first().size() == 1){
                ret.puid = data.rows.first().first().toInt();
                ret.logid = druid  + " for " + QString::number(ret.puid);
                return ret;
            }
        }

        // If the code got here there was a problem with the query.
        log.appendError("On VERIFY PATIENT: Error on query for PatIDTable, after adding it. A number of rows and columns different than 1 was returned");
        ret.errorCode = DBACK_DBCOMM_ERROR;
        return ret;

    }
    else{
        if (data.rows.first().size() > 0)  {
            //qWarning() << "The keyid for the patient is" << data.rows;
            ret.puid = data.rows.first().first().toInt();
        }
        else {
            log.appendError("On VERIFY PATIENT: Error on query for PatIDTable. Rows were returned but with not columns");
            ret.errorCode = DBACK_DBCOMM_ERROR;
            return ret;
        }
    }

    ret.logid = druid  + " for " + QString::number(ret.puid);
    return ret;
}


bool DBCommSSLServer::initAllDBS(){
    if (!dbConnBase->open()){
        log.appendError("ERROR : Could not start SQL Connection to the Base DB: " + dbConnBase->getError());
        return false;
    }
    if (!dbConnID->open()){
        log.appendError("ERROR : Could not start SQL Connection to the Patient ID DB: " + dbConnID->getError());
        return false;
    }
    if (!dbConnPatData->open()){
        log.appendError("ERROR : Could not start SQL Connection to the Patient Data DB: " + dbConnPatData->getError());
        return false;
    }
    return true;
}

DBInterface * DBCommSSLServer::getDBIFFromTable(const QString &tableName){
    if (tableName == TABLE_PATIENTD_IDS) return dbConnID;
    else if (tableName == TABLE_PATDATA) return dbConnPatData;
    else return dbConnBase;
}

void DBCommSSLServer::processSQLRequest(quint64 socket){

    QString where = "DB processSQLRequest: ";
    SSLIDSocket *sslsocket = sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(socket,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        return;
    }

    if (!initAllDBS()) return;

    DataPacket dp = sslsocket->getDataPacket();
    QString tx_type = dp.getField(DataPacket::DPFI_DB_QUERY_TYPE).data.toString();

    // Which tables affect which transactions:
    QStringList tableNames = dp.getField(DataPacket::DPFI_DB_TABLE).data.toString().split(DB_TRANSACTION_LIST_SEP);

    // List of columns for each transaction
    QStringList allCols = dp.getField(DataPacket::DPFI_DB_COL).data.toString().split(DB_TRANSACTION_LIST_SEP);
    QList<QStringList> columnsList;
    for (qint32 i = 0; i < allCols.size(); i++){
        columnsList << allCols.at(i).split(DB_COLUMN_SEP);
    }

    DataPacket tx;

    if (tx_type == SQL_QUERY_TYPE_GET){

        // It is either getting Doctor data or Patient Data
        QStringList conditions = dp.getField(DataPacket::DPFI_DB_CONDITIION).data.toString().split(DB_TRANSACTION_LIST_SEP);

        // Executing each transaction and appending the result

        QStringList dberrors;
        QStringList queryresults;

        //qWarning() << "Table Names" << tableNames;

        for (qint32 i = 0; i < tableNames.size(); i++){

            DBInterface *dbConnection = getDBIFFromTable(tableNames.at(i));

            if (!dbConnection->readFromDB(tableNames.at(i),columnsList.at(i),conditions.at(i))){
                dberrors << dbConnection->getError();
                queryresults << "";
                log.appendError("On SQL Transaction: " + dbConnection->getError());
            }
            else{
                DBData dbdata = dbConnection->getLastResult();
                queryresults << dbdata.joinRowsAndCols(DB_ROW_SEP,DB_COLUMN_SEP);
                dberrors << "";
            }
        }

        // Adding the tables
        tx.addString(dp.getField(DataPacket::DPFI_DB_TABLE).data.toString(),DataPacket::DPFI_DB_TABLE);
        // Adding the columns
        tx.addString(dp.getField(DataPacket::DPFI_DB_COL).data.toString(),DataPacket::DPFI_DB_COL);
        // Adding the values
        tx.addString(queryresults.join(DB_TRANSACTION_LIST_SEP),DataPacket::DPFI_DB_VALUE);
        // Adding the errors
        tx.addString(dberrors.join(DB_TRANSACTION_LIST_SEP),DataPacket::DPFI_DB_ERROR);

    }
    else{

        // Data will be inserted in table
        // Special processing is required if table is the patient table.

        QStringList allValues = dp.getField(DataPacket::DPFI_DB_VALUE).data.toString().split(DB_TRANSACTION_LIST_SEP);
        QList<QStringList> values;
        for (qint32 i = 0; i < allValues.size(); i++){
            values << allValues.at(i).split(DB_COLUMN_SEP);
        }

        // A set should return either the errors or an ack.
        QStringList dberrors;
        quint8 code = DBACK_ALL_OK;

        for (qint32 i = 0; i < tableNames.size(); i++){
            //log.appendStandard("SET " + QString::number(i) +  " on TABLE " + tableNames.at(i) + ": COLUMNS -> " + columnsList.at(i).join("|") + ". VALUES: " + values.at(i).join("|"));

            //qWarning() << "Setting information on" << tableNames.at(i);

            VerifyDBRetStruct ret;

            // TWO CHECKS Are Required:
            // 1) If this is a doctor information, it must come from an existing insitution
            // 2) If this is a patient it must come from an existing doctor.
            if (tableNames.at(i) == TABLE_DOCTORS){
                ret = verifyDoctor(columnsList.at(i),values.at(i));
                if (ret.errorCode != DBACK_ALL_OK) break;
            }

            if (tableNames.at(i) == TABLE_PATDATA){
                ret = verifyPatient(columnsList.at(i),values.at(i));
                if (ret.errorCode != DBACK_ALL_OK) {
                    code = ret.errorCode;
                    break;
                }
                // Replacing the universal ID with the DB ID.
                // qWarning() << "Index of the pat uid" << ret.indexOfPatUid << "the puid" << ret.puid;
                values[i][ret.indexOfPatUid] = QString::number(ret.puid);
            }

            // This takes advantage of the fact that there are no tables with the same names in the different databases.
            DBInterface *dbConnection = getDBIFFromTable(tableNames.at(i));

            if (!dbConnection->insertDB(tableNames.at(i),columnsList.at(i),values.at(i),ret.logid)){
                dberrors << dbConnection->getError();
                log.appendError("On SQL Transaction: " + dbConnection->getError());
            }
        }

        //log.appendStandard("Setting SET ACK Field with " + QString::number(code));
        tx.addValue(code,DataPacket::DPFI_DB_SET_ACK);
        if (dberrors.size() > 0){
            tx.addString(dberrors.join(DB_TRANSACTION_LIST_SEP),DataPacket::DPFI_DB_ERROR);
        }

    }

    QByteArray ba = tx.toByteArray();
    qint64 num = sslsocket->socket()->write(ba.constData(),ba.size());
    if (num != ba.size()){
        log.appendError("Failure sending db ans to host: " + sslsocket->socket()->peerAddress().toString());
    }

    // Closing connection to db.
    dbConnBase->close();
    dbConnID->close();
    dbConnPatData->close();

    // In this case the transaction is done.
    sockets.releaseSocket(socket,where);

}

void DBCommSSLServer::processUpdateRequest(quint64 socket){

    QString where = "DB processUpdateRequest: ";
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

    // Saving flogs if any.
    if (sslsocket->getDataPacket().hasInformationField(DataPacket::DPFI_UPDATE_FLOGNAMES)){

        QString filenames_str = sslsocket->getDataPacket().getField(DataPacket::DPFI_UPDATE_FLOGNAMES).data.toString();
        QString filecontents_str = sslsocket->getDataPacket().getField(DataPacket::DPFI_UPDATE_FLOGCONTENT).data.toString();
        QStringList filenames = filenames_str.split(DB_LIST_IN_COL_SEP);
        QStringList filecontens = filecontents_str.split(DB_LIST_IN_COL_SEP);

        if (filecontens.size() != filenames.size()){
            log.appendError("Number of flog filenames : (" + QString::number(filecontens.size())  + ") does not match the number of file contents (" + QString::number(filenames.size()) + ")");
            sendUpdateAns(DataPacket(),socket,"FAILED");
            sockets.releaseSocket(socket,where);
            return;
        }

        for (qint32 i = 0; i < filecontens.size(); i++){
            QFile file(flogDirPath + "/" + filenames.at(i));
            if (!file.open(QFile::WriteOnly)){
                log.appendError("Could not open f log file for writing: " + file.fileName());
                continue;
            }
            QTextStream writer(&file);
            writer << filecontens.at(i);
            file.close();
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

void DBCommSSLServer::sendUpdateAns(DataPacket tx, quint64 socket, const QString &ans){
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

void DBCommSSLServer::changedState(quint64 id){

    Q_UNUSED(id)
    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    //    QAbstractSocket::SocketState state = sslsocket->socket()->state();
    //    QHostAddress addr = sslsocket->socket()->peerAddress();
    //    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    //    log.appendStandard(addr.toString() + ": " + QString("New socket state, ") + metaEnum.valueToKey(state));

}


