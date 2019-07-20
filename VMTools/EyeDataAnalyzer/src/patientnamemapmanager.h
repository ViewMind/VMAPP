#ifndef PATIENTNAMEMAPMANAGER_H
#define PATIENTNAMEMAPMANAGER_H

#include <QDataStream>
#include <QHash>
#include "../../../CommonClasses/common.h"
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/VariantMapSerializer/variantmapserializer.h"
#include "../../../CommonClasses/SQLConn/dbdescription.h"

#define FILENAME_DBDATA_MAP  "00drpatdata.dat"
#define FILENAME_PATUID_MAP  "00uidpuidmap.dat"

#define UNIFIED_CSV_ID_DISPLAY_ID    0
#define UNIFIED_CSV_ID_NAME_LASTNAME 1

class PatientNameMapManager
{
public:
    PatientNameMapManager();

    void loadPatNameDB();

    QString fromSerializedMapData(const QString &data);
    QString addSerializedIDMap(const QString &data);

    //bool isPuidInNameMap(const QString &puid) const;
    //QString getPatName(const QString &puid) const;

    ConfigurationManager getPatientNameFromDirname(const QString &dirname) const;
    //QString getDisplayIDFromDirname(const QString &dirname, qint32 whichID);
    void printMap() const;

private:
    QVariantMap dbdata;
    QVariantMap dbpuid;
};

#endif // PATIENTNAMEMAPMANAGER_H
