#include "renderservertriangleitem.h"

RenderServerTriangleItem::RenderServerTriangleItem(qreal x1, qreal y1, qreal x2, qreal y2, qreal x3, qreal y3):RenderServerItem() {
    this->x1 = x1;
    this->y1 = y1;
    this->x2 = x2;
    this->y2 = y2;
    this->x3 = x3;
    this->y3 = y3;
    this->itemType = "Triangle";
}

void RenderServerTriangleItem::setPos(qreal x, qreal y){
    this->x1 = x;
    this->y1 = y;
}

void RenderServerTriangleItem::render(RenderServerPacket *packet) const {

    QVariantList t;
    if (packet->containsPayloadField(RenderControlPacketFields::SPEC_LIST)){
        t = packet->getPayloadField(RenderControlPacketFields::SPEC_LIST).toList();
    }

    QVariantMap spec;
    QVariantList x,y;
    x << this->x1 << this->x2 << this->x3;
    y << this->y1 << this->y2 << this->y3;

    spec[RenderControlPacketFields::TYPE] = GL2DItemType::TYPE_TRIANGLE;
    spec[RenderControlPacketFields::X] = x;
    spec[RenderControlPacketFields::Y] = y;
    spec[RenderControlPacketFields::COLOR] = this->fillColor;
    spec[RenderControlPacketFields::BORDER_COLOR] = this->borderColor;
    spec[RenderControlPacketFields::BORDER_WIDTH] = this->borderWidth;

    t << spec;
    packet->setPayloadField(RenderControlPacketFields::SPEC_LIST,t);

}

void RenderServerTriangleItem::updateBRect(){
    qreal max_x = qMax(this->x1,qMax(this->x2,this->x3));
    qreal min_x = qMin(this->x1,qMin(this->x2,this->x3));
    qreal max_y = qMax(this->y1,qMax(this->y2,this->y3));
    qreal min_y = qMin(this->y1,qMin(this->y2,this->y3));

    this->bRect.setTop(min_y);
    this->bRect.setLeft(min_x);
    this->bRect.setWidth(max_x - min_x);
    this->bRect.setHeight(max_y - min_y);
}
