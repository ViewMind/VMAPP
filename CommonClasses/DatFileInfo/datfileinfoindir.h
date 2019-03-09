#ifndef DATFILEINFO_H
#define DATFILEINFO_H

#include <QDir>
#include "../common.h"
#include "../LogInterface/loginterface.h"

class DatFileInfoInDir
{
public:

    struct DatInfo{
        QString basename;
        QString fileName;
        QString validEye;
        QString extraInfo;
        QString code;
        QString date;
        QString hour;
        QString orderString;
        QString toString() const{
            return "FILE: " + fileName + ". TYPE: " + basename + ". DATE: " + date + " @ "  + hour
                    + ". EXTRA: " + extraInfo + ". VALID EYE: " + validEye;
        }
        bool isFileCompatibleWith(const DatInfo &info){
            return (info.validEye == validEye) && (info.extraInfo == extraInfo) && (info.date == date);

        }
    };


    struct ReportGenerationStruct {
        qint32 readingFileIndex;
        qint32 bindingBCFileIndex;
        qint32 bindingUCFileIndex;
        void clear() { readingFileIndex = -1; bindingBCFileIndex = -1;
                       bindingUCFileIndex = -1; }
    };

    DatFileInfoInDir();

    void setDatDirectory(const QString &dir, bool listRepEvenIfTheyExist = false);
    bool hasPendingReports() const;

    // Accessing the file lists in different ways.
    QStringList getReadingFileList() const;
    QStringList getBindingBCFileList() const;
    QStringList getBindingUCFileList() const;
    QStringList getBindingUCFileListCompatibleWithSelectedBC(qint32 selectedBC);
    QStringList getFileSetAndReportName(const ReportGenerationStruct &repgen) const;
    QStringList getFileSetAndReportName(const QStringList &fileList);

    // Functions that parse the file names and gather the information in a DatInfo structure.
    static DatInfo getDatFileInformation(const QString &file);
    static DatInfo getBindingFileInformation(const QString &bindingFile);
    static DatInfo getReadingInformation(const QString &readingFile);
    //static DatInfo getRerportInformation(const QString &repfile);
    static qint32 getValidEyeForDatList(const QStringList &list);


private:

    // Used for iteration
    QList<QStringList> fileSets;
    qint32 currentFileSet;

    // File lists by type.
    QStringList   filesReading;
    QStringList   filesBindingBC;
    QStringList   filesBindingUC;
    QList<qint32> filesBindingUCValidIndexes;

    // Generalization that only gets any of the code lists for file lists.
    QStringList getFileList(const QStringList &infoList) const;


};

#endif // DATFILEINFO_H
