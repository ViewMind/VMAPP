#ifndef DBINTERFACE_H
#define DBINTERFACE_H

#include <QHash>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include "dbdescription.h"
#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

class DBInterface
{
public:

    DBInterface();

    // Initializes the connection to the database returning true if sucessfull.
    bool initDB(ConfigurationManager *config);

    // Insert Query
    bool insertDB(const QString &table, const QStringList &columns, const QStringList &values);

    // Select Query. Returns the first row only
    bool readFromDB(const QString &table, const QStringList &columns, const QString &conditions);

    // Update Query.
    bool updateDB(const QString &table, const QStringList &columns, const QStringList &values, const QString &condition);

    // Delete Query
    bool deleteRowFromDB(const QString &table, const QString &condition);

    // Closing the connection to the server
    void close() { dbConnection.close(); }

    QString getError() const {return error;}
    DBData getLastResult() const {return lastResult;}

private:

    // The connectiont tot he database
    QSqlDatabase dbConnection;

    // The last error produces
    QString error;

    // The last result (a single row).
    DBData lastResult;

};

#endif // DBINTERFACE_H
