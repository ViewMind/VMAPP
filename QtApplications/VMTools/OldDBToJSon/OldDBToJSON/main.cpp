#include <QCoreApplication>
#include <QVariantMap>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDataStream>
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QVariantMap map;
    QFile file("tosend.dat");
    file.open(QFile::ReadOnly);
    QDataStream reader(&file);
    qint32 version;

    reader >> version;
    reader >> map;
    file.close();

    QJsonDocument json = QJsonDocument::fromVariant(map);
    QByteArray data  = json.toJson(QJsonDocument::Indented);
    //std::cout << QString(data).toStdString() << std::endl;
    QVariantMap patientData = map["PATIENT_DATA"].toMap();
    QStringList patient_keys = patientData.keys();
    QStringList fields = {"firstname", "lastname", "sex", "puid", "birthdate", "displayID"};
    QList<QStringList> ids;
    for (qint32 i = 0; i < patient_keys.size(); i++){
        QStringList row;
        QVariantMap patient = patientData[patient_keys.at(i)].toMap();
        for (qint32 j = 0; j < fields.size(); j++){
            row << patient.value(fields.at(j)).toString();
        }
        ids << row;
    }

    qDebug() << ids;

    QFile output("ids.csv");
    output.open(QFile::WriteOnly);
    QTextStream writer(&output);
    writer << fields.join(",") << "\n";

    for (qint32 i = 0; i < ids.size(); i++){
        writer << ids.at(i).join(",") << "\n";
    }

    output.close();


    return a.exec();
}
