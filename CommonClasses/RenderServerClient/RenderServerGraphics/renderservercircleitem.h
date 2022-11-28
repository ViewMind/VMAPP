#ifndef RENDERSERVERCIRCLEITEM_H
#define RENDERSERVERCIRCLEITEM_H

#include <QString>
#include "renderserveritem.h"

class RenderServerCircleItem : public RenderServerItem
{
public:
    RenderServerCircleItem(qreal x, qreal y, qreal w, qreal h);
    RenderServerCircleItem(const QVariantMap &idata);

    void setPos (qreal x, qreal y) override;
    void scale(qreal scale) override;
    void moveBy(qreal x, qreal y) override;
    qreal x() const override;
    qreal y() const override;

private:
    void updateBRect();
};

#endif // RENDERSERVERCIRCLEITEM_H
