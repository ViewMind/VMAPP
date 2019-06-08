#ifndef UICONFIGMAP_H
#define UICONFIGMAP_H

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

#define UI_MAP_EXPECTED_SIZE     3

#define UIMAP_PATIENT_SUBJECT    0
#define UIMAP_INDEX_BEHAVIORAL   1
#define UIMAP_BLOCK_BIND_OPTIONS 2

class UIConfigMap : public QObject
{
    Q_OBJECT
public:
    UIConfigMap();

    // SETTER
    bool setConfigurationString(const QString &configStr);
    // Getting setup error if there was any.
    QString getError() const {return error;}

    // GETTERS
    Q_INVOKABLE QString getStructure() const { return options.value(UIMAP_PATIENT_SUBJECT); }
    Q_INVOKABLE QString getIndexBehavioral() const { return options.value(UIMAP_INDEX_BEHAVIORAL); }
    Q_INVOKABLE QString getBlockedBinding() const { return options.value(UIMAP_BLOCK_BIND_OPTIONS); }


private:
    QString error;
    QHash<qint32,QString> options;

};

#endif // UICONFIGMAP_H
