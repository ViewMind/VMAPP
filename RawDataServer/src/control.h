#ifndef CONTROL_H
#define CONTROL_H

#include <QSslSocket>
#include <QObject>
#include <QTimer>
#include <QMetaEnum>
#include <iostream>

#define VIEWMIND_DATA_REPO          "viewmind-raw-eye-data"
#define RAW_DATA_SERVER_PASSWORD    "hashpass"
#define RAW_DATA_SERVER_DELWORKDIR  "delworkdir"
#define FILE_FAW_DATA_SERVER_CONF   "conf"

#include "s3interface.h"
#include "../../CommonClasses/DataPacket/datapacket.h"
#include "../../CommonClasses/server_defines.h"
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/SQLConn/dbinterface.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/LocalInformationManager/localinformationmanager.h"
#include "../../CommonClasses/server_defines.h"
#include "ssllistener.h"
#include "filelister.h"

#ifdef SERVER_LOCALHOST
   #define  ETDIR_PATH              "../../EyeServer/bin/etdir"
#else
   #define  ETDIR_PATH              "../EyeServer/etdir/"
#endif


class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);
    void startServer();

private slots:
    void on_newConnection();
    void on_encryptedSuccess();
    void on_socketStateChanged(QAbstractSocket::SocketState state);
    void on_socketError(QAbstractSocket::SocketError error);
    void on_sslErrors(const QList<QSslError> &errors);
    void on_readyRead();
    void on_disconnected();

    void onTimerTimeout();

signals:
    void exitRequested();

private:

    ConfigurationManager config;
    LogInterface logger;

    // The database interfaces
    DBInterface dbConnBase;
    DBInterface dbConnID;

    // Listening from new connections.
    SSLListener *listener;
    //QSslSocket *socket;
    QList<QSslSocket*> socketList;
    QTimer timer;
    DataPacket rx;

    QStringList getInstitutionUIDPair();
    bool verifyPassword(const QString &password);
    void processRequest();
    void clearSocket(const QString &fromWhere);

};

#endif // CONTROL_H
