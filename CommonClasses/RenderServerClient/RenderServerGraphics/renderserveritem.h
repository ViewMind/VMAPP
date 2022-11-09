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


class RenderServerItem
{
public:
    RenderServerItem();

    virtual void setPos(qreal x, qreal y);
    virtual ~RenderServerItem();

    /**
     * @brief setZValue
     * @param z
     * In QGraphicsItems this controls the z value of 2D items and defines what is seen and what is not.
     * So in here, even though the value is kept it is used as way to make the item visible or not.
     */
    void setZValue(double z);
    bool isVisible() const;

    void setBrush(const QBrush &brush);
    void setPen(const QPen &pen);

    /**
     * @brief render
     * @param packet
     *
     * "Renders" the item to it's specs on a render server packet.
     */
    virtual void render(RenderServerPacket *packet) const;
    QString getType() const;
    QRectF boundingRect() const;

protected:
    QString fillColor;
    QString borderColor;
    float borderWidth;
    float zValue;
    bool visible;
    QRectF bRect;
    QString itemType;

};

#endif // RENDERSERVERITEM_H
