#include "variantmapserializer.h"

VariantMapSerializer::VariantMapSerializer()
{

}

QString VariantMapSerializer::serializeOneLevelVariantMap(const QString &serialized_map, const QVariantMap map2serialize){
    QStringList ids = map2serialize.keys();
    QStringList fields;
    for (qint32 i = 0; i < ids.size(); i++){
        fields << ids.at(i) + VMS_SEP_KEYVALUE << map2serialize.value(ids.at(i)).toString();
    }
    if (serialized_map.isEmpty()) return fields.join(VMS_SEP_FIELDS);
    else return  fields.join(VMS_SEP_FIELDS) + VMS_SEP_FIELDS + serialized_map;
}

QVariantMap VariantMapSerializer::serialOneLevelStringToVariantMap(const QString &serialized_map, QVariantMap acc, QString *error, bool makeTwoWay){
    QStringList fields = serialized_map.split(VMS_SEP_FIELDS);
    for (qint32 i = 0; i < fields.size(); i++){
        QStringList keyvaluepair = fields.at(i).split(VMS_SEP_KEYVALUE);
        if (keyvaluepair.size() != 2) {
            *error = "Unexpected key value pair field: " + fields.at(i);
            return acc;
        }
        acc[keyvaluepair.first()] = keyvaluepair.last();
        if (makeTwoWay) acc[keyvaluepair.last()] = keyvaluepair.first();
    }
    return acc;
}


QString VariantMapSerializer::serializeTwoLevelVariantMap(const QString &serialized_map, const QVariantMap map2serialize){
    QStringList serialized_maps;
    QStringList uids = map2serialize.keys();
    for (qint32 j = 0; j < uids.size(); j++){
        QStringList fields;
        fields << uids.at(j);
        QStringList keys = map2serialize.value(uids.at(j)).toMap().keys();
        for (qint32 k = 0; k < keys.size(); k++){
            fields << keys.at(k) + VMS_SEP_KEYVALUE + map2serialize.value(uids.at(j)).toMap().value(keys.at(k)).toString();
        }
        serialized_maps << fields.join(VMS_SEP_FIELDS);
    }
    if (serialized_map.isEmpty()) return serialized_maps.join(VMS_SEP_VALUE_SET);
    else return  serialized_maps.join(VMS_SEP_VALUE_SET) + VMS_SEP_VALUE_SET + serialized_map;
}

QVariantMap VariantMapSerializer::serialTwoLevelStringToVariantMap(const QString &serialized_map, QVariantMap acc, QString *error){
    QStringList dataSets = serialized_map.split(VMS_SEP_VALUE_SET);
    for (qint32 i = 0; i < dataSets.size(); i++){
        QStringList fields = dataSets.at(i).split(VMS_SEP_FIELDS); // Separating into fields. The first is just the map name.
        if (fields.size() < 2) {
            *error = "Found data set with less than 2 fields: " + dataSets.at(i);
            return acc;
        }
        QString id = fields.first();
        QVariantMap m;
        for (qint32 j = 1; j < fields.size(); j++){
            QStringList keyvaluepair = fields.at(j).split(VMS_SEP_KEYVALUE);
            if (keyvaluepair.size() != 2) {
                *error = "Unexpected key value pair field: " + fields.at(j);
                return acc;
            }
            m[keyvaluepair.first()] = keyvaluepair.last();
        }
        acc[id] = m;
    }
    return acc;
}
