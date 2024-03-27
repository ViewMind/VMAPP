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
#include <QStandardPaths>

#include "../../CommonClasses/LogInterface/staticthreadlogger.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/FileDownloader/filedownloader.h"
#include "../../CommonClasses/HWRecog/hwrecognizer.h"
#include "../../CommonClasses/StudyControl/studycontrol.h"
#include "studyendoperations.h"
#include "eyexperimenter_defines.h"
#include "countries.h"
#include "apiclient.h"
#include "loaderflowcomm.h"

class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = nullptr, LoaderFlowComm *c = nullptr);
    ~Loader() override;

    //////////////////////////// UI Functions ////////////////////////////
    Q_INVOKABLE QString getStringForKey(const QString &key, bool fromLangFile = true);
    Q_INVOKABLE QStringList getStringListForKey(const QString &key, bool fromLangFile = true);
    Q_INVOKABLE void setExplanationLanguage();
    Q_INVOKABLE QVariantMap getExplanationLangMap();
    Q_INVOKABLE QVariantMap getStudyNameMap();
    Q_INVOKABLE bool getLoaderError() const;
    Q_INVOKABLE QString getWindowTilteVersion();
    Q_INVOKABLE QString getVersionNumber() const;
    Q_INVOKABLE QString getInstitutionName() const;
    Q_INVOKABLE bool isVMConfigPresent() const;
    Q_INVOKABLE void changeGetVMConfigScreenLanguage(const QString &var);
    Q_INVOKABLE QVariantList getAvailableEvaluations();
    Q_INVOKABLE void logUIMessage(const QString &message, bool isError);
    Q_INVOKABLE void openUserManual();
    Q_INVOKABLE bool processingParametersArePresent() const;
    Q_INVOKABLE void openURLInBrowser(const QString &url);
    Q_INVOKABLE bool instanceDisabled() const;
    Q_INVOKABLE int  checkHMDChangeStatus() const;
    Q_INVOKABLE QVariantList findPossibleDupes(QString name, QString lname, QString personalID, QString birthDate);
    Q_INVOKABLE QVariantMap getInstIDFileInfo() const;
    Q_INVOKABLE QVariantMap getHWInfo() const;
    Q_INVOKABLE QString getInstanceUID() const;
    Q_INVOKABLE qint32 getLastHTTPCodeReceived() const;
    Q_INVOKABLE QVariantMap getTaskCodeToNameMap() const;

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
    Q_INVOKABLE QStringList getLoginEmails(bool with_name = false) const;

    //////////////////////////// SUBJECT REALATED FUNCTIONS ////////////////////////////
    Q_INVOKABLE QString addOrModifySubject(QString suid, const QString &name, const QString &lastname, const QString &institution_id, const QString &birthdate,  const QString &gender, qint32 formative_years, const QString &email);
    Q_INVOKABLE void modifySubjectSelectedMedic(const QString &selectedMedic);
    Q_INVOKABLE QVariantMap filterSubjectList(const QString &filter);
    Q_INVOKABLE bool setSelectedSubject(const QString &suid);    
    Q_INVOKABLE QVariantMap getCurrentSubjectInfo();
    Q_INVOKABLE void clearSubjectSelection();
    Q_INVOKABLE QString getCurrentlySelectedAssignedDoctor() const;
    Q_INVOKABLE bool areThereAnySubjects() const;

    //////////////////////////// MEDIC RELATED FUNCTION ////////////////////////////
    Q_INVOKABLE QVariantMap getMedicList() const;

    //////////////////////////// CONFIGURATION FUNCTIONS ////////////////////////////
    Q_INVOKABLE QVariant getDebugOption(const QString &debugOption);
    Q_INVOKABLE QString getSettingsString(const QString &key, const QString &defvalue = "");
    Q_INVOKABLE void setSettingsValue(const QString& key, const QVariant &var);

    //////////////////////////// FILE MANAGEMENT FUNCTIONS ////////////////////////////
    Q_INVOKABLE QString getCurrentSubjectStudyFile() const;

    ////////////////////////// REPORT GENERATING FUNCTIONS ////////////////////////////
    Q_INVOKABLE QVariantMap getReportsForLoggedEvaluator();
    Q_INVOKABLE void setCurrentStudyFileToSendOrDiscard(const QString &file);
    Q_INVOKABLE qint32 wasThereAnProcessingUploadError() const;

    ////////////////////////// API REQUESTS ////////////////////////////
    Q_INVOKABLE void requestOperatingInfo();
    Q_INVOKABLE void sendStudy(QString discard_reason, const QString &comment);
    Q_INVOKABLE qint32 getLastAPIRequest();
    Q_INVOKABLE void requestActivation(int institution, int instance, const QString &key);
    Q_INVOKABLE void sendSupportEmail(const QString &subject,const QString &body, const QString &evaluator_name, const QString &evaluator_email);
    Q_INVOKABLE void sendFunctionalControlData(const QString &instituion, const QString &instance, const QString &email, const QString &password, const QString &fname, const QString &lname);

    //////////////////////////// PROTOCOL RELATED FUNCTIONS ////////////////////////////
    Q_INVOKABLE bool addProtocol(const QString &name, const QString &id);
    Q_INVOKABLE void editProtocol(const QString &id, const QString &newName);
    Q_INVOKABLE void deleteProtocol(const QString &id);
    Q_INVOKABLE QVariantMap getProtocolList();


