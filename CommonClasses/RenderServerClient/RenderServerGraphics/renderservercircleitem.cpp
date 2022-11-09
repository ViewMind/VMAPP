#include "renderservercircleitem.h"

RenderServerCircleItem::RenderServerCircleItem(qreal x, qreal y, qreal w, qreal h):RenderServerItem(){
    Q_UNUSED(h)
    this->R = w/2;
    this->center_x = x + this->R;
    this->center_y = y + this->R;
    this->itemType = "Circle";
    this->updateBRect();
}

// Since the interface is that of the QGraphicsEllipse Item, the x and y coordinates
// used are that of the top left corner of the item. So they need to be compesated.
void RenderServerCircleItem::setPos(qreal x, qreal y){
    this->center_x = x + this->R;
    this->center_y = y + this->R;
    this->updateBRect();
}


void RenderServerCircleItem::render(RenderServerPacket *packet) const {

    QVariantList c;
    if (packet->containsPayloadField(RenderControlPacketFields::SPEC_LIST)){
        c = packet->getPayloadField(RenderControlPacketFields::SPEC_LIST).toList();
    }

    qDebug() << "Redering circle";

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
