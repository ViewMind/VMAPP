#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QUrl>
#include <QFileInfo>
#include <QQuickWindow>
#include <QSystemSemaphore>
#include <QSharedMemory>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QDesktopServices>

#include "../../CommonClasses/LogInterface/staticthreadlogger.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/FileDownloader/filedownloader.h"
#include "../../CommonClasses/HWRecog/hwrecognizer.h"
#include "studyendoperations.h"
#include "eyexperimenter_defines.h"
#include "countries.h"
#include "localdb.h"
#include "apiclient.h"
#include "updater.h"


class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = nullptr, ConfigurationManager *c = nullptr, CountryStruct *cs = nullptr);
    ~Loader() override;

    //////////////////////////// UI Functions ////////////////////////////
    Q_INVOKABLE QString getStringForKey(const QString &key, bool fromLangFile = true);
    Q_INVOKABLE QStringList getStringListForKey(const QString &key, bool fromLangFile = true);
    Q_INVOKABLE void setExplanationLanguage();
    Q_INVOKABLE QVariantMap getExplanationLangMap() const;
    Q_INVOKABLE QVariantMap getStudyNameMap();
    Q_INVOKABLE bool getLoaderError() const;
    Q_INVOKABLE QString getWindowTilteVersion();
    Q_INVOKABLE QStringList getCountryList();
    Q_INVOKABLE QStringList getCountryCodeList();
    Q_INVOKABLE int getDefaultCountry(bool offset = true);
    Q_INVOKABLE QString getCountryCodeForCountry(const QString &country);
    Q_INVOKABLE int getCountryIndexFromCode(const QString &code);
    Q_INVOKABLE QString getVersionNumber() const;
    Q_INVOKABLE QString getInstitutionName() const;
    Q_INVOKABLE bool isVMConfigPresent() const;
    Q_INVOKABLE void changeGetVMConfigScreenLanguage(const QString &var);
    Q_INVOKABLE void storeNewStudySequence(const QString &name, const QVariantList &sequence);
    Q_INVOKABLE QVariantList getStudySequence(const QString &name);
    Q_INVOKABLE QVariantMap getStudySequenceListAndCurrentlySelected() const;
    Q_INVOKABLE void deleteStudySequence(const QString &name);
    Q_INVOKABLE void setCurrentStudySequence(const QString &name);
    Q_INVOKABLE void logUIMessage(const QString &message, bool isError);


    Q_INVOKABLE void openUserManual();
    Q_INVOKABLE bool processingParametersArePresent() const;

    //////////////////////////// UPDATE RELATED FUNCTIONS ////////////////////////////
    Q_INVOKABLE QString getNewUpdateVersionAvailable() const;
    Q_INVOKABLE qint32 getRemainingUpdateDenials() const;
    Q_INVOKABLE void updateDenied();
    Q_INVOKABLE void startUpdate();
    Q_INVOKABLE bool isFirstTimeRun() const;
    Q_INVOKABLE QStringList getLatestVersionChanges();

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
    Q_INVOKABLE QString addOrModifySubject(QString suid, const QString &name, const QString &lastname, const QString &institution_id, const QString &birthdate,
                                           const QString &birthCountry,  const QString &gender, qint32 formative_years, const QString &email);
    Q_INVOKABLE void modifySubjectSelectedMedic(const QString &suid, const QString &selectedMedic);
    Q_INVOKABLE QVariantMap filterSubjectList(const QString &filter);
    Q_INVOKABLE bool setSelectedSubject(const QString &suid);    
    Q_INVOKABLE void setStudyMarkerFor(const QString &study, const QString &value);
    Q_INVOKABLE QString getStudyMarkerFor(const QString &study);
    Q_INVOKABLE QVariantMap getCurrentSubjectInfo();
    Q_INVOKABLE void clearSubjectSelection();
    Q_INVOKABLE QString getCurrentlySelectedAssignedDoctor() const;
    Q_INVOKABLE bool areThereAnySubjects() const;

    //////////////////////////// MEDIC RELATED FUNCTION ////////////////////////////
    Q_INVOKABLE QVariantMap getMedicList() const;

    //////////////////////////// CONFIGURATION FUNCTIONS ////////////////////////////
    Q_INVOKABLE QString getConfigurationString(const QString &key);
    Q_INVOKABLE QVariant getDebugOption(const QString &debugOption);
    Q_INVOKABLE bool getConfigurationBoolean(const QString &key);
    Q_INVOKABLE void setSettingsValue(const QString& key, const QVariant &var);
    Q_INVOKABLE void setValueForConfiguration(const QString &key, const QVariant &var);

    //////////////////////////// FILE MANAGEMENT FUNCTIONS ////////////////////////////
    Q_INVOKABLE bool createSubjectStudyFile(const QVariantMap &studyconfig, const QString &medic, const QString &protocol);
    Q_INVOKABLE QString getCurrentSubjectStudyFile() const;

    ////////////////////////// REPORT GENERATING FUNCTIONS ////////////////////////////
    Q_INVOKABLE QVariantMap getReportsForLoggedEvaluator();
    Q_INVOKABLE void setCurrentStudyFileToSendOrDiscard(const QString &file);
    Q_INVOKABLE qint32 wasThereAnProcessingUploadError() const;

    ////////////////////////// API REQUESTS ////////////////////////////
    Q_INVOKABLE void requestOperatingInfo(bool logOnly);
    Q_INVOKABLE void sendStudy(QString discard_reason, const QString &comment);
    Q_INVOKABLE qint32 getLastAPIRequest();
    Q_INVOKABLE void requestActivation(int institution, int instance, const QString &key);

    //////////////////////////// PROTOCOL RELATED FUNCTIONS ////////////////////////////
    Q_INVOKABLE bool addProtocol(const QString &name, const QString &id);
    Q_INVOKABLE void editProtocol(const QString &id, const QString &newName);
    Q_INVOKABLE void deleteProtocol(const QString &id);
    Q_INVOKABLE QVariantMap getProtocolList();


