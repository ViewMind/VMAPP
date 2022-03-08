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
//#include <QTextCodec>
#include <QMessageAuthenticationCode>
#include <QDataStream>
#include <iostream>
//#include "restapicontroller.h"
#include "../../../CommonClasses/RestAPIController/restapicontroller.h"

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);


    QString key                     = "58a3b6efb87046cffc32130750f51f057f5b82a6138c574cdf8c2df34291df88d0681185b0fbe1ad829607ec173920fe17d8d3a2980cc7342218f71ac6115f58";
    QString secret                  = "d4c2b2f31022011176962021103ab004a7a430c5ff0aa6ad71410babf950ea331aa27133acffd5b6d308a49dca3cf09ffb98d6c2fd67de69b70d01910bb31921b6186d20f497da20140d51df786f33722c50564136c3924863add7b6d9f2963279f412237c772178e90b089699965f77a715785a5a52e6fee8ce3b9bf1807cf5";

    QString test_file                 = "my_test_file.json";
    QString APIURL                  = "http://192.168.1.12/vmapi";
    //QString APIURL                    = "http://localhost/vmapi";
    //QString APIURL                  = "http://localhost/tests/";
    //QString APIURL                  = "https://eu-api.viewmind.ai/";
    QString endpoint                  = "/institution/operating_information/2";
    //QString endpoint                = "/portal_users/addnologpusers/1";
    //QString endpoint                = "/institution/getupdate/1";
    //QString endpoint                = "getfile.php";
    //QString endpoint                = "/reports/generate/1";
    //QString zipfile                 = "2021_07_19_20_19_43_912188.zip";
    QVariantMap URLParameters;

    // Let's add some parameters to the URL
    URLParameters.insert("ppkey","gazepoint");
    URLParameters.insert("instance",0);
    URLParameters.insert("version","16.3.0");

    RESTAPIController rest_controller;
    rest_controller.setBaseAPI(APIURL);

    rest_controller.setAPIEndpoint(endpoint);
    rest_controller.setURLParameters(URLParameters);    

    // Basic Post Data
    QVariantMap data;
    data.insert("institution_id",2);
    data.insert("institution_instance",0);
    // data.insert("data","some data");

//    QVariantMap testdata;
//    testdata.insert("institution_id","1");
//    testdata.insert("institution_instance","0");
//    testdata.insert("Doctor","Ariel Ñoño");
//    testdata.insert("email","some@somewhere.com");
//    QVariantList patients;
//    for (qint32 i = 0; i < 3; i++){
//        QVariantMap patient;
//        patient.insert("Name","Patient " + QString::number(i));
//        patient.insert("ID",QString::number(i));
//        patients << patient;
//    }
//    testdata.insert("Patients",patients);


    //QJsonDocument json = QJsonDocument::fromVariant(testdata);
    //QString s(json.toJson(QJsonDocument::Indented));
    //std::cout << s.toStdString() << std::endl;
    //data.insert("data",s);

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
//    if (!rest_controller.appendFileForRequest(zipfile,"FileToProcess")){
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

    //Appending the data as JSON makes it igonre all both previous add ons, but thats the intention. (Destroys POST Data).
    //rest_controller.setJSONData(testdata);

    // Adding salt to ensure uniqueness of signature.
    rest_controller.addSalt();

    //std::cout << "Sending Payload: " << std::endl;
    //std::cout << QString(rest_controller.getPayload()).toStdString() << std::endl;

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
            std::cout << QString(raw_reply).toStdString() << std::endl;
        }
        else{
           std::cout << QString(doc.toJson(QJsonDocument::Indented)).toStdString() << std::endl;
        }
    }
    else{
        QByteArray raw_reply = rest_controller.getReplyData();
        qDebug() << "Size of reply: " << raw_reply.size();
        QMap<QString,QString> rheaders = rest_controller.getResponseHeaders();
        QString searchFor = "Content-Disposition";

        if (rheaders.contains(searchFor)){
            // Getting the file name.
            QString filename = "";
            QString content_disposition = rheaders.value(searchFor);
            QStringList parts = content_disposition.split(";");
            for (qint32 i = 0; i < parts.size(); i++){
                if (parts.at(i).contains("filename")){
                    QStringList key_value = parts.at(i).split("=");
                    filename = key_value.last();
                    break;
                }
            }

            if (filename.isEmpty()){
                qDebug() << "Could not determine filename";
                return 0;
            }

            QFile receivedFile(filename);
            if (!receivedFile.open(QFile::WriteOnly)){
                qDebug() << "Could not open " + receivedFile.fileName() + " for writing";
                return 0;
            }
            QDataStream fileWriter(&receivedFile);
            qDebug() << "Raw Reply Size" << raw_reply.size();
            //fileWriter << raw_reply;
            fileWriter.writeRawData(raw_reply.constData(), raw_reply.size());
            receivedFile.close();
            std::cout << "Created " << receivedFile.fileName().toStdString() << std::endl;
            qDebug() << "Receive file size: " << receivedFile.size();

        }
        else{
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

//            // This should be a pdf.
//            QFile pdffile("downloaded.pdf");
//            if (!pdffile.open(QFile::WriteOnly)){
//                qDebug() << "Could not open " + pdffile.fileName() + " for writing";
//                return 0;
//            }
//            QDataStream pdfwriter(&pdffile);
//            pdfwriter << raw_reply;
//            pdffile.close();
//            std::cout << "Created " << pdffile.fileName().toStdString();
    }

    return 0;

}