signals:
    void finishedRequest();
    void sendSupportEmailDone(bool allOK);
    void titleBarUpdate();
    void downloadProgressUpdate(qreal progress, qreal hours, qreal minutes, qreal seconds, qint64 bytesDowloaded, qint64 bytesTotal);

public slots:
    void onNotificationFromFlowControl(QVariantMap notification);

private slots:
    void receivedAPIResponse();
    void updateDownloadFinished(bool allOk);
    void startUpSequenceCheck(); // Start up sequence value checker. When it reaches 2 the start up sequence finished. Needs to be a slot that the qc checker can connect to when finished
    void onFileDownloaderUpdate(qreal progress, qreal hours, qreal minutes, qreal seconds, qint64 bytesDowloaded, qint64 bytesTotal);

private:

    bool loadingError;
    bool isLicenceFilePresent;
    ConfigurationManager language;
    ConfigurationManager explanationStrings;

    // This is required by a few function in the UI hence we load it when we load the language and then we keep it.
    QVariantMap taskCodeToNameMap;

    // Temporarily store instance data while doing functional verification.
    ConfigurationManager tempInstanceData;

    // String that stores the new version of the application if one is avaible. For display.
    QString newVersionAvailable;

    // The API communication client.
    APIClient apiclient;

    // The pretty name for the eyetracker/HMD being used. Mainly for display on the title bar.
    QString eyeTrackerName;

    // Start up sequence flag. It requires two process to be done in order to actually kill the wait screen.
    quint8 startUpSequenceFlag;

    // The object is used to make sure al QCI files are generated. Will almost certainly be ONLY of use when updating to the version of the
    // APP that started using the unified QC.
    StudyEndOperations qcChecker;

    // The object required to download the update files from the URL provided by the API.
    FileDownloader fileDownloader;

    // Flag used to indicate whethre the app.zip file was redownloaded or not.
    bool updateDownloadSkipped;

    // Access to the local databaese and global configurations.
    LoaderFlowComm *comm;

    // Flag that indicates that it's the first time running this particular version of the application.
    bool firstTimeRun;

    // Flag for checking uplaod error
    qint32 processingUploadError;

    // Used mainly for updating the title bar.
    QString institutionStringIdentification;

    // We store the HTTP Code from the last API Request
    qint32 lastHTTPCodeReceived;

    // Loads the appropiate language file.
    void changeLanguage();

    // Moves sent files to the processed directory
    void moveProcessedFiletToProcessedDirectory();

    // Clean the failed calibrations directory.
    void cleanCalibrationDirectory();

    // Processing errors.
    static const qint32 FAIL_CODE_NONE = 0;
    static const qint32 FAIL_BAD_ACTIVATION_RETURN = 1;
    static const qint32 FAIL_CODE_SERVER_ERROR = 2;
    static const qint32 FAIL_FILE_CREATION = 3;
    static const qint32 FAIL_INSTANCE_DISABLED = 4;

};

#endif // LOADER_H
