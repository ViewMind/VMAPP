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
    void setMedicalRecordsMaxDayDiff(qint64 ddiff)  {maxDayDiff = ddiff;}
    void setWorkingDir(const QString &dir, qint32 id2use) { workingDirectory = dir; idToUse = id2use;}
    void setGlobalMaximumDispersionParameter(const qint32 md) {maximumDispersionWindow = md;}
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
        QString studyDate;
        QString dbpuid;
        QString toString() const { return "FILE: " + filePath + ". CONF: " + configurationFile + ". DISPID: " + displayID;}
    };

    QString workingDirectory;
    qint32 idToUse;
    qint32 maximumDispersionWindow;

    // Information on how to process a file.
    QHash<QString, QList<DatFileProcessingStruct> > processingList;

    // To access DB data
    PatientNameMapManager *patdata;

    // Medical records string lists
    QStringList mrListEvals;
    QStringList mrListMeds;
    QStringList mrListRNM;
    QStringList headerMedRec;
    qint64 maxDayDiff;

    // Fill the processing list
    void recursiveFillProcessingList(const QString &dir);

    QString generateLocalCSV(DatFileProcessingStruct dfps, int exp_type);
    QString appendCSV(const QString &fileToAppend, const DatFileProcessingStruct &dfps, const QString &csvdata);
    QStringList getClosestMedicalRecord(const QString &dbpuid, const QString &studyDate);

    // Most CSVs when loaded come with an empty line at the bottom.
    // In order to make sure no valuable information is actually deleted this checks for the presence of anything other
    // than , in the line.
    bool isEmptyCSVLine(QString csvline);

};

#endif // BATCHCSVPROCESSING_H
