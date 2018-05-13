#ifndef EDPBASE_H
#define EDPBASE_H

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QFileInfo>
#include <QPainter>
#include <QPoint>
#include <QDebug>
#include <QGraphicsScene>
#include "movingwindowalgorithm.h"
#include "../../CommonClasses/Experiments/experimentdatapainter.h"

struct MonitorGeometry{
    qreal XmmToPxRatio;
    qreal YmmToPxRatio;
    qreal distanceToMonitorInMilimiters;
};

class EDPBase: public QObject
{
    Q_OBJECT

public:
    EDPBase(ConfigurationManager *c);

    // The actual processing function that varies depending on the processing required.
    // Reimplemented in each class inherited.
    virtual bool doEyeDataProcessing(const QString &data){ Q_UNUSED(data) error = "BASE EDP CLASS"; return false;}

    // This function does the setup based on the type of processing required.
    bool configure(const QString &fileName, const QString &managerConfigData);

    // Setting the distance to monitor
    void setMonitorGeometry(const MonitorGeometry &mg) {monitorGeometry = mg;}

    // Get error if there was one. Same for warnings
    QString getError() const {return error;}
    QString getWarnings() const {return warnings; }

    // The name of the patient.
    QString getSubjectID() {return subjectIdentifier.replace("_"," ");}

    // Getting and setting the moving window parameters.
    MovingWindowParameters getMovingWindowParameters() const {return mwa.parameters;}
    void calculateWindowSize() {mwa.parameters.calculateWindowSize();}
    void setMovingWindowParameters(const MovingWindowParameters &mwp) {mwa.parameters = mwp; }

    // Setting sacadic latency parameters
    void setPixelsInSacadicLatency(qint32 p){pixelsInSacadicLatency = p;}

    // Setting the Fielding margin
    void setFieldingMarginInMM(qint32 fm) {fieldingMarginInMM = fm;}

    // Changes whether to generate the images or not.
    void setFastProcessing(bool enable) {  skipImageGeneration = enable; }

    QString getOuputMatrixFileName() const { return outputFile; }

    QStringList getFilteredLines() const {return filteredLinesList;}

signals:
    // This signal is intened to update a progress bar. It will give value between 0 and 100 to represent percent.
    void updateProgress(qint32 progress);

protected:

    // The drawing and experiment manager
    ExperimentDataPainter *manager;

    // Configuration manager just in case
    ConfigurationManager *config;

    // The subject string identifier
    QString subjectIdentifier;

    // The working directory, should have the patient name.
    QString workingDirectory;

    // The directory where images are saved.
    QString outputImageDirectory;

    // The output file
    QString outputFile;

    // The error message
    QString error;

    // Warning message
    QString warnings;

    // Filtered lines list.
    QStringList filteredLinesList;

    // Used to process data faster.
    bool skipImageGeneration;

    // The number of images that will be processed. Obtained from the header of the data file
    qint32 numberToProcess;

    // Used to determine when the eye left the center position of the screen.
    qint32 pixelsInSacadicLatency;

    // Used to calculate the amplitude of the sacade
    MonitorGeometry monitorGeometry;

    // Fielding margin used to estimate if the subjected "looked at" the correct target.
    qreal fieldingMarginInMM;

    // The object to calculate the fixation
    MovingWindowAlgorithm mwa;

    // Auxiliary function to average a column of a matrix between rows startRow and endRow, inclusive.
    qreal averageColumnOfMatrix(const DataMatrix &data, qint32 col, qint32 startRow, qint32 endRow);

    // Auxiliary function that counts how many zeros there are in the column col, between start and end rows.
    qint32 countZeros(const DataMatrix &data, qint32 col, qint32 startRow, qint32 endRow, qreal tol = 1e-6) const;

    // Auxiliary function that draws the fixations of each eye in an image
    void drawFixationOnImage(const QString &outputBaseFileName, const Fixations &l, const Fixations &r);

    // Filtering function to remove spurious data.
    struct FilterReturn{
        QString removedLines;
        QStringList filteredLines;
    };

};

#endif // EDPBASE_H
