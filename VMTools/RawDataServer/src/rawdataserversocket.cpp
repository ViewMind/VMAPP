#include "rawdataserversocket.h"

RawDataServerSocket::RawDataServerSocket(QSslSocket *newSocket, quint64 id, ConfigurationManager *c):SSLIDSocket(newSocket,id)
{
    QString idAsStr = QString::number(id);
    QString transactionID = QDateTime::currentDateTime().toString("yyyy_MM_dd_hh_mm_ss") + "_TID" + idAsStr;

    config = c;
    verifcationPassword = config->getString(RAW_DATA_SERVER_PASSWORD);

    finishedTask = true;

    // Creating the DATABASE Connections.
    // Initializing the database connections.
    QString user = config->getString(CONFIG_DBUSER);
    QString passwd = config->getString(CONFIG_DBPASSWORD);

    QString host = config->getString(CONFIG_DATA_DBHOST);
    QString dbname = config->getString(CONFIG_DATA_DBNAME);
    quint16 port = config->getInt(CONFIG_DATA_DBPORT);
    dbConnBase.setupDB(DB_NAME_BASE,host,dbname,user,passwd,port,"",false);
    dbInstanceNames << dbConnBase.getInstanceName();

    host = config->getString(CONFIG_ID_DBHOST);
    dbname = config->getString(CONFIG_ID_DBNAME);
    port = config->getInt(CONFIG_ID_DBPORT);
    dbConnID.setupDB(DB_NAME_ID,host,dbname,user,passwd,port,"",false);
    dbInstanceNames << dbConnID.getInstanceName();

    host = config->getString(CONFIG_PATDATA_DBHOST);
    dbname = config->getString(CONFIG_PATDATA_DBNAME);
    port = config->getInt(CONFIG_PATDATA_DBPORT);
    dbConnPatData.setupDB(DB_NAME_PATDATA,host,dbname,user,passwd,port,"",false);
    dbInstanceNames << dbConnPatData.getInstanceName();

    host = config->getString(CONFIG_DASH_DBHOST);
    dbname = config->getString(CONFIG_DASH_DBNAME);
    port = config->getInt(CONFIG_DASH_DBPORT);
    dbConnDash.setupDB(DB_NAME_DASHBOARD,host,dbname,user,passwd,port,"",false);
    dbInstanceNames << dbConnDash.getInstanceName();


    // Customized log file.
    log.setLogFileLocation(QString(DIRNAME_SERVER_LOGS) + "/" + transactionID);
    log.appendStandard("Created connection id: " + transactionID + " with address " + sslSocket->peerAddress().toString());
    timer.setInterval(CONNECTION_TIMEOUT);
    timer.start();
}

void RawDataServerSocket::on_encryptedSuccess(){
    SSLIDSocket::on_encryptedSuccess();
    log.appendStandard("Established encrypted connection waiting for data to be sent");
    timer.setInterval(CONNECTION_WAIT_FOR_REQUEST);
    timer.start();
}

void RawDataServerSocket::on_timeout(){
    SSLIDSocket::on_timeout();
    log.appendError("Reached timeout and the information has not arrived. disconnecocting ...");
    on_disconnected();
}


void RawDataServerSocket::on_disconnected(){
    timer.stop();
    doDisconnects();
    if (!disconnectReceived){
        if (!finishedTask){
            log.appendError("The task is pending completion. Waiting for completion...");
        }
        else{
            log.appendStandard("Finsihed task. Requesting socket deletion....");
            dbConnBase.close();
            dbConnID.close();
            dbConnPatData.close();
            dbConnDash.close();
            emit(socketDone(ID));
        }
        disconnectReceived = true;
    }
    else{
        log.appendStandard("Finsihed task. Requesting socket deletion (was disconnected previously)....");
        dbConnBase.close();
        dbConnID.close();
        dbConnPatData.close();
        dbConnDash.close();
        emit(socketDone(ID));
    }
}

void RawDataServerSocket::doDisconnects(){
    SSLIDSocket::doDisconnects();
}


