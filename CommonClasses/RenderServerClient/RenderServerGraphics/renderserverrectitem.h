#ifndef RENDERSERVERRECTITEM_H
#define RENDERSERVERRECTITEM_H

#include "renderserveritem.h"

class RenderServerRectItem : public RenderServerItem
{

public:
    RenderServerRectItem(qreal x, qreal y, qreal width, qreal height);
    RenderServerRectItem(const QVariantMap &itemData);
    QVariantMap getItemData() const override;


    void setPos(qreal x, qreal y) override;
    void render(RenderServerPacket *packet) const override;
    void scale(qreal scale) override;
    void moveBy(qreal dx, qreal dy) override;
    qreal x() const override;
    qreal y() const override;

private:
    qreal top;
    qreal left;
    qreal width;
    qreal height;

    void updateBRect();

};

#endif // RENDERSERVERRECTITEM_H
