#ifndef BATCHCSVPROCESSING_H
#define BATCHCSVPROCESSING_H

#include <QThread>

#include "../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpimages.h"
#include "../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpreading.h"
#include "../../../CommonClasses/DatFileInfo/datfileinfoindir.h"
#include "../../../CommonClasses/DataAnalysis/rawdataprocessor.h"
#include "patientnamemapmanager.h"

class BatchCSVProcessing : public QThread
{
    Q_OBJECT
public:
    BatchCSVProcessing();
    void setDBConnect(PatientNameMapManager *db) { patdata = db;}
    void setWorkingDir(const QString &dir, qint32 id2use) { workingDirectory = dir; idToUse = id2use;}
    void run();

    QStringList getErrors() const {return errors;}
    QStringList getMessage() const {return messages;}

signals:
    void updateAdvance(qint32 p);

private:

    QStringList errors;
    QStringList messages;

    struct DatFileProcessingStruct{
        QString filePath;
        QString configurationFile;
        QString displayID;
        QString age;
        QString toString() const { return "FILE: " + filePath + ". CONF: " + configurationFile + ". DISPID: " + displayID;}
    };

    QString workingDirectory;
    qint32 idToUse;

    // Information on how to process a file.
    QHash<QString, QList<DatFileProcessingStruct> > processingList;

    // To access DB data
    PatientNameMapManager *patdata;

    // Fill the processing list
    void recursiveFillProcessingList(const QString &dir);

    QString generateLocalCSV(DatFileProcessingStruct dfps, bool isReading);
    QString appendCSV(const QString &fileToAppend, const QString &displayID, const QString &patientAge, const QString &csvdata);

};

#endif // BATCHCSVPROCESSING_H
