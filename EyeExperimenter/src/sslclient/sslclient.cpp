#include "sslclient.h"

SSLClient::SSLClient(QObject *parent, ConfigurationManager *c) :
    QObject(parent)
{

    if (!QSslSocket::supportsSsl()){
        log.appendError("There is no support for SSL. Please request help from ViewMind to solve this issue.");
        return;
    }

    // Creating the socket and making the connections.
    socket = new QSslSocket(this);
    connect(socket,SIGNAL(encrypted()),this,SLOT(on_encryptedSuccess()));
    connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),this,SLOT(on_socketStateChanged(QAbstractSocket::SocketState)));
    connect(socket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_socketError(QAbstractSocket::SocketError)));
    connect(socket,SIGNAL(sslErrors(QList<QSslError>)),this,SLOT(on_sslErrors(QList<QSslError>)));
    connect(socket,SIGNAL(readyRead()),this,SLOT(on_readyRead()));

    // Timer connection
    connect(&timer,&QTimer::timeout,this,&SSLClient::on_timeOut);

    // Setting up the configuation and checking that everything is there.
    config = c;

    if (config == nullptr){
        log.appendError("BAD SSLCLIENT INITIALIZATION");
        return;
    }

    if (!c->containsKeyword(CONFIG_SERVER_ADDRESS)){
        log.appendError("Server address was not set in the configuration file");
        return;
    }

    if (!c->containsKeyword(CONFIG_TCP_PORT)){
        log.appendError("Server port was not set in the configuration file");
        return;
    }

    if (!c->containsKeyword(CONFIG_TCP_PORT_DBCOMM)){
        log.appendError("SQL Server port was not set in the configuration file");
        return;
    }

    clientState = CS_CONNECTING;
}

void SSLClient::sendFinishedSignal(bool okvalue){
    if (!sentTransactionFinishedSignal){
        sentTransactionFinishedSignal = true;
        emit(transactionFinished(okvalue));
    }
}

void SSLClient::requestReport(){
    if (!sslEnabled()){
        log.appendError("Attempted request even though SSL is not enabled");
        sendFinishedSignal(false);
        return;
    }
    sentTransactionFinishedSignal = false;
    connectToServer();
}

void SSLClient::setDBTransactionType(const QString &type){
    queryType = type;
    valuesForColumnsList = "";
    tableNames = "";
    columnList = "";
    conditionsList = "";
}

void SSLClient::appendSET(const QString &tableName, const QStringList &columns, const QStringList &values){

    if (queryType != SQL_QUERY_TYPE_SET) return;

    if (tableNames.isEmpty()){
        tableNames = tableName;
        columnList = columns.join(DB_COLUMN_SEP);
        valuesForColumnsList = values.join(DB_COLUMN_SEP);
    }
    else {
        tableNames = tableNames + DB_TRANSACTION_LIST_SEP + tableName;
        columnList = columnList + DB_TRANSACTION_LIST_SEP + columns.join(DB_COLUMN_SEP);
        valuesForColumnsList = valuesForColumnsList + DB_TRANSACTION_LIST_SEP + values.join(DB_COLUMN_SEP);
    }

}

void SSLClient::appendGET(const QString &tableName, const QStringList &columns, const QString &condition){

    if (queryType != SQL_QUERY_TYPE_GET) return;

    if (tableNames.isEmpty()){
        tableNames = tableName;
        columnList = columns.join(DB_COLUMN_SEP);
        conditionsList = condition;
    }
    else {
        tableNames = tableNames + DB_TRANSACTION_LIST_SEP + tableName;
        columnList = columnList + DB_TRANSACTION_LIST_SEP + columns.join(DB_COLUMN_SEP);
        conditionsList = conditionsList + DB_TRANSACTION_LIST_SEP + condition;
    }

}

