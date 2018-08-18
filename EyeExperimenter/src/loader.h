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
    Q_INVOKABLE void setConfigurationString(const QString &key, const QString &value);
    Q_INVOKABLE void setConfigurationBoolean(const QString &key, bool value);
    Q_INVOKABLE void setConfigurationInt(const QString &key, qint32 value);
    Q_INVOKABLE QString hasValidOutputRepo(const QString &dirToCheck = "");
    Q_INVOKABLE QString getWindowTilteVersion(){ return EXPERIMENTER_VERSION; }
    Q_INVOKABLE bool createPatientDirectory(const QString &patientuid);
    Q_INVOKABLE QRect frameSize(QObject *window);
    Q_INVOKABLE QStringList getCountryList() {return countries->getCountryList();}
    Q_INVOKABLE QStringList getPatientList() {return patientListForDoctor; }
    Q_INVOKABLE QStringList getDoctorList() {return userDoctorInfo; }
    Q_INVOKABLE int getDefaultCountry();
    Q_INVOKABLE void addNewDoctorToDB(QVariantMap dbdata);
    Q_INVOKABLE void addNewPatientToDB(QVariantMap dbdatareq, QVariantMap dbdataopt);
    Q_INVOKABLE void getUserDoctorInfoFromDB();
    Q_INVOKABLE void getPatientListFromDB();

signals:
    void newDoctorAdded();
    void newPatientAdded();
    void updatedDoctorList();
    void updatedDoctorPatientList();

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

    // The list of the THIS PC users (doctors)
    QStringList userDoctorInfo;
    QStringList patientListForDoctor;

    // To connect to the DB in the server.
    SSLDBClient *dbClient;

    // Type of DB Operations.
    typedef enum {DBO_NEW_DR, DBO_NEW_PATIENT, DBO_LIST_PATIENTS, DBO_LIST_DOCTORS} DBOperation;
    DBOperation dbOperation;

    // Loads default configurations when they don't exist.
    void loadDefaultConfigurations();

    // Creates the directory substructure
    bool createDirectorySubstructure(QString drname, QString pname, QString baseDir, QString saveAs);

    // Sets the language for program.
    void changeLanguage();
};

#endif // LOADER_H
