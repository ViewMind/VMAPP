#ifndef EYEDATAPROCESSINGTHREAD_H
#define EYEDATAPROCESSINGTHREAD_H

/************************************************
 * Processes the raw eyetracker data.
 * 1) It generates the intermediate CSV Files
 * 2) Processes the CSV files and generates the
 *    final output
 * *********************************************/

#include <QThread>

#include "EyeMatrixGenerator/edpreading.h"
#include "EyeMatrixGenerator/edpimages.h"
#include "EyeMatrixGenerator/edpfielding.h"
#include "EyeMatrixProcessor/eyematrixprocessor.h"

#define   MSG_TYPE_STD                                   0
#define   MSG_TYPE_SUCC                                  1
#define   MSG_TYPE_ERR                                   2
#define   MSG_TYPE_WARN                                  3
#define   NUMBER_OF_PARTS_FOR_BINDING_FILE               7

class RawDataProcessor : public QObject
{
    Q_OBJECT
public:

    RawDataProcessor(QObject *parent = 0);

    // The first method is used when calling the program automatically by another program. The second one when used to directly process the file.
    void initialize(ConfigurationManager *c, const QSet<QString> &exps);
    void initialize(ConfigurationManager *c, const QStringList &filesToProcess);
    QString getReportFileOutput() const {return reportFileOutput; }
    QHash<QString,FixationList> getFixations() const {return fixations;}

    // The actual processing function
    void run();

signals:
    void appendMessage(const QString &msg, qint32 type);

private:

    // For configuration and logging
    ConfigurationManager *config;

    // Experiments to actually process
    QSet<QString> experiments;

    // Processed fixations in the proper structure. This is used to draw fixations if needed.
    QHash<QString,FixationList> fixations;

    // The files that will be processed
    QString dataReading;
    QString dataBindingUC;
    QString dataBindingBC;
    QString dataFielding;

    // The matrix file
    QString matrixReading;
    QString matrixBindingUC;
    QString matrixBindingBC;
    QString matrixFielding;

    // Required for configuring the processors
    MonitorGeometry mgeo;
    MovingWindowParameters mwp;    

    // The full path to the generated file.
    QString reportFileOutput;

    //---------------------------------------- AUX Functions -------------------------------------------

    // The structure is used to return data gathered by each function so that it can be used in the main function.
    struct TagParseReturn{
        bool ok;
        QString version;
        QString filePath;
    };

    // Separates the data from the experiment description in the data files.
    // Requires an structure as it needs to return a boolean to indicate a problem AND
    // The version of the experiment which is located in the header. Empty means version 1.

    TagParseReturn separateInfoByTag(const QString &file, const QString &tag, QString *data, QString *experiment);

    void commonInitialization();

    TagParseReturn csvGeneration(EDPBase *processor, const QString &id, const QString &dataFile, const QString &header);

    bool getResolutionToConfig(const QString &firstline);

    void generateReportFile(const DataSet::ProcessingResults &res, const QHash<qint32,bool> whatToAdd);

    QString formatBindingResultsForPrinting(const EDPImages::BindingAnswers & ans);

    QString getVersionForBindingExperiment(bool bound);

};

#endif // EYEDATAPROCESSINGTHREAD_H
