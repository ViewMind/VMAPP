#ifndef LOCALDB_H
#define LOCALDB_H

#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include "../../CommonClasses/debug.h"
#include "../../CommonClasses/RawDataContainer/VMDC.h"
#include "fuzzystringcompare.h"


namespace APINames {

   static const char * MAIN_DATA      = "data";
   static const char * MAIN_HTTP_CODE = "http_code";
   static const char * MAIN_MESSAGE   = "message";

   namespace ReturnDataFields {
      static const char * PROC_PARAMS     = "proc_params";
      static const char * INST_COUNTRY    = "institution_country";
      static const char * REC_PASSWD      = "recovery_password";
      static const char * FREQ_PARAMS     = "freq_params";
      static const char * UPDATE_DL_URL   = "url";
      static const char * HMD_CHANGE_SN   = "hmd_change_sn";
      static const char * AVAILABLE_EVALS = "available_evaluations";
   }

   namespace UpdateParams{
      static const char * UPDATE_ET_CHANGE = "update_et_change";
      static const char * UPDATE_VERSION   = "update_version";
   }

   namespace Medics {
      static const char * NAME     = "medics";
      static const char * FNAME    = "name";
      static const char * LASTNAME = "lastname";
      static const char * ROLE     = "user_role";
      static const char * KEYID    = "keyid";
      static const char * EMAIL    = "email";
   }

}

namespace EvaluationStatus {

   static const char * ONGOING      = "ongoing";
   static const char * BAD_QCI      = "badqci";
   static const char * READY_UPLOAD = "ready_upload";

}


class LocalDB
{
public:

    // Main fields.
    static const char * MAIN_EVALUATOR_DATA;
    static const char * MAIN_SUBJECT_DATA;
    static const char * MAIN_PROTOCOL;
    static const char * MAIN_CHECKSUM;
    static const char * MAIN_MEDICS;
    static const char * MAIN_PROCESSING_PARAMETERS;
    static const char * MAIN_QC_PARAMETERS;
    static const char * MAIN_APP_VERSION;
    static const char * MAIN_APP_UPDATE_DELAY_COUNTER;
    static const char * MAIN_LAST_APP_DOWNLOADED;
    static const char * MAIN_DB_VERSION;
    static const char * MAIN_RECOVERY_PASSWORD;
    static const char * MAIN_LAST_LOG_UPLOAD;
    static const char * MAIN_PREFERENCES;
    static const char * MAIN_INSTANCE_ENABLED;
    static const char * MAIN_HMD_CHANGE;
    static const char * MAIN_AVAILABLE_EVALS;
    static const char * MAIN_ONGOING_EVALUATIONS;

    // Evaluator fields
    static const char * APPUSER_NAME;
    static const char * APPUSER_LASTNAME;
    static const char * APPUSER_PASSWORD;
    static const char * APPUSER_EMAIL;        // <- This will be used as the key for the evaluator
    static const char * APPUSER_VIEWMIND_ID;  // <- This will be used as the key for the medic.

    // Subject Fields
    static const char * SUBJECT_NAME;
    static const char * SUBJECT_LASTNAME;
    static const char * SUBJECT_INSTITUTION_ID;
    static const char * SUBJECT_BIRTHDATE;
    static const char * SUBJECT_BIRTHCOUNTRY;
    static const char * SUBJECT_YEARS_FORMATION;
    static const char * SUBJECT_CREATION_DATE;
    static const char * SUBJECT_CREATION_DATE_INDEX;
    static const char * SUBJECT_GENDER;
    static const char * SUBJECT_STUDY_MARKERS;
    static const char * SUBJECT_LOCAL_ID;  // <- This is the local key as well.
    static const char * SUBJECT_ASSIGNED_MEDIC;
    static const char * SUBJECT_BDATE_DISPLAY;
    static const char * SUBJECT_SORTABLE_NAME;
    static const char * SUBJECT_EMAIL;
    static const char * SUBJECT_FIELD_IN_SERVER_RESPONSE;
    static const char * SUBJECT_MODIFIED_FLAG;
    static const char * SUBJECT_UPDATED_IDS;

