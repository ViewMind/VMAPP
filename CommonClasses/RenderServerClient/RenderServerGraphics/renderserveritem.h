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
   const QString IMAGE    = "Image";
}

class RenderServerItem
{
public:

    static const qint32 ANIMATION_BEGIN_NO_DELAY = 0;
    static const qint32 ANIMATION_BEGIN_DELAY    = 1;
    static const qint32 ANIMATION_DELAY_END      = 2;
    static const qint32 ANIMATION_END            = 3;

    RenderServerItem();
    RenderServerItem(const QVariantMap &idata);

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

    void setDisplayOnly(bool disp_only);

    /**
     * @brief render
     * @param packet
     *
     * "Renders" the item to it's specs on a render server packet.
     */
    void render(RenderServerPacket *packet) const;

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

    void setItemID(const qint32 &id);
    qint32 getItemID() const;

    /**
     * @brief setReferenceYForTransformations - When doing transformations a regular y axis is assumed. This sets the max vertical resolution so the proper transformation can be done.
     * @param refY
     */
    void setReferenceYForTransformations(qreal refY);

    /**
     * @brief getItemData - List of all values necessary to recreate the item.
     * @return A List of values to be interpreted by the copy constructor of the tiem.
     */
    QVariantMap getItemData() const;

    /**
     * @brief addAnimationParameter - Configures an item parameter to be animated, that is changed linearly at a fixed space. On real value type paramters can be animated.
     * @param name - The name of the animation paramter
     * @param start -  The start value of the parater
     * @param end - The end value of the parameter.
     * @param duration - Number of ms the animation shall last, for the parameter to get from start to end.
     * @param animationInterval - The rate at which the parameter of the item needs to be moved. Used for computing the number of steps.
     * @param delay - Number of ms before actually starting moving the parameter. During this time the parameter will remain in it's initial value.
     * @return The error message if there was one. Otherwise an empty string if all was ok.
     * @details The animation parameter needs to be able to be cast to real value and it the item needs to be have a set ID.
     */
    QString addAnimationParameter(const QString &name, qreal start, qreal end, qreal duration, qreal animationInterval, qreal delay);

    /**
     * @brief animate - Animates all parameters added with addAnimationParameters.
     * @return Retunrs a map of parameters which triggered signals. The value of the map is the signal triggered.
     */
    QMap<QString, qint32> animate();

    /**
     * @brief clearAnimationData - deletes the animation structures.
     */
    void clearAnimationData();

    static QString AnimationSignalType2String(qint32 type);

protected:
    QRectF bRect;
    QVariantMap itemData;

    QPointF scaleAPointAroundTFOrigin(qreal x, qreal y, qreal scale) const;
    QPointF scaleAPointAroundTFOrigin(const QPointF &point, qreal scale) const;




};

#endif // RENDERSERVERITEM_H
