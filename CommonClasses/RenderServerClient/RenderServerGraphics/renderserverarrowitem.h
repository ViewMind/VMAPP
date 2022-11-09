#ifndef RENDERSERVERARROWITEM_H
#define RENDERSERVERARROWITEM_H

#include "renderserveritem.h"

class RenderServerArrowItem : public RenderServerItem
{

public:

    RenderServerArrowItem(qreal x1, qreal y1, qreal x2, qreal y2, qreal arrowHeadLength, qreal arrowHeadHeightMult, bool isTriangleArrow);

    void setPos(qreal x, qreal y) override;
    void render(RenderServerPacket *packet) const override;

private:

    qreal x1;
    qreal y1;
    qreal x2;
    qreal y2;
    qreal headLength;
    qreal headHeightMultiplier;
    bool isTriangleArrow;

    void updateBRect();

};

#endif // RENDERSERVERARROWITEM_H
