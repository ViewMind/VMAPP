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

    struct DBData{
        QStringList columns;
        QString error;
        QList<QStringList> rows;
        void clear(){
            columns.clear();
            rows.clear();
            error = "";
        }
        QString joinRowsAndCols(const QString &rowSep, const QString &colSep){
            QStringList compactedrows;
            for (qint32 i = 0; i < rows.size(); i++){
                compactedrows << rows.at(i).join(colSep);
            }
            return compactedrows.join(rowSep);
        }
        void fillRowsFromString(const QString &data, const QString &rowSep, const QString &colSep){
            QStringList crows = data.split(rowSep);
            for (qint32 i= 0; i < crows.size(); i++){
                    rows << crows.at(i).split(colSep);
            }
        }
    };

    DBInterface();

    // Initializes the connection to the database returning true if sucessfull.
    bool initDB(const ConfigurationManager &config);

    // Insert Query
    bool insertDB(const QString &table, const QStringList &columns, const QStringList &values);

    // Select Query. Returns the first row only
    bool readFromDB(const QString &table, const QStringList &columns, const QString &conditions);

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