    // Protocol Fiedls
    static const char * PROTOCOL_NAME;
    static const char * PROTOCOL_CREATION_DATE;
    static const char * PROTOCOL_ID;

    // Settings field
    static const char * PREF_UI_LANG;
    static const char * PREF_EXP_LANG;
    static const char * PREF_LAST_SEL_PROTOCOL;

    // On Going Evaluations Field
    static const char * EVAL_ID;
    static const char * EVAL_STATUS;
    static const char * EVAL_CLINICIAN;
    static const char * EVAL_EVALUATOR;
    static const char * EVAL_ISO_DATE;
    static const char * EVAL_TIME;
    static const char * EVAL_TYPE;
    static const char * EVAL_TIMESTAMP;
    static const char * EVAL_TASKS;
    static const char * EVAL_SUBJECT;
    static const char * EVAL_COMMENT;
    static const char * EVAL_PROTOCOL;

    // Tasks subfields for ongoing evaluations
    static const char * TASK_TYPE;
    static const char * TASK_FILE;
    static const char * TASK_QCI;
    static const char * TASK_QCI_OK;
    static const char * TASK_UPLOADED;
    static const char * TASK_DISCARDED;


    // This is left for Legacy reasons. It's not really used.
    static const qint32 LOCALDB_VERSION = 3;

    // Constructor
    LocalDB();

    // Sets the working file for the local DB and the backup directory, checking that they exists and loading the data if nencessary.
    bool setDBFile(const QString &dbfile, const QString &bkp_dir, bool pretty_print_db = false, bool disable_checksum = false);

    // This replaces old keys used in the medic map to newer keys. Should basically be DEPRACATED in the next release version.
    // Old medic keys are numbers, new ones are their email.
    void replaceMedicKeys();

    // Compares the value, if present of the DB version to the fixed current value to determine this. This will be false until saved for the very first time.
    bool isVersionUpToDate() const;

    // Add or modify a subject. Modification ocurrs if the subject ID exists.
    bool addOrModifySubject(const QString &subject_id, QVariantMap subject_data);

    // Whenver data for a subject is uploaded, their modifed flag should be set to false.
    bool markSubjectsAsUpdated(const QVariantList &subject_ids);

    // This function is specifically for changing the assigned medic for a given the subject. The subject MUST exist.
    bool modifyAssignedMedicToSubject(const QString &subject_id, const QString &medic);

    // Add or modify an evaluator. Modification ocurrs if the subject ID exists. Passwords are set in a separate function.
    // Two email entries are provided because changing the email means changing the username adn the old one cannot be left in the system.
    bool addOrModifyEvaluator(const QString &email, const QString &oldemail, QVariantMap evaluator_data);

    // Sets the passowrd for the evaluator if it exists and returns true. Otherwise returns false.
    bool setPasswordForEvaluator(const QString &email, const QString &plaintext_password);

    // If the evaluator exists and stored password hash matches hashed parameter, then it returns true. Otherwise it returns false.
    bool passwordCheck(const QString &email, const QString &plaintext_password);

    // Checks if an evaluator username exists.
    bool checkEvaluatorExists(const QString &evaluator) const;

    // Returns true if a log upload is necessary. False otherwise.
    bool checkForLogUpload();

    // Sets the last log upload date.
    void setLogUploadMark();

    // Returns, if it exists, the value of a particular field for a given subject.
    QString getSubjectFieldValue(const QString &subject_id, const QString &field) const;

    // Returns the complte map data for a given evaluator.
    QVariantMap getEvaluatorData(const QString &evaluator) const;

    // Returns, if exists, the map with all of the subject data.
    QVariantMap getSubjectData(const QString &subjectID) const;

    // Searches the list of subjects for one with a matching internalID and retuns the information.
    QVariantMap getSubjectDataByInternalID(const QString &internalID) const;

    // Returns a list with display data of all subjects that match the filter.
    QVariantMap getDisplaySubjectList(QString filter, const QStringList &months);

    // Returns a list of doctors to display
    QVariantMap getMedicDisplayList() const;

    // Gets full medic data map.
    QVariantMap getMedicData(const QString &key) const;

