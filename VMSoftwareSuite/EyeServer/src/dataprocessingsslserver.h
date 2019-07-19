#ifndef SSLMANAGER_H
#define SSLMANAGER_H

#include <QMetaEnum>
#include <QHash>
#include <QtMath>

#include "../../CommonClasses/SQLConn/dbinterface.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/SSLIDSocketMap/sslidsocketmap.h"
#include "../../CommonClasses/server_defines.h"
#include "ssllistener.h"

class DataProcessingSSLServer: public QObject
{
    Q_OBJECT
public:
    DataProcessingSSLServer(QObject *parent = Q_NULLPTR);

    // Start the server
    void startServer(ConfigurationManager *c);
    void setDBConnections(DBInterface *base, DBInterface *id, DBInterface *patdata){ dbConnBase = base; dbConnID = id; dbConnPatData = patdata; }

private slots:
    void on_newConnection();
    void on_lostConnection();
    void on_newSSLSignal(quint64 socket, quint8 signaltype);

private:

    struct EyeRepGenGeneratedFiles{
        QString repFile;
        QStringList fdbFiles;
        QString graphFile;
        QString mailFile;
        QString dbfFile;
        bool allOk;
    };

    // Logging to file.
    LogInterface log;

    // The configuration
    ConfigurationManager *config;

    // Listens for incoming connections and enables de SSL capabilities by using a QSSLSocket
    SSLListener *listener;

    // Handle to do DB queries on the different databases
    DBInterface *dbConnBase;
    DBInterface *dbConnID;
    DBInterface *dbConnPatData;

    // The queue of information that needs to be processed
    //QHash<quint64,SSLIDSocket*> sockets;
    SSLIDSocketMap sockets;
    QList<quint64> queue;

    // Keep count of the number of parallel proceses
    QSet<quint64> socketsBeingProcessed;

    // Generates ever increasing values for unique socket ids.
    quint64 idGen;

    // Helper fucntions to generate messages.
    void changedState(quint64 id);
    void socketErrorFound(quint64 id);
    void sslErrorsFound(quint64 id);

    // Remove form the map and the list
    void removeSocket(quint64 id, const QString &where);

    // Launch processor
    void lauchEyeReportProcessor(quint64 socket);

    // If processes slots are available, requests information from the first socket in the queue.
    void requestProcessInformation();

    // Final Step in the process, sending the report.
    void sendReport(quint64 socket);

    //void removeAllRepFiles(const QString &directory);
    EyeRepGenGeneratedFiles getNonStandardFileNamesFromEyeRepGen(const QString &directory);
    void sendFreqErrorEmail(qint32 instUID, const QString &emailBodyFileName, const QString &graphFile);

    // Check if the report can be requested.
    quint8 verifyReportRequest(qint32 UID, const QString &etserial);
    void decreaseReportCount(qint32 UID, const QString &logid);
    bool initAllDBS();
    QString getPatientID(const QString &pat_uid);

};

#endif // SSLMANAGER_H
