#ifndef VMRUNNINGLOADER_H
#define VMRUNNINGLOADER_H

#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

class VMRunningLoader : public QObject
{
    Q_OBJECT

public:
    explicit VMRunningLoader(QObject *parent = nullptr);

    Q_INVOKABLE QVariantMap getTitleMap();
    Q_INVOKABLE QVariantMap getMessageMap();

private:

    QVariantMap titles;
    QVariantMap messages;

};

#endif // VMRUNNINGLOADER_H
