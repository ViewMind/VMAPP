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
#include "restapicontroller.h"

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);


    QString key                     = "2142cb83472a49d646b9bc2955bc9df4486ea47d2a4edf1e254d20d29b1e0a2a42a259e04de87336f65aea0c73d31677e9cf9ad4529b67d55f4b193ab88f9913";
    QString secret                  = "f2a494289f40f2a8408fdf3e71bba0c81e7d0f83d2ef510a54216315c2968999aba6a004aa0072e87d7479181496544fab46233d5afb715e1b4a1fd447116d5b17df8fb07f3e9b70fca3265f05c2b58a327901a0676ee4db3afc973254213b15a245e745faed70960d97259b5d15872ddf7988dcc743328a3287dcc947266028";

    QString test_file               = "my_test_file.json";
    //QString APIURL                  = "http://localhost/vmapi";
    QString APIURL                  = "http://192.168.1.12/vmapi";
    //QString endpoint                = "hola/juan/de/los";
    QString endpoint                = "/portal_users/getallmedical/1";
    QString imageFile               = "/home/web/dashboard-complete/docs/images/layout.png";
    QVariantMap URLParameters;

    // Let's add some parameters to the URL
    //URLParameters.insert("paramA","3736");
    //URLParameters.insert("paramB","somethign_wicked");

    RESTAPIController rest_controller;
    rest_controller.setBaseAPI(APIURL);

    rest_controller.setAPIEndpoint(endpoint);
    rest_controller.setURLParameters(URLParameters);

    // Lets create a test file.
    QVariantMap data;
    data.insert("institution_id",1);
    data.insert("institution_instance",0);

    QVariantMap testdata;
    testdata.insert("Name","Ariel Ñoño");
    testdata.insert("Age","38");
    QVariantMap Hobbies;
    Hobbies.insert("Modelling","mediocre");
    Hobbies.insert("Drawing","bad");
    Hobbies.insert("Singing","Beareable");
    testdata.insert("Hobbies",Hobbies);

    QJsonDocument json = QJsonDocument::fromVariant(testdata);
    QFile file(test_file);
    if (!file.open(QFile::WriteOnly)){
        qDebug() << "Could not create the test file. Exiting";
        return 0;
    }
    QTextStream writer(&file);
    writer << QString(json.toJson());
    file.close();

    // Lets append the file.
    if (!rest_controller.appendFileForRequest(test_file,"MyFileKey")){
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
    // rest_controller.setJSONData(data);

    // Adding salt to ensure uniqueness of signature.
    rest_controller.addSalt();

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
        std::cout << QString(raw_reply).toStdString() << std::endl;
    }
    else{
        QByteArray raw_reply = rest_controller.getReplyData();
        if (endpoint != "get/pdf"){
            std::cout << QString(raw_reply).toStdString() << std::endl;
            //        qDebug() << "The output";
            //        QTextCodec* codec = QTextCodec::codecForLocale();
            //        QTextDecoder* decoder = codec->makeDecoder();
            //        QString text = decoder->toUnicode(raw_reply);
            //        qDebug() << text;
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
