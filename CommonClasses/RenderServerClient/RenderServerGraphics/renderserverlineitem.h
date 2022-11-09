#ifndef RENDERSERVERLINEITEM_H
#define RENDERSERVERLINEITEM_H

#include "renderserveritem.h"

class RenderServerLineItem : public RenderServerItem
{
public:
    RenderServerLineItem(qreal x1, qreal y1, qreal x2, qreal y2);
    void setPos (qreal x, qreal y) override;
    void render(RenderServerPacket *packet) const override;

private:
    qreal x1;
    qreal y1;
    qreal x2;
    qreal y2;

    void updateBRect();

};

#endif // RENDERSERVERLINEITEM_H
