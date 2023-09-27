#ifndef MAINTENANCEMANAGER_H
#define MAINTENANCEMANAGER_H

#include <QThread>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QProcess>
#include <QJsonParseError>
#include <QJsonDocument>
#include <QJsonObject>

#include "../../CommonClasses/DirTools/dirrunner.h"
#include "../../CommonClasses/DirTools/dircompare.h"
#include "messagelogger.h"
#include "debugoptions.h"
#include "defines.h"
#include "langs.h"
#include "paths.h"


class MaintenanceManager : public QThread
{
    Q_OBJECT
public:

    typedef enum {
                  ACTION_UPDATE,
                  ACTION_DIAGNOSE,
                  ACTION_CONTACT_SUPPORT,
                  ACTION_CORRECT,
                  ACTION_NONE
                 } Action;

    MaintenanceManager();

    void setAction(Action action);
    void setRecommendedAction(Action action);
    void setDryRun(bool dr);

    void run();

    Action getLastPerformedAction() const;
    Action getRecommendedAction() const;

    bool wasLastActionASuccess() const;

signals:

    void message(qint32 type, QString message);
    void progressUpdate(qreal percent, QString message);

private slots:
    void onDirCompareProgress(double p, QString filename);

private:

    typedef enum {
        CA_MOVE_DIR,
        CA_REMOVE_DIR,
        CA_COPY_FILE,
        CA_UNTAR_APP,
        CA_CREATE_SHORTCUT,
        CA_DELETE_FILE,
        CA_RESTORE_DB
    } CorrectiveActionType;

    typedef struct {
        CorrectiveActionType type;
        QString source;
        QString destination;
        QString error_key;
        QString warning_key;
        qint32 progress;
        QString progress_label;
    } CorrectiveAction;

    QList<CorrectiveAction> correctiveActions;

    Action currentAction;
    Action recommendedAction;
    bool actionSuccessfull;

    bool dryRun;

    // Variables resuling from running diagnosis.
    bool anyDiagnosisStepsFailed;
    bool eebkpExists;
    bool eyeExpUnderScoreExists;
    bool eebkpContainsETData;
    bool eyeExpUnderScoreContainsETData;
    bool currentInstallExists;
    QStringList missingFilesInInstall;
    QStringList corruptedFilesInInstall;
    bool currentDBCorrupted;
    bool currentLicenseFileCorrupted;

    void error(const QString &s);
    void succes(const QString &s);
    void warning(const QString &s);
    void display(const QString &s);
    void log(const QString &s);

    /**
     * @brief updateTheApp - Sequence of steps to update the EyeExperimenter application to a newer version.
     */
    void doActionUpdate();

    /**
     * @brief runDiagnostics - Sequence of checks for the most common issues regarding a non working application.
     */
    void doActionRunDiagnostics();

    /**
     * @brief getRecommendedActionFromDiagnosisResults - Uses the diagnosis results to implemente a recommended action logic.
     */
    void getRecommendedActionFromDiagnosisResults();
    /**
     * @brief uncompressAppPackage - Calls tar.exe to uncompress the app.zip file contained in the application directory.
     * @param destination - Where shoudl the uncompressed directory be located.
     * @param output - Pointer to string where the output fo the command will be stored.
     * @param errorString - Pointer to the string twhere the error string will be stored.
     * @return True if successfull, false otherwise.
     */
    bool uncompressAppPackage(const QString &destination, QString *output, QString *errorString);

    /**
     * @brief computeMD5ForFile - Computes the MD5 checksum for a file.
     * @param filename - The name of the file whose chesum will be computed.
     * @return The string of the hex for the computed md5
     */
    QString computeMD5ForFile(const QString &filename);

    /**
     * @brief isLocalDBOK - Searches the current DB for the EE and computes the checksum comparing with it's stored checksum.
     * @param override_path - If empty it checks the DB of the current installation, otherwise uses the DB in the provided path.
     * @return True if they are the same false, otherwise.
     */
    bool isLocalDBOK(const QString &override_path = "");

    /**
     * @brief performCorrectiveActions - Goes through the list of corrective actions and performs them.
     * @param sayNotDo - If false the log will be written but the actual action won't be taken. For debug.
     */
    bool performCorrectiveActions(bool sayNotDo = false);

    /**
     * @brief restoreDBToPreviousValidPoint - Searches the DB BKP dir for the newest DB BKP that has a valid checksum and copies it to the current local DB.
     * @return True if successfull, false otherwise.
     */

    bool restoreDBToPreviousValidPoint();

    /**
     * @brief addCorrectiveAction - Convenience function to add corecctive actions to the list that will be used by performCorrective actions.
     * @param type - The type of corrective action.
     * @param source - The source file over which the corrective action is taken.
     * @param destination - The destination (for file/directory copying and moving)
     * @param error_key - If the action fails the error langauge key to display the error. If empty no error is displayed.
     * @param warning_key - If the action fials then a warning instead of a error is issued if this is not empty.
     * @param progress_label - If not empty the label text on the progress Bar.
     */

    void addCorrectiveAction(CorrectiveActionType type,
                              const QString &source,
                              const QString &destination,
                              const QString &error_key = "",
                              const QString &warning_key = "",
                              const QString &progress_label = "");

    /**
     * @brief createDesktopShortcut - Creates a shortuct to the desktop.
     * @param pathToEXE - The path to the executable file.
     * @param shortcutName - The name of the shortcut in the desktop.
     * @return True if successfull. False otherwise.
     */
    bool createDesktopShortcut(const QString &pathToEXE, const QString &shortcutName);

    /**
     * @brief fillCorrectiveActionsForUpdateProcess - Adds all the corrective actions that comprise an update process.
     */

    void fillCorrectiveActionsForUpdateProcess();

    /**
     * @brief findCommonPath - fidns the largetst path in common between the two directories.
     * @param dir1 - The first dir
     * @param dir2 - The second dir
     * @return Returning the common path between the two dirs.
     */
    QString findCommonPath(const QString &dir1, const QString &dir2);

};

#endif // MAINTENANCEMANAGER_H