void RawDataServerSocket::on_readyRead(){
    QByteArray ba = socket()->readAll();
    quint8 ans = rx.bufferByteArray(ba);
    if (ans == DataPacket::DATABUFFER_RESULT_DONE){
        timer.stop();
        if (rx.hasInformationField(DataPacket::DPFI_LOCAL_DB_BKP)){
            log.appendStandard("Local DB BKP Request");
            finishedTask = false;
            oprLocalDBBkp();
        }
        else if (rx.hasInformationField(DataPacket::DPFI_VMID_TABLE)){
            log.appendStandard("VM ID Table Request");
            finishedTask = false;
            oprVMIDTableRequest();
        }
        else if (rx.hasInformationField(DataPacket::DPFI_DB_MEDICAL_RECORD_FILE)){
            log.appendStandard("Medical Record Request");
            finishedTask = false;
            oprMedicalRecords();
        }
        else if (rx.hasInformationField(DataPacket::DPFI_DB_INST_PASSWORD)) {
            // Data query.
            log.appendStandard("RAW Data Request");
            finishedTask = false;
            oprProcessRequest();
        }
        else if (rx.hasInformationField(DataPacket::DPFI_INST_NAMES)){
            // Assuming is the Institution only query.
            log.appendStandard("Institution list request");
            finishedTask = false;
            oprInstList();
        }
        else {
            log.appendError("Non update request reached the server, it is ignored");
            on_disconnected();
        }
    }
    else if (ans == DataPacket::DATABUFFER_RESULT_ERROR){
        timer.stop();
        log.appendError("Finshed receiving data but it was corrupted.");
        on_disconnected();
    }
}


void RawDataServerSocket::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    log.appendWarning(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
}

void RawDataServerSocket::on_socketStateChanged(QAbstractSocket::SocketState state){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    log.appendStandard(QString("SOCKET STATE: ") + metaEnum.valueToKey(state));
}

void RawDataServerSocket::on_sslErrors(const QList<QSslError> &errorlist){
    QHostAddress addr = sslSocket->peerAddress();
    for (qint32 i = 0; i < errorlist.size(); i++){
        log.appendWarning("SSL Error," + errorlist.at(i).errorString() + " from Address: " + addr.toString());
    }
}

RawDataServerSocket::~RawDataServerSocket(){
    timer.stop();
    if (sslSocket != nullptr) delete sslSocket;
}

