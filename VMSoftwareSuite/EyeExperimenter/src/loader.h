#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QUrl>
#include <QQuickWindow>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QCoreApplication>

#include "../../../CommonClasses/LogInterface/loginterface.h"
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "Experiments/experiment.h"
#include "eyexperimenter_defines.h"
#include "countries.h"
#include "localdb.h"
#include "subjectdirscanner.h"
#include "apiclient.h"
#include "qualitycontrol.h"


class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = nullptr, ConfigurationManager *c = nullptr, CountryStruct *cs = nullptr);
    ~Loader();

    //////////////////////////// UI Functions ////////////////////////////
    Q_INVOKABLE QString getStringForKey(const QString &key);
    Q_INVOKABLE QStringList getStringListForKey(const QString &key);
    Q_INVOKABLE bool getLoaderError() const;

    Q_INVOKABLE QString getWindowTilteVersion();
    Q_INVOKABLE QStringList getCountryList();
    Q_INVOKABLE QStringList getCountryCodeList();
    Q_INVOKABLE int getDefaultCountry(bool offset = true);
    Q_INVOKABLE QString getCountryCodeForCountry(const QString &country);
    Q_INVOKABLE int getCountryIndexFromCode(const QString &code);

    Q_INVOKABLE QString getVersionNumber() const;
    Q_INVOKABLE QString getManufactureDate() const;
    Q_INVOKABLE QString getSerialNumber() const;
    Q_INVOKABLE QString getUniqueAuthorizationNumber() const;
    Q_INVOKABLE QString getInstitutionName() const;

    //////////////////////////// EVALUATOR RELATED FUNCTIONS ////////////////////////////
    Q_INVOKABLE void logOut();
    Q_INVOKABLE bool isLoggedIn();
    Q_INVOKABLE bool checkIfEvaluatorEmailExists(const QString &username) const;
    Q_INVOKABLE void addOrModifyEvaluator(const QString &email, const QString &oldemail, const QString &password, const QString &name, const QString &lastname);
    Q_INVOKABLE bool evaluatorLogIn(const QString &username, const QString &password);
    Q_INVOKABLE void updateCurrentEvaluator(const QString &username);
    Q_INVOKABLE QVariantMap getCurrentEvaluatorInfo() const;
    Q_INVOKABLE QStringList getLoginEmails() const;

    //////////////////////////// SUBJECT REALATED FUNCTIONS ////////////////////////////
    Q_INVOKABLE void addOrModifySubject(QString suid, const QString &name, const QString &lastname, const QString &institution_id,
                                        const QString &age, const QString &birthdate, const QString &birthCountry,
                                        const QString &gender, qint32 formative_years, qint32 selectedMedic);

    Q_INVOKABLE QVariantMap filterSubjectList(const QString &filter);
    Q_INVOKABLE bool setSelectedSubject(const QString &suid);    
    Q_INVOKABLE void setStudyMarkerFor(const QString &study, const QString &value);
    Q_INVOKABLE QString getStudyMarkerFor(const QString &study);
    Q_INVOKABLE QVariantMap getCurrentSubjectInfo();
    Q_INVOKABLE void clearSubjectSelection();
    Q_INVOKABLE QString getCurrentlySelectedAssignedDoctor() const;

    //////////////////////////// MEDIC RELATED FUNCTION ////////////////////////////
    Q_INVOKABLE QVariantMap getMedicList() const;

    //////////////////////////// CONFIGURATION FUNCTIONS ////////////////////////////
    Q_INVOKABLE QString getConfigurationString(const QString &key);
    Q_INVOKABLE bool getConfigurationBoolean(const QString &key);
    Q_INVOKABLE void setSettingsValue(const QString& key, const QVariant &var);
    Q_INVOKABLE void setValueForConfiguration(const QString &key, const QVariant &var);

    //////////////////////////// FILE MANAGEMENT FUNCTIONS ////////////////////////////
    Q_INVOKABLE bool createSubjectStudyFile(const QVariantMap &studyconfig, qint32 medic, const QString &protocol);

    ////////////////////////// REPORT GENERATING FUNCTIONS ////////////////////////////
    Q_INVOKABLE QVariantMap getReportsForLoggedEvaluator();
    Q_INVOKABLE void setCurrentStudyFileForQC(const QString &file);
    Q_INVOKABLE QStringList getStudyList() const;
    Q_INVOKABLE QVariantMap getStudyGraphData(const QString &study, qint32 selectedGraph);

    ////////////////////////// API REQUESTS ////////////////////////////
    Q_INVOKABLE void requestOperatingInfo();

    //////////////////////////// PROTOCOL RELATED FUNCTIONS ////////////////////////////
    Q_INVOKABLE bool addProtocol(const QString &p);
    Q_INVOKABLE void deleteProtocol(const QString &p);
    Q_INVOKABLE QStringList getProtocolList();

signals:
    void finishedRequest();

private slots:
    void receivedRequest();

private:

    LogInterface logger;
    bool loadingError;
    ConfigurationManager *configuration;
    ConfigurationManager language;

    // The API communication client.
    APIClient apiclient;

    // The local database
    LocalDB localDB;

    // Quality Control Computations
    QualityControl qc;

    // The list of countries and their codes.
    CountryStruct *countries;

    // Loads default configurations when they don't exist.
    void loadDefaultConfigurations();

    // Sets the language for program.
    void changeLanguage();



};

#endif // LOADER_H
