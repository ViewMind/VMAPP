#ifndef SSLIDSOCKET_H
#define SSLIDSOCKET_H

#include <QSslSocket>
#include <QTimer>
#include <QProcess>
#include <QDateTime>
#include "../../CommonClasses/DataPacket/datapacket.h"

#define  S3_BASE_COMMAND     "aws s3 cp"
#define  S3_PARMETERS        "--quiet"
#define  TIME_FORMAT_STRING  "yyyy_MM_dd_hh_mm_ss"

/*****************************************************************
 * The objective of this class is to provide an identifying value
 * to the Sockect. This is required to properley identify the
 * QSslSocket signals that will be all connected to the same slot
 * It also contains convenience function for doing some simple data
 * processing on the received bytes as well as saving files as
 * well as saving the data to S3 storage (as it is basically the same
 * action).
 * ***************************************************************/

class SSLIDSocket: public QObject
{
    Q_OBJECT

public:

    static const quint8 SSL_SIGNAL_ENCRYPTED     = 0;
    static const quint8 SSL_SIGNAL_STATE_CHANGED = 1;
    static const quint8 SSL_SIGNAL_SSL_ERROR     = 2;
    static const quint8 SSL_SIGNAL_ERROR         = 3;
    static const quint8 SSL_SIGNAL_DATA_RX_DONE  = 4;
    static const quint8 SSL_SIGNAL_DISCONNECTED  = 5;
    static const quint8 SSL_SIGNAL_TIMEOUT       = 6;
    static const quint8 SSL_SIGNAL_DATA_RX_ERROR = 7;
    static const quint8 SSL_SIGNAL_PROCESS_DONE  = 8;

    SSLIDSocket();
    SSLIDSocket(QSslSocket *newSocket, quint64 id, const QString &s3);
    ~SSLIDSocket();

    QSslSocket* socket() const {return sslSocket;}

    DataPacket getDataPacket() const {return rx;}

    void startTimeoutTimer(qint32 ms);
    void stopTimer() {timer.stop();}

    // Defines where the files received will be saved. If all is ok, it proceeds to generate them.
    // Returns a error message if there is one.
    QString setWorkingDirectoryAndSaveAllFiles(const QString &baseDir);
    QString getWorkingDirectory() const { return workingDirectory; }
    void processData(const QString &processorPath, const QStringList &args);

    bool isValid() const { return sslSocket != nullptr; }
    quint64 getID() const {return ID;}

signals:
    void sslSignal(quint64 id, quint8 signalID);

private slots:
    void on_encryptedSuccess();
    void on_socketStateChanged(QAbstractSocket::SocketState state);
    void on_socketError(QAbstractSocket::SocketError error);
    void on_sslErrors(const QList<QSslError> &errors);
    void on_readyRead();
    void on_disconnected();
    void on_timeout();
    void on_processFinished(qint32 status);

private:
    QString s3Address;
    QString workingDirectory;
    QSslSocket *sslSocket;
    DataPacket rx;
    QTimer timer;
    QProcess process;
    quint64 ID;
};

#endif // SSLIDSOCKET_H
