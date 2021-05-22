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


    QString key                     = "here is a really cool key";  // For signing the message.
    QString test_file               = "my_test_file.json";
    QString APIURL                  = "http://localhost/api_test";
    //QString endpoint                = "hola/juan/de/los";
    QString endpoint                = "get/pdf";
    QVariantMap URLParameters;

    // Let's add some parameters to the URL
    URLParameters.insert("paramA","3736");
    URLParameters.insert("paramB","somethign_wicked");


    RESTAPIController rest_controller;
    rest_controller.setBaseAPI(APIURL);

    rest_controller.setAPIEndpoint(endpoint);
    rest_controller.setURLParameters(URLParameters);

    // Lets create a test file.
    QVariantMap data;
    data.insert("Name","Ariel Ñoño");
    data.insert("Age","38");
    QVariantMap Hobbies;
    Hobbies.insert("Modelling","mediocre");
    Hobbies.insert("Drawing","bad");
    Hobbies.insert("Singing","Beareable");
    data.insert("Hobbies",Hobbies);

    QJsonDocument json = QJsonDocument::fromVariant(data);
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

    // Lets append the data
    rest_controller.setPOSTDataToSend(data);

    // Appending the data as JSON makes it igonre all both previous add ons, but thats the intention.
    rest_controller.setJSONData(data);

    QString auth_string = QMessageAuthenticationCode::hash(rest_controller.getPayload(), key.toUtf8(), QCryptographicHash::Sha3_512).toHex();

    // Adding headers to the request.
    rest_controller.addHeaderToRequest("AuthType","EyeExperimenterClient");
    rest_controller.addHeaderToRequest("Authentication",auth_string);

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
