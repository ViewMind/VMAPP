#ifndef SSLDBCLIENT_H
#define SSLDBCLIENT_H

#include "sslclient.h"

/****************************************************************************************************
 * This class contains manages sets and gets sent to the DB section of the EyeServer.
 * **************************************************************************************************/

class SSLDBClient: public SSLClient
{

    Q_OBJECT

public:
    SSLDBClient(QObject *parent = 0, ConfigurationManager *c = nullptr);
    ~SSLDBClient();

    // When using the client with the SQL Server.
    void setDBTransactionType(const QString & type);

    // Queue SET type transaction. Won't do anything if type is GET.
    void appendSET(const QString &tableName, const QStringList &columns, const QStringList &values);

    // Queue SET type transaction. Won't do anything if type is SET.
    void appendGET(const QString &tableName, const QStringList &columns, const QString &condition);

    // Executes the queued up transaction.
    void runDBTransaction();

    // The result of GET Transactions.
    QList<DBData> getDBData() const { return dbdata; }

private slots:

    // SSL and TCP Related slots
    void on_encryptedSuccess();
    void on_socketError(QAbstractSocket::SocketError error);
    void on_readyRead();

    // Timeout function.
    void on_timeOut();

private:

    // State of the transacition to DB.
    typedef enum {CS_CONNECTING_TO_SQL_GET, CS_CONNECTING_TO_SQL_SET, CS_WAIT_DB_DATA} ClientState;
    ClientState clientState;

    // Data obtained from an SQL request
    QList<DBData> dbdata;

    // For queuing up the query data
    QString queryType;
    QString tableNames;
    QString columnList;
    QString valuesForColumnsList;
    QString conditionsList;

};

#endif // SSLDBCLIENT_H
