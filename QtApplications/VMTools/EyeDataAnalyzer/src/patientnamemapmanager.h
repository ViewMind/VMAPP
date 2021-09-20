#ifndef PATIENTNAMEMAPMANAGER_H
#define PATIENTNAMEMAPMANAGER_H

#include <QDataStream>
#include <QHash>
#include <QJsonDocument>
#include <QJsonObject>
#include "../../../CommonClasses/common.h"
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/VariantMapSerializer/variantmapserializer.h"
#include "../../../CommonClasses/SQLConn/dbdescription.h"

#define FILENAME_DBDATA_MAP  "00drpatdata.dat"
#define FILENAME_PATUID_MAP  "00uidpuidmap.dat"
#define FILENAME_VMID_MAP    "00vmidmap.dat"
#define FILENAME_MED_RECS    "00medrecs.dat"
#define FILENAME_PATDATA     "00patdata.dat"

#define UNIFIED_CSV_ID_DID    0
#define UNIFIED_CSV_ID_DBPUID 1
#define UNIFIED_CSV_ID_HPUID  2
#define UNIFIED_CSV_ID_PUID   3
#define UNIFIED_CSV_ID_NAME   4

#define ID_PUID                      "id_puid"
#define ID_HPUID                     "id_hpuid"
#define ID_DID                       "id_did"
#define ID_DBUID                     "id_dbuid"
#define ID_NAME                      "id_name"

class PatientNameMapManager
{
public:
    PatientNameMapManager();

    void loadPatNameDB();
    void setInstitutions(const QStringList &uids, const QStringList &names);

    QString fromSerializedMapData(const QString &data);
    QString addSerializedIDMap(const QString &data);
    QString addVMIDTableData(const QString &table, const QString &instuid);
    QString addMedicalRecords(const QString &json);

    QVariantMap getClosestMedicalRecord(const QString &dbpuid, const qint64 &studyDateInJulianDays, qint32 maxDayDiff);
    QVariantMap getPatientDatabaseRecord(const QString &dbpuid);

    //bool isPuidInNameMap(const QString &puid) const;
    //QString getPatName(const QString &puid) const;

    ConfigurationManager getPatientNameFromDirname(const QString &dirname) const;
    ConfigurationManager getPatientIDInfoFromDBPUID(const QString &dbpuid) const;
    QHash<QString,QString> getInstHash() const { return instituions; }
    QStringList getDBPUIDForInst(const QString &instuid);

    //QString getDisplayIDFromDirname(const QString &dirname, qint32 whichID);
    void printMap() const;

private:
    QVariantMap dbdata;
    QVariantMap dbiddata;
    QVariantMap dbvmid;
    QVariantMap dbpatdata;
    QHash<QString,QString> instituions;
    QVariantMap dbmedrecs;
};

#endif // PATIENTNAMEMAPMANAGER_H
