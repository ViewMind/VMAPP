#ifndef DATFILEINFO_H
#define DATFILEINFO_H

#include <QDir>
#include "../common.h"
#include "../LogInterface/loginterface.h"

#define   LIST_INDEX_READING     0
#define   LIST_INDEX_BINDING_BC  1
#define   LIST_INDEX_BINDING_UC  2

#define   LIST_NUMBER_OF_LISTS   3

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

    void setDatDirectory(const QString &dir);
    bool hasPendingReports() const;

    // Accessing the file lists in different ways.
    QStringList getReadingFileList() const;
    QStringList getBindingBCFileList() const;
    QStringList getBindingUCFileList() const;
    QStringList getBindingUCFileListCompatibleWithSelectedBC(qint32 selectedBC);
    QStringList getFileSetAndReportName(const ReportGenerationStruct &repgen) const;
    QStringList getFileSetAndReportName(const QStringList &fileList);
    QString getDatFileNameFromSelectionDialogIndex(qint32 index, qint32 whichList) const;


    // Functions that parse the file names and gather the information in a DatInfo structure.
    static DatInfo getDatFileInformation(const QString &file);
    static DatInfo getBindingFileInformation(const QString &bindingFile);
    static DatInfo getReadingInformation(const QString &readingFile);
    static qint32 getValidEyeForDatList(const QStringList &list);


private:

    // Used for iteration

    // File lists by type.
    QStringList   filesReading;
    QStringList   filesBindingBC;
    QStringList   filesBindingUC;
    QList<qint32> filesBindingUCValidIndexes;

    // Generalization that only gets any of the code lists for file lists.
    QStringList getFileList(const QStringList &infoList) const;

    // Smart insertion so that file list are order form newer to older
    void insertIntoListAccordingToOrder(const QString &fileName, QStringList *list, QStringList *order);

    // Const version that enables to check for existance fo UC Files compatible with BC Files in a const function.
    QStringList getBindingUCFileListCompatibleWithSelectedBC(qint32 selectedBC, QList<qint32> *validIndexes) const;


};

#endif // DATFILEINFO_H
