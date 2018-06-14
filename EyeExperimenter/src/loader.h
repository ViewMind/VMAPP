#ifndef LOADER_H
#define LOADER_H

#include <QObject>
#include <QUrl>
#include <QQuickWindow>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "eye_experimenter_defines.h"

class Loader : public QObject
{
    Q_OBJECT
public:
    explicit Loader(QObject *parent = nullptr, ConfigurationManager *c = nullptr);
    Q_INVOKABLE QString getStringForKey(const QString &key);
    Q_INVOKABLE QString getConfigurationString(const QString &key);
    Q_INVOKABLE bool getConfigurationBoolean(const QString &key);
    Q_INVOKABLE bool getLoaderError() const {return loadingError; }
    Q_INVOKABLE void setConfigurationString(const QString &key, const QString &value);
    Q_INVOKABLE void setConfigurationBoolean(const QString &key, bool value);
    Q_INVOKABLE QString hasValidOutputRepo(const QString &dirToCheck = "");
    Q_INVOKABLE QString getWindowTilteVersion(){ return EXPERIMENTER_VERSION; }
    Q_INVOKABLE bool createPatientDirectory(const QString &patient, const QString &age, const QString &email);
    Q_INVOKABLE QRect frameSize(QObject *window);

signals:

public slots:

private:
    LogInterface logger;
    bool loadingError;
    ConfigurationManager *configuration;
    ConfigurationManager language;

    // Loads default configurations when they don't exist.
    void loadDefaultConfigurations();

    // Creates the directory substructure
    bool createDirectorySubstructure(QString drname, QString pname, QString baseDir, QString saveAs);

    // Sets the language for program.
    void changeLanguage();

};

#endif // LOADER_H
