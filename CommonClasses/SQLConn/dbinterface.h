#ifndef DBINTERFACE_H
#define DBINTERFACE_H

#include <QHash>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QCryptographicHash>
#include <QDateTime>
#include <QTextStream>

#include "dbdescription.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

class DBInterface
{
public:

    DBInterface();

    // Initializes the connection to thez database returning true if sucessfull.
    void setupDB(const QString &instanceName, const QString &host, const QString &dbname, const QString &user, const QString &passwd, quint16 port, const QString log_file, bool add = true);

    // Insert Query
    bool insertDB(const QString &table, const QStringList &columns, const QStringList &values, const QString logid);

    // Select Query. Returns the first row only
    bool readFromDB(const QString &table, const QStringList &columns, const QString &conditions);

    // Update Query.
    bool updateDB(const QString &table, const QStringList &columns, const QStringList &values, const QString &condition, const QString logid);

    // Delete Query
    bool deleteRowFromDB(const QString &table, const QString &condition, const QString logid);

    // Get last generated keyid from a table;
    qint32 getNewestKeyid(const QString &keyidColName, const QString &table);

    // Execute special query
    bool specialQuery(const QString &query, const QStringList &columns);

    bool hasTransactions() { return dbConnection.driver()->hasFeature(QSqlDriver::Transactions); }

    bool startTransaction() { return dbConnection.transaction(); }
    bool commit() { return dbConnection.commit(); }
    bool doRollBack() { return dbConnection.rollback(); }

    // Closing the connection to the server
    bool open();
    void close() { dbConnection.close(); }

    QString getError() const {return error;}
    DBData getLastResult() const {return lastResult;}

    QString getInstanceName() const { return providedInstanceName; }

private:

    // The connectiont tot he database
    QSqlDatabase dbConnection;

    // The last error produces
    QString error;

    // The last result (a single row).
    DBData lastResult;

    // Making sure init is only called once.
    bool dbSetupDone;

    // The provided instance name.
    QString providedInstanceName;

    // File that will log each modification in the data base.
    QString logFile;
    QString dblogid;
    void log(const QString &query, const QString &logid);

};

#endif // DBINTERFACE_H
