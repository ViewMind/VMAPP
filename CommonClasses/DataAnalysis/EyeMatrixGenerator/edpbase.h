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

    // VR Constants. Should be kept the same for ANY VR Implementations.
    static const qreal VR_VIEW_WIDTH;
    static const qreal VR_VIEW_HEIGHT;
    static const qreal VR_VIEW_DISTANCE_TO_MONITOR;
    static const qreal SACADE_NORM_CONSTANT;

    qreal XmmToPxRatio;
    qreal YmmToPxRatio;
    qreal distanceToMonitorInMilimiters;
    qreal resolutionWidth;
    qreal resolutionHeight;
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
    void setFieldingMargin(qint32 fm) {fieldingMargin = fm;}

    QString getOuputMatrixFileName() const { return outputFile; }

    QStringList getFilteredLines() const {return filteredLinesList;}

    FixationList getEyeFixations() const {return eyeFixations;}

    QStringList getSamplingFrequencyCheck() const { return samplingFrequencyCheck; }

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
    qreal fieldingMargin;

    // The object to calculate the fixation
    MovingWindowAlgorithm mwa;

    // Flag that raises if sampling check does not hold.
    QStringList samplingFrequencyCheck;

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
        void reset();
        // This function is left for legacy reasons along with all the pareamters used to compute it.
        qreal calculateSacadeAmplitudeOLD(qreal x, qreal y, const MonitorGeometry &monitorGeometry);
        qreal calculateSacadeAmplitude(qreal x, qreal y, const MonitorGeometry &monitorGeometry);
    };

    // Sum of the duration of a fixation list.
    qreal getGaze(const Fixations &fixations);

    // Calculate the sampling frequency.
    qreal calculateSamplingFrequency (const DataMatrix &eventData, qint32 timeCol);

};

#endif // EDPBASE_H