void SSLClient::runDBTransaction(){

    txDP.clearAll();
    txDP.addString(queryType,DataPacket::DPFI_DB_QUERY_TYPE);
    txDP.addString(tableNames,DataPacket::DPFI_DB_TABLE);
    txDP.addString(columnList,DataPacket::DPFI_DB_COL);

    if (queryType == SQL_QUERY_TYPE_SET){
        clientState = CS_CONNECTING_TO_SQL_SET;
        txDP.addString(valuesForColumnsList,DataPacket::DPFI_DB_VALUE);
    }
    else{
        clientState = CS_CONNECTING_TO_SQL_GET;
        txDP.addString(conditionsList,DataPacket::DPFI_DB_CONDITIION);
    }

    sentTransactionFinishedSignal = false;

    socket->connectToHostEncrypted(config->getString(CONFIG_SERVER_ADDRESS),(quint16)config->getInt(CONFIG_TCP_PORT_DBCOMM));
    startTimeoutTimer();
}

void SSLClient::connectToServer()
{
    // A configuration is created in order to get the patient information.
    QString directory = config->getString(CONFIG_PATIENT_DIRECTORY);

    if (!QDir(directory).exists()){
        log.appendError("Patient directory does not exist");
        sendFinishedSignal(false);
        return;
    }

    txDP.clearAll();

    bool reading, binding;
    reading = true;
    binding = true;


    QString readingfile;
    if (config->getBool(CONFIG_DEMO_MODE)) readingfile = ":/demo_data/reading_2018_06_03.dat";
    else readingfile = getNewestFile(directory,FILE_OUTPUT_READING);

    if (readingfile.isEmpty()){
        log.appendWarning("WARNING: No reading file found");
        reading = false;
    }
    else{
        txDP.addFile(readingfile,DataPacket::DPFI_READING);
    }

    QString bindingBC;
    if (config->getBool(CONFIG_DEMO_MODE)) bindingBC = ":/demo_data/binding_bc_2018_06_03.dat";
    else bindingBC = getNewestFile(directory,FILE_OUTPUT_BINDING_BC);

    if (bindingBC.isEmpty()){
        log.appendWarning("WARNING: No binding BC file found");
        binding = false;
    }
    else{
        txDP.addFile(bindingBC,DataPacket::DPFI_BINDING_BC);
    }

    QString bindingUC;
    if (config->getBool(CONFIG_DEMO_MODE)) bindingUC = ":/demo_data/binding_uc_2018_06_03.dat";
    else bindingUC = getNewestFile(directory,FILE_OUTPUT_BINDING_UC);

    if (bindingUC.isEmpty()){
        log.appendWarning("WARNING: No binding UC file found");
        binding = false;
    }
    else{
        txDP.addFile(bindingUC,DataPacket::DPFI_BINDING_UC);
    }
    if (!reading && !binding){
        log.appendError("ERROR: No processing is possible due to missing files.");
        sendFinishedSignal(false);
        return;
    }


    // Creating the packet to send to the server.
    txDP.addString(config->getString(CONFIG_DOCTOR_UID),DataPacket::DPFI_DOCTOR_ID);
    txDP.addString(config->getString(CONFIG_PATIENT_UID),DataPacket::DPFI_PATIENT_ID);
    txDP.addValue(config->getInt(CONFIG_VALID_EYE),DataPacket::DPFI_VALID_EYE);

    // Requesting connection and ack
    informationSent = false;
    socket->connectToHostEncrypted(config->getString(CONFIG_SERVER_ADDRESS),(quint16)config->getInt(CONFIG_TCP_PORT));

    // Starting timeout timer.
    clientState = CS_CONNECTING;
    startTimeoutTimer();
}

