#ifndef PATIENTNAMEMAPMANAGER_H
#define PATIENTNAMEMAPMANAGER_H

#include <QDataStream>
#include <QHash>
#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

#define FILENAME_PATUID_MAP  "patnameuidmap.dat"

class PatientNameMapManager
{
public:
    PatientNameMapManager();

    void loadPatNameDB();
    QString fromSerializedMapData(const QString &data);
    bool isPuidInNameMap(const QString &puid) const;
    QString getPatName(const QString &puid) const;
    ConfigurationManager getPatientNameFromDirname(const QString &dirname) const;
    void printMap() const;

private:
    QVariantMap mapdata;
};

#endif // PATIENTNAMEMAPMANAGER_H
