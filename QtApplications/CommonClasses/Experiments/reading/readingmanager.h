#ifndef READINGMANGER_H
#define READINGMANGER_H

#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsEllipseItem>
#include <QFontMetrics>
#include "readingparser.h"
#include "../experimentdatapainter.h"

class ReadingManager: public ExperimentDataPainter
{
public:

    // State machine for the reading trials
    typedef enum {QS_POINT, QS_PHRASE, QS_QUESTION, QS_INFORMATION} QuestionState;

    ReadingManager();

    // Reimplemented functions for common interface.
    bool parseExpConfiguration(const QString &contents) override;
    void init(qreal display_resolution_width, qreal display_resolution_height)  override;
    void configure(const QVariantMap &configuration) override;
    qint32 size() const override {return phrases.size();}
    qreal  sizeToProcess() const override {return size();}

    // Individual functionality.
    ReadingParser::Phrase getPhrase(qint32 index) const { return phrases.at(index);}
    void drawPhrase(QuestionState qstate, qint32 currentQuestion, bool simpleDraw = false);
    bool drawPhrase(const QString &idStr);
    qint32 getEscapeX() const { return static_cast<qint32>(escapeX); }
    qint32 getEscapeY() const { return static_cast<qint32>(escapeY); }
    qint32 getCurrentTargetX() const { return static_cast<qint32>(currentTargetX); }
    qint32 getCurrentTargetY() const { return static_cast<qint32>(currentTargetY); }
    qint32 getValidClickAreaSize() const { return validClickAreas.size(); }
    void highlightOption(const QPoint &point);
    qint32 getCharIndex(qint32 x);
    QString getDescription() const {return description;}
    QList<QRectF> getOptionTargetBoxes() const;
    void enableDemoMode() override;

    // Configuration key for VR Enabled which is necesssary to determine font type and size. (When using mouse the monitor is used).
    static const char * CONFIG_IS_USING_VR;

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
    qreal escapeX, escapeY;

    // These variables hold where the subject SHOULD Look to advance
    qreal currentTargetX, currentTargetY;

    // When options are presented this list contains the clickable options
    QList<QRectF> validClickAreas;
    QList<QGraphicsRectItem*> rectButtonPointers;

    // Used to provide some area extra around an option so as to provide a larger clickable target
    const qint32 AIR = 20;

    // Values selected from
    qreal escape_point_k;
    qint32 font_size;
    QString font_name;

    // VR vs Monitor dependant constants.
    static const int VR_READING_FONT_SIZE;
    static const qreal VR_READING_ESCAPE_POINT_XY_K;
    static const char *VR_READING_FONT_NAME;

    static const int READING_FONT_SIZE;
    static const qreal READING_ESCAPE_POINT_XY_K;
    static const char *READING_FONT_NAME;

    static const char * SELECTED_BACKGROUND_COLOR;

};

#endif // READINGMANGER_H
