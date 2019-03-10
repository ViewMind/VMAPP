#ifndef LOCALINFORMATIONMANAGER_H
#define LOCALINFORMATIONMANAGER_H

#define USESSL

#include <QVariantMap>
#include <QDataStream>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "../../CommonClasses/DatFileInfo/datfileinfoindir.h"

#ifdef USESSL
#include "sslclient/ssldbclient.h"
#endif

#define   LOCAL_DB                                      "localdb.dat"

class LocalInformationManager
{
public:

    LocalInformationManager();
    void resetMedicalInstitutionForAllDoctors(const QString &inst_uid);
    void setDirectory(const QString &workDir);
    void enableBackups(const QString &backupDir);
    void addDoctorData(const QString &dr_uid, const QStringList &cols, const QStringList &values, const QString &password, bool hidden);
    void addPatientData(const QString &druid, const QString &patient_uid, const QStringList &cols, const QStringList &values);
    bool isDoctorValid(const QString &dr_uid);
    bool doesDoctorExist(const QString &uid) const;
    bool doesPatientExist(const QString &druid, const QString &patuid) const;
    void validateDoctor(const QString &dr_uid);
    QString getDoctorPassword(const QString &uid);
    QList<QStringList> getPatientListForDoctor(const QString &druid, const QString &filter = "");
    QList<QStringList> getDoctorList(bool forceShow = false);
    QString getFieldForPatient(const QString &druid, const QString &patuid, const QString &field) const;
    QVariantMap getDoctorInfo(const QString &uid) {return localDB.value(uid).toMap();}
    QVariantMap getPatientInfo(const QString &druid, const QString &patuid) const;
    QList<QStringList> getAllPatientInfo(const QString &filter = "") const;
    QString getWorkDirectory() const {return workingDirectory;}
    void setUpdateFlagTo(bool flag);
    // Used ONLY in the LocalDBMng program
    void deleteDoctor(const QString &uid);
    void setDoctorData(const QString &uid, const QStringList &keys, const QVariantList &values);
    // FOR DEBUGGING ONLY
    void printLocalDB();

    // Synch function. Returns false if the there is nothing to synch. (No changes to Doctor and Patient data).
#ifdef USESSL
    bool setupDBSynch(SSLDBClient *client);
#endif

    // Interface with Dat Info Dir
    QStringList getFileListForPatient(const QString &patuid, qint32 whichList) const;
    QStringList getBindingUCFileListCompatibleWithSelectedBC(const QString &patuid, qint32 selectedBC);
    QStringList getReportNameAndFileSet(const QString &patuid, const DatFileInfoInDir::ReportGenerationStruct &repgen);
    QStringList getReportNameAndFileSet(const QString &patuid, const QStringList &fileList);
    QString getDatFileFromIndex(const QString &patuid, qint32 index, qint32 whichList) const;
    void fillPatientDatInformation(const QString &druid);

private:

    static const QString PATIENT_DATA;
    static const QString DOCTOR_UPDATE;
    static const QString PATIENT_UPDATE;
    static const QString DOCTOR_PASSWORD;
    static const QString DOCTOR_VALID;
    static const QString DOCTOR_HIDDEN;
    static const qint32  LOCAL_DB_VERSION = 2;

    // Working directory.
    QString workingDirectory;
    QString backupDirectory;

    //ConfigurationManager *config;
    LogInterface log;
    QVariantMap localDB;

    // Used to iterate over unprocessed information.
    QHash<QString, DatFileInfoInDir> patientReportInformation;    

    void backupDB();
    void loadDB();
    bool isHidden(const QString &uid);


};
#endif // LOCALINFORMATIONMANAGER_H
