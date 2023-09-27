#ifndef SUPPORTCONTACT_H
#define SUPPORTCONTACT_H

#include <QThread>
#include <QMessageAuthenticationCode>

#include "../../CommonClasses/RestAPIController/restapicontroller.h"
#include "debugoptions.h"

class SupportContact: public QObject
{

    Q_OBJECT

public:

    SupportContact();
    void configure(const QString instID, const QString &number, const QString &key, const QString &hash_code);
    void setMessageAndLogFileName(const QString &message, const QString &filename);
    void send();

    QString getError() const;

private slots:
    void gotReply();

signals:
    void finished();

private:

    RESTAPIController comm;

    QString institution;
    QString instance;
    QString key;
    QString hash;
    QString logFileName;
    QString comments;

    bool isDone;
    QString error;

    bool sendRequest();

    QString mailTemplate;

};

#endif // SUPPORTCONTACT_H
