#include "renderserverarrowitem.h"

RenderServerArrowItem::RenderServerArrowItem(qreal x1, qreal y1, qreal x2, qreal y2, qreal arrowHeadLength, qreal arrowHeadHeightMult, bool isTriangleArrow):RenderServerItem(){

    this->x1 = x1;
    this->x2 = x2;
    this->y1 = y1;
    this->y2 = y2;
    this->headHeightMultiplier = arrowHeadHeightMult;
    this->headLength = arrowHeadLength;
    this->isTriangleArrow = isTriangleArrow;
    this->itemType = "Arrow";
    this->updateBRect();

}

void RenderServerArrowItem::setPos(qreal x, qreal y){
    this->x1 = x;
    this->y1 = y;
    this->updateBRect();
}

qreal RenderServerArrowItem::x() const {
    return this->x1;
}

qreal RenderServerArrowItem::y() const {
    return this->y1;
}

void RenderServerArrowItem::render(RenderServerPacket *packet) const {

    QVariantList a;
    if (packet->containsPayloadField(RenderControlPacketFields::SPEC_LIST)){
        a = packet->getPayloadField(RenderControlPacketFields::SPEC_LIST).toList();
    }

    QVariantMap arrow;

    QVariantList x, y;
    x << this->x1 << this->x2;
    y << this->y1 << this->y2;

    arrow[RenderControlPacketFields::TYPE] = GL2DItemType::TYPE_ARROW;
    arrow[RenderControlPacketFields::X] = x;
    arrow[RenderControlPacketFields::Y] = y;
    arrow[RenderControlPacketFields::COLOR] = this->fillColor;
    arrow[RenderControlPacketFields::BORDER_COLOR] = this->borderColor;
    arrow[RenderControlPacketFields::BORDER_WIDTH] = this->borderWidth;
    arrow[RenderControlPacketFields::ARROW_HEAD_HEIGHT] = this->headHeightMultiplier;
    arrow[RenderControlPacketFields::ARROW_HEAD_LENGTH] = this->headLength;
    arrow[RenderControlPacketFields::ARROW_TYPE_TRIANGLE] = this->isTriangleArrow;

    a << arrow;

    packet->setPayloadField(RenderControlPacketFields::SPEC_LIST,a);

}

void RenderServerArrowItem::updateBRect(){

    qreal max_x = qMax(this->x1,this->x2);
    qreal min_x = qMin(this->x1,this->x2);
    qreal max_y = qMax(this->y1,this->y2);
    qreal min_y = qMin(this->y1,this->y2);

    this->bRect.setTop(min_y);
    this->bRect.setLeft(min_x);
    this->bRect.setWidth(max_x - min_x);
    this->bRect.setHeight(max_y - min_y);

}

void RenderServerArrowItem::scale(qreal scale){
    QPointF p1 = this->scaleAPointAroundTFOrigin(this->x1,this->y1,scale);
    QPointF p2 = this->scaleAPointAroundTFOrigin(this->x2,this->y2,scale);
    this->x1 = p1.x();
    this->y1 = p1.y();
    this->x2 = p2.x();
    this->y2 = p2.y();
    this->headHeightMultiplier = this->headHeightMultiplier*scale;
    this->headLength = this->headLength*scale;
    this->updateBRect();
}

void RenderServerArrowItem::moveBy(qreal dx, qreal dy){
    this->x1 = this->x1 + dx;
    this->y1 = this->y1 + dy;
    this->x2 = this->x2 + dx;
    this->y2 = this->y2 + dy;
    this->updateBRect();
}
