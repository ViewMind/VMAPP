#ifndef EDPBASE_H
#define EDPBASE_H

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QFileInfo>
#include <QDebug>
#include "movingwindowalgorithm.h"
#include "../../ConfigurationManager/configurationmanager.h"

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
    void configure(const QString &fileName, const QString &exp);

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

    QString getOuputMatrixFileName() const { return outputFile; }

    QStringList getFilteredLines() const {return filteredLinesList;}

    FixationList getEyeFixations() const {return eyeFixations;}

protected:

    // Stored fixation results
    FixationList eyeFixations;

    // Configuration manager just in case
    ConfigurationManager *config;

    // The subject string identifier
    QString subjectIdentifier;

    // The working directory, should have the patient name.
    QString workingDirectory;

    // The output file
    QString outputFile;

    // The error message
    QString error;

    // Warning message
    QString warnings;

    // Filtered lines list.
    QStringList filteredLinesList;

    // Used to store processed data in order to do a frequency analysis.
    QList<DataMatrix> eventData;

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

    // Filtering function to remove spurious data.
    struct FilterReturn{
        QString removedLines;
        QStringList filteredLines;
    };

    // Structured used to calculate the sacade amplitude.
    struct SacadeAmplitudeCalculator{

        qreal lastX;
        qreal lastY;

        void reset(){
            lastX = -1;
            lastY = -1;
        }

        qreal calculateSacadeAmplitude(qreal x, qreal y, const MonitorGeometry &monitorGeometry){
            qreal sacade = 0;
            if ((lastX > -1) && (lastY > -1)){
                // Calculating the sacade
                qreal xINmm = (lastX - x)*monitorGeometry.XmmToPxRatio;
                qreal yINmm = (lastY - y)*monitorGeometry.YmmToPxRatio;
                qreal delta = qSqrt(qPow(xINmm,2) + qPow(yINmm,2));
                sacade = qAtan(delta/monitorGeometry.distanceToMonitorInMilimiters)*180.0/3.141516;
            }
            lastX = x;
            lastY = y;
            return sacade;
        }

    };

    // Sum of the duration of a fixation list.
    qreal getGaze(const Fixations &fixations);

    // Calculate the sampling frequency.
    qreal calculateSamplingFrequency (const DataMatrix &eventData, qint32 timeCol);

};

#endif // EDPBASE_H
