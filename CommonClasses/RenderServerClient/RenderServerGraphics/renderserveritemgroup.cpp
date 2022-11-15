#include "renderserveritemgroup.h"

RenderServerItemGroup::RenderServerItemGroup(const QList<RenderServerItem*> &items) {
    this->items = items;
}

void RenderServerItemGroup::moveBy(qreal dx, qreal dy){
    for (qint32 i = 0; i < this->items.size(); i++){
        this->items[i]->moveBy(dx,dy);
    }
}

void RenderServerItemGroup::setTransformOriginPoint(qreal x, qreal y){
    for (qint32 i = 0; i < this->items.size(); i++){
        this->items[i]->setTransformOriginPoint(x,y);
    }
}

void RenderServerItemGroup::setScale(qreal scale){
    for (qint32 i = 0; i < this->items.size(); i++){
        this->items[i]->scale(scale);
    }
}
