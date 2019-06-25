#ifndef VARIANTMAPSERIALIZER_H
#define VARIANTMAPSERIALIZER_H

#include <QVariantMap>
#include <QDebug>

#define   VMS_SEP_VALUE_SET                                  "<=>"
#define   VMS_SEP_KEYVALUE                                   ":::"
#define   VMS_SEP_FIELDS                                     "<=1=>"

class VariantMapSerializer
{
public:
    VariantMapSerializer();

    static QString serializeOneLevelVariantMap(const QString &serialized_map, const QVariantMap map2serialize);
    static QString serializeTwoLevelVariantMap(const QString &serialized_map, const QVariantMap map2serialize);
    static QVariantMap serialOneLevelStringToVariantMap(const QString &serialized_map, QVariantMap acc, QString *error, bool makeTwoWay);
    static QVariantMap serialTwoLevelStringToVariantMap(const QString &serialized_map, QVariantMap acc, QString *error);

};

#endif // VARIANTMAPSERIALIZER_H
