#ifndef STUDYENDOPERATIONS_H
#define STUDYENDOPERATIONS_H

#include <QThread>
#include <QProcess>

#include "localdb.h"
#include "eyexperimenter_defines.h"
#include "../../CommonClasses/RawDataContainer/viewminddatacontainer.h"

namespace StudyConstTimes {

   namespace Binding {
       static const qint32 TIME_FINISH = 1000;
       static const qint32 TIME_START_CROSS = 2000;
       static const qint32 TIME_WHITE_TRANSITION = 900;
   }

   namespace GoNoGo {
      static const qint32 TIME_CROSS = 1000;
   }

   namespace NBack {
      static const qint32 TRANSITION_TIME = 500;
   }
}

class StudyEndOperations : public QThread
{
public:

    explicit StudyEndOperations();

    /**
     * @brief setFileListToProcess - Batch processing function
     * @param fileList - The file list of files on which to do processing.
     * @details If this function is called then all files in this list will be processed when run. The list assumes a .json file list
     * which will all conduce to an _metadata.json file and qci file creation. This class will the detect the type fo file (legacy or current)
     * and compute QCI accordingly.
     */
    void setFileListToProcess(const QStringList &fileList);

    void setFileToProcess(const QString &file, const QString &idx_file);

    void doStudyFileProcessing();

    void run() override;

    bool wasOperationSuccssful() const;

    QVariantMap getLastQCIData() const;

    /**
     * @brief setFilesToProcessFromViewMindDataDirectory
     * @details The function will scan the viewmind_etdata directory for directories.
     * It will then scan each of those directories for .json files and add them to a list
     * which it will return. It will then call setFileListToProcess so that the next time
     * the thread is run, it will process ALL json files.
     */
    void setFilesToProcessFromViewMindDataDirectory();

private:

    // Path to both the json file and idx file generated at the end of the experiment.
    QString dataFile;
    QString idxFile;

    // To add generality we always process file from a list.
    QStringList filesToProcessJSON;
    QStringList filesToProcessIDX;

    // Flag to indicate whether the file processing encountered an error or not.
    bool operationsSuccess;

    // The data structure for the study unde analysis.
    ViewMindDataContainer vmdc;

    // The sampling period according to the processing parameters for the eyetracker. As provided by the server.
    qreal sampling_period;

    // The file name of the compressed tar file, both jsut the fill name and the full path.
    QString fullPathCompressedZip;
    QString compressedZipFile;

    // This file contains the data necessary for indexing.
    QString qciFile;
    QVariantMap qciData;

    // The data required for the indexed entry in the local database.
    qreal fileQCIndex;
    bool  fileQCPass;

    // Compute QCI Function. Assumes that the duration field in the study configuration is present
    qreal computeQCI(const QString &study);

    // Function that iterates over all trials, over each data set and computes the
    qreal computeNumberOfDataPointsIn2DStudy(const QVariantList &trials) const;

    // For legacy support. If a file does not contain, in it's studies the duration field, then the QCI is computed with the timestamps of the raw data (2D) study.
    qreal computeQCIOnLegacy2DStudy(const QVariantList &trials) const;
    qreal computeQCIOnLegacy3DStudy(const QVariantList &timevector) const;


    // Gets all the data necessary from the study medata to create a proper index in the local db.
    bool createQCIStudyFile(const QString &evalID);

    // Creates the tar ball file for both the JSON and IDX file and then deletes them.
    bool createTarFileAndCleanup();

};

#endif // STUDYENDOPERATIONS_H
