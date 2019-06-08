#ifndef RDATAPROCESSOR_H
#define RDATAPROCESSOR_H

#include <QProcess>
#include <QFileInfo>

#include "../../CommonClasses/SQLConn/dbdescription.h"
#include "../../ConfigurationManager/configurationmanager.h"
#include "../../common.h"

#define  RSCRIPT_READING    "res/reading.R"
#define  RSCRIPT_BINDING    "res/binding.R"
#define  RSCRIPT_RUNNABLE   "Rscript"


#define  FILE_R_OUT_READING "r_reading_output"
#define  FILE_R_OUT_TEMPRDA "reading.rda"
#define  FILE_R_OUT_BINDING "binding_output"

class RDataProcessor
{
public:
    RDataProcessor();

    void setWorkDirectory(const QString &wdir) { workDirectory = wdir; results.clear(); dbdata.clear();}
    QString processReading(const QString &readingFile);
    QString processBinding(const QString &bcfile, const QString &ucfile, qint32 bcright, qint32 ucright);

    QString getError() const {return error;}

    ConfigurationManager getResults() const {return results;}
    QHash<QString,qreal> getDBData() const {return dbdata;}

private:
    QString error;
    ConfigurationManager results;
    QString workDirectory;
    QHash<QString,qreal> dbdata;
    bool checkAndMerge(const QString &outputconf);

};

#endif // RDATAPROCESSOR_H
