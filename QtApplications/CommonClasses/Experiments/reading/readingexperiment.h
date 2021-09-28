#ifndef READINGEXPERIMENT_H
#define READINGEXPERIMENT_H

#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QPainter>
#include <QImage>
#include <QFontMetrics>
#include "../experiment.h"
#include "readingmanager.h"
#include "../../EyeSelector/eyeselector.h"

class ReadingExperiment : public Experiment
{
public:
    ReadingExperiment(QWidget *parent = nullptr, const QString &study_type = "");
    ~ReadingExperiment() override;

    bool startExperiment(const QString &workingDir, const QString &experimentFile,
                                     const QVariantMap &studyConfig) override;

    bool loadConfiguration(const QString &fileName);

public slots:
    void newEyeDataAvailable(const EyeTrackerData &data) override;

protected:
    void keyPressHandler(int keyPressed) override;
    QString getExperimentDescriptionFile(const QVariantMap &studyConfig) override;

private:

    // Handle to the Reading manager
    ReadingManager *m;

    // Selection state machine for selecting answers with eyes.
    EyeSelector eyeSelector;

    // The current state.
    ReadingManager::QuestionState qstate;

    // The pointer to the current question.
    qint32 currentQuestion;

    // When answering a question the answer get stored in here.
    QString response;

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
                              qint32 characterIndexL);

    QList<qint32> calculateWordAndCharacterPostion(qint32 x, qint32 y);

    // Will be computed as a percent of the size of the width of the drawing area.
    qreal readingHitTolerance;

    static const qreal  K_READING_HIT_TOLERANCE;
    static const qint32 TIME_TO_HOLD_FOR_SELECTION_IN_SECONDS;

    void resetStudy() override;

};

#endif // READINGEXPERIMENT_H
