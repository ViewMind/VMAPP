#ifndef RENDERSERVERITEM_H
#define RENDERSERVERITEM_H

/**
 * @brief The RenderServerItem class
 * Base class for all render server items that will be used to replace QGraphicsItems.
 */

#include <QString>
#include <QPen>
#include <QBrush>
#include <QDebug>

#include "../RenderServerPackets/RenderServerPacketNames.h"
#include "../RenderServerPackets/renderserverpacket.h"

namespace RenderServerItemTypeName {
   const QString CIRCLE   = "Circle";
   const QString LINE     = "Line";
   const QString RECT     = "Rectangle";
   const QString TRIANGLE = "Triangle";
   const QString ARROW    = "Arrow";
   const QString TEXT     = "Text";
}

class RenderServerItem
{
public:

    RenderServerItem();
    RenderServerItem(const QVariantMap &itemData);

    virtual void setPos(qreal x, qreal y);
    virtual ~RenderServerItem();

    /**
     * @brief setZValue
     * @param z
     * In QGraphicsItems this controls the z value of 2D items and defines what is seen and what is not.
     * So in here, even though the value is kept it is used as way to make the item visible or not.
     */
    void setZValue(double z);
    qreal z() const;
    bool isVisible() const;
    void setVisible(bool visible);

    void setBrush(const QBrush &brush);
    void setPen(const QPen &pen);
    void setTransformOriginPoint(qreal x, qreal y);

    /**
     * @brief render
     * @param packet
     *
     * "Renders" the item to it's specs on a render server packet.
     */
    virtual void render(RenderServerPacket *packet) const;

    /**
     * @brief scale - scales the item. It does so differently for each item type. If not implemented for the item, it does nothing.
     * @param scale - Scale factor. Larger than one the item grows in size. Smaller than one the item reduces it's size. Less than zero does nothing.
     */
    virtual void scale(qreal scale);

    /**
     * @brief moveBy - Moves the item on x and y.
     * @param dx How much to move on x
     * @param dy How much to move on y
     */
    virtual void moveBy(qreal dx, qreal dy);

    QString getType() const;
    QRectF boundingRect() const;

    virtual qreal x() const;
    virtual qreal y() const;

    /**
     * @brief setReferenceYForTransformations - When doing transformations a regular y axis is assumed. This sets the max vertical resolution so the proper transformation can be done.
     * @param refY
     */
    void setReferenceYForTransformations(qreal refY);

    /**
     * @brief getItemData - List of all values necessary to recreate the item.
     * @return A List of values to be interpreted by the copy constructor of the tiem.
     */
    virtual QVariantMap getItemData() const;

protected:
    QString fillColor;
    QString borderColor;
    qreal borderWidth;
    qreal zValue;
    bool visible;
    bool roundCaps;
    QRectF bRect;
    QString itemType;
    QPointF tfOrigin;
    qreal referenceYTF;

    QPointF scaleAPointAroundTFOrigin(qreal x, qreal y, qreal scale) const;
    QPointF scaleAPointAroundTFOrigin(const QPointF &point, qreal scale) const;

};

#endif // RENDERSERVERITEM_H
