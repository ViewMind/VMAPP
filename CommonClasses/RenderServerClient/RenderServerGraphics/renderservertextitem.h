#ifndef RENDERSERVERTEXTITEM_H
#define RENDERSERVERTEXTITEM_H

#include "renderserveritem.h"
#include <QFont>

/**
 * @brief The RenderServerTextItem class
 * Provides a common interface with QGraphicsSimpleTextItem.
 * Some functionality does need to be changed in order to properly render
 * the text in the server.
 */


class RenderServerTextItem : public RenderServerItem
{
public:
    RenderServerTextItem(const QString &text);
    RenderServerTextItem(const QVariantMap &idata);


    /**
     * @brief setPos
     * @param x the center of the text field
     * @param y the center of the text field.
     * Unlike the QGraphicsSimpleTextItem, for simplicity this is sets the ceter mas point for where the text will be displayed.
     */
    void setPos (qreal x, qreal y) override;
    void setAlignment(const QString &alignment);

    void setText(const QString &text);
    void setFont(const QFont &font);
    void setDefaultTextColor(const QColor &col);

    qreal x() const override;
    qreal y() const override;

    const qreal DEFAULT_LETTER_RATIO = 361.0/601.0;
    const char ALIGN_LEFT   = 'l';
    const char ALIGN_RIGHT  = 'r';
    const char ALIGN_CENTER = 'c';


private:
    void computeBoundingRect();
};

#endif // RENDERSERVERTEXTITEM_H
