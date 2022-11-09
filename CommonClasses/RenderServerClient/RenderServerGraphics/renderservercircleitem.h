#ifndef RENDERSERVERCIRCLEITEM_H
#define RENDERSERVERCIRCLEITEM_H

#include <QString>
#include "renderserveritem.h"

class RenderServerCircleItem : public RenderServerItem
{
public:
    RenderServerCircleItem(qreal x, qreal y, qreal w, qreal h);
    void setPos (qreal x, qreal y) override;
    void render(RenderServerPacket *packet) const override;

private:
    qreal R;
    qreal center_x;
    qreal center_y;

    void updateBRect();
};

#endif // RENDERSERVERCIRCLEITEM_H
