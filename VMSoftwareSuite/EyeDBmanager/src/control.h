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
#define EYE_DB_MNG_VERSION   "1.0.0"
#define FILE_DB_LOG          "dblog.log"

#define DB_FINISH_ACTION_COMMIT   0
#define DB_FINISH_ACTION_ROLLBACK 1
#define DB_FINISH_ACTION_CLOSE    2

#define  S3_BASE_COMMAND     "aws s3 cp"
#define  S3_PARMETERS        "--quiet"
#define  S3_BUCKET           ""
#define  TIME_FORMAT_STRING  "yyyy_MM_dd_hh_mm_ss"

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

    // For saving to the output
    qint32 numberOfRemaingEvaluations;

    // The actual function that take actions in the program
    void checkMode();
    void storeMode();

    // Common to al DB related stuff at the end.
    void finishUp(quint8 commitBase, quint8 commitID, quint8 commitPatData, qint32 code);

};

#endif // CONTROL_H
