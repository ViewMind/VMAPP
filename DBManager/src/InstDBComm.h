#ifndef INSTDBCOMM_H
#define INSTDBCOMM_H

#include <QCryptographicHash>
#include <QtGlobal>
#include <QTime>
#include <QStringList>
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/SQLConn/dbinterface.h"

#define CONFIG_FILE     "dbdata"

class InstDBComm
{
public:
    InstDBComm();

    struct Institution {

        Institution(){
            name = "";
            keyid = "";
            uid = "";
            etserial = "";
            etbrand = "";
            etmodel = "";
            numEvals = "";
            password = "";
            ok = true;
        }

        QString name;
        QString keyid;
        QString uid;
        QString etserial;
        QString etbrand;
        QString etmodel;
        QString numEvals;
        QString password;
        bool ok;
    };

    struct Users {
        QString name;
        QString lastName;
        QString keyid;
        QString uid;
        bool ok;
    };


    // Connection initialization.
    bool initConnection();

    // Queries sinthezised as functions.
    qint32 addNewInstitution(const Institution &inst);
    bool updateNewInstitution(const Institution &inst);
    bool resetPassword(const QString &keyidInst);
    bool deleteUserInfo(const QString &uid);
    QStringList getPossibleTestUsers(bool *isOk);
    QList<Institution> getAllInstitutions(bool *isOk);
    Institution getInstitutionInfo(const QString &keyidInst);

    // Close connection.
    void close() {db.close();}

    // Getting the generated data
    QString getGeneratedPassword() const {return generatedPassword;}
    qint32 getGeeneratedUID() const {return generatedUID;}
    QString getConnectionMsg() const {return connectionMsg;}

    QString getError() const {return error;}

private:

    static const qint32 MAX_UID_SIZE_IN_BITS = 30;
    static const qint32 PASSWORD_SIZE = 8;

    // The Database object
    DBInterface db;

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

};

#endif // INSTDBCOMM_H
