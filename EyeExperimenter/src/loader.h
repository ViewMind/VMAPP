#ifndef LOADER_H
#define LOADER_H

#include <QObject>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = nullptr);
    Q_INVOKABLE QString getStringForKey(const QString &key);
    Q_INVOKABLE QString getDoctorsName();

signals:

public slots:

private:
    ConfigurationManager configuration;
    ConfigurationManager language;

};

#endif // LOADER_H
