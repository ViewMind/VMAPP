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

class RawDataProcessor : public QObject
{
    Q_OBJECT
public:

    RawDataProcessor(QObject *parent = 0);
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
    // Separates the data from the experiment description in the data files.
    // Returns the error message if there was one.
    bool separateInfoByTag(const QString &file, const QString &tag, QString *data, QString *experiment);

    void commonInitialization();

    QString csvGeneration(EDPBase *processor, const QString &id, const QString &dataFile, const QString &header);

    bool getResolutionToConfig(const QString &firstline);

    void generateReportFile(const DataSet::ProcessingResults &res, const QHash<qint32,bool> whatToAdd);

    QString formatBindingResultsForPrinting(const EDPImages::BindingAnswers & ans);

};

#endif // EYEDATAPROCESSINGTHREAD_H
