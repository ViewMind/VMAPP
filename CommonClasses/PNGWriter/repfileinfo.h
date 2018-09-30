#ifndef REPFILEINFO_H
#define REPFILEINFO_H

#include <QVariantMap>
#include <QDir>

#include "../common.h"
#include "../ConfigurationManager/configurationmanager.h"
#include "../LogInterface/loginterface.h"

#define KEY_REPNAME "repname"
#define KEY_DATE    "date"
#define KEY_READING "reading"
#define KEY_BINDING "binding"
#define KEY_INDEX   "repname"
#define KEY_SELFLAG "vmRepSelected"

class RepFileInfo
{
public:
    RepFileInfo();
    void setDirectory(const QString &directory);
    void prepareIteration() {currentIterValue = 0;}
    QVariantMap nextReportInfo();
    QVariantMap getRepData(qint32 index) const {return repData.at(index);}
    QVariantMap getRepData(const QString &fileName) const;

private:

    LogInterface logger;
    qint32 currentIterValue;
    QList<QVariantMap> repFileInfo;
    QList<QVariantMap> repData;

};

#endif // REPFILEINFO_H
