#include "ssldbclient.h"

SSLDBClient::SSLDBClient(QObject *parent, ConfigurationManager *c):SSLClient(parent,c)
{
    // The program will not load without time configuration so the proper configuration check is unnecessary.
}

void SSLDBClient::setDBTransactionType(const QString &type){
    queryType = type;
    valuesForColumnsList = "";
    tableNames = "";
    columnList = "";
    conditionsList = "";
}

void SSLDBClient::appendSET(const QString &tableName, const QStringList &columns, const QStringList &values){

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

void SSLDBClient::appendGET(const QString &tableName, const QStringList &columns, const QString &condition){

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

void SSLDBClient::runDBTransaction(){

    txDP.clearAll();
    txDP.addString(queryType,DataPacket::DPFI_DB_QUERY_TYPE);
    txDP.addString(tableNames,DataPacket::DPFI_DB_TABLE);
    txDP.addString(columnList,DataPacket::DPFI_DB_COL);

    dbdata.clear();
    transactionIsOk = false;
    errorCode = DBACK_ALL_OK;

    if (queryType == SQL_QUERY_TYPE_SET){
        clientState = CS_CONNECTING_TO_SQL_SET;
        txDP.addString(valuesForColumnsList,DataPacket::DPFI_DB_VALUE);
        //qWarning() << "Sending a Set" << tableNames << columnList << valuesForColumnsList;
    }
    else{
        clientState = CS_CONNECTING_TO_SQL_GET;
        txDP.addString(conditionsList,DataPacket::DPFI_DB_CONDITIION);
    }

    socket->connectToHostEncrypted(config->getString(CONFIG_SERVER_ADDRESS),TCP_PORT_DB_COMM);
    startTimeoutTimer(config->getInt(CONFIG_CONNECTION_TIMEOUT)*1000);
}

void SSLDBClient::on_encryptedSuccess(){
    QByteArray ba = txDP.toByteArray();
    qint64 num = socket->write(ba.constData(),ba.size());
    bool ans;
    if (num != ba.size()){
        log.appendError("ERROR: Unable to send the information to SQL Server. Write failed");
        ans = false;
    }
    else{
        ans = true;
    }

    // If there was a problem everything is stopped.
    if (!ans){
        socket->disconnectFromHost();
        return;
    }

    if (clientState == CS_CONNECTING_TO_SQL_SET){
        // Awaiting ACK from server
        clientState = CS_WAIT_SET_ACK;
    }
    else{
        // Awaiting data from server
        clientState = CS_WAIT_DB_DATA;
    }
    rxDP.clearAll();
    startTimeoutTimer(config->getInt(CONFIG_WAIT_DBDATA_TIMEOUT)*1000);
}

void SSLDBClient::on_readyRead(){

    quint8 errcode = rxDP.bufferByteArray(socket->readAll());

    if (errcode == DataPacket::DATABUFFER_RESULT_DONE){

        dbdata.clear();
        timer.stop();

        if (clientState == CS_WAIT_DB_DATA){

            // Got the data form the DB.

            QStringList tableNames = rxDP.getField(DataPacket::DPFI_DB_TABLE).data.toString().split(DB_TRANSACTION_LIST_SEP);
            QStringList allColumns = rxDP.getField(DataPacket::DPFI_DB_COL).data.toString().split(DB_TRANSACTION_LIST_SEP);
            QStringList allErrors= rxDP.getField(DataPacket::DPFI_DB_ERROR).data.toString().split(DB_TRANSACTION_LIST_SEP,QString::KeepEmptyParts);
            QStringList allValues= rxDP.getField(DataPacket::DPFI_DB_VALUE).data.toString().split(DB_TRANSACTION_LIST_SEP,QString::KeepEmptyParts);

            for (qint32 i = 0; i < tableNames.size(); i++){
                DBData datum;
                datum.error = allErrors.at(i);
                datum.columns = allColumns.at(i).split(DB_COLUMN_SEP);
                datum.fillRowsFromString(allValues.at(i),DB_ROW_SEP,DB_COLUMN_SEP);
                dbdata << datum;
            }

            // The client disconnects, since it allready has all the info.
            transactionIsOk = true;
            socket->disconnectFromHost();
        }
        else {
            // Expecting ACK from a set operation or error.

            if (rxDP.hasInformationField(DataPacket::DPFI_DB_SET_ACK)){
                errorCode = rxDP.getField(DataPacket::DPFI_DB_SET_ACK).data.toInt();
                if (errorCode != DBACK_ALL_OK){
                    if (rxDP.hasInformationField(DataPacket::DPFI_DB_ERROR)){
                        // The errors must be separated:
                        QStringList dberrors = rxDP.getField(DataPacket::DPFI_DB_ERROR).data.toString().split(DB_TRANSACTION_LIST_SEP);
                        for (qint32 i = 0; i < dberrors.size(); i++){
                            log.appendError("On DB Synch: " + dberrors.at(i));
                        }
                    }
                }
                else{
                    transactionIsOk = true;
                }
                socket->disconnectFromHost();
            }
            else {
                // Otherwise ACK has arrived and is all good.
                log.appendError("Expecting DB Set ACK but somehting else arrived.");
                socket->disconnectFromHost();
            }
        }

    }
    else if (errcode == DataPacket::DATABUFFER_RESULT_ERROR){
        log.appendError("Buffering data from the receiver");
        rxDP.clearAll();
        socket->disconnectFromHost();
    }

}


//************************************* Socket and SSL Errors, Socket state changes ****************************************

void SSLDBClient::on_socketError(QAbstractSocket::SocketError error){
    SSLClient::on_socketError(error);
    QMetaEnum metaEnum = QMetaEnum::fromType<QAbstractSocket::SocketError>();
    if (error != QAbstractSocket::RemoteHostClosedError) log.appendError("Disconnecting on socket error: " + QString(metaEnum.valueToKey(error)) + " on DB connection");
    if (socket->state() == QAbstractSocket::ConnectedState) socket->disconnectFromHost();
    timer.stop();
}

//*************************************************************************************************************************

void SSLDBClient::on_timeOut(){
    timer.stop();
    switch(clientState){
    case CS_WAIT_DB_DATA:
        log.appendError("SQL Server request for information did not arrive before expected time. Closing connection. Please retry.");
        break;
    case CS_WAIT_SET_ACK:
        log.appendError("SQL Server request for information did not arrive before expected time. Closing connection. Please retry.");
        break;
    case CS_CONNECTING_TO_SQL_GET:
    case CS_CONNECTING_TO_SQL_SET:
        log.appendError("SQL server connection notice did not arrive before the expected time. Closing connection. Please retry.");
        break;
    }
    socket->disconnectFromHost();
}

SSLDBClient::~SSLDBClient()
{

}


