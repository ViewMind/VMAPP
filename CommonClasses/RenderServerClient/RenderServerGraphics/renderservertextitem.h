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
    RenderServerTextItem(const QVariantMap &itemData);


    /**
     * @brief setPos
     * @param x the center of the text field
     * @param y the center of the text field.
     * Unlike the QGraphicsSimpleTextItem, for simplicity this is sets the ceter mas point for where the text will be displayed.
     */
    void setPos (qreal x, qreal y) override;
    void render(RenderServerPacket *packet) const override;
    QVariantMap getItemData() const override;

    void setText(const QString &text);
    void setFont(const QFont &font);
    void setDefaultTextColor(const QColor &col);

    const qreal DEFAULT_LETTER_RATIO = 361.0/601.0;
    const char ALIGN_LEFT   = 'l';
    const char ALIGN_RIGHT  = 'r';
    const char ALIGN_CENTER = 'c';


private:
    qreal x;
    qreal y;
    qreal fontSize;    
    qreal lineSpacing;
    char alignment;
    QString text;

    void computeBoundingRect();
    void computeTopLeftInRenderServer();

};

#endif // RENDERSERVERTEXTITEM_H
