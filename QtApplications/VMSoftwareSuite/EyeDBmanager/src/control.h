#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QCoreApplication>
#include <QProcess>
#include <iostream>
#include <QElapsedTimer>

#include "../../../CommonClasses/common.h"
#include "../../../CommonClasses/server_defines.h"
#include "../../../CommonClasses/SQLConn/dbinterface.h"
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"

#define EYE_DB_MNG_NAME      "EyeDBManager"
#define EYE_DB_MNG_VERSION   "1.2.1"
#define FILE_DB_LOG          "dblog.log"

#define DB_FINISH_ACTION_COMMIT   0
#define DB_FINISH_ACTION_ROLLBACK 1
#define DB_FINISH_ACTION_CLOSE    2

#define  S3_BASE_COMMAND     "aws s3 cp"
#define  S3_PARMETERS        "--quiet"
#define  S3_BUCKET           ""
#define  TIME_FORMAT_STRING  "yyyy_MM_dd_hh_mm_ss"

/****************************************************************************
 * Version  1.2.1 03/03/2021
 * -> Removal of dependency of SERVER_PRODUCTION/SERVER_LOCALHOST Macro.
 * Version  1.2.0 25/12/2020
 * -> DB configuration is loaded from a local file 
 * -> Changed way of executing AWS commands so that it's possible to know if it failed. 
 *****************************************************************************/

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);

    // Starts the application functionality.
    void run();

signals:
    // Singal used ot indicate that the processing is done.
    void done();

private:

    ConfigurationManager configuration;
    LogInterface log;

    QString workingDirectory;
    QString commFile;

    // All necessary steps to properly quit
    void exitProgram(qint32 code);

    // The database interfaces
    DBInterface dbConnBase;
    DBInterface dbConnID;
    DBInterface dbConnPatData;
    DBInterface dbConnDash;

    // For saving to the output
    qint32 numberOfRemaingEvaluations;

    // The actual function that take actions in the program
    void checkMode();
    void storeMode(const QString &action);
    void patDataStoreMode(const QString &puid, const QString &instUID);

    // Helper fucntions
    void storeDoctorData();
    void storePatientData();

    // Common to al DB related stuff at the end.
    void finishUp(quint8 commitBase, quint8 commitID, quint8 commitPatData, quint8 commitDash, qint32 code);

};

#endif // CONTROL_H
