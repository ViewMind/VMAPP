#include "dbinterface.h"

DBInterface::DBInterface()
{
    dbSetupDone = false;
}

void DBInterface::setupDB(const QString &instanceName, const QString &host, const QString &dbname, const QString &user, const QString &passwd, quint16 port, const QString log_file, bool add){

    if (dbSetupDone) return;

    if (add) dbConnection = QSqlDatabase::addDatabase("QMYSQL",instanceName);
    else dbConnection = QSqlDatabase::database(instanceName);
    dbConnection.setHostName(host);
    dbConnection.setDatabaseName(dbname);
    dbConnection.setUserName(user);
    dbConnection.setPassword(passwd);
    //qWarning() << "Setting up DB Connection" << instanceName << "with " << dbConnection.userName() << dbConnection.password();
    if (port != 0){
        dbConnection.setPort(port);
    }

    providedInstanceName = instanceName;

    dbSetupDone = true;
    logFile = log_file;
    dblogid = " [" + dbname + "@" + host + "] ";
}

bool DBInterface::open(){
    //qWarning() << "Opening DB Connection with " << dbConnection.userName() << dbConnection.password() << dbConnection.databaseName() << dbConnection.port() << dbConnection.hostName();
    if (!dbConnection.open()){        
        error = dbConnection.lastError().text();
        return false;
    }
    //qWarning() << "ERROR:" << dbConnection.lastError().text();
    return true;
}


bool DBInterface::insertDB(const QString &table, const QStringList &columns, const QStringList &values, const QString logid, bool dontQuote){


    QString query = "INSERT INTO " + table + "(";

    QStringList quotedVals;
    for (qint32 i = 0; i < values.size(); i++){
        if (!values.at(i).contains('(') && !dontQuote){
            quotedVals << "'" + values.at(i) + "'";
        }
        else{
            quotedVals << values.at(i);
        }
    }

    query = query  + columns.join(",") + ") VALUES (" + quotedVals.join(",") + ")";

    QSqlQuery q(dbConnection);

    //qWarning() << "INSERT QUERY" << query;

    if (!q.exec(query)){
        error = "INSERT Error on query: " + query + ". ERROR: " + q.lastError().text();
        return false;
    }
    log(query,logid);

    return true;

}

bool DBInterface::updateDB(const QString &table, const QStringList &columns, const QStringList &values, const QString &condition, const QString logid){
   QString query = "UPDATE ";
   if (columns.isEmpty()){
       error = "SELECT Error. Columns cannot be an empty list";
       return false;
   }

   query = query + table + " SET ";
   QStringList tojoin;
   for (qint32 i = 0; i < columns.size(); i++){
       tojoin << columns.at(i) + " = '" + values.at(i) + "'";
   }
   query = query + tojoin.join(",");
   if (condition.isEmpty()){
       error = "UPDATE Error: Cannto update withouth a condition";
       return false;
   }
   query = query + " WHERE " + condition;

   QSqlQuery q(dbConnection);
   if (!q.exec(query)){
       error = "UPDATE Error on query: " + query + ". ERROR: " + q.lastError().text();
       return false;
   }
   log(query,logid);

   return true;

}

bool DBInterface::deleteRowFromDB(const QString &table, const QString &condition, const QString logid){
    QString query = "DELETE FROM " + table + " WHERE " + condition;
    QSqlQuery q(dbConnection);
    if (!q.exec(query)){
        error = "DELETE Error on query: " + query + ". ERROR: " + q.lastError().text();
        return false;
    }
    log(query,logid);
    return true;
}

qint32 DBInterface::getNewestKeyid(const QString &keyidColName, const QString &table){
    QString query = "SELECT MAX(" + keyidColName + ") FROM " + table;
    QSqlQuery q(dbConnection);
    if (!q.exec(query)){
        error = "SELECT MAX, Error on query: " + query + ". ERROR: " + q.lastError().text();
        return -1;
    }

    if (!q.next()){
        error = "SELECT MAX Did not rerturn any results";
        return -1;
    }
    return q.value(0).toInt();

}

bool DBInterface::readFromDB(const QString &table, const QStringList &columns, const QString &conditions){

    QString query = "SELECT ";
    if (columns.isEmpty()){
        error = "SELECT Error. Columns cannot be an empty list";
        return false;
    }
    query = query + columns.join(",");
    query = query + " FROM " + table;
    if (conditions != ""){
        query = query + " WHERE " + conditions;
    }

    QSqlQuery q(dbConnection);
    //qWarning() << "EXECUTING READ QUERY: " << query;
    if (!q.exec(query)){
        error = "SELECT Error on query: " + query + ". ERROR: " + q.lastError().text();
        return false;
    }

    lastResult.clear();

    lastResult.columns = columns;

    while (q.next()){
        QStringList row;
        for (qint32 i = 0; i < columns.size(); i++){
            row << q.value(i).toString();
        }
        lastResult.rows << row;
    }

    return true;
}

bool DBInterface::specialQuery(const QString &query, const QStringList &columns){

    QSqlQuery q(dbConnection);
    //qWarning() << "EXECUTING READ QUERY: " << query;
    if (!q.exec(query)){
        error = "SPECIAL QUERY: " + query + ". ERROR: " + q.lastError().text();
        return false;
    }

    lastResult.clear();

    lastResult.columns = columns;

    while (q.next()){
        QStringList row;
        for (qint32 i = 0; i < columns.size(); i++){
            row << q.value(i).toString();
        }
        lastResult.rows << row;
    }

    return true;


}

void DBInterface::log(const QString &query, const QString &logid){


    if (logFile.isEmpty()) return;

    QString timestamp = "[" + QDateTime::currentDateTime().toString("yyyy-MM-dd hh::mm::ss") + "] ";
    QString logline = timestamp +  " [" + logid + "] "  +  dblogid + "-> "  + query + "\n";

    QFile file(logFile);
    if (!file.open(QFile::Append)) return;
    QTextStream writer(&file);
    writer << logline;
    file.close();
}