void SSLClient::on_encryptedSuccess(){
    if (clientState == CS_CONNECTING){
        log.appendSuccess("Established encrypted connection to the server. Waiting for acknowledge ... ");
        rxDP.clearAll();
        timer.stop();
        clientState = CS_WAIT_FOR_ACK;
        startTimeoutTimer();
    }
    else if ((clientState == CS_CONNECTING_TO_SQL_SET) || (clientState == CS_CONNECTING_TO_SQL_GET)){
        log.appendSuccess("Established encrypted connection to the sql server. Sending request to server ... ");
        QByteArray ba = txDP.toByteArray();
        qint64 num = socket->write(ba.constData(),ba.size());
        bool ans;
        if (num != ba.size()){
            log.appendError("ERROR: Unable to send the information to SQL Server. Please try again.");
            ans = false;
        }
        else{
            ans = true;
        }

        // If there was a problem everything is stopped.
        if (!ans){
            clientState = CS_CONNECTING;
            socket->disconnectFromHost();
            return;
        }

        if (clientState == CS_CONNECTING_TO_SQL_SET){
            // A Set does not need to do anything else. A get needs to wait for a response.
            clientState = CS_CONNECTING;
            sentTransactionFinishedSignal = true;
            emit(transactionFinished(ans));
        }

    }
    else {
        log.appendError("Invalid state upon encrypted success closing off the connection");
        socket->disconnectFromHost();
        clientState = CS_CONNECTING;
        sendFinishedSignal(false);
        return;
    }
}

void SSLClient::on_readyRead(){

    if (clientState == CS_CONNECTING) return;

    quint8 errcode = rxDP.bufferByteArray(socket->readAll());

    if (errcode == DataPacket::DATABUFFER_RESULT_DONE){

        if (clientState == CS_CONNECTING_TO_SQL_GET){

            // Got the data form the DB.
            dbdata.clear();

            QStringList tableNames = rxDP.getField(DataPacket::DPFI_DB_TABLE).data.toString().split(DB_TRANSACTION_LIST_SEP);
            QStringList allColumns = rxDP.getField(DataPacket::DPFI_DB_COL).data.toString().split(DB_TRANSACTION_LIST_SEP);
            QStringList allErrors= rxDP.getField(DataPacket::DPFI_DB_ERROR).data.toString().split(DB_TRANSACTION_LIST_SEP,QString::KeepEmptyParts);
            QStringList allValues= rxDP.getField(DataPacket::DPFI_DB_VALUE).data.toString().split(DB_TRANSACTION_LIST_SEP,QString::KeepEmptyParts);

            for (qint32 i = 0; i < tableNames.size(); i++){
                DBInterface::DBData datum;
                datum.error = allErrors.at(i);
                datum.columns = allColumns.at(i).split(DB_COLUMN_SEP);
                datum.fillRowsFromString(allValues.at(i),DB_ROW_SEP,DB_COLUMN_SEP);
                dbdata << datum;
            }

            // The client disconnects, since it allready has all the info.
            socket->disconnectFromHost();
            sentTransactionFinishedSignal = true;
            emit(transactionFinished(true));

        }
        else if (clientState == CS_WAIT_FOR_ACK){

            if (rxDP.hasInformationField(DataPacket::DPFI_SEND_INFORMATION)){
                // Since in the information was requested, it is now sent.
                QByteArray ba = txDP.toByteArray();
                qint64 num = socket->write(ba.constData(),ba.size());
                if (num != ba.size()){
                    log.appendError("ERROR: Unable to send the information. Please try again.");
                    socket->disconnectFromHost();
                    sendFinishedSignal(false);
                    return;
                }
                log.appendStandard("LOG: Sent requested information to server: " + QString::number(num) + " bytes");
                rxDP.clearBufferedData();
                clientState = CS_WAIT_FOR_REPORT;
                timer.stop();
                startTimeoutTimer();
            }
            else{
                // This is not it and it should start again.
                rxDP.clearBufferedData();
                return;
            }

        }
        else{ // We are waiting for a report

            if (!rxDP.isInformationFieldOfType(DataPacket::DPFI_REPORT,DataPacket::DPFT_FILE)){
                rxDP.clearBufferedData();
                return;
            }
            timer.stop();

            // At this point the report was received so it is saved
            QString reportPath = rxDP.saveFile(config->getString(CONFIG_PATIENT_DIRECTORY),DataPacket::DPFI_REPORT);
            if (reportPath.isEmpty()){
                log.appendError("ERROR: Could not save the report to the directory: " + reportPath + ". Please try again");
                sendFinishedSignal(false);
            }
            else{
                log.appendSuccess("Report saved to: " + reportPath);
                config->addKeyValuePair(CONFIG_REPORT_PATH,reportPath);
                sendFinishedSignal(true);
            }

            rxDP.clearAll();
            socket->disconnectFromHost();
        }

    }
    else if (errcode == DataPacket::DATABUFFER_RESULT_ERROR){
        log.appendError("ERROR: Buffering data from the receiver");
        rxDP.clearAll();
        socket->disconnectFromHost();
    }

}


