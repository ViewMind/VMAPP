#include "renderserveritem.h"

RenderServerItem::RenderServerItem()
{
    this->borderColor = "#000000";
    this->fillColor   = "#000000";
    this->zValue      = 0;
    this->visible     = true;
}

void RenderServerItem::setZValue(double z){
    if (this->itemType == "Circle"){
        qDebug() << "Setting the circle z to " << z;
    }
    this->zValue = static_cast<float>(z);
    if (z <= 0) this->visible = false;
    else this->visible = true;
}


void RenderServerItem::setBrush(const QBrush &brush){
    this->fillColor = brush.color().name();
}

void RenderServerItem::setPen(const QPen &pen){
    this->borderColor = pen.color().name();
    this->borderWidth = static_cast<float>(pen.widthF());
}

QRectF RenderServerItem::boundingRect() const {
    return bRect;
}

bool RenderServerItem::isVisible() const {
    return this->visible;
}

void RenderServerItem::setPos(qreal x, qreal y){
    Q_UNUSED(x)
    Q_UNUSED(y)
}

void RenderServerItem::render(RenderServerPacket *packet) const{
    Q_UNUSED(packet)
}

QString RenderServerItem::getType() const {
    return this->itemType;
}

RenderServerItem::~RenderServerItem(){

}
