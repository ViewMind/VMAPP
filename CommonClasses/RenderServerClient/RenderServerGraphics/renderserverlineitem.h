#ifndef RENDERSERVERLINEITEM_H
#define RENDERSERVERLINEITEM_H

#include "renderserveritem.h"

class RenderServerLineItem : public RenderServerItem
{
public:
    RenderServerLineItem(qreal x1, qreal y1, qreal x2, qreal y2);
    RenderServerLineItem(const QVariantMap &itemData);
    QVariantMap getItemData() const override;

    void setPos (qreal x, qreal y) override;
    void render(RenderServerPacket *packet) const override;
    void scale(qreal scale) override;
    void moveBy(qreal dx, qreal dy) override;
    qreal x() const override;
    qreal y() const override;

    void setRoundCapUse(bool enable);

private:
    qreal x1;
    qreal y1;
    qreal x2;
    qreal y2;
    void updateBRect();

};

#endif // RENDERSERVERLINEITEM_H
