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
    Q_INVOKABLE QString getStringForKey(const QString &key);
    Q_INVOKABLE QStringList getStringListForKey(const QString &key);
    Q_INVOKABLE QString getConfigurationString(const QString &key);
    Q_INVOKABLE bool getConfigurationBoolean(const QString &key);
    Q_INVOKABLE bool getLoaderError() const {return loadingError; }
    Q_INVOKABLE void setSettingsValue(const QString& key, const QVariant &var);
    Q_INVOKABLE void setValueForConfiguration(const QString &key, const QVariant &var) {configuration->addKeyValuePair(key,var);}
    Q_INVOKABLE QString hasValidOutputRepo(const QString &dirToCheck = "");
    Q_INVOKABLE QString getWindowTilteVersion(){ return EXPERIMENTER_VERSION; }
    Q_INVOKABLE bool createPatientDirectory(const QString &patientuid);
    Q_INVOKABLE QRect frameSize(QObject *window);
    Q_INVOKABLE QStringList getCountryList() {return countries->getCountryList();}
    Q_INVOKABLE QStringList getPatientList();
    Q_INVOKABLE QStringList getUIDList();
    Q_INVOKABLE QStringList getPatientIsOKList();
    Q_INVOKABLE QStringList getDoctorList();
    Q_INVOKABLE QString getDoctorUIDByIndex(qint32 selectedIndex);
    Q_INVOKABLE QVariantMap getCurrentDoctorInformation() {return lim->getCurrentDoctorInfo();}
    Q_INVOKABLE QVariantMap getCurrentPatientInformation() {return lim->getCurrentPatientInfo();}
    Q_INVOKABLE int getDefaultCountry(bool offset = true);
    Q_INVOKABLE QString getCountryCodeForCountry(const QString &country) { return countries->getCodeForCountry(country); }
    Q_INVOKABLE int getCountryIndexFromCode(const QString &code) { return countries->getIndexFromCode(code); }
    Q_INVOKABLE void addNewDoctorToDB(QVariantMap dbdata);
    Q_INVOKABLE void addNewPatientToDB(QVariantMap dbdatareq, QVariantMap dbdataopt);
    Q_INVOKABLE void startDBSync();

signals:
    void synchDone();

public slots:
    void onTransactionFinished(bool isOk);
    void onDisconnectFromDB();

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

    // To connect to the DB in the server.
    SSLDBClient *dbClient;

    // Loads default configurations when they don't exist.
    void loadDefaultConfigurations();

    // Creates the directory substructure
    bool createDirectorySubstructure(QString drname, QString pname, QString baseDir, QString saveAs);

    // Sets the language for program.
    void changeLanguage();
};

#endif // LOADER_H
