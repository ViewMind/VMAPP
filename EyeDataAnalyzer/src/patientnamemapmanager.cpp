#include "patientnamemapmanager.h"

PatientNameMapManager::PatientNameMapManager()
{
}

void PatientNameMapManager::loadPatNameDB(){
    QFile file(FILENAME_PATUID_MAP);
    if (!file.open(QFile::ReadOnly)) return;
    QDataStream reader(&file);
    reader >> mapdata;
    file.close();
}

QString PatientNameMapManager::fromSerializedMapData(const QString &data){
    QStringList dataSets = data.split(SEP_VALUE_SET);
    for (qint32 i = 0; i < dataSets.size(); i++){
        QStringList fields = dataSets.at(i).split(SEP_FIELDS); // Separating into fields. The firs is just the map name.
        if (fields.size() < 2) return "Found data set with less than 2 fields: " + dataSets.at(i);
        QString id = fields.first();
        QVariantMap m;
        for (qint32 j = 1; j < fields.size(); j++){
            QStringList keyvaluepair = fields.at(j).split(SEP_KEYVALUE);
            if (keyvaluepair.size() != 2) return "Unexpected key value pair field: " + fields.at(j);
            m[keyvaluepair.first()] = keyvaluepair.last();
        }
        mapdata[id] = m;
    }

    QFile file(FILENAME_PATUID_MAP);
    if (!file.open(QFile::WriteOnly)) return "Could not open output file for writing";

    QDataStream reader(&file);
    reader << mapdata;
    file.close();

    return "";
}

ConfigurationManager PatientNameMapManager::getPatientNameFromDirname(const QString &dirname) const{
    ConfigurationManager config;

    QString puid;
    QStringList parts = dirname.split("/",QString::SkipEmptyParts);

    if (parts.size() >= 2) puid = parts.at(parts.size()-2);
    else return config;

    // This will get rid of the extra zeros that the dirname might have.
    qint32 puidnum = puid.toInt();
    puid = QString::number(puidnum);

    if (!patNames.contains(puid)) return config;
    config.addKeyValuePair(CONFIG_PATIENT_NAME,patNames.value(puid));
    return config;
}

void PatientNameMapManager::printMap() const{
    QStringList keys = patNames.keys();
    for (qint32 i = 0; i < keys.size(); i++){
        qDebug() << keys.at(i) << ":" << patNames.value(keys.at(i));
    }
}
