#include "renderserverrectitem.h"

RenderServerRectItem::RenderServerRectItem(qreal x, qreal y, qreal width, qreal height):RenderServerItem() {
    this->top = y;
    this->left = x;
    this->width = width;
    this->height = height;
    this->itemType = "Rect";
    //qDebug() << x << y << width << height;
}

void RenderServerRectItem::setPos(qreal x, qreal y){
    this->top = y;
    this->left = x;
}

void RenderServerRectItem::updateBRect() {
    this->bRect.setTop(this->top);
    this->bRect.setLeft(this->left);
    this->bRect.setWidth(this->width);
    this->bRect.setHeight(this->height);
}


void RenderServerRectItem::render(RenderServerPacket *packet) const {


    QVariantList b;
    if (packet->containsPayloadField(RenderControlPacketFields::SPEC_LIST)){
        b = packet->getPayloadField(RenderControlPacketFields::SPEC_LIST).toList();
    }

    // qDebug() << "Rendeing Box" << top << left << width << height;

    QVariantMap spec;

    QVariantList x,y;
    x << this->left;
    y << this->top;

    spec[RenderControlPacketFields::X] = x;
    spec[RenderControlPacketFields::Y] = y;

    spec[RenderControlPacketFields::TYPE] = GL2DItemType::TYPE_RECT;
    spec[RenderControlPacketFields::TOP] = this->top;
    spec[RenderControlPacketFields::LEFT] = this->left;
    spec[RenderControlPacketFields::WIDTH] = this->width;
    spec[RenderControlPacketFields::HEIGHT] = this->height;
    spec[RenderControlPacketFields::COLOR] = this->fillColor;
    spec[RenderControlPacketFields::BORDER_COLOR] = this->borderColor;
    spec[RenderControlPacketFields::BORDER_WIDTH] = this->borderWidth;

    b << spec;
    packet->setPayloadField(RenderControlPacketFields::SPEC_LIST,b);

}
