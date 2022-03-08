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
#include "../../../CommonClasses/debug.h"
#include "../../../CommonClasses/RawDataContainer/VMDC.h"


namespace APINames {

   static const char * MAIN_DATA = "data";

   namespace ProcParams {
      static const char * NAME = "proc_params";
   }

   namespace FreqParams{
      static const char * NAME = "freq_params";
   }

   namespace RecoveryPassword {
      static const char * NAME = "recovery_password";
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
    static const char * MAIN_DB_VERSION;
    static const char * MAIN_RECOVERY_PASSWORD;

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

    // Marker for each subject
    static const char * MARKER_VALUE;
    static const char * MARKER_TIME;

    // Protocol Fiedls
    static const char * PROTOCOL_NAME;
    static const char * PROTOCOL_CREATION_DATE;
    static const char * PROTOCOL_ID;

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

    // List the emails of all evaluators.
    QStringList getUsernameEmails() const;

    // Get the QC Parameters as they came in from the server
    QVariantMap getQCParameters() const;

    // Adds a "bookmark" for multi part studies. The bookmark has two parts a value and a time stamp. If the bookmark exists it is simply overwritten and the time stamp updated.
    bool addStudyMarkerForSubject(const QString &suid, const QString &study, const QString &value);

    // Retrieves the marker for a study in two parte the value and the ISO timestamp in forma YYYY-MM-dd HH:mm
    QVariant getMarkerForStudy(const QString &suid, const QString &study) const;

    // Deletes the study bookmark.
    bool removeMarkerForSubject(const QString &suid, const QString &study);

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

    // Sets the recovery password from the operating information.
    bool setRecoveryPasswordFromServerResponse(const QVariantMap &response);

    // Retrieves the processing parameters.
    QVariantMap getProcessingParameters() const;

    // Get the protocol list
    QVariantMap getProtocolList() const;

    // Error string. For logging purposes.
    QString getError() const;

    // Sets the current version. If it exists and it's different it returns true. Otherwise it returns false. Allows simple check to know when a version changed.
    // Also if version change (or was created) update postpone counter is reset.
    bool setApplicationVersion(const QString &version);

    // Decreases the number of times remaining an update can be delayed.
    void deniedUpdate();

    // Returns the number of times remaining that the user can say NO to an update. 0 if it doesn't exist.
    qint32 getRemainingUpdateDelays() const;

    // Returns tht number of patients on the DB.
    qint32 getSubjectCount() const;


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

    // The contents of this function will change every time the DB changes versions as it might require modification of existing data.
    // It needs to be called upon successfull loading of DB;
    void updatesToPreviousDBVersions();

};

#endif // LOCALDB_H
