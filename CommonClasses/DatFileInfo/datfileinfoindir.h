#ifndef DATFILEINFO_H
#define DATFILEINFO_H

#include "../common.h"
#include "../LogInterface/loginterface.h"

// This class parses all files in a directory that have the format
// specified in the constructor. It assumes the following:

class DatFileInfoInDir
{
public:

    struct DatInfo{
        QString basename;
        QString fileName;
        QString extraInfo;
        QString date;
        qint32  hour;
        QString toString() const{
            return "FILE: " + fileName + ". TYPE: " + basename + ". DATE: " + date + " @ "  + QString::number(hour) + ". EXTRA: " + extraInfo;
        }
    };
    typedef QHash< QString, QList<DatInfo> > DatInfoHash ;

    DatFileInfoInDir();

    void setDatDirectory(const QString &dir);
    bool hasPendingReports() const;

    // Functions that parse the file names and gather the information in a DatInfo structure.
    static DatInfo getBindingFileInformation(const QString &bindingFile);
    static DatInfo getReadingInformation(const QString &readingFile);
    static DatInfo getRerportInformation(const QString &repfile);

    /// FOR DEBUGGING ONLY.
    void printData();

private:

    struct DatInfoAndRep{
        DatInfoHash datInfo;
        QStringList expectedRepFiles;
        QHash<QString,QStringList> reportFileSet;
    };

    // Data structure:
    // 1) Each entry corresponds to a date.
    // 2) For each date there is a list of DatInfo for each of the base file names (binding uc, binding bc and reading). Also associated for each date, there is a rep file.
    // 3) The list is ordered from oldest to newest.
    QHash<QString,DatInfoAndRep> filesByDate;

    // Parsing functions based on file name
    void insertDatIntoInfoList(QList<DatInfo> *list, const DatInfo &info);

    // Given the information gathered from a Hash of the list of dat infos associated to files from each experiment,
    // the function generates the expected name for the report file. Then checks that againts existing report files
    // If the file does not exist then the list of files that are needed for processing are saved as a set associated to the
    // file report name.
    void setExpectedReportFileSet(const QString &date, const QSet<QString> existingReports);

};

#endif // DATFILEINFO_H