//************************************* Socket and SSL Errors, Socket state changes ****************************************

void SSLClient::on_sslErrors(const QList<QSslError> &errors){
    for (qint32 i = 0; i < errors.size(); i++){
        log.appendWarning("SSL ERROR: " + errors.at(i).errorString());
    }
    socket->ignoreSslErrors();
}

void SSLClient::on_socketStateChanged(QAbstractSocket::SocketState state){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketState>();
    log.appendStandard(QString("Log: Socket state - ") + metaEnum.valueToKey(state));
    if (state == QAbstractSocket::UnconnectedState){
        emit(diconnectionFinished());
    }
}

void SSLClient::on_socketError(QAbstractSocket::SocketError error){
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    log.appendWarning(QString("SOCKET ERROR: ") + metaEnum.valueToKey(error));
    // If there is an error then the timer should be stopped.
    if (timer.isActive()) timer.stop();
    socket->disconnectFromHost();
    sendFinishedSignal(false);
}

//*************************************************************************************************************************

void SSLClient::on_timeOut(){
    switch(clientState){
    case CS_CONNECTING:
        log.appendError("ERROR: Server connection notice did not arrive before the expected time. Closing connection. Please retry.");
        emit(transactionFinished(false));
        break;
    case CS_WAIT_FOR_REPORT:
        log.appendError("ERROR: Server generated report did not arrive before expected time. Closing connection. Please retry.");
        emit(transactionFinished(false));
        break;
    case CS_WAIT_FOR_ACK:
        log.appendError("ERROR: Server request for information did not arrive before expected time. Closing connection. Please retry.");
        emit(transactionFinished(false));
        break;
    case CS_CONNECTING_TO_SQL_GET:
    case CS_CONNECTING_TO_SQL_SET:
        log.appendError("ERROR: SQL server connection notice did not arrive before the expected time. Closing connection. Please retry.");
        emit(transactionFinished(false));
        break;
    }
    socket->disconnectFromHost();
}

void SSLClient::startTimeoutTimer(){
    QString message = "WARNING: ";
    bool showMessage = false;
    qint32 timeout;

    switch(clientState){
    case CS_WAIT_FOR_REPORT:
        timeout = DEFAULT_TIMEOUT_WAIT_ACK;
        if (config->containsKeyword(CONFIG_CONNECTION_TIMEOUT)){
            timeout = config->getInt(CONFIG_CONNECTION_TIMEOUT)*1000;
        }
        else{
            message = message + "Connection timeout was not configured.";
            showMessage = true;
        }
        break;
    case CS_WAIT_FOR_ACK:
        timeout = DEFAULT_TIMEOUT_WAIT_ACK;
        if (config->containsKeyword(CONFIG_DATA_REQUEST_TIMEOUT)){
            timeout = config->getInt(CONFIG_DATA_REQUEST_TIMEOUT)*1000;
        }
        else{
            message = message + "Data request timeout was not configured.";
            showMessage = true;
        }
        break;
    default:
        timeout = DEFAULT_TIMEOUT_CONNECTION;
        if (config->containsKeyword(CONFIG_CONNECTION_TIMEOUT)){
            timeout = config->getInt(CONFIG_CONNECTION_TIMEOUT)*1000;
        }
        else{
            message = message + "Connection timeout was not configured.";
            showMessage = true;
        }
        break;
    }

    if(showMessage){
        log.appendWarning(message + ". Using default timeout of " + QString::number(timeout/1000) + " seconds.");
    }

    // Starting waiting for data timer.
    timer.setInterval(timeout);
    timer.start();
}

SSLClient::~SSLClient()
{

}

