#ifndef RENDERSERVERTRIANGLEITEM_H
#define RENDERSERVERTRIANGLEITEM_H

#include "renderserveritem.h"

class RenderServerTriangleItem : public RenderServerItem
{

public:

    RenderServerTriangleItem(const QPolygonF &triangle);
    RenderServerTriangleItem(const QVariantMap &itemData);
    QVariantMap getItemData() const override;

    void setPos (qreal x, qreal y) override;
    void render(RenderServerPacket *packet) const override;
    void scale(qreal scale) override;
    void moveBy(qreal dx, qreal dy) override;
    qreal x() const override;
    qreal y() const override;

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
