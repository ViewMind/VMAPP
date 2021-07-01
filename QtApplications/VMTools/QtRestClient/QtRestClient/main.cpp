#include <QCoreApplication>

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QEventLoop>
#include <QObject>
#include <QVariantMap>
#include <QJsonDocument>
#include <QFile>
#include <QHttpMultiPart>
#include <QTextCodec>
#include <QMessageAuthenticationCode>
#include <QDataStream>
#include <iostream>
//#include "restapicontroller.h"
#include "../../../CommonClasses/RestAPIController/restapicontroller.h"

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);


    QString key                     = "b1074e66a5a9822969bf769d435b49ef347a18c01ef058f732ecd3a13aa45c6c286d13378c4e2fbb91c0089912e4c842fc9dee557af2c19be46fecd94b18e176";
    QString secret                  = "14568c6a792eb5caee4cd18f5c5d8ea5d24ee3c1334909961beeb61b038466a24362a0e3f6e03f0b06950335d8a9b8e58d91675dd3ed6e836301a39bccf8673e08f2e7ca757430548dd8368ec15119c9989b65b35be6458d9ee1ff62a8edd5242b7db5723b2071328df38d544175d100efcb92e4b4ccf09c2bb4cf6f5b01449c";

    QString test_file               = "my_test_file.json";
    //QString APIURL                  = "http://192.168.1.12/vmapi";
    QString APIURL                  = "http://localhost/vmapi";
    //QString APIURL                  = "https://eu-api.viewmind.ai/";
    //QString endpoint                = "/institution/operating_information/1";
    QString endpoint                = "/reports/generate/1";
    QString zipfile                 = "/home/ariel/repos/viewmind_projects/VMTools/RawJSONDataDev/bin/binding2019_12_19_12_54.zip";
    QVariantMap URLParameters;

    // Let's add some parameters to the URL
    //URLParameters.insert("ppkey","gazepoint");
    URLParameters.insert("instance",0);

    RESTAPIController rest_controller;
    rest_controller.setBaseAPI(APIURL);

    rest_controller.setAPIEndpoint(endpoint);
    rest_controller.setURLParameters(URLParameters);

    // Lets create a test file.
    QVariantMap data;
    data.insert("institution_id",1);
    data.insert("institution_instance",0);

//    QVariantMap testdata;
//    testdata.insert("Name","Ariel Ñoño");
//    testdata.insert("Age","38");
//    QVariantMap Hobbies;
//    Hobbies.insert("Modelling","mediocre");
//    Hobbies.insert("Drawing","bad");
//    Hobbies.insert("Singing","Beareable");
//    testdata.insert("Hobbies",Hobbies);

//    QJsonDocument json = QJsonDocument::fromVariant(testdata);
//    QFile file(test_file);
//    if (!file.open(QFile::WriteOnly)){
//        qDebug() << "Could not create the test file. Exiting";
//        return 0;
//    }
//    QTextStream writer(&file);
//    writer << QString(json.toJson());
//    file.close();

    // Lets append the file.
    if (!rest_controller.appendFileForRequest(zipfile,"FileToProcess")){
        qDebug() << "Error appending file: " << rest_controller.getErrors();
        return 0;
    }

//    // And now let's append an image file
//    if (!rest_controller.appendFileForRequest(imageFile,"image")){
//        qDebug() << "Error appending file: " << rest_controller.getErrors();
//        return 0;
//    }

    // Lets append the data
    rest_controller.setPOSTDataToSend(data);

    // Appending the data as JSON makes it igonre all both previous add ons, but thats the intention.
    //rest_controller.setJSONData(data);

    // Adding salt to ensure uniqueness of signature.
    rest_controller.addSalt();

    //qDebug() << rest_controller.getPayload();

    QString auth_string = QMessageAuthenticationCode::hash(rest_controller.getPayload(), secret.toUtf8(), QCryptographicHash::Sha3_512).toHex();

    // Adding headers to the request.
    rest_controller.addHeaderToRequest("AuthType","VMClient");
    rest_controller.addHeaderToRequest("Authentication",key);
    rest_controller.addHeaderToRequest("Signature",auth_string);

    // Generate the request.
    if (!rest_controller.sendPOSTRequest()){
        qDebug() << "Error sending POST request: " << rest_controller.getErrors();
        return 0;
    }

    QObject::connect(&rest_controller, &RESTAPIController::gotReplyData, QCoreApplication::quit);
    a.exec();

    if (rest_controller.didReplyHaveAnError()){
        qDebug() << "Error in the request";
        qDebug() << rest_controller.getErrors();
        QByteArray raw_reply = rest_controller.getReplyData();
        QJsonParseError json_error;
        QJsonDocument doc = QJsonDocument::fromJson(raw_reply,&json_error);
        if (json_error.error != QJsonParseError::NoError){
            qDebug() << raw_reply;
        }
        else{
           std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString() << std::endl;
        }
    }
    else{
        QByteArray raw_reply = rest_controller.getReplyData();
        if (endpoint != "get/pdf"){
            //std::cout << QString(raw_reply).toStdString() << std::endl;
            QJsonParseError json_error;
            QJsonDocument doc = QJsonDocument::fromJson(raw_reply,&json_error);            
            if (json_error.error != QJsonParseError::NoError){
                std::cout << "Could not parse JSON Ouput. Reason: " << json_error.errorString().toStdString() << std::endl;
                std::cout << raw_reply.toStdString() << std::endl;
            }
            else{
                std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString() << std::endl;
            }
        }
        else{
            // This should be a pdf.
            QFile pdffile("downloaded.pdf");
            if (!pdffile.open(QFile::WriteOnly)){
                qDebug() << "Could not open " + pdffile.fileName() + " for writing";
                return 0;
            }
            QDataStream pdfwriter(&pdffile);
            pdfwriter << raw_reply;
            pdffile.close();
            std::cout << "Created " << pdffile.fileName().toStdString();
        }
    }

    return 0;

}
