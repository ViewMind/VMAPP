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
    sockets[socket->getID()] = socket;

    // Doing the connection SIGNAL-SLOT
    connect(socket,&SSLIDSocket::sslSignal,this,&DBCommSSLServer::on_newSSLSignal);

    // The SSL procedure.
    sslsocket->setPrivateKey(":/certificates/server.key");
    sslsocket->setLocalCertificate(":/certificates/server.csr");
    sslsocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslsocket->startServerEncryption();

}


// Handling all signals
void DBCommSSLServer::on_newSSLSignal(quint64 socket, quint8 signaltype){

    switch (signaltype){
    case SSLIDSocket::SSL_SIGNAL_DISCONNECTED:
        if (sockets.contains(socket)){
            log.appendStandard("Lost connection from: " + sockets.value(socket)->socket()->peerAddress().toString());
            removeSocket(socket);
        }
        break;
    case SSLIDSocket::SSL_SIGNAL_ENCRYPTED:
        log.appendSuccess("SSL Handshake completed for address: " + sockets.value(socket)->socket()->peerAddress().toString());
        sockets.value(socket)->startTimeoutTimer(config->getInt(CONFIG_DATA_REQUEST_TIMEOUT)*1000);
        break;
    case SSLIDSocket::SSL_SIGNAL_ERROR:
        socketErrorFound(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_DATA_RX_DONE:
        // Information has arrived ok. Starting the process.
        //log.appendStandard("Done buffering data for " + QString::number(socket));
        sockets.value(socket)->stopTimer();
        processSQLRequest(socket);
        break;
    case SSLIDSocket::SSL_SIGNAL_DATA_RX_ERROR:
        sockets.value(socket)->stopTimer();
        // Data is most likely corrupted.
        if (sockets.contains(socket)){
            sockets.value(socket)->stopTimer();
            log.appendError("Buffering data from: " + sockets.value(socket)->socket()->peerAddress().toString() + " gave an error");
            removeSocket(socket);
        }
        break;
    case SSLIDSocket::SSL_SIGNAL_TIMEOUT:
        // This means that the data did not arrive on time.
        if (sockets.contains(socket)){
            sockets.value(socket)->stopTimer();
            log.appendError("Data from " + sockets.value(socket)->socket()->peerAddress().toString() + " did not arrive in time. SocketID: " + QString::number(socket));
            removeSocket(socket);
        }
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
}


void DBCommSSLServer::removeSocket(quint64 id){
    if (sockets.contains(id)) {
        if (sockets.value(id)->isValid()){
            sockets.value(id)->socket()->disconnectFromHost();
        }
        sockets.remove(id);
    }
}


void DBCommSSLServer::socketErrorFound(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    if (!sockets.contains(id)) return;

    QAbstractSocket::SocketError error = sockets.value(id)->socket()->error();
    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    if (error != QAbstractSocket::RemoteHostClosedError)
        log.appendError(QString("Socket Error found: ") + metaEnum.valueToKey(error) + QString(" from Address: ") + addr.toString());
    else
        log.appendStandard(QString("Closed connection from Address: ") + addr.toString());

    // Eliminating it from the list.
    removeSocket(id);

}

void DBCommSSLServer::sslErrorsFound(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    if (!sockets.contains(id)) return;

    QList<QSslError> errorlist = sockets.value(id)->socket()->sslErrors();
    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        log.appendError("SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }

    // Eliminating it from the list.
    removeSocket(id);

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

    if (!initAllDBS()) return;

    DataPacket dp = sockets.value(socket)->getDataPacket();
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
    qint64 num = sockets.value(socket)->socket()->write(ba.constData(),ba.size());
    if (num != ba.size()){
        log.appendError("Failure sending db ans to host: " + sockets.value(socket)->socket()->peerAddress().toString());
    }

    // Closing connection to db.
    dbConnBase->close();
    dbConnID->close();
    dbConnPatData->close();

    // In this case the transaction is done.
    removeSocket(socket);

}

void DBCommSSLServer::changedState(quint64 id){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    if (!sockets.contains(id)) return;

    QAbstractSocket::SocketState state = sockets.value(id)->socket()->state();
    QHostAddress addr = sockets.value(id)->socket()->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    log.appendStandard(addr.toString() + ": " + QString("New socket state, ") + metaEnum.valueToKey(state));

}


