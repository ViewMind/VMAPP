#ifndef FREQUENCYANDDISPERSIONSWEEPER_H
#define FREQUENCYANDDISPERSIONSWEEPER_H

#include <QThread>
#include "datasetdownsampler.h"
#include "../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/edpreading.h"
#include "../../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h"

#define OUTPUT_SWEEP_FILE_BASE "sweep_results"

class FrequencyAndDispersionSweeper: public QThread
{
    Q_OBJECT
public:

    struct SweepParameters{
        QString fileName;
        int startFrequency;
        int frequencyStep;
        int startMaxDispersionValue;
        int endMaxDispersionValue;
    };

    FrequencyAndDispersionSweeper();

    void run() override;

    void setSweepParameters(const SweepParameters &sp);

    QString getError() const {return error;}


signals:
    void updateProgress(qint32 percent);

private:
    SweepParameters sweepParameters;
    QString error;
    qint32  stepCounter;
    qint32  totalNumberOfSteps;
    QList<qint32> frequencyList;
    QList<qint32> maxDispersionList;
    QList< QList<qint32> > totalNumerOfFixations;

    QList<qint32> doMaxDispersionSweep(const QString &fileName);

};

#endif // FREQUENCYANDDISPERSIONSWEEPER_H