void RawDataServerSocket::oprMedicalRecords(){
    if (!verifyPassword(rx.getField(DataPacket::DPFI_DB_INST_PASSWORD).data.toString())){
        sendErrorMessage("Wrong Password");
        return;
    }

    if (!dbConnBase.open()){
        log.appendError("Getting requested information, could not open DB CON BASE: " + dbConnBase.getError());
        sendErrorMessage("Internal DB Open Error");
        return;
    }

    if (!dbConnID.open()){
        log.appendError("Getting requested information, could not open DB CON ID: " + dbConnID.getError());
        sendErrorMessage("Internal DB Open Error");
        return;
    }


    if (!dbConnPatData.open()){
        log.appendError("Getting requested information, could not open DB CON PAT DATA: " + dbConnPatData.getError());
        sendErrorMessage("Internal DB Open Error");
        return;
    }

    QString instUID = rx.getField(DataPacket::DPFI_DB_INST_UID).data.toString();
    log.appendStandard("Getting PUID <-> HPUID map for institution: " + instUID);

    QStringList columns;
    QString condition;
    DBData dbres;

    ///////////////////////////////////////////////////////////////////////////////////
    log.appendStandard("Getting the doctors for said instituion");

    columns << TDOCTOR_COL_UID;
    condition = QString(TDOCTOR_COL_MEDICAL_INST) + " = '" + instUID + "'";
    if (!dbConnBase.readFromDB(TABLE_DOCTORS,columns,condition)){
        log.appendError("Getting doctor ID Information: " + dbConnBase.getError());
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    dbres = dbConnBase.getLastResult();
    if (dbres.rows.isEmpty()){
        log.appendError("Getting doctor ID information query returned empty");
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    QStringList druidlist;
    for (qint32 i = 0; i < dbres.rows.size(); i++){
        if (dbres.rows.at(i).size() != columns.size()){
            log.appendError("Getting doctor ID information: expected number of columns is " + QString::number(columns.size()) + " but result " + QString::number(i)
                            + " has " + QString::number(dbres.rows.at(i).size()));
            sendErrorMessage("Internal DB Query ERROR");
            return;
        }
        druidlist << dbres.rows.at(i).first();
    }

    ///////////////////////////////////////////////////////////////////////////////////
    log.appendStandard("Getting the PUIDS for given doctors: " + druidlist.join("--"));
    columns.clear();

    columns << TPATDATA_COL_PUID << TPATDATA_COL_DOCTORID;
    condition = QString(TPATDATA_COL_DOCTORID) +  " IN ('" + druidlist.join("','") + "')";
    if (!dbConnPatData.readFromDB(TABLE_PATDATA,columns,condition)){
        log.appendError("Getting PUID Information: " + dbConnPatData.getError());
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    dbres = dbConnPatData.getLastResult();
    if (dbres.rows.isEmpty()){
        log.appendError("Getting doctor ID information query returned empty");
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    QSet<QString> puidset;  // using a set to avoid duplicates.
    for (qint32 i = 0; i < dbres.rows.size(); i++){
        if (dbres.rows.at(i).size() != columns.size()){
            log.appendError("Getting PUID ID information: expected number of columns is " + QString::number(columns.size()) + " but result " + QString::number(i)
                            + " has " + QString::number(dbres.rows.at(i).size()));
            sendErrorMessage("Internal DB Query ERROR");
            return;
        }
        puidset << dbres.rows.at(i).first();
    }

    QStringList puidlist = puidset.toList();

    ///////////////////////////////////////////////////////////////////////////////////
    log.appendStandard("Getting medical records for patients: " + puidlist.join("--"));
    columns.clear();

    columns << TPATMEDREC_COL_PUID               // 0
            << TPATMEDREC_COL_DATE               // 1
            << TPATMEDREC_COL_EVALS              // 2
            << TPATMEDREC_COL_MEDICATION         // 3
            << TPATMEDREC_COL_PRESUMP_DIAGNOSIS  // 4
            << TPATMEDREC_COL_KEYID              // 5
            << TPATMEDREC_COL_REC_INDEX          // 6
            << TPATMEDREC_COL_RNM;               // 7

    qint32 indexOfPUID     = columns.indexOf(TPATMEDREC_COL_PUID);
    qint32 indexOfRecIndex = columns.indexOf(TPATMEDREC_COL_REC_INDEX);


    QStringList columnInRecord;
    columnInRecord << TPATMEDREC_COL_DATE << TPATMEDREC_COL_EVALS << TPATMEDREC_COL_MEDICATION
                   << TPATMEDREC_COL_PRESUMP_DIAGNOSIS << TPATMEDREC_COL_RNM << TPATMEDREC_COL_REC_INDEX;

    condition = QString(TPATMEDREC_COL_PUID) +  " IN ('" + puidlist.join("','") + "') ORDER BY " + QString(TPATMEDREC_COL_KEYID) + " ASC";

    if (!dbConnPatData.readFromDB(TABLE_MEDICAL_RECORDS,columns,condition)){
        log.appendError("Getting medical record information: " + dbConnPatData.getError());
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }


    dbres = dbConnPatData.getLastResult();
    QVariantMap medicalInformation; // Each entry will contain a map with the patient info in the server and a list of maps, representing the medical records used.


    for (qint32 i = 0; i < dbres.rows.size(); i++){
        QStringList row = dbres.rows.at(i);
        if (row.size() != columns.size()){
            log.appendError("Getting medical record information. expected number of columns is " + QString::number(columns.size()) + " but result " + QString::number(i)
                            + " has " + QString::number(row.size()));
            sendErrorMessage("Internal DB Query ERROR");
            return;
        }
        QString puid = row.at(indexOfPUID);
        qint32 recInd = row.at(indexOfRecIndex).toInt();

        QVariantMap patientInfo;
        QVariantList patientRecords;
        if (medicalInformation.contains(puid)) {
            patientInfo = medicalInformation.value(puid).toMap();
            patientRecords = patientInfo.value(RAW_DATA_SERVER_RETURN_MEDICAL_RECORDS).toList();
        }

        // The new record will either be overwrite a blank record or a overwrite an older record.
        QVariantMap record;
        for (qint32 j = 0; j < columnInRecord.size(); j++){
            QString col = columnInRecord.at(j);
            record[col] = row.at(columns.indexOf(col));
        }

        if (recInd >= patientRecords.size()){
            // Completing the list to the record index
            qint32 num2add = recInd + 1 - patientRecords.size();
            for (qint32 j = 0; j < num2add; j++) {
                QVariantMap newRecord;
                newRecord[TPATMEDREC_COL_REC_INDEX] = patientRecords.size();
                patientRecords << newRecord;
            }
        }

        patientRecords[recInd] = record; // Since the keyid is newer larger keyids will ALWAYS replaces lower keyids.
        patientInfo[RAW_DATA_SERVER_RETURN_MEDICAL_RECORDS] = patientRecords;
        medicalInformation[puid] = patientInfo;
    }

    ///////////////////////////////////////////////////////////////////////////////////
    log.appendStandard("Getting the patient information");
    columns.clear();

    columns << TPATDATA_COL_BIRTHCOUNTRY
            << TPATDATA_COL_BIRTHDATE
            << TPATDATA_COL_DATE_INS
            << TPATDATA_COL_DOCTORID
            << TPATDATA_COL_FIRSTNAME
            << TPATDATA_COL_FORMATIVE_YEARS
            << TPATDATA_COL_LASTNAME
            << TPATDATA_COL_PUID
            << TPATDATA_COL_SEX;

    condition = QString(TPATDATA_COL_PUID) +  " IN ('" + puidlist.join("','") + "') ORDER BY " + QString(TPATDATA_COL_KEYID) + " ASC";

    if (!dbConnPatData.readFromDB(TABLE_PATDATA,columns,condition)){
        log.appendError("Getting Patient Information : " + dbConnPatData.getError());
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    dbres = dbConnPatData.getLastResult();

    for (qint32 i = 0; i < dbres.rows.size(); i++){
        QVariantMap patientInfo;
        QStringList row = dbres.rows.at(i);
        if (row.size() != columns.size()){
            log.appendError("Getting patient information. expected number of columns is " + QString::number(columns.size()) + " but result " + QString::number(i)
                            + " has " + QString::number(row.size()));
            sendErrorMessage("Internal DB Query ERROR");
            return;
        }

        QString puid = "-1";
        for (qint32 j = 0; j < columns.size(); j++){
            if (columns.at(j) == TPATDATA_COL_PUID){
                puid = row.at(j);
            }
            else{
                patientInfo[columns.at(j)] = row.at(j);
            }
        }

        QVariantMap patStruct = medicalInformation.value(puid).toMap();
        patStruct[RAW_DATA_SERVER_RETURN_PATIENT_DATA] = patientInfo;
        medicalInformation[puid] = patStruct;

    }


    ///////////////////////////////////////////////////////////////////////////////////
    // Once the data is gathered is sent as a JSON String.
    QJsonDocument jsonDoc = QJsonDocument::fromVariant(medicalInformation);

    log.appendStandard("Sending medical records back to client");
    // Separating as string lists.
    DataPacket tx;
    tx.addString(QString(jsonDoc.toJson(QJsonDocument::Compact)),DataPacket::DPFI_DB_MEDICAL_RECORD_FILE);
    QByteArray ba = tx.toByteArray();
    qint64 num = sslSocket->write(ba.constData(),ba.size());
    if (num != ba.size()){
        log.appendError("Failure sending medical record data table to client");
    }

    finishedTask = true;
    on_disconnected();

}

void RawDataServerSocket::oprLocalDBBkp(){

    if (!verifyPassword(rx.getField(DataPacket::DPFI_DB_INST_PASSWORD).data.toString())){
        sendErrorMessage("Wrong Password");
        return;
    }

    // Password checks out. Searching for the existance of the Backup.
    QString instUID = rx.getField(DataPacket::DPFI_LOCAL_DB_BKP).data.toString();
    QString instEDirname = config->getString(CONFIG_ETDIR_PATH) + "/" + instUID;
    QStringList dirsInInstPath = QDir(instEDirname).entryList(QStringList(),QDir::Dirs|QDir::NoDotAndDotDot);

    log.appendStandard("Requested local DB for institution: " + instUID);

    QString serializedMap;
    serializedMap = "";

    QVariantMap patientIDmap;

    for (qint32 i = 0; i < dirsInInstPath.size(); i++){
        QString dbfilename = instEDirname + "/" + dirsInInstPath.at(i) + "/" + QString(FILE_LOCAL_DB_BKP);
        if (!QFile(dbfilename).exists()) {
            log.appendWarning("Could not load local db file: " + dbfilename);
            continue;
        }

        log.appendStandard("Loading local DB info for " + dbfilename);

        // Getting the local information and a list of the hashed patient ids.
        LocalInformationManager lim;
        lim.setWorkingFile(dbfilename);
        serializedMap = lim.serialDoctorPatientString(serializedMap);

        patientIDmap = lim.getHashedIDPatientMap(patientIDmap);
    }

    QVariantMap puidTouidMap;
    if (!serializedMap.isEmpty()){

        // Getting hashed puid to puid map.
        if (!dbConnID.open()){
            log.appendError("Getting local db bkp, could not open DB CON ID: " + dbConnID.getError());
            sendErrorMessage("Internal DB Open error");
            return;
        }

        log.appendStandard("Getting the ID Table data for the patientIDs requested");
        QStringList cols; cols << TPATID_COL_KEYID << TPATID_COL_UID;
        QString condition = QString(TPATID_COL_UID) +  " IN ('" + patientIDmap.keys().join("','") + "')";
        if (!dbConnID.readFromDB(TABLE_PATIENTD_IDS,cols,condition)){
            log.appendError("Getting the patiend ID hashes: " + dbConnID.getError());
            sendErrorMessage("Internal DB Query error");
            return;
        }

        DBData res = dbConnID.getLastResult();
        for (qint32 i = 0; i < res.rows.size(); i++){
            if (res.rows.at(i).size() != 2){
                log.appendError("Getting information from Patient Table IDs expected 2 column But got: " + QString::number(res.rows.at(i).size()));
                sendErrorMessage("Internal DB Query error");
                return;
            }

            QString hashid = res.rows.at(i).last();
            QString puid   = res.rows.at(i).first();

            if (patientIDmap.contains(hashid)){
                puidTouidMap[puid] = patientIDmap.value(hashid).toString();
            }
            else{
                log.appendError("NO PUID FOUND for HASH: " + hashid);
            }
        }
    }


    QString serializedIDmap;
    if (puidTouidMap.size() > 0){
        serializedIDmap = VariantMapSerializer::serializeOneLevelVariantMap("",puidTouidMap);
    }
    else{
        log.appendError("No backup data found");
        sendErrorMessage("No available data to backup");
        return;
    }

    DataPacket tx;
    tx.addString(serializedMap,DataPacket::DPFI_SERIALIZED_DB);
    tx.addString(serializedIDmap,DataPacket::DPFI_PUID_LIST);
    QByteArray ba = tx.toByteArray();
    qint64 num = sslSocket->write(ba.constData(),ba.size());
    if (num != ba.size()){
        log.appendError("Failure sending the wrong password message");
    }
    else log.appendStandard("Finished sending the serialized maps");
    finishedTask = true;
    on_disconnected();
}

void RawDataServerSocket::oprProcessRequest(){
    DataPacket tx;

    if (!dbConnBase.open()){
        log.appendError("Getting requested information, could not open DB CON BASE: " + dbConnBase.getError());
        sendErrorMessage("Internal DB Open Error");
        return;
    }

    if (!dbConnID.open()){
        log.appendError("Getting requested information, could not open DB CON ID: " + dbConnID.getError());
        sendErrorMessage("Internal DB Open Error");
        return;
    }

    QString password = rx.getField(DataPacket::DPFI_DB_INST_PASSWORD).data.toString();
    QString instUID = rx.getField(DataPacket::DPFI_DB_INST_UID).data.toString();
    QString startDate = rx.getField(DataPacket::DPFI_DATE_START).data.toString();
    QString endDate = rx.getField(DataPacket::DPFI_DATE_END).data.toString();

    log.appendStandard("Getting data for INST: " + instUID + " between " + startDate + " and " + endDate);

    if (!verifyPassword(password)){
        sendErrorMessage("Wrong password");
        return;
    }

    ///////////////// Getting the puids in the time frame and from the institution rquestested
    QStringList columns;
    QString query;
    query = "SELECT DISTINCT r.puid FROM tEyeResults AS r, tDoctors AS d WHERE r.study_date >= '" + startDate + "' AND r.study_date <= '"
            + endDate + "' AND r.doctorid = d.uid AND d.medicalinstitution = '" + instUID + "'";
    columns << TEYERES_COL_PUID;

    if (!dbConnBase.specialQuery(query,columns)){
        log.appendError("Getting information from the Eye Results table: " + dbConnBase.getError());
        return;
    }

    DBData res = dbConnBase.getLastResult();
    QStringList puids;
    for (qint32 i = 0; i < res.rows.size(); i++){
        if (res.rows.at(i).size() != 1){
            log.appendError("Getting information from Eye Result Table expected 1 column But got: " + QString::number(res.rows.at(i).size()));
            sendErrorMessage("Internal DB Query Error");
            return;
        }
        puids << res.rows.at(i).first();
    }

    if (puids.isEmpty()){
        sendErrorMessage("No patients from institution " + instUID + " were found between " + startDate + " and " + endDate);
        return;
    }

    ///////////////// Getting the hashes from the puid.
    columns.clear();
    columns << TPATID_COL_KEYID << TPATID_COL_UID;
    QString condition;
    condition = "keyid IN (" + puids.join(",") + ")";
    if (!dbConnID.readFromDB(TABLE_PATIENTD_IDS,columns,condition)){
        log.appendError("Getting the patiend ID hashes: " + dbConnID.getError());
        sendErrorMessage("No patients from institution " + instUID + " were found between " + startDate + " and " + endDate);
        return;
    }

    res = dbConnID.getLastResult();
    QHash<QString,QString> puidHashMap;
    for (qint32 i = 0; i < res.rows.size(); i++){
        if (res.rows.at(i).size() != 2){
            sendErrorMessage("Internal DB Query Error");
            return;
        }
        puidHashMap[res.rows.at(i).last()] = res.rows.at(i).first();
        //qWarning() << "HASHMAP:" <<  res.rows.at(i).last() << res.rows.at(i).first();
    }


    ///////////////// Getting the data from S3
    log.appendStandard("Getting the data from S3");
    QStringList hashes = puidHashMap.keys();
    QStringList downloadedDirectories;

    S3Interface s3;
    s3.setS3Bin(VIEWMIND_DATA_REPO);

    for (qint32 i = 0; i < hashes.size(); i++){
        log.appendStandard("Downloading the directory: " + hashes.at(i));
        QString name = puidHashMap.value(hashes.at(i));
        while (name.size() < 10) name = "0" + name;
        downloadedDirectories << name;
        s3.copyRecursively(hashes.at(i), name);
    }

    ///////////////// Processing the resulting files in order to send them.
    log.appendStandard("Processing the downloaded files in order to send them ...");
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
        log.appendError("Could not read the downlaoded files");
        sendErrorMessage("Error loading downlaoded files");
        return;
    }

    // If all was ok. The information is sent back.
    tx.addString(fileNames.join(DB_LIST_IN_COL_SEP),DataPacket::DPFI_RAW_DATA_NAMES);
    tx.addString(fileContents.join(DB_LIST_IN_COL_SEP),DataPacket::DPFI_RAW_DATA_CONTENT);
    QByteArray ba = tx.toByteArray();
    qint64 num = sslSocket->write(ba.constData(),ba.size());
    if (num != ba.size()){
        log.appendError("Failure sending the failed downloaded files error");
    }

    finishedTask = true;
    on_disconnected();
}

void RawDataServerSocket::oprInstList(){

    QStringList ans;
    QStringList columns;

    if (!dbConnDash.open()){
        log.appendError("Getting institutions names and uids, could not open DB CON DASH: " + dbConnDash.getError());
        sendErrorMessage("Internal DB Open ERROR");
        return;
    }

    columns << TINST_COL_ENABLED << TINST_COL_UID << TINST_COL_NAME;

    if (!dbConnDash.readFromDB(TABLE_INSTITUTION,columns,"")){
        log.appendError("Getting institutions names and uids: " + dbConnDash.getError());
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    DBData res = dbConnDash.getLastResult();

    for (qint32 i = 0; i < res.rows.size(); i++){

        if (res.rows.at(i).size() != 3){
            log.appendError("Getting institutions expected three values per row. But got: " + QString::number(res.rows.at(i).size()));
            sendErrorMessage("Internal DB Query ERROR");
            return;
        }
        ans << res.rows.at(i).first();
        ans << res.rows.at(i).at(1);
        ans << res.rows.at(i).last();
    }

    DataPacket tx;
    tx.addString(ans.join(DB_LIST_IN_COL_SEP),DataPacket::DPFI_INST_NAMES);
    QByteArray ba = tx.toByteArray();
    qint64 num = sslSocket->write(ba.constData(),ba.size());
    if (num != ba.size()){
        log.appendError("Failure sending then institution list information");
    }
    finishedTask = true;
    on_disconnected();
}

void RawDataServerSocket::oprVMIDTableRequest(){
    if (!verifyPassword(rx.getField(DataPacket::DPFI_DB_INST_PASSWORD).data.toString())){
        sendErrorMessage("Wrong Password");
        return;
    }

    if (!dbConnBase.open()){
        log.appendError("Getting requested information, could not open DB CON BASE: " + dbConnBase.getError());
        sendErrorMessage("Internal DB Open Error");
        return;
    }

    if (!dbConnID.open()){
        log.appendError("Getting requested information, could not open DB CON ID: " + dbConnID.getError());
        sendErrorMessage("Internal DB Open Error");
        return;
    }


    if (!dbConnPatData.open()){
        log.appendError("Getting requested information, could not open DB CON PAT DATA: " + dbConnPatData.getError());
        sendErrorMessage("Internal DB Open Error");
        return;
    }

    QString instUID = rx.getField(DataPacket::DPFI_DB_INST_UID).data.toString();
    log.appendStandard("Getting PUID <-> HPUID map for institution: " + instUID);

    QStringList columns;
    QString condition;
    DBData dbres;

    ///////////////////////////////////////////////////////////////////////////////////
    log.appendStandard("Getting the doctors for said instituion");

    columns << TDOCTOR_COL_UID;
    condition = QString(TDOCTOR_COL_MEDICAL_INST) + " = '" + instUID + "'";
    if (!dbConnBase.readFromDB(TABLE_DOCTORS,columns,condition)){
        log.appendError("Getting doctor ID Information: " + dbConnBase.getError());
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    dbres = dbConnBase.getLastResult();
    if (dbres.rows.isEmpty()){
        log.appendError("Getting doctor ID information query returned empty");
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    QStringList druidlist;
    for (qint32 i = 0; i < dbres.rows.size(); i++){
        if (dbres.rows.at(i).size() != columns.size()){
            log.appendError("Getting doctor ID information: expected number of columns is " + QString::number(columns.size()) + " but result " + QString::number(i)
                            + " has " + QString::number(dbres.rows.at(i).size()));
            sendErrorMessage("Internal DB Query ERROR");
            return;
        }
        druidlist << dbres.rows.at(i).first();
    }

    ///////////////////////////////////////////////////////////////////////////////////
    log.appendStandard("Getting the PUIDS for given doctors: " + druidlist.join("--"));
    columns.clear();

    columns << TPATDATA_COL_PUID << TPATDATA_COL_DOCTORID;
    condition = QString(TPATDATA_COL_DOCTORID) +  " IN ('" + druidlist.join("','") + "')";
    if (!dbConnPatData.readFromDB(TABLE_PATDATA,columns,condition)){
        log.appendError("Getting PUID Information: " + dbConnPatData.getError());
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    dbres = dbConnPatData.getLastResult();
    if (dbres.rows.isEmpty()){
        log.appendError("Getting doctor ID information query returned empty");
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    QSet<QString> puidset;  // using a set to avoid duplicates.
    for (qint32 i = 0; i < dbres.rows.size(); i++){
        if (dbres.rows.at(i).size() != columns.size()){
            log.appendError("Getting PUID ID information: expected number of columns is " + QString::number(columns.size()) + " but result " + QString::number(i)
                            + " has " + QString::number(dbres.rows.at(i).size()));
            sendErrorMessage("Internal DB Query ERROR");
            return;
        }
        puidset << dbres.rows.at(i).first();
    }

    QStringList puidlist = puidset.toList();


    ///////////////////////////////////////////////////////////////////////////////////
    log.appendStandard("Getting the HPUID List for PUIDLIST: " + puidlist.join("--"));
    columns.clear();
    columns << TPATID_COL_UID << TPATID_COL_KEYID;
    condition = QString(TPATID_COL_KEYID) +  " IN ('" + puidlist.join("','") + "')";
    if (!dbConnID.readFromDB(TABLE_PATIENTD_IDS,columns,condition)){
        log.appendError("Getting patient ID information: " + dbConnID.getError());
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }
    dbres = dbConnID.getLastResult();

    if (dbres.rows.isEmpty()){
        log.appendError("Getting patient ID information query returned empty");
        sendErrorMessage("Internal DB Query ERROR");
        return;
    }

    QHash<QString,QString> map;
    for (qint32 i = 0; i < dbres.rows.size(); i++){
        if (dbres.rows.at(i).size() != columns.size()){
            log.appendError("Getting patient ID information: expected number of columns is " + QString::number(columns.size()) + " but result " + QString::number(i)
                            + " has " + QString::number(dbres.rows.at(i).size()));
            sendErrorMessage("Internal DB Query ERROR");
            return;
        }
        // So that the hash and the PUID is in order.
        log.appendStandard("Adding HPUID: " + dbres.rows.at(i).first() + " mapped to " + dbres.rows.at(i).last());
        map[dbres.rows.at(i).first()] = dbres.rows.at(i).last();
    }

    log.appendStandard("Sending data back to client");
    // Separating as string lists.
    QStringList hpuidlist = map.keys();
    puidlist.clear();
    for (qint32 i = 0; i < hpuidlist.size(); i++) puidlist << map.value(hpuidlist.at(i));
    DataPacket tx;
    log.appendStandard("Seding back: " + puidlist.join(",") + "-" + hpuidlist.join(","));
    tx.addString(puidlist.join(",") + "-" + hpuidlist.join(","),DataPacket::DPFI_VMID_TABLE);
    QByteArray ba = tx.toByteArray();
    qint64 num = sslSocket->write(ba.constData(),ba.size());
    if (num != ba.size()){
        log.appendError("Failure sending vm id data table to client");
    }

    finishedTask = true;
    on_disconnected();

}

void RawDataServerSocket::sendErrorMessage(const QString &errMsg){
    DataPacket tx;
    log.appendStandard("Sending back error message: " + errMsg);
    tx.addString(errMsg,DataPacket::DPFI_DB_ERROR);
    QByteArray ba = tx.toByteArray();
    qint64 num = sslSocket->write(ba.constData(),ba.size());
    if (num != ba.size()){
        log.appendError("Failure sending error message on getting the local DB");
    }
    finishedTask = true;
    on_disconnected();
}

bool RawDataServerSocket::verifyPassword(const QString &password){
    QString hash =  QString(QCryptographicHash::hash(password.toLatin1(),QCryptographicHash::Sha256).toHex());
    if (hash == verifcationPassword) return true;
    else {
        log.appendError("Wrong Passsword: " + password);
        log.appendError("Expected hash: " + verifcationPassword + " but got hash " + hash);
        return false;
    }
}
