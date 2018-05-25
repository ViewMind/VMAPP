#ifndef EYEDATAPROCESSINGTHREAD_H
#define EYEDATAPROCESSINGTHREAD_H

/************************************************
 * The final output of this class is .tex of the
 * retport. PDF generation and viewing is handled
 * by the main thread.
 * *********************************************/

#include <QThread>
#include <QGraphicsScene>

#include "EyeMatrixGenerator/edpreading.h"
#include "EyeMatrixGenerator/edpimages.h"
#include "EyeMatrixGenerator/edpfielding.h"
#include "EyeMatrixProcessor/eyematrixprocessor.h"

#define   MSG_TYPE_STD                                   0
#define   MSG_TYPE_SUCC                                  1
#define   MSG_TYPE_ERR                                   2
#define   MSG_TYPE_WARN                                  3

class EyeDataProcessingThread : public QThread
{
    Q_OBJECT
public:
    EyeDataProcessingThread();
    void initialize(ConfigurationManager *c, const QSet<QString> &exps);

    // The actual processing function
    void run();

public slots:
    void onUpdateProgress(qint32 v);

signals:
    void updateProgressBar(qint32 p);
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

    //---------------------------------------- AUX Functions -------------------------------------------
    QString getNewestFile(const QString &directory, const QString &baseName);

    // Separates the data from the experiment description in the data files.
    // Returns the error message if there was one.
    bool separateInfoByTag(const QString &file, const QString &tag, QString *data, QString *experiment);

    QString csvGeneration(EDPBase *processor, const QString &id, const QString &dataFile, const QString &header);

    bool getResolutionToConfig(const QString &firstline);

};

#endif // EYEDATAPROCESSINGTHREAD_H