signals:
    void finishedRequest();
    void logUploadFinished();

private slots:
    void receivedRequest();
    void updateDownloadFinished(bool allOk);
    void startUpSequenceCheck(); // Start up sequence value checker. When it reaches 2 the start up sequence finished. Needs to be a slot that the qc checker can connect to when finished

private:

    bool loadingError;
    bool isLicenceFilePresent;
    ConfigurationManager *configuration;
    ConfigurationManager language;
    ConfigurationManager explanationStrings;

    // Module to recognize HW in computer.
    HWRecognizer hwRecognizer;

    // String that stores the new version of the application if one is avaible. For display.
    QString newVersionAvailable;

    // The API communication client.
    APIClient apiclient;

    // Start up sequence flag. It requires two process to be done in order to actually kill the wait screen.
    quint8 startUpSequenceFlag;

    // The object is used to make sure al QCI files are generated. Will almost certainly be ONLY of use when updating to the version of the
    // APP that started using the unified QC.
    StudyEndOperations qcChecker;

    // The object required to download the update files from the URL provided by the API.
    FileDownloader fileDownloader;

    // The local database
    LocalDB localDB;

    // Flag that indicates that it's the first time running this particular version of the application.
    bool firstTimeRun;

    // The list of countries and their codes.
    CountryStruct *countries;

    // Flag for checking uplaod error
    qint32 processingUploadError;

    // In order for the update to function properly the system command that calls the update script needs to be started in a separate thread.
    Updater updater;

    // Loads the appropiate language file.
    void changeLanguage();

    // Loads default configurations when they don't exist.
    void loadDefaultConfigurations();

    // Moves sent files to the processed directory
    void moveProcessedFiletToProcessedDirectory();

    // Clean the failed calibrations directory.
    void cleanCalibrationDirectory();

    // Processing errors.
    static const qint32 FAIL_CODE_NONE = 0;
    static const qint32 FAIL_BAD_ACTIVATION_RETURN = 1;
    static const qint32 FAIL_CODE_SERVER_ERROR = 2;
    static const qint32 FAIL_FILE_CREATION = 3;

    // List of available studies with no reports ready.
    static const QStringList NO_REPORT_STUDIES;

    // Number of graphs to be shown during QC.
    static const qint32 NUMBER_OF_QC_GRAPHS = 3;

    // Minimum number of points to conform a fixation.
    static const qint32 MINIMUM_NUMBER_OF_DATAPOINTS_IN_FIXATION = 4;

    // When a study needs to run but no report can be generated, the study is treated EXACLTY
    // like a discarded study, but the discard reason (a string code) is specific to this effect.
    static const QString NO_REPORT_DISCARD_CODE;


};

#endif // LOADER_H
