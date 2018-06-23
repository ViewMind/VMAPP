#ifndef EYEDATAPROCESSINGTHREAD_H
#define EYEDATAPROCESSINGTHREAD_H

/************************************************
 * Class that controls the calls
 * *********************************************/

#include <QThread>
//#include <QGraphicsScene>

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
    QString getReportFileOutput() const {return reportFileOutput; }

    // The actual processing function
    void run();

signals:
    void appendMessage(const QString &msg, qint32 type);

private:

    // For configuration and logging
    ConfigurationManager *config;

    // Experiments to actually process
    QSet<QString> experiments;

    // The files that will be processed
    QString dataReading;
    QString dataBindingUC;
    QString dataBindingBC;
    QString dataFielding;

    // The matrix fiel
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

    QString csvGeneration(EDPBase *processor, const QString &id, const QString &dataFile, const QString &header);

    bool getResolutionToConfig(const QString &firstline);

    void generateReportFile(const DataSet::ProcessingResults &res, const QHash<qint32,bool> whatToAdd);

    QString formatBindingResultsForPrinting(const EDPImages::BindingAnswers & ans);

};

#endif // EYEDATAPROCESSINGTHREAD_H
