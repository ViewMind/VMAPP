#ifndef READINGMANGER_H
#define READINGMANGER_H

#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QFontMetrics>
#include "readingparser.h"
#include "experimentdatapainter.h"

class ReadingManager: public ExperimentDataPainter
{
public:

    // State machine for the reading trials
    typedef enum {QS_POINT, QS_PHRASE, QS_QUESTION, QS_INFORMATION} QuestionState;

    ReadingManager();

    // Reimplemented functions for common interface.
    bool parseExpConfiguration(const QString &contents);
    void init(ConfigurationManager *c);
    qint32 size() const {return phrases.size();}
    qreal  sizeToProcess() const {return size();}

    // Individual functionality.
    ReadingParser::Phrase getPhrase(qint32 index) const { return phrases.at(index);}
    void drawPhrase(QuestionState qstate, qint32 currentQuestion, bool simpleDraw = false);
    bool drawPhrase(const QString &idStr);
    qint32 getEscapeX() const { return escapeX; }
    qint32 getEscapeY() const { return escapeY; }
    qint32 getCurrentTargetX() const { return currentTargetX; }
    qint32 getCurrentTargetY() const { return currentTargetY; }
    qint32 getValidClickAreaSize() const { return validClickAreas.size(); }
    qint32 isPointContainedInAClickArea(const QPoint &point);
    qint32 getCharIndex(qint32 x);
    QString getDescription() const {return description;}

private:

    // The phrases structure.
    ReadingParser::Phrases phrases;

    // Font for the questions
    QFont questionFont;

    // The width, in pixels of one character.
    qint32 charWidth;

    // Language and description for the selected language file.
    QString description;

    // The current question and the items that are hidden and then brought to front
    QGraphicsSimpleTextItem *phraseToShow;
    QGraphicsEllipseItem *escapePoint;
    QGraphicsEllipseItem *startPoint;

    // The radious of the dot that marks where the next sentence is going to start.
    // and where the escape point is.
    qreal R;
    qint32 escapeX, escapeY;

    // These variables hold where the subject SHOULD Look to advance
    qint32 currentTargetX, currentTargetY;

    // When options are presented this list contains the clickable options
    QList<QRect> validClickAreas;

    // Used to provide some area extra around an option so as to provide a larger clickable target
    const qint32 AIR = 20;

    void enableDemoMode();

};

#endif // READINGMANGER_H
