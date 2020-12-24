#include "control.h"

Control::Control(QObject *parent):QObject(parent)
{

    // Creating the configuration verifier
    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cv[RAW_DATA_SERVER_PASSWORD] = cmd;

    // Database configuration
    cmd.clear();
    cv[CONFIG_DBPASSWORD] = cmd;
    cv[CONFIG_DBUSER] = cmd;

    cv[CONFIG_PATDATA_DBHOST] = cmd;
    cv[CONFIG_PATDATA_DBNAME] = cmd;

    cv[CONFIG_ID_DBHOST] = cmd;
    cv[CONFIG_ID_DBNAME] = cmd;

    cv[CONFIG_DATA_DBHOST] = cmd;
    cv[CONFIG_DATA_DBNAME] = cmd;

    cv[CONFIG_DASH_DBHOST] = cmd;
    cv[CONFIG_DASH_DBNAME] = cmd;

    cmd.type = ConfigurationManager::VT_INT;
    cv[CONFIG_DATA_DBPORT] = cmd;
    cv[CONFIG_ID_DBPORT] = cmd;
    cv[CONFIG_PATDATA_DBPORT] = cmd;
    cv[CONFIG_DASH_DBPORT] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_BOOL;
    cv[RAW_DATA_SERVER_DELWORKDIR] = cmd;

    cmd.clear();
    cv[CONFIG_ETDIR_PATH] = cmd;

    config.setupVerification(cv);

    // Creating the the listner.
    listener = new SSLListener(this);

    idGen = 0;
}

void Control::startServer(){

    QDir(".").mkdir(DIRNAME_SERVER_LOGS);
    if (!QDir(DIRNAME_SERVER_LOGS).exists()){
        logger.appendError("Could not find or create the log directory");
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    if (!config.loadConfiguration(CONFIG_FILE,COMMON_TEXT_CODEC)){
        QString error = config.getError();
        error = error.replace("<br>","\n");
        logger.appendError("Configuration file errors:\n"+error);
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    if (!QSslSocket::supportsSsl()){
        logger.appendError("No support for SSL found. Cannot continue");
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

    // The database interfaces. will only be used to test the setting. The actual connection will be created in the socket.
    DBInterface dbConnBase;
    DBInterface dbConnID;
    DBInterface dbConnPatData;
    DBInterface dbConnDash;

    // Initializing the database connections.
    QString user = config.getString(CONFIG_DBUSER);
    QString passwd = config.getString(CONFIG_DBPASSWORD);

    QString host = config.getString(CONFIG_DATA_DBHOST);
    QString dbname = config.getString(CONFIG_DATA_DBNAME);
    quint16 port = config.getInt(CONFIG_DATA_DBPORT);
    dbConnBase.setupDB(DB_NAME_BASE,host,dbname,user,passwd,port,"");
    //qWarning() << "Connection information: " + user + "@" + host + " with passwd " + passwd + ", port: " + QString::number(port) + " to db: " + dbname;

    host = config.getString(CONFIG_ID_DBHOST);
    dbname = config.getString(CONFIG_ID_DBNAME);
    port = config.getInt(CONFIG_ID_DBPORT);
    dbConnID.setupDB(DB_NAME_ID,host,dbname,user,passwd,port,"");
    //qWarning() << "Connection information: " + user + "@" + host + " with passwd " + passwd + ", port: " + QString::number(port) + " to db: " + dbname;

    host = config.getString(CONFIG_PATDATA_DBHOST);
    dbname = config.getString(CONFIG_PATDATA_DBNAME);
    port = config.getInt(CONFIG_PATDATA_DBPORT);
    dbConnPatData.setupDB(DB_NAME_PATDATA,host,dbname,user,passwd,port,"");

    host = config.getString(CONFIG_DASH_DBHOST);
    dbname = config.getString(CONFIG_DASH_DBNAME);
    port = config.getInt(CONFIG_DASH_DBPORT);
    dbConnDash.setupDB(DB_NAME_DASHBOARD,host,dbname,user,passwd,port,"");

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

    if (!dbConnPatData.open()){
        logger.appendError("FAIL TEST CONNECTION DB PAT DATA: " + dbConnPatData.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }
    dbConnPatData.close();

    if (!dbConnDash.open()){
        logger.appendError("FAIL TEST CONNECTION DB DASH: " + dbConnDash.getError());
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }
    dbConnDash.close();


    logger.appendStandard("DB Connection test succesfull");

    // Listen for new connections.
    connect(listener,&SSLListener::newConnection,this,&Control::on_newConnection);

    if (!listener->listen(QHostAddress::Any,TCP_PORT_RAW_DATA_SERVER)){
        logger.appendError("ERROR : Could not start SSL Server: " + listener->errorString());
        emit(exitRequested());
        return;
    }

    // Checking the ET directory exists
    if (!QDir(config.getString(CONFIG_ETDIR_PATH)).exists()){
        logger.appendError("ETDIR Directory could not be found at: " + config.getString(CONFIG_ETDIR_PATH));
        std::cout << "ABNORMAL EXIT: Please check the log file" << std::endl;
        emit(exitRequested());
        return;
    }

}

void Control::on_newConnection(){

    // New connection is available.
    QSslSocket *sslsocket = (QSslSocket*)(listener->nextPendingConnection());
    RawDataServerSocket *socket = new RawDataServerSocket(sslsocket,idGen,&config);

    logger.appendStandard("Created new connection with ID: " + QString::number(idGen));

    idGen++;

    if (!socket->isValid()) {
        logger.appendError("ERROR: Could not cast incomming socket connection");
        return;
    }

    // Doing the connections.
    connect(socket,&SSLIDSocket::socketDone,this,&Control::on_removeSocket);
    sockets.addSocket(socket);

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

void Control::on_removeSocket(quint64 socket){

    QString where = "onRemoveSocket";
    RawDataServerSocket *sslsocket = (RawDataServerSocket*)sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(socket,where);
        logger.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        return;
    }

    logger.appendStandard("Removing socket: " + QString::number(sslsocket->getID()));
//    QStringList dbids = sslsocket->getDBInstanceNames();
//    for (qint32 i = 0; i < dbids.size(); i++){
//        QSqlDatabase::removeDatabase(dbids.at(i));
//    }
//    logger.appendStandard("Finished removing all DB connections");

    sockets.releaseSocket(socket,where);
    sockets.deleteSocket(socket,where);

}