    // Generates the map to update the data in the server in the next operating information call.
    QVariantMap getSubjectDataToUpdate() const;

    // List the emails of all evaluators. If withname is true then the string list returns Name - email.
    QStringList getUsernameEmails(bool withname = false) const;

    // Get the QC Parameters as they came in from the server
    QVariantMap getQCParameters() const;

    // Adds a protocol to the protocol list. Returs true if successfull or false if it exists.
    bool addProtocol(const QString &protocol_name, const QString &protocol_id, bool edit);

    // If it is exits, the protocol is removed from the list.
    bool removeProtocol(const QString &protocol_id);

    // Checking if processing parameters have been set.
    bool processingParametersPresent() const;

    // Set the processing parameters from the server side response.
    bool setProcessingParametersFromServerResponse(const QVariantMap &response);

    // Set the qc parameters from the server side response.
    bool setQCParametersFromServerResponse(const QVariantMap &response);

    // Set the medical info from the database.
    bool setMedicInformationFromRemote(const QVariantMap &response);

    // Merges the patient data with the current data in the DB. Returns the number of subjects added.
    qint32 mergePatientDBFromRemote(const QVariantMap &response);

    // Sets the recovery password from the operating information.
    bool setRecoveryPasswordFromServerResponse(const QVariantMap &response);

    // Retrieves the processing parameters.
    QVariantMap getProcessingParameters() const;

    // Get the protocol list
    QVariantMap getProtocolList() const;

    // Error string. For logging purposes.
    QString getError() const;

    // The version of the app that was last downloaded.
    QString getLastDownloadedApp() const;

    // Sets the current version. If it exists and it's different it returns true. Otherwise it returns false. Allows simple check to know when a version changed.
    // Also if version change (or was created) update postpone counter is reset.
    bool setApplicationVersion(const QString &version);

    // Decreases the number of times remaining an update can be delayed.
    void deniedUpdate();

    // Returns the number of times remaining that the user can say NO to an update. 0 if it doesn't exist.
    qint32 getRemainingUpdateDelays() const;

    // Returns tht number of patients on the DB.
    qint32 getSubjectCount() const;

    // Sets a preferences in the DB.
    bool setPreference(const QString &preference, const QVariant &variant);

    // Sets the last downloaded app.
    bool setLastDownloadedApp(const QString &version);

    // Sets/Gets the HMD Change SN
    QString getHMDChangeSN() const;
    bool setHMDChangeSN(const QString &sn);

    // Returns a preference that was set, as a string. As of the writing of this, we only need strings.
    // The second parameter can be used to return a store and return a value if the required one doesn't exist
    QVariant getPreference(const QString &preference, const QString &retAndStoreIfDoenstExist = "");

    // Does a fuzzy search for the name lastname and year of birth and returns possible matches.
    QVariantList possibleNewPatientMatches(QString name, QString lastname, QString personalID, QString iso_birthdate, const QStringList &months) const;

    // Set and get the available evaluations.
    void setAvailableEvaluations(const QVariantMap &available);
    QVariantMap getAvailableEvals() const;

    // Set and get the instance enabled status.
    void setInstanceEnableTo(bool enabled);
    bool isInstanceEnabled() const;


    ///////////////////////////// OnGoing Evaluation Functions //////////////////////////////

    // Creates a new evaluations for the specified subject of the specified type.
    // The instance prefix is formed using the isntance and institution number using an underscore to separate them.
    // The resulting unique identifier is returned.
    QString createNewEvaluation(const QString &subjectID,
                                const QString &clinician,
                                const QString &evaluator,
                                const QString &protocol,
                                const QString &instance_prefix,
                                const QString &evalType);

    // This will return a list of object with two fields type and file comprised of all tasks with a -1 qci for the evaluation.
    QVariantList getRemainingTasksForEvaluation(const QString &evaluationID);

    // Returns the full evaluation struct.
    QVariantMap getEvaluation(const QString &evaluationID) const;

