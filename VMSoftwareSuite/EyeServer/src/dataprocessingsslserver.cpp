#include "dataprocessingsslserver.h"

DataProcessingSSLServer::DataProcessingSSLServer(QObject *parent):QObject(parent)
{
    // Creating the the listner.
    listener = new SSLListener(this);

    // Listen for new connections.
    connect(listener,&SSLListener::newConnection,this,&DataProcessingSSLServer::on_newConnection);

    // Start generator from zero.
    idGen = 0;
}

bool DataProcessingSSLServer::startServer(ConfigurationManager *c){

    config = c;

    // Checking that the output repo exists.
    QDir d(DIRNAME_SERVER_WORKDIR);
    if (!d.exists()){
        QDir pwd(".");
        pwd.mkdir(DIRNAME_SERVER_WORKDIR);
        if (!d.exists()){
            log.appendError("Work directory does not exist and could not be created");
            return false;
        }
    }

    // Checking that the log repo exists.
    QDir l(DIRNAME_SERVER_LOGS);
    if (!l.exists()){
        QDir pwd(".");
        pwd.mkdir(DIRNAME_SERVER_LOGS);
        if (!l.exists()){
            log.appendError("Log directory does not exist and could not be created");
            return false;
        }
    }


    if (!c->containsKeyword(CONFIG_DATA_REQUEST_TIMEOUT)){
        log.appendError("Data timeout not present");
        return false;
    }
    else log.appendStandard("Data request is configured as " + config->getString(CONFIG_DATA_REQUEST_TIMEOUT));

#ifdef SERVER_LOCALHOST
    c->addKeyValuePair(CONFIG_EYEPROCESSOR_PATH,"/home/ariela/repos/viewmind_projects/VMSoftwareSuite/EyeReportGenerator/bin/EyeReportGen");
    c->addKeyValuePair(CONFIG_EYEDBMANAGER_PATH,"/home/ariela/repos/viewmind_projects/VMSoftwareSuite/EyeDBmanager/bin/EyeDBmanager");
#else
    c->addKeyValuePair(CONFIG_EYEPROCESSOR_PATH,"/home/ec2-user/EyeReportGenerator/bin/EyeReportGen");
    c->addKeyValuePair(CONFIG_EYEDBMANAGER_PATH,"/home/ec2-user/EyeDBmanager/EyeDBmanager");
#endif

    // Checking that the processor and the db manager exists.
    QFile eyeproc(c->getString(CONFIG_EYEPROCESSOR_PATH));
    if (!eyeproc.exists()){
        log.appendError("EyeReportGenerator could not be found at: " + c->getString(CONFIG_EYEPROCESSOR_PATH));
        return false;
    }

    QFile eyedbmng(c->getString(CONFIG_EYEDBMANAGER_PATH));
    if (!eyedbmng.exists()){
        log.appendError("EyeDBMManager could not be found at: " + c->getString(CONFIG_EYEDBMANAGER_PATH));
        return false;
    }

    if (!listener->listen(QHostAddress::Any,TCP_PORT_DATA_PROCESSING)){
        log.appendError("Could not start SSL Server: " + listener->errorString());
        return false;
    }

    return true;

}

void DataProcessingSSLServer::on_newConnection(){


    ProcessingSocket::ProcessingSocketData data;
    data.eyeDBMngPath = config->getString(CONFIG_EYEDBMANAGER_PATH);
    data.eyeRepGenPath = config->getString(CONFIG_EYEPROCESSOR_PATH);
    data.ID = idGen;
    data.timeOutInMs = config->getInt(CONFIG_DATA_REQUEST_TIMEOUT)*1000;

    // New connection is available.
    QSslSocket *sslsocket = (QSslSocket*)(listener->nextPendingConnection());
    ProcessingSocket *socket = new ProcessingSocket(sslsocket,data);


    if (!socket->isValid()) {
        log.appendError("Could not cast incomming socket connection");
        return;
    }

    idGen++;

    // Saving the socket.
    sockets.addSocket(socket);

    // Doing the connection SIGNAL-SLOT
    bool ans = connect(socket,&SSLIDSocket::socketDone,this,&DataProcessingSSLServer::on_removeSocket);

    log.appendStandard("Creating processing socket: " + socket->getTimeStampID());

    // The SSL procedure.
    sslsocket->setPrivateKey(":/certificates/server.key");
    sslsocket->setLocalCertificate(":/certificates/server.csr");
    sslsocket->setPeerVerifyMode(QSslSocket::VerifyNone);
    sslsocket->startServerEncryption();

}

void DataProcessingSSLServer::on_removeSocket(quint64 socket){

    QString where = "onRemoveSocket";
    ProcessingSocket *sslsocket = (ProcessingSocket*)sockets.getSocketLock(socket,where);
    if (sslsocket == nullptr){
        sockets.releaseSocket(socket,where);
        log.appendError("Attempting to get null socket on " + where + " for id : " + QString::number(socket));
        return;
    }

    log.appendStandard("Removing socket: " + sslsocket->getTimeStampID());

    sockets.releaseSocket(socket,where);
    sockets.deleteSocket(socket,where);

}

