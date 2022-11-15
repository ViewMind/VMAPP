#include "renderserverlineitem.h"

RenderServerLineItem::RenderServerLineItem(qreal x1, qreal y1, qreal x2, qreal y2)
{
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
    this->borderWidth = 1.0;
    this->itemType = "Line";
    this->updateBRect();
}

void RenderServerLineItem::setPos(qreal x, qreal y){
    this->x1 = x;
    this->y1 = y;
    this->updateBRect();
}


void RenderServerLineItem::render(RenderServerPacket *packet) const{
    QVariantList l;
    if (packet->containsPayloadField(RenderControlPacketFields::SPEC_LIST)){
        l = packet->getPayloadField(RenderControlPacketFields::SPEC_LIST).toList();
    }

    QVariantMap spec;

    QVariantList x,y;
    x << this->x1 << this->x2;
    y << this->y1 << this->y2;

    spec[RenderControlPacketFields::TYPE] = GL2DItemType::TYPE_LINE;
    spec[RenderControlPacketFields::X] = x;
    spec[RenderControlPacketFields::Y] = y;
    spec[RenderControlPacketFields::USE_ROUND_CAPS] = this->roundCaps;
    spec[RenderControlPacketFields::COLOR] = this->borderColor;
    spec[RenderControlPacketFields::BORDER_WIDTH] = this->borderWidth;

    l << spec;
    packet->setPayloadField(RenderControlPacketFields::SPEC_LIST,l);
}

void RenderServerLineItem::updateBRect(){

    qreal max_x = qMax(this->x1,this->x2);
    qreal min_x = qMin(this->x1,this->x2);
    qreal max_y = qMax(this->y1,this->y2);
    qreal min_y = qMin(this->y1,this->y2);

    this->bRect.setTop(min_y);
    this->bRect.setLeft(min_x);
    this->bRect.setWidth(max_x - min_x);
    this->bRect.setHeight(max_y - min_y);

}

qreal RenderServerLineItem::x() const {
    return this->x1;
}

qreal RenderServerLineItem::y() const {
    return this->y1;
}

void RenderServerLineItem::scale(qreal scale){
    QPointF p1 = this->scaleAPointAroundTFOrigin(this->x1,this->y1,scale);
    QPointF p2 = this->scaleAPointAroundTFOrigin(this->x2,this->y2,scale);
    this->x1 = p1.x();
    this->y1 = p1.y();
    this->x2 = p2.x();
    this->y2 = p2.y();
    this->updateBRect();
}

void RenderServerLineItem::moveBy(qreal dx, qreal dy){
    this->x1 = this->x1 + dx;
    this->y1 = this->y1 + dy;
    this->x2 = this->x2 + dx;
    this->y2 = this->y2 + dy;
    this->updateBRect();
}
