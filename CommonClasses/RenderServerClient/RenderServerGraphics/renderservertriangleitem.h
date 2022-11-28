#ifndef RENDERSERVERTRIANGLEITEM_H
#define RENDERSERVERTRIANGLEITEM_H

#include "renderserveritem.h"

class RenderServerTriangleItem : public RenderServerItem
{

public:

    RenderServerTriangleItem(const QPolygonF &triangle);
    RenderServerTriangleItem(const QVariantMap &idata);

    void setPos (qreal x, qreal y) override;
    void scale(qreal scale) override;
    void moveBy(qreal dx, qreal dy) override;
    qreal x() const override;
    qreal y() const override;

private:

    void updateBRect();

};

#endif // RENDERSERVERTRIANGLEITEM_H
