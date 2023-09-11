#ifndef EYE_TRACKER_DEFINES_H
#define EYE_TRACKER_DEFINES_H

#include <QString>
#include "debug.h"

namespace Globals {

namespace EyeTrackerNames{
static const QString HP    = "HP Omnicept";
static const QString VARJO = "Varjo Aero";
}

namespace EyeTrackerKeys {
static const QString HP    = "hpomnicept";
static const QString VARJO = "varjo";
}

static QVariantMap SortMapListByStringValue(QList<QVariantMap> list, const QString &index){

    // First we sort by order code.
    bool swapDone = true;
    while (swapDone){
        swapDone = false;
        for (qint32 i = 0; i < list.size()-1; i++){
            if (list.at(i).value(index).toString() > list.at(i+1).value(index).toString()){
                QVariantMap map = list.at(i);
                list.replace(i,list.at(i+1));
                list.replace(i+1,map);
                swapDone = true;
            }
        }
    }

    // Then we form a map. When iteratin through the map, it should do it in order code, since it's alphebetical.
    QVariantMap ans;
    //qDebug() << "After sorting list size" << list.size();
    for (qint32 i = 0; i < list.size(); i++){
        ans.insert(list.at(i).value(index).toString(),list.at(i));
    }
    //qDebug() << "Returning a map of size" << ans.size();
    return ans;
}

static QVariantMap LoadJSONFileToVariantMap(const QString &filename, QString *error){

    *error = "";
    QFile file(filename);

    if (!file.open(QFile::ReadOnly)){
        *error = "Could not open file '" + filename + "' for reading";
        return QVariantMap();
    }

    QTextStream reader(&file);
    QString json = reader.readAll();
    file.close();

    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(),&json_error);
    if (doc.isNull()){
        *error = "Error parsing the JSON string of the study description: " + json_error.errorString();
        return QVariantMap();
    }
    return doc.object().toVariantMap();


}

static bool SaveVariantMapToJSONFile(const QString &filename, const QVariantMap &map, bool indented = true){
    QJsonDocument doc = QJsonDocument::fromVariant(map);

    QFile file(filename);
    if (!file.open(QFile::WriteOnly)) return false;

    QTextStream writer(&file);
    if (indented) writer << doc.toJson(QJsonDocument::Indented);
    else writer << doc.toJson(QJsonDocument::Compact);
    file.close();

    return true;

}


}


#endif // EYE_TRACKER_DEFINES_H
