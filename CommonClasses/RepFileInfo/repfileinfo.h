#ifndef REPFILEINFO_H
#define REPFILEINFO_H

#include <QVariantMap>
#include <QDir>

#include "../common.h"
#include "../ConfigurationManager/configurationmanager.h"
#include "../LogInterface/loginterface.h"

#define KEY_REPNAME     "repname"
#define KEY_DATE        "date"
#define KEY_READING     "reading"
#define KEY_BINDING     "binding"
#define KEY_FIELDING    "fielding"
#define KEY_NBACKRT     "nbackrt"
#define KEY_INDEX       "repname"
#define KEY_SELFLAG     "vmRepSelected"
#define KEY_ISUPTODATE  "uptodate"
#define KEY_FILELIST    "filelist"
#define KEY_ORDERCODE   "ordercode"
#define KEY_BIND_NUM    "bindnumtargets"
#define KEY_FILENAME    "filename"

class RepFileInfo
{
public:

    struct AlgorithmVersions{
        qint32 readingAlg;
        qint32 bindingAlg;
        qint32 fieldingAlg;
        qint32 nbackrtAlg;
        QString binding_code;
        QString reading_code;
        QString fielding_code;
        QString nbackrt_code;
    };

    RepFileInfo();
    void setDirectory(const QString &directory, AlgorithmVersions alg_ver);
    void prepareIteration() {currentIterValue = 0;}
    QVariantMap nextReportInfo();
    QVariantMap getRepData(qint32 index) const {return repData.at(index);}
    QVariantMap getRepFileInfo(qint32 index) const {return repFileInfo.at(index); }
    QVariantMap getRepData(const QString &fileName, qint32 *index) const;

private:

    struct FileList {
        QStringList fileList;
        bool isUpToDate;
    };

    LogInterface logger;
    qint32 currentIterValue;
    QList<QVariantMap> repFileInfo;
    QList<QVariantMap> repData;

    FileList isReportUpToDate(const QString &directory, const QString &report_file, const AlgorithmVersions &algver);
    void insertInOrderInReportList(const QVariantMap &info, const QVariantMap &data);

};

#endif // REPFILEINFO_H
