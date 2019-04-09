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

    connect(&timer,&QTimer::timeout,this,&Control::onTimerTimeout);
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
    socket = nullptr;

}

void Control::on_newConnection(){

    if (socket == nullptr) return;

    // New connection is available.
    socket = (QSslSocket*)(listener->nextPendingConnection());


    if (!socket->isValid()) {
        logger.appendError("ERROR: Could not cast incomming socket connection");
        return;
    }


    // Doing the connections.
    connect(socket,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(socket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(socket,&QSslSocket::readyRead,this,&Control::on_readyRead);
    connect(socket,&QSslSocket::disconnected,this,&Control::on_disconnected);

    // The SSL procedure.
    socket->setPrivateKey(":/certificates/server.key");
    socket->setLocalCertificate(":/certificates/server.csr");
    socket->setPeerVerifyMode(QSslSocket::VerifyNone);
    socket->startServerEncryption();

}


/***************************************************** QTCP SOCKETS SLOTS ****************************************************/
void Control::on_encryptedSuccess(){
    logger.appendStandard("CONNECTED With client. Awaiting for a request");
    rx.clearAll();
    timer.start(50000);
}

void Control::onTimerTimeout(){
    logger.appendError("Timeout waiting for a request from the client");
    timer.stop();
    clearSocket();
}

void Control::on_sslErrors(const QList<QSslError> &errors){

    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    if (socket == nullptr) return;

    QHostAddress addr = socket->peerAddress();
    for (qint32 i = 0; i < errors.size(); i++){
        logger.appendError("SSL Error," + errors.at(i).errorString() + " from Address: " + addr.toString());
    }

    // Eliminating it from the list.
    clearSocket();

}

void Control::on_socketError(QAbstractSocket::SocketError error){
    // Check necessary since multipe signasl are triggered if the other socket dies. Can only
    // be removed once. After that, te program crashes.
    if (socket == nullptr) return;

    QHostAddress addr = socket->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    if (error != QAbstractSocket::RemoteHostClosedError)
        logger.appendError(QString("Socket Error found: ") + metaEnum.valueToKey(error) + QString(" from Address: ") + addr.toString());
    else
        logger.appendStandard(QString("Closed connection from Address: ") + addr.toString());

    // Eliminating it from the list.
    clearSocket();
}

void Control::on_socketStateChanged(QAbstractSocket::SocketState state){
    if (socket == nullptr) return;
    QHostAddress addr = socket->peerAddress();
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    logger.appendStandard(addr.toString() + ": " + QString("New socket state, ") + metaEnum.valueToKey(state));
}

void Control::on_readyRead(){
    QByteArray ba = socket->readAll();
    quint8 ans = rx.bufferByteArray(ba);
    if (ans == DataPacket::DATABUFFER_RESULT_DONE){
        timer.stop();
        if (rx.hasInformationField(DataPacket::DPFI_DB_INST_PASSWORD)){
            // Data query.
            processRequest();
            return;
        }
        else {
            // Institution list query.
            QStringList instuids = getInstitutionUIDPair();
            DataPacket tx;
            tx.addString(instuids.join(DB_LIST_IN_COL_SEP),DataPacket::DPFI_INST_NAMES);
            QByteArray ba = tx.toByteArray();
            qint64 num = socket->write(ba.constData(),ba.size());
            if (num != ba.size()){
                logger.appendError("Failure sending then institution list information");
            }
            clearSocket();
        }
    }
    else if (ans == DataPacket::DATABUFFER_RESULT_ERROR){
        timer.stop();
        logger.appendError("Buffering data gave an error");
    }
}

void Control::on_disconnected(){
    logger.appendStandard("Socket was disconnected");
    clearSocket();
}

/*****************************************************************************************************************************/

bool Control::verifyPassword(const QString &password){
    QString hash =  QString(QCryptographicHash::hash(password.toLatin1(),QCryptographicHash::Sha256).toHex());
    return (hash == config.getString(RAW_DATA_SERVER_PASSWORD));
}

void Control::processRequest(){
    // NOTHING TO DO.
}

QStringList Control::getInstitutionUIDPair(){
    QStringList ans;
    QStringList columns;

    if (!dbConnBase.open()){
        logger.appendError("Getting institutions names and uids, could not open DB CON BASE");
        return ans;
    }

    columns << TINST_COL_UID << TINST_COL_NAME;

    if (!dbConnBase.readFromDB(TABLE_INSTITUTION,columns,"")){
        logger.appendError("Getting institutions names and uids: " + dbConnBase.getError());
        return ans;
    }

    DBData res = dbConnBase.getLastResult();

    for (qint32 i = 0; i < res.rows.size(); i++){

        if (res.rows.at(i).size() != 2){
            logger.appendError("Getting institutions expected two values per row. But got: " + QString::number(res.rows.at(i).size()));
            return QStringList();
        }
        ans << res.rows.at(i).first();
        ans << res.rows.at(i).last();
    }

    dbConnBase.close();

    return ans;

}

void Control::clearSocket(){
    if (socket != nullptr){
        logger.appendStandard("About to delete socket");
        delete socket;
        socket = nullptr;
        logger.appendStandard("Socket deleted!");
    }
}
