#ifndef DBQUERIES_H
#define DBQUERIES_H

#include <QCryptographicHash>
#include <QtGlobal>
#include <QTime>
#include <QStringList>
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/SQLConn/dbinterface.h"
#include "../../CommonClasses/server_defines.h"
#include "../../CommonClasses/common.h"

#define CONFIG_FILE     "dbdata"

class DBQueries
{
public:
    DBQueries();

    typedef QHash<QString,QString> StringMap;

    typedef enum {BSMT_INSTITUTION, BSMT_PLACED_PRODUCT} BaseStringMapType;

    // Connection initialization.
    bool initConnection();

    // Queries sinthezised as functions.
    qint32 addNewInstitution(StringMap inst);
    bool updateNewInstitution(StringMap inst);
    bool resetPassword(const QString &keyidInst);
    bool deleteTestUsers();
    qint32 addNewProduct(StringMap product);
    bool updateProduct(StringMap product);
    QList<StringMap> getAllInstitutions(bool *isOk);
    QList<StringMap> getAllProductsForInstitutions(const QString &instUID, bool *isOk);
    QStringList searchForPlacedProducts(const QString &search, bool *isOk);
    StringMap getInstitutionInfo(const QString &keyidPP, bool *isOk);
    StringMap getProductInformation(const QString &keyidProd, bool *isOk);

    // Close connection.
    void close() {dbBase.close(); dbPatData.close(); dbPatID.close();}

    // Getting the generated data
    QString getGeneratedPassword() const {return generatedPassword;}
    qint32 getGeeneratedUID() const {return generatedUID;}
    QString getConnectionMsg() const {return connectionMsg;}
    QString getError() const {return error;}

    // Creating empty maps.
    StringMap getEmptyStringMap(BaseStringMapType bsmt);

private:

    static const qint32 MAX_UID_SIZE_IN_BITS = 30;
    static const qint32 PASSWORD_SIZE = 8;

    // The Database objects
    DBInterface dbBase;
    DBInterface dbPatID;
    DBInterface dbPatData;

    // Getting the last error.
    QString error;

    // Comunication variables;
    QString generatedPassword;
    qint32 generatedUID;
    QString connectionMsg;

    // Auxiliary function
    qint32 generateUID(const QSet<qint32> & existing);
    QString generatePassword();

    // Character pool for random number generation
    QList<QStringList> characterPool;

    // Helper function required each time a string map needs to be saved into the DB
    void stringMapToColumnValuePair(const StringMap &map, QStringList *cols, QStringList *vals);

};

#endif // DBQUERIES_H
