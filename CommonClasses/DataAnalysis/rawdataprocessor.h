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
#include "../../CommonClasses/DataAnalysis/RScriptCaller/rdataprocessor.h"
#include "../../CommonClasses/DatFileInfo/datfileinfoindir.h"
#include "../../CommonClasses/DataAnalysis/BarGrapher/bargrapher.h"
#include "../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h"

#define   MSG_TYPE_STD                                   0
#define   MSG_TYPE_SUCC                                  1
#define   MSG_TYPE_ERR                                   2
#define   MSG_TYPE_WARN                                  3


class RawDataProcessor : public QObject
{
    Q_OBJECT
public:

    // The structure is used to return data gathered by each function so that it can be used in the main function.
    struct TagParseReturn{
        bool ok;
        //bool freqCheckErrors = false;
        QString version;
        QString filePath;
    };

    RawDataProcessor(QObject *parent = nullptr);

    // The first method is used when calling the program automatically by another program. The second one when used to directly process the file.
    void initialize(ConfigurationManager *c);
    QString getReportFileOutput() const {return reportFileOutput; }
    QHash<QString,FixationList> getFixations() const {return fixations;}

    // Access to the configuration manager. (Required to get the Width and Height once the file is processed).
    ConfigurationManager * getConfiguration() const { return config; }

    // The actual processing function
    void run();

    TagParseReturn separateInfoByTag(const QString &file, const QString &tag, QString *data, QString *experiment);

signals:
    void appendMessage(const QString &msg, qint32 type);

private:

    // For configuration and logging
    ConfigurationManager *config;

    // Processed fixations in the proper structure. This is used to draw fixations if needed.
    QHash<QString,FixationList> fixations;

    // Bar graph structure. Mirrors fixations structure.
    QList<BarGrapher::BarGraphOptions> graphValues;

    // The source *.dat files.
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

    // Body of email with frequency errors
    QString frequencyErrorMailBody;

    //---------------------------------------- AUX Functions -------------------------------------------

    // Separates the data from the experiment description in the data files.
    // Requires an structure as it needs to return a boolean to indicate a problem AND
    // The version of the experiment which is located in the header. Empty means version 1.

    TagParseReturn csvGeneration(EDPBase *processor, const QString &id, const QString &dataFile, const QString &header);

    bool getResolutionToConfig(const QString &firstline);

    void generateReportFile(const ConfigurationManager &res, const QString &repFileCode , bool freqErrorsOk);

    bool generateFDBFile(const QString &datFile, const FixationList &fixList, bool isFielding);

    QString getVersionForBindingExperiment(bool bound);

    bool getFrequencyCheckErrors(const QStringList & ferrors);

    void barGraphOptionsFromFixationList(const FixationList &fixlist, const QString &fileName);

    QString formatBindingResultsForPrinting(const EDPImages::BindingAnswers &ans, const QString &ID);

};

#endif // EYEDATAPROCESSINGTHREAD_H
