#ifndef JSON_FILE_STORE_H
#define JSON_FILE_STORE_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QFile>
#include <QJsonParseError>

namespace Globals {

static QVariantMap LoadJSONFileToVariantMap(const QString &filename, QString *error){

    *error = "";
    QFile file(filename);

    if (!file.open(QFile::ReadOnly)){
        *error = "Could not open file '" + filename + "' for reading";
        return QVariantMap();
    }


    //data.reserve(50000000);
    //QTextStream reader(&file);
    //QString json = reader.readAll();

    QString json(file.readAll());
    file.close();

    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8(),&json_error);
    if (doc.isNull()){
        *error = "Error parsing the JSON string: " + json_error.errorString() + " From file: " + filename;
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

#endif // JSON_FILE_STORE_H
