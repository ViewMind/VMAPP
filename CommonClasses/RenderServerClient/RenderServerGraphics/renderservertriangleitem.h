#ifndef RENDERSERVERTRIANGLEITEM_H
#define RENDERSERVERTRIANGLEITEM_H

#include "renderserveritem.h"

class RenderServerTriangleItem : public RenderServerItem
{

public:

    RenderServerTriangleItem(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3);

    void setPos(qreal x, qreal y) override;
    void render(RenderServerPacket *packet) const override;

private:
    qreal x1;
    qreal x2;
    qreal x3;
    qreal y1;
    qreal y2;
    qreal y3;

    void updateBRect();

};

#endif // RENDERSERVERTRIANGLEITEM_H
