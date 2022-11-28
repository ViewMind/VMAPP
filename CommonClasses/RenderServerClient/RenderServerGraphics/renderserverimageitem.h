#ifndef RENDERSERVERIMAGEITEM_H
#define RENDERSERVERIMAGEITEM_H

#include "renderserveritem.h"

class RenderServerImageItem : public RenderServerItem
{
public:

    RenderServerImageItem(const QString &fname);
    RenderServerImageItem(const QVariantMap &idata);

    void setDimensions(bool fitToWidth, qreal value_to_fit, qreal w, qreal h);

    void setPos(qreal x, qreal y) override;
    void scale(qreal scale) override;
    void moveBy(qreal dx, qreal dy) override;
    qreal x() const override;
    qreal y() const override;

private:

    void updateBRect();



};

#endif // RENDERSERVERIMAGEITEM_H
