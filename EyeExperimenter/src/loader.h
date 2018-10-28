#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QUrl>
#include <QQuickWindow>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "sslclient/ssldbclient.h"
#include "eye_experimenter_defines.h"
#include "countries.h"
#include "localinformationmanager.h"

class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = nullptr, ConfigurationManager *c = nullptr, CountryStruct *cs = nullptr);
    ~Loader();
    Q_INVOKABLE QString getStringForKey(const QString &key);
    Q_INVOKABLE QStringList getStringListForKey(const QString &key);
    Q_INVOKABLE QString getConfigurationString(const QString &key);
    Q_INVOKABLE bool getConfigurationBoolean(const QString &key);
    Q_INVOKABLE bool getLoaderError() const {return loadingError; }
    Q_INVOKABLE void setSettingsValue(const QString& key, const QVariant &var);
    Q_INVOKABLE void setValueForConfiguration(const QString &key, const QVariant &var) {configuration->addKeyValuePair(key,var);}
    Q_INVOKABLE bool checkETChange();
    Q_INVOKABLE QString hasValidOutputRepo(const QString &dirToCheck = "");
    Q_INVOKABLE QString getWindowTilteVersion(){ return EXPERIMENTER_VERSION; }
    Q_INVOKABLE bool createPatientDirectory();
    Q_INVOKABLE QRect frameSize(QObject *window);
    Q_INVOKABLE QStringList getCountryList() {return countries->getCountryList();}
    Q_INVOKABLE QStringList getPatientList();
    Q_INVOKABLE QStringList getUIDList();
    Q_INVOKABLE QStringList getPatientIsOKList();
    Q_INVOKABLE QStringList getDoctorList();
    Q_INVOKABLE QString getDoctorUIDByIndex(qint32 selectedIndex);
    Q_INVOKABLE bool isDoctorValidated(qint32 selectedIndex);
    Q_INVOKABLE bool isDoctorPasswordCorrect(const QString &password);
    Q_INVOKABLE bool doesCurrentDoctorHavePassword() { return !lim->getCurrentDoctorPassword().isEmpty(); }
    Q_INVOKABLE QVariantMap getCurrentDoctorInformation() {return lim->getCurrentDoctorInfo();}
    Q_INVOKABLE QVariantMap getCurrentPatientInformation() {return lim->getCurrentPatientInfo();}
    Q_INVOKABLE int getDefaultCountry(bool offset = true);
    Q_INVOKABLE QString getCountryCodeForCountry(const QString &country) { return countries->getCodeForCountry(country); }
    Q_INVOKABLE void setAgeForCurrentPatient();
    Q_INVOKABLE int getCountryIndexFromCode(const QString &code) { return countries->getIndexFromCode(code); }
    Q_INVOKABLE void addNewDoctorToDB(QVariantMap dbdata);
    Q_INVOKABLE void addNewPatientToDB(QVariantMap dbdatareq, QVariantMap dbdataopt);
    Q_INVOKABLE void startDBSync();
    Q_INVOKABLE void requestDrValidation(const QString &instPassword, qint32 selectedDr);
    Q_INVOKABLE void prepareForRequestOfPendingReports();
    Q_INVOKABLE bool wasDBTransactionOk() {if (wasDBTransactionStarted) return dbClient->getTransactionStatus(); else return true;}
    Q_INVOKABLE QString loadTextFile(const QString &fileName);
    Q_INVOKABLE QStringList getErrorMessageForCode(quint8 code);

signals:
    void synchDone();

    // Signal to FlowControl, indicating the next file set to process.
    void nextFileSet(const QStringList &fileSet);   

    // Signal with the result message of verifying the institution password
    void instPasswordVerifyResults(QString verifyMsg);

public slots:
    // For when the DB Transaction has finished.
    void onDisconnectFromDB();

    // Request of the flow control for the next set of files to process.
    void onRequestNextPendingReport();

private:
    LogInterface logger;
    bool loadingError;
    ConfigurationManager *configuration;
    ConfigurationManager language;

    // To control data
    LocalInformationManager *lim;

    // The list of countries and their codes.
    CountryStruct *countries;

    // The list that holds list names and corresponding uids
    QList<QStringList> nameInfoList;

    // Temporarily save the hasshed institution password for comparison.s
    QString hasshedPassword;
    QString drUIDtoValidate;

    // To connect to the DB in the server. Flags are required to provide the proper information to the QML side.
    SSLDBClient *dbClient;
    bool wasDBTransactionStarted;

    // Loads default configurations when they don't exist.
    void loadDefaultConfigurations();

    // Creates the directory substructure
    bool createDirectorySubstructure(QString drname, QString pname, QString baseDir, QString saveAs);

    // Sets the language for program.
    void changeLanguage();
};

#endif // LOADER_H
