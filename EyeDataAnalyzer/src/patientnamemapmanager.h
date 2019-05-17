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
    bool isPuidInNameMap(const QString &puid) const { return patNames.contains(puid); }
    QString getPatName(const QString &puid) const {return patNames.value(puid);}
    ConfigurationManager getPatientNameFromDirname(const QString &dirname) const;
    void addToMap(const QStringList &puids, const QStringList &patnames);
    void printMap() const;

private:
    QHash<QString,QString> patNames;
};

#endif // PATIENTNAMEMAPMANAGER_H
