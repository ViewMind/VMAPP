#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include "../../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../../CommonClasses/common.h"

#define  SERVER_IP              "192.168.1.10"    // Local server
//#define  SERVER_IP              "18.220.30.34"  // Production server
#define  FILE_EYEEXP_SETTINGS   "../settings"

class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = nullptr);
    Q_INVOKABLE QString getStringForKey(const QString &key);

signals:

public slots:

private:
    LogInterface logger;
    bool loadingError;
    ConfigurationManager language;


};

#endif // LOADER_H
