#ifndef DEBUG_H
#define DEBUG_H

#include <QVariantMap>
#include <QJsonDocument>
#include <QJsonObject>
#include <iostream>

namespace Debug {

    static QString QVariantMapToString(const QVariantMap &map){
        QJsonDocument json = QJsonDocument::fromVariant(map);
        QByteArray data  = json.toJson(QJsonDocument::Indented);
        return QString(data);
    }

    static void prettpPrintQVariantMap(const QVariantMap &map){
        std::cout << QVariantMapToString(map).toStdString() << std::endl;
    }


}

#endif // DEBUG_H
