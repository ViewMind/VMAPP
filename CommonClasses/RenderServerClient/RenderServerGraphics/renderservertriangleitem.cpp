#include "renderservertriangleitem.h"

RenderServerTriangleItem::RenderServerTriangleItem(const QPolygonF &triangle): RenderServerItem()
{

    this->x1 = 0;
    this->x2 = 0;
    this->x3 = 0;
    this->y1 = 0;
    this->y2 = 0;
    this->y3 = 0;

    if (triangle.size() < 3) return;

    this->x1 = triangle.at(0).x();
    this->y1 = triangle.at(0).y();

    this->x2 = triangle.at(1).x();
    this->y2 = triangle.at(1).y();

    this->x3 = triangle.at(2).x();
    this->y3 = triangle.at(2).y();

    this->itemType = "Triangle";

}


void RenderServerTriangleItem::setPos (qreal x, qreal y){
    qreal dx = x - this->x1;
    qreal dy = y - this->y1;
    this->moveBy(dx,dy);
}

void RenderServerTriangleItem::render(RenderServerPacket *packet) const {

    QVariantList t;
    if (packet->containsPayloadField(RenderControlPacketFields::SPEC_LIST)){
        t = packet->getPayloadField(RenderControlPacketFields::SPEC_LIST).toList();
    }

    QVariantMap tri;

    QVariantList x, y;
    x << this->x1 << this->x2 << this->x3;
    y << this->y1 << this->y2 << this->y3;

    tri[RenderControlPacketFields::TYPE] = GL2DItemType::TYPE_TRIANGLE;
    tri[RenderControlPacketFields::X] = x;
    tri[RenderControlPacketFields::Y] = y;
    tri[RenderControlPacketFields::COLOR] = this->fillColor;
    tri[RenderControlPacketFields::BORDER_COLOR] = this->borderColor;
    tri[RenderControlPacketFields::BORDER_WIDTH] = this->borderWidth;

    t << tri;

    packet->setPayloadField(RenderControlPacketFields::SPEC_LIST,t);

}

void RenderServerTriangleItem::scale(qreal scale){
    QPointF p1 = this->scaleAPointAroundTFOrigin(this->x1,this->y1,scale);
    QPointF p2 = this->scaleAPointAroundTFOrigin(this->x2,this->y2,scale);
    QPointF p3 = this->scaleAPointAroundTFOrigin(this->x3,this->y3,scale);
    this->x1 = p1.x();
    this->y1 = p1.y();
    this->x2 = p2.x();
    this->y2 = p2.y();
    this->x3 = p3.x();
    this->y3 = p3.y();
    this->updateBRect();
}

void RenderServerTriangleItem::moveBy(qreal dx, qreal dy){
    this->x1 = this->x1 + dx;
    this->y1 = this->y1 + dy;
    this->x2 = this->x2 + dx;
    this->y2 = this->y2 + dy;
    this->x3 = this->x3 + dx;
    this->y3 = this->y3 + dy;
    this->updateBRect();
}

qreal RenderServerTriangleItem::x() const {
    return this->x1;
}

qreal RenderServerTriangleItem::y()  const {
    return this->y1;
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
