#ifndef LOCALINFORMATIONMANAGER_H
#define LOCALINFORMATIONMANAGER_H

#include <QVariantMap>
#include <QDataStream>
#include <QCryptographicHash>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "../../CommonClasses/DatFileInfo/datfileinfoindir.h"
#include "../../CommonClasses/VariantMapSerializer/variantmapserializer.h"

#define   DR_ID_LENGTH                                   4
#define   PAT_ID_LENGTH                                  4
#define   EVAL_ID_LENGTH                                 10

class LocalInformationManager
{
public:

    struct DisplayLists {
        QStringList patientNames;
        QStringList patientUIDs;
        QStringList patientISOKList;
        QStringList doctorNames;
        QStringList doctorUIDs;
        QStringList creatorNames;
        QStringList creatorUIDs;
        QStringList patientDisplayIDs;
        void clear() { patientNames.clear(); patientISOKList.clear(); patientUIDs.clear(); doctorNames.clear();
                       doctorUIDs.clear(); creatorNames.clear(); creatorUIDs.clear(); patientDisplayIDs.clear();}
    };

    LocalInformationManager();
    void resetMedicalInstitutionForAllDoctors(const QString &inst_uid);
    void setDirectory(const QString &workDir, const QString &eyeexpid, const QString &instUID);
    void setWorkingFile(const QString &fileName);
    void setViewAllFlag(bool flagValue);
    bool getViewAllFlag() const;
    void enableBackups(const QString &backupDir);
    void addDoctorData(const QString &druid, const QStringList &cols, const QStringList &values, const QString &password, bool hidden);
    void addPatientData(const QString &patient_uid, const QString &creator_uid, const QStringList &cols, const QStringList &values);
    bool isDoctorValid(const QString &dr_uid);
    bool doesDoctorExist(const QString &uid) const;
    bool doesPatientExist(const QString &patuid) const;
    void validateDoctor(const QString &dr_uid);
    QString newDoctorID();
    QString newPatientID();
    QString newEvaluationID();
    QString getDoctorPassword(const QString &uid);
    DisplayLists getPatientListForDoctor(const QString &druid, const QString &filter = "");
    DisplayLists getDoctorList(bool forceShow = false);
    QString getFieldForPatient(const QString &patuid, const QString &field) const;
    QVariantMap getDoctorInfo(const QString &uid) const;
    QVariantMap getPatientInfo(const QString &patuid) const;
    QString getWorkDirectory() const {return workingDirectory;}
    void saveIDTable(const QString &fileName, const QStringList &tableHeaders);
    qint32 getRemainingEvals() const;
    void setRemainingEvals(qint32 remevals);

    QHash<QString,QString> getPatientHashedIDMap() const;
    QVariantMap getHashedIDPatientMap(QVariantMap hidmap) const;

    // Interface with the protocol list;
    bool addProtocol(const QString & protocol);
    void deleteProtocol(const QString &protocol);
    QStringList getProtocolList(bool full) const;


    // Used ONLY in the LocalDBMng program
    bool deleteDoctor(const QString &uid);
    void setDoctorData(const QString &uid, const QStringList &keys, const QVariantList &values);

    // Synch function. Returns false if the there is nothing to synch. (No changes to Doctor and Patient data).
    bool createPatAndDrDBFiles(const QString &patid, const QString &drid);

    // Interface with Dat Info Dir
    QStringList getFileListForPatient(const QString &patuid, qint32 whichList) const;
    QStringList getBindingUCFileListCompatibleWithSelectedBC(const QString &patuid, qint32 selectedBC);
    QStringList getReportNameAndFileSet(const QString &patuid, const DatFileInfoInDir::ReportGenerationStruct &repgen);
    QStringList getReportNameAndFileSet(const QString &patuid, const QStringList &fileList);
    QString getDatFileFromIndex(const QString &patuid, qint32 index, qint32 whichList) const;
    void fillPatientDatInformation(const QString &patient);

    // Used for the debugging.
    void printDBToConsole();
    QString printDBToString() const;

    // Used for trasmiting the DB over the net as a string. Just the doctor and patient data
    QString serialDoctorPatientString(const QString &serialized_map) const;

private:

    static const QString PATIENT_CREATOR;
    static const QString DOCTOR_DATA;
    static const QString DOCTOR_COUNTER;
    static const QString EVALUATION_COUNTER;
    static const QString PATIENT_COUNTER;
    static const QString PATIENT_DATA;
    static const QString DOCTOR_UPDATE;
    static const QString PATIENT_UPDATE;
    static const QString DOCTOR_PASSWORD;
    static const QString DOCTOR_VALID;
    static const QString DOCTOR_HIDDEN;
    static const QString FLAG_VIEWALL;
    static const QString PROTOCOLS;
    static const QString PROTOCOL_VALID;
    static const QString REMAING_EVALS;
    static const qint32  LOCAL_DB_VERSION = 6;

    // Working directory.
    QString workingDirectory;
    QString backupDirectory;

    //ConfigurationManager *config;
    LogInterface log;
    QVariantMap localDB;

    // Patient Dir information. First hash is for the patient, second is for the doctor (in case two doctors added the patient twice).
    QHash<QString, DatFileInfoInDir > patientReportInformation;

    void backupDB();
    void loadDB(QString eyeexpid, QString instUID, QString fileName = FILE_LOCAL_DB);
    bool isHidden(const QString &uid);

    QString recursiveToSerialString(QString acc, qint32 hierarchy);


};
#endif // LOCALINFORMATIONMANAGER_H
