#ifndef RDATAPROCESSOR_H
#define RDATAPROCESSOR_H

#include <QProcess>
#include <QFileInfo>

#include "../../SQLConn/dbdescription.h"
#include "../../ConfigurationManager/configurationmanager.h"
#include "../../common.h"

#define  RSCRIPT_READING    "res/reading_es.R"
#define  RSCRIPT_BINDING2   "res/binding2.R"
#define  RSCRIPT_BINDING3   "res/binding3.R"
#define  RSCRIPT_NBACKRT    "res/nback_rt.R"
#define  RSCRIPT_GONOGO     "res/gonogo.R"
#define  RSCRIPT_RUNNABLE   "Rscript"

#define  FILE_R_OUT_NBACKRT "nbackk_rt_output"
#define  FILE_R_OUT_READING "r_reading_output"
#define  FILE_R_OUT_TEMPRDA "reading.rda"
#define  FILE_R_OUT_BINDING "binding_output"
#define  FILE_R_OUT_GONOGO  "gonogo_output"

class RDataProcessor
{
public:
    RDataProcessor();

    void setWorkDirectory(const QString &wdir) { workDirectory = wdir; results.clear(); dbdata.clear();}
    QString processReading(const QString &readingFile);
    QString processBinding(const QString &bcfile, const QString &ucfile, qint32 bcright, qint32 ucright);
    QString processNBackRT(const QString &nbackrtFile);
    QString processGoNoGo(const QString &gonogoFile);

    QString getError() const {return error;}
    QString getWarning() const {return warning;}

    ConfigurationManager getResults() const {return results;}
    QHash<QString,qreal> getDBData() const {return dbdata;}

private:
    QString error;
    QString warning;
    ConfigurationManager results;
    QString workDirectory;
    QHash<QString,qreal> dbdata;
    bool checkAndMerge(const QString &outputconf);
};

#endif // RDATAPROCESSOR_H
