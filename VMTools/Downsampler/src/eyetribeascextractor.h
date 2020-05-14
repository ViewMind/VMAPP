#ifndef EYETRIBEASCEXTRACTOR_H
#define EYETRIBEASCEXTRACTOR_H

#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QThread>

#include "../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h"
#include "../../../CommonClasses/common.h"

class EyeTribeASCExtractor : public QThread
{
    Q_OBJECT
public:
    EyeTribeASCExtractor();

    struct MDSweepParameters{
        QString fileName;
        qint32 startMD;
        qint32 endMD;
        qreal  resX;
        qreal  resY;
        qreal  resScale;
    };


    QString getError() const {return error;}

    void run() override;

    void setupSweep(const MDSweepParameters &params);
    bool movingWindowOnASCFile(const QString &fileName, qint32 maxDispersionToUse);
    bool generateFixationFileFromASC(const QString &fileName, qreal resolutionX, qreal  resolutionY);
    bool genrateCSVFixFileBySentences(const QString &fileName, qint32 maxDispersion);

signals:
    void updateProgress(qint32 value);

private:
    QString error;
    QString ascFileToSweep;
    qreal resolutionWidth;
    qreal resoltuionHeight;
    qreal normResolutionScale;
    qint32 mdStart;
    qint32 mdEnd;
    bool saveStringList(const QStringList &list, const QString &outFileName);
};

#endif // EYETRIBEASCEXTRACTOR_H
