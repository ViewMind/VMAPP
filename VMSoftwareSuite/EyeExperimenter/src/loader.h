#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QUrl>
#include <QQuickWindow>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QCoreApplication>

#include "../../../CommonClasses/LogInterface/loginterface.h"
#include "Experiments/experiment.h"
#include "eyexperimenter_defines.h"
#include "countries.h"
#include "localdb.h"
#include "subjectdirscanner.h"
#include "apiclient.h"

#define  MAX_UPDATES_TO_SHOW        3
#define  NUMBER_SECONDS_IN_A_DAY    86400
#define  NUMBER_OF_PERCEPTION_PARTS 8


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
                                        const QString &gender, qint32 formative_years);

    Q_INVOKABLE QVariantMap filterSubjectList(const QString &filter);
    Q_INVOKABLE bool setSelectedSubject(const QString &suid);    
    Q_INVOKABLE void setStudyMarkerFor(const QString &study, const QString &value);
    Q_INVOKABLE QString getStudyMarkerFor(const QString &study);
    Q_INVOKABLE QVariantMap getCurrentSubjectInfo();
    Q_INVOKABLE void clearSubjectSelection();

    //////////////////////////// CONFIGURATION FUNCTIONS ////////////////////////////
    Q_INVOKABLE QString getConfigurationString(const QString &key);
    Q_INVOKABLE bool getConfigurationBoolean(const QString &key);
    Q_INVOKABLE void setSettingsValue(const QString& key, const QVariant &var);
    Q_INVOKABLE void setValueForConfiguration(const QString &key, const QVariant &var);

    //////////////////////////// FILE MANAGEMENT FUNCTIONS ////////////////////////////
    Q_INVOKABLE bool createSubjectStudyFile(const QVariantMap &studyconfig);

    ////////////////////////// REPORT GENERATING FUNCTIONS ////////////////////////////
    Q_INVOKABLE QList<QVariantMap> getReportsForLoggedEvaluator();

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

    // The local database
    LocalDB localDB;

    // The list of countries and their codes.
    CountryStruct *countries;

    // Loads default configurations when they don't exist.
    void loadDefaultConfigurations();

    // Sets the language for program.
    void changeLanguage();

    APIClient apiclient;

    static const char * FILENAME_BASE_READING;
    static const char * FILENAME_BASE_BINDING;
    static const char * FILENAME_BASE_NBACKVS;
    static const char * FILENAME_BASE_NBACKRT;
    static const char * FILENAME_BASE_NBACKMS;
    static const char * FILENAME_BASE_GONOGO;
    static const char * FILENAME_BASE_PERCEPTION;

    /**
     * @brief findOngoingStudyFileNames
     * @param study_config the study configuration to match.
     * @details This will search for status "ongoing" files in the patient directory, that match the study configuration and time critera that depends on each study. If it finds one it will return the expected trial list type for it's next expected trial list type.
     * @return A Map. For each expected trial list type, what file does it belong to. If more than one the most recent one is used.
     */
    QMap<QString,QString> findOngoingStudyFileNames(QVariantMap study_config);

};

#endif // LOADER_H
