#ifndef MAINTENANCEMANAGER_H
#define MAINTENANCEMANAGER_H

#include <QThread>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QFile>
#include <QProcess>

#include "../../CommonClasses/DirTools/dirrunner.h"
#include "../../CommonClasses/DirTools/dircompare.h"
#include "messagelogger.h"
#include "debugoptions.h"
#include "defines.h"
#include "langs.h"


class MaintenanceManager : public QThread
{
    Q_OBJECT
public:

    typedef enum {ACTION_UPDATE,
                  ACTION_DIAGNOSE,
                  ACTION_NONE
    } Action;

    MaintenanceManager();

    void setAction(Action action);

    void run();

    Action getAction() const;
    Action getRecommendedAction() const;

    bool wasLastActionASuccess() const;

    QString getEyeExpWorkDir() const;

signals:

    void message(qint32 type, QString message);
    void progressUpdate(qreal percent, QString message);

private slots:
    void onDirCompareProgress(double p, QString filename);

private:

    Action currentAction;
    Action recommendedAction;
    bool actionSuccessfull;    
    QString eyeExpWorkDir;

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
    void getRecommendedActionFromDiagnosisResults(const QString &eeInstallDirPath);


    bool uncompressAppPackage(const QString &destination, QString *output, QString *errorString);
};

#endif // MAINTENANCEMANAGER_H
