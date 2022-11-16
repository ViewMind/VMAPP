#include "renderservercircleitem.h"

RenderServerCircleItem::RenderServerCircleItem(qreal x, qreal y, qreal w, qreal h):RenderServerItem(){
    Q_UNUSED(h)
    this->R = w/2;
    this->center_x = x + this->R;
    this->center_y = y + this->R;
    this->itemType = RenderServerItemTypeName::CIRCLE;
    this->updateBRect();
}

RenderServerCircleItem::RenderServerCircleItem(const QVariantMap &itemData): RenderServerItem(itemData) {
    this->center_x = itemData.value(RenderControlPacketFields::X).toReal();
    this->center_y = itemData.value(RenderControlPacketFields::Y).toReal();
    this->R = itemData.value(RenderControlPacketFields::RADIOUS).toReal();
    this->updateBRect();
}

QVariantMap RenderServerCircleItem::getItemData() const {
    QVariantMap itemData = RenderServerItem::getItemData();
    itemData[RenderControlPacketFields::X] = this->center_x;
    itemData[RenderControlPacketFields::Y] = this->center_y;
    itemData[RenderControlPacketFields::RADIOUS] = this->R;
    return itemData;
}


// Since the interface is that of the QGraphicsEllipse Item, the x and y coordinates
// used are that of the top left corner of the item. So they need to be compesated.
void RenderServerCircleItem::setPos(qreal x, qreal y){
    this->center_x = x + this->R;
    this->center_y = y + this->R;
    this->updateBRect();
}

qreal RenderServerCircleItem::x() const {
    return this->center_x - this->R;
}

qreal RenderServerCircleItem::y() const {
    return this->center_y - this->R;
}

void RenderServerCircleItem::render(RenderServerPacket *packet) const {

    QVariantList c;
    if (packet->containsPayloadField(RenderControlPacketFields::SPEC_LIST)){
        c = packet->getPayloadField(RenderControlPacketFields::SPEC_LIST).toList();
    }

    //qDebug() << "Redering circle";

    QVariantList x,y;
    x << this->center_x;
    y << this->center_y;

    QVariantMap spec;
    spec[RenderControlPacketFields::TYPE] = GL2DItemType::TYPE_CIRCLE;
    spec[RenderControlPacketFields::X] = x;
    spec[RenderControlPacketFields::Y] = y;
    spec[RenderControlPacketFields::RADIOUS] = this->R;
    spec[RenderControlPacketFields::COLOR] = this->fillColor;
    spec[RenderControlPacketFields::BORDER_COLOR] = this->borderColor;
    spec[RenderControlPacketFields::BORDER_WIDTH] = this->borderWidth;

    c << spec;
    packet->setPayloadField(RenderControlPacketFields::SPEC_LIST,c);

}

void RenderServerCircleItem::updateBRect(){
    this->bRect.setLeft(this->center_x - this->R);
    this->bRect.setTop(this->center_y - this->R);
    this->bRect.setWidth(2*this->R);
    this->bRect.setHeight(2*this->R);
}

void RenderServerCircleItem::scale(qreal scale){
    // Assuming scale change maitaining it's center.
    if (scale < 0) return;
    this->R = this->R*scale;
    QPointF newCenter = this->scaleAPointAroundTFOrigin(this->center_x,this->center_y,scale);
    this->center_x = newCenter.x();
    this->center_y = newCenter.y();
    this->updateBRect();
}

void RenderServerCircleItem::moveBy(qreal dx, qreal dy) {
    this->center_x = this->center_x + dx;
    this->center_y = this->center_y + dy;
    this->updateBRect();
}