    // This will update the task inside the selected evaluation. If the qci_ok is false the evaluation status will change
    // Also this will automatically generate a new file for the missing task.
    // If request Redo is true then a new task is created regardless of the qci ok flag and the qci and qci_ok are NOT updated.
    bool updateEvaluation(const QString &evaluationID, const QString &taskFileName, qreal qci, bool qci_ok, bool requestRedo);

    // The evaluation display map is a structure especially created to fill the fields of task break down dialog, when
    // reviewing an evaluation in the front end.
    // The override time is required because the one of the return values is flag used to indicate if REDO is available.
    // REDO is NOT availabe if the study is too old or if ANY tasks have been uploaded.
    QVariantMap   getEvaluationDisplayMap(const QString &evaluationID, const QStringList &months, const QString &ui_lang_code, qlonglong override_time);

    // Returns a list of objects. Each object belongs to an ongoing evaluation with each field
    // representing an evaluation that meets the following criteria:
    // 1) It belongs to the specified evaluator
    // 2) It's a finished evaluation or  It's an ongoing evaluation less than 24 hrs old.
    // Some display texts are required. Language texts provides them for the currently selected ui language.
    QList<QVariantMap> getEvaluationTableInformation(const QString &evaluator, qlonglong override_timeout, const QStringList &months, QStringList languangeTexts, const QString &ui_lang_code);

    // Changes the field uploded of an evaluation to true. When all evaluations have been changed to true, the the task is removed from the DB.
    bool updateTaskInEvaluationAsUploaded(const QString &evalID, const QString &taskID);

    // Sets the comment field for the evaluation.
    void setEvaluationComment(const QString &evalID, const QString &comment);

    // Gets the expired evaluation's IDs. Will clean up any tasks from expired evals so as to prevent trying to upload non existant tasks.
    // The base work dir is the absolute path for viewmind etdata directory.
    QStringList getExpiredEvaluationID(qlonglong override_timeout, const QString &baseWorkDir);

private:

    QVariantMap data;
    QString error;
    QString dbfile;
    QString dbBKPdir;
    bool enablePrettyPrint;

    // Saves DB if there's been a change and inmediatly constructs a backup.
    bool saveAndBackup();

    // Function to see if a subject map matches a filter string.
    bool filterMatchSubject(const QVariantMap &subject, const QString &filter);

    // Used to verify data integrity.
    QString computeDataHash();

    // The maximum number of times an update can be delayed before the user is forced to update.
    const qint32 MAX_ALLOWED_UPDATE_DELAYS = 3;

    // The log upload frequency, expressed in milisecons.
    const qint64 LOG_UPLOAD_FREQ_IN_MS = 2*24*60*60*1000; // 2 Days expressed in ms.

    // The thresholds for considering a match when searching for names recently entered.
    const qreal FNAME_FUZZY_MATCH_THRESHOLD = -0.1; // So that a zero will match withouth having to add the equal to the comparison
    const qreal LNAME_FUZZY_MATCH_THRESHOLD = 0.3;

    const qint64 EVALUATION_DISCARD_TIME = 24*60*60; // 1 Day in seconds.

    // The contents of this function will change every time the DB changes versions as it might require modification of existing data.
    // It needs to be called upon successfull loading of DB;
    void updatesToPreviousDBVersions();

    // Transforms an ISO date to Day 3LetterMonth Year. It's univeral for display.
    QString buildDisplayBirthDate(const QString &iso_bdate, const QStringList &months) const;

    // Creating a new task object for evaluation's task list. Basically just inits the map. The indexing file name is returned in the second paratemer.
    // The counter value is required to know how many tasks were done before the current one.This allows the counter to be part of the name and hence
    // the order in which the tasks are carried out, even in a map, to be predictable.
    QVariantMap createNewTaskObjectForEvaluationTaskList(const QString &evaluationID, const QString &task, qint32 counter, QString *tarTaskFileName);

    // Algorithm to put together a text that represents a subject taking into accout if any or all of it's id values are empty.
    QString createSubjectTableDisplayText(const QString &subject_id);

    // Clean up tasks for non existent files. Should only be used in task lists of expired evaluations.
    QVariantMap cleanUpTasksForEval(QVariantMap tasks, const QString &baseDir);


};

#endif // LOCALDB_H
