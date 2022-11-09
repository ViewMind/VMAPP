#ifndef RENDERSERVERRECTITEM_H
#define RENDERSERVERRECTITEM_H

#include "renderserveritem.h"

class RenderServerRectItem : public RenderServerItem
{

public:
    RenderServerRectItem(qreal x, qreal y, qreal width, qreal height);

    void setPos(qreal x, qreal y) override;
    void render(RenderServerPacket *packet) const override;

private:
    qreal top;
    qreal left;
    qreal width;
    qreal height;

    void updateBRect();

};

#endif // RENDERSERVERRECTITEM_H
