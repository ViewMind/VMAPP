#ifndef RENDERSERVERITEMGROUP_H
#define RENDERSERVERITEMGROUP_H

#include "renderserveritem.h"

class RenderServerItemGroup
{
public:

    RenderServerItemGroup(const QList<RenderServerItem*> &items);

    void setTransformOriginPoint(qreal x, qreal y);
    void setScale(qreal scale);
    void moveBy(qreal dx, qreal dy);

private:
    QList<RenderServerItem*> items;

};

#endif // RENDERSERVERITEMGROUP_H
