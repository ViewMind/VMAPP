#ifndef LOADER_H
#define LOADER_H

#include <QObject>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "eye_experimenter_defines.h"

class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = nullptr);
    Q_INVOKABLE QString getStringForKey(const QString &key);
    Q_INVOKABLE QString getDoctorsName();
    Q_INVOKABLE QString getWindowTilteVersion(){ return EXPERIMENTER_VERSION; }

signals:

public slots:

private:
    ConfigurationManager configuration;
    ConfigurationManager language;

};

#endif // LOADER_H
