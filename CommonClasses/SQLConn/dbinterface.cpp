#include "dbinterface.h"

DBInterface::DBInterface()
{
    dbConnection = QSqlDatabase::addDatabase("QMYSQL");
}

bool DBInterface::initDB(ConfigurationManager *config){

    dbConnection.setHostName(config->getString(CONFIG_DBHOST));
    dbConnection.setDatabaseName(config->getString(CONFIG_DBNAME));
    dbConnection.setUserName(config->getString(CONFIG_DBUSER));
    dbConnection.setPassword(config->getString(CONFIG_DBPASSWORD));
    if (config->containsKeyword(CONFIG_DBPORT)){
        dbConnection.setPort(config->getInt(CONFIG_DBPORT));
    }

    if (!dbConnection.open()){
        error = dbConnection.lastError().text();
        return false;
    }

    return true;
}


bool DBInterface::insertDB(const QString &table, const QStringList &columns, const QStringList &values){


    QString query = "INSERT INTO " + table + "(";

    QStringList quotedVals;
    for (qint32 i = 0; i < values.size(); i++){
        if (!values.at(i).contains('(')){
            quotedVals << "'" + values.at(i) + "'";
        }
        else{
            quotedVals << values.at(i);
        }
    }

    query = query  + columns.join(",") + ") VALUES (" + quotedVals.join(",") + ")";

    QSqlQuery q;
    if (!q.exec(query)){
        error = "INSERT Error on query: " + query + ". ERROR: " + q.lastError().text();
        return false;
    }

    return true;

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

    QSqlQuery q;
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
