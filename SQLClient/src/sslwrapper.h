#ifndef SSLWRAPPER_H
#define SSLWRAPPER_H

#include <QObject>
#include <QTime>
#include "../../EyeExperimenter/src/sslclient/ssldbclient.h"
#include "../../EyeExperimenter/src/sslclient/ssldataprocessingclient.h"

class SSLWrapper : public QObject
{
    Q_OBJECT
public:
    explicit SSLWrapper(QObject *parent = nullptr);
    void sendSomeRandomPatientData(qint32 n, bool sendOpt = false);
    void addSomeRandomDoctorData(qint32 n);
    void requestPatientData(const QStringList &uid);
    void sendDataToProcess();

private slots:
    void onTransactionFinished(bool isOk);
    void onTransactionFinishedForDataProcessing(bool isOk);
    void onDisconnectionDone();

private:

    // Bare minimum configuration to test the SQL communication parts fo the SSL Client
    ConfigurationManager config;

    // Created doctor DNI
    QStringList createdDRDNI;

    // SSL Clients
    SSLDBClient *clientDB;
    SSLDataProcessingClient *clientDataProcessing;

    // Some very simple random generating functions
    bool enableOpt;

    // Transaction type
    bool isGET;

    bool doingProcessing;

    QStringList someRandomStrings;
    QStringList someRandomNames;
    void addSETTransaction();
    QString getRandomStringFromList() const { int i = qrand() % someRandomStrings.size(); return someRandomStrings.at(i); }
    QString getRandomFirstName() const { int i = qrand() % someRandomNames.size(); return someRandomNames.at(i).split(" ").first(); }
    QString getRandomLastName() const { int i = qrand() % someRandomNames.size(); return someRandomNames.at(i).split(" ").last(); }
    QString getRandomDrUID() const { int i = qrand() % createdDRDNI.size(); return createdDRDNI.at(i); }
};

#endif // SSLWRAPPER_H
