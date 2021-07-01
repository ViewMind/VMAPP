#ifndef UPDATESOCKET_H
#define UPDATESOCKET_H

#include "../../../CommonClasses/server_defines.h"
#include "../../../CommonClasses/SSLIDSocketMap/sslidsocket.h"

#define  CONNECTION_TIMEOUT  10*1000  //10 seconds

#define  DIRNAME_UPDATE_DIR                "etdir"
#define  DIRNAME_UPDATE_DIR_LOG_SUBIDR     "logs"
#define  DIRNAME_UPDATE_DIR_FLOGS_SUBIDR   "flogs"
#define  DIRNAME_LOCAL_DB_BKPS             "dbbkps"
#define  FILENAME_EYE_LAUNCHER             "etdir/launcher.exe"
#define  FILENAME_CONFIGURATION            "configuration"
#define  FILENAME_CHANGELOG                "changelog"
#define  FILENAME_EYE_EXPERIMENTER         "EyeExperimenter.exe"

class UpdateSocket : public SSLIDSocket
{
public:
    UpdateSocket();
    UpdateSocket(QSslSocket *newSocket, quint64 id);
    ~UpdateSocket();

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
    bool finishedUpdatingProcess;

    void processUpdateRequest();
    void sendUpdateAns(DataPacket tx, const QString &ans);

};

#endif // UPDATESOCKET_H
