#ifndef RAWDATASERVERSOCKET_H
#define RAWDATASERVERSOCKET_H

#include "../../../CommonClasses/server_defines.h"
#include "../../../CommonClasses/SSLIDSocketMap/sslidsocket.h"
#include "../../../CommonClasses/SQLConn/dbdescription.h"
#include "../../../CommonClasses/SQLConn/dbinterface.h"
#include "../../../CommonClasses/LocalInformationManager/localinformationmanager.h"
#include "s3interface.h"
#include "filelister.h"

#define CONNECTION_TIMEOUT          5000 // in ms.
#define CONNECTION_WAIT_FOR_REQUEST 20000 // in ms
#define VIEWMIND_DATA_REPO          "viewmind-raw-eye-data"
#define RAW_DATA_SERVER_PASSWORD    "hashpass"
#define RAW_DATA_SERVER_DELWORKDIR  "delworkdir"
#define FILE_RAW_DATA_SERVER_CONF   "conf"


class RawDataServerSocket: public SSLIDSocket
{
public:

    RawDataServerSocket();
    RawDataServerSocket(QSslSocket *newSocket, quint64 id, ConfigurationManager *c);
    ~RawDataServerSocket();
    QStringList getDBInstanceNames() const {return dbInstanceNames;}

private slots:
    void on_encryptedSuccess();
    void on_socketStateChanged(QAbstractSocket::SocketState state);
    void on_socketError(QAbstractSocket::SocketError error);
    void on_sslErrors(const QList<QSslError> &errors);
    void on_readyRead();
    void on_disconnected();
    void on_timeout();
    void doDisconnects();

private:

    // The database interfaces
    DBInterface dbConnBase;
    DBInterface dbConnID;
    DBInterface dbConnPatData;
    DBInterface dbConnDash;

    QString verifcationPassword;
    ConfigurationManager *config;
    QStringList dbInstanceNames;

    bool finishedTask;

    void oprLocalDBBkp();
    void oprVMIDTableRequest();
    void oprProcessRequest();
    void oprInstList();
    void oprMedicalRecords();


    void sendErrorMessage(const QString &errMsg);

    bool verifyPassword(const QString &password);

};

#endif // RAWDATASERVERSOCKET_H
