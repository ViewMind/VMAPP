#ifndef RESTAPICONTROLLER_H
#define RESTAPICONTROLLER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QEventLoop>
#include <QObject>
#include <QVariantMap>
#include <QJsonDocument>
#include <QFile>
#include <QHttpMultiPart>
#include <QDateTime>
#include <iostream>

class RESTAPIController: public QObject
{
    Q_OBJECT
public:
    RESTAPIController(QObject *parent = nullptr);

    // Must be called to set data and files for request again. Clears everything but the URL.^
    void resetRequest();

    // The Base API URL
    void setBaseAPI(const QString baseAPI);

    // Set the endpoint and parameters
    void setAPIEndpoint(const QString &endpoint);

    // Set the parameters passed in the URL.
    void setURLParameters(const QVariantMap &map);

    // Appending data to send in the request. POST data CANNOT contain a dictionary as one of the fields.
    // The PHP documentation identifies this a From DATa.
    bool setPOSTDataToSend(const QVariantMap &map);

    // Sets the JSON data to send in the request. This is NOT compatible with sending files and form data.
    // It's either one or the other, but cannot be both. Setting this will clear the the files and form data to send.
    void setJSONData(const QVariantMap &json);

    // Appending a file to send in the request.
    bool appendFileForRequest(const QString &file_path, const QString &file_key);

    // Seding the HTTP POST request with the whatever endopoints and parameteres we want.
    bool sendPOSTRequest();

    // Add headers with specific content to the HTTP request.
    void addHeaderToRequest(const QString &headerName, const QString &headerValue);

    // Getting the data from the reply as is.
    QByteArray getReplyData() const;

    // Can be used for a polling method.
    bool isRequestFinished() const;

    // Check if there was an error in the request.
    bool didReplyHaveAnError() const;

    // Get the payload of a request.
    // If setJSONData was used the concatenation of the json string + endpoint_and_parameters is used.
    // Otherwise the json string of the data to send + all the files + endpoint_and_parameters is used.
    QByteArray getPayload() const;

    // Adds a salt field to the current data to send and sets it to the current time stamp to the second
    // WARNING any field named "salt" will be over written. If setJSONData is called, salt will be lost.
    void addSalt();

    // Errors if there were any.
    QStringList getErrors() const;

private slots:
    void gotReply();

signals:
    void gotReplyData();

private:
    QString baseAPIURL;

    QString endpoint;
    QVariantMap URLparameters;
    QString endPointAndParameters;

    QStringList errors;
    QVariantMap dataToSend;
    QMap<QString,QString> filesToSend;
    QNetworkAccessManager manager;
    QNetworkReply *reply;
    QByteArray replyData;
    QMap<QString, QString> headers;
    bool errorInReply;
    bool sendDataAsJSON;

    // Used to create the finalized URL from the base API, the endpoint and whatever parameters were used.
    void makeEndpointAndParameters();

    void stringifyData();

    static const char * SALT_FIELD;

};

#endif // RESTAPICONTROLLER_H
