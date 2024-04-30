#ifndef RESTAPICONTROLLER_H
#define RESTAPICONTROLLER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDebug>
#include <QTimer>
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

    // Same as set JSON data, but simplified. The data is straight copied into the request.
    // If set JSON data was called this will override it.
    void setRawStringDataToSend(const QString &rawData);

    // Appending a file to send in the request.
    bool appendFileForRequest(const QString &file_path, const QString &file_key);

    // Seding the HTTP POST request with the whatever endopoints and parameteres we want.
    bool sendPOSTRequest();

    // Seding the HTTP GET request with the whatever endopoints and parameteres we want.
    bool sendGETRequest();

    // Sets basic authentication for the API request.
    void setBasicAuth(const QString &username, const QString &password);

    // Add headers with specific content to the HTTP request.
    void addHeaderToRequest(const QString &headerName, const QString &headerValue);

    // Getting the data from the reply as is.
    QByteArray getReplyData() const;

    // Can be used for a polling method.
    bool isRequestFinished() const;

    // Check if there was an error in the request.
    bool didReplyHaveAnError() const;

    // When sending a file in a multi part message, the file is opened but cannot be closed unit the API interaction is completed
    // So manipulation of that file unitl the application itself is closed is not possible.
    // To solve this issue the temporary file handles are stored in a list of pointers. Calling this function the calls the "close"
    // function for all file handles and clears the list of file handles itself.
    void clearFileToSendHandles();

    // Get the payload of a request.
    // If setJSONData was used the concatenation of the json string + endpoint_and_parameters is used.
    // Otherwise the json string of the data to send + all the files + endpoint_and_parameters is used.
    QByteArray getPayload() const;

    // Adds a salt field to the current data to send and sets it to the current time stamp to the second
    // WARNING any field named "salt" will be over written. If setJSONData is called, salt will be lost.
    void addSalt();

    // Errors if there were any.
    QStringList getErrors() const;

    QMap<QString,QString> getResponseHeaders() const;

private slots:
    void gotReply();
    void onTimeOut();

signals:
    void gotReplyData();

private:
    QString baseAPIURL;

    QString endpoint;
    QVariantMap URLparameters;
    QString endPointAndParameters;

    QString basicAuthUsername;
    QString basicAuthPassword;

    QStringList errors;
    QVariantMap dataToSend;
    QString dataToSendAsRawData;
    QMap<QString,QString> filesToSend;
    QList<QFile*> filesToSendHandles;
    QNetworkAccessManager manager;
    QNetworkReply *reply;
    QTimer timeoutTimer;
    QByteArray replyData;
    QMap<QString, QString> headers;
    QMap<QString,QString> responseHeaders;
    bool errorInReply;
    bool sendDataAsJSON;

    // Used to create the finalized URL from the base API, the endpoint and whatever parameters were used.
    void makeEndpointAndParameters();

    void stringifyData();

    static const char * SALT_FIELD;

    // Timeout is 2 minutes.
    const qint32 TIMEOUT = 120000;

};

#endif // RESTAPICONTROLLER_H
