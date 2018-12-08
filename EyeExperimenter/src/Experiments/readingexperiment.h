#ifndef READINGEXPERIMENT_H
#define READINGEXPERIMENT_H



#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QPainter>
#include <QImage>
#include <QFontMetrics>
#include "experiment.h"
#include "../../CommonClasses/Experiments/readingmanager.h"


class ReadingExperiment : public Experiment
{
public:
    ReadingExperiment(QWidget *parent = 0);
    ~ReadingExperiment();
    bool startExperiment(ConfigurationManager *c);
    void togglePauseExperiment();
    bool loadConfiguration(const QString &fileName);

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data);

protected:
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);

private:

    // Handle to the Reading manager
    ReadingManager *m;

    // The current state.
    ReadingManager::QuestionState qstate;

    // The pointer to the current question.
    qint32 currentQuestion;

    // Advances the state machine.
    void determineQuestionState();

    // See if the current data point is withing tolerance radious of the current target
    bool isPointWithInTolerance(int x, int y);

    // The logic for actully advancing to next phrase
    void advanceToTheNextPhrase();

    // The function that actually writes the data to the HDD. Returns false if there is a problem.
    void appendEyeTrackerData(const EyeTrackerData &data,
                          qint32 wordIndexR,
                          qint32 characterIndexR,
                          qint32 wordIndexL,
                          qint32 characterIndexL,
                          qint32 sentenceLength);

    // Save selected answer in multiple choice question
    void saveAnswer(qint32 selected);

    // Create eye fixation matrix, called at the end of the experiment.
    bool createEyeFixationMatrix();

    QList<qint32> calculateWordAndCharacterPostion(qint32 x, qint32 y);

};

#endif // READINGEXPERIMENT_H
