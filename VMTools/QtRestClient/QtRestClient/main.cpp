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


    //QString key                     = "2142cb83472a49d646b9bc2955bc9df4486ea47d2a4edf1e254d20d29b1e0a2a42a259e04de87336f65aea0c73d31677e9cf9ad4529b67d55f4b193ab88f9913";
    //QString secret                  = "f2a494289f40f2a8408fdf3e71bba0c81e7d0f83d2ef510a54216315c2968999aba6a004aa0072e87d7479181496544fab46233d5afb715e1b4a1fd447116d5b17df8fb07f3e9b70fca3265f05c2b58a327901a0676ee4db3afc973254213b15a245e745faed70960d97259b5d15872ddf7988dcc743328a3287dcc947266028";

    QString key                     = "47f5f5255208e55dd6147bdbb38a23a1b4d00c51b7c49002408a7cf03bc46e0da49390f044fc201bf7ed153cbacdb4e335ca793cf171dbb1cfee717745eff78b";
    QString secret                  = "788ec095f99e4984f05c08d9ab0ae41d1b0b4d7f2e16f217b4a341f0824a802cd19f3070128d9ea67df39abe4964eb918384bccf2965bdfdba0ab9688ee8dbf5030ea23693a8da9315311ad46500255f5cc664554ea71ef9b054c6e4bfd22e4af964b5b1ccac85dd9522f55bd8a1c84470750b68069ab6cfc108c9d90a48cfd7";

    QString test_file               = "my_test_file.json";
    //QString APIURL                  = "http://192.168.1.12/vmapi";
    QString APIURL                  = "https://eu-api.viewmind.ai/";
    QString endpoint                = "/institution/operating_information/1";
    QString imageFile               = "/home/web/dashboard-complete/docs/images/layout.png";
    QVariantMap URLParameters;

    // Let's add some parameters to the URL
    URLParameters.insert("ppkey","gazepoint");
    //URLParameters.insert("paramB","somethign_wicked");

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

//    // Lets append the file.
//    if (!rest_controller.appendFileForRequest(test_file,"MyFileKey")){
//        qDebug() << "Error appending file: " << rest_controller.getErrors();
//        return 0;
//    }

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
        std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString() << std::endl;
    }
    else{
        QByteArray raw_reply = rest_controller.getReplyData();
        if (endpoint != "get/pdf"){
            //std::cout << QString(raw_reply).toStdString() << std::endl;
            QJsonParseError json_error;
            QJsonDocument doc = QJsonDocument::fromJson(raw_reply,&json_error);
            std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString() << std::endl;
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
