#include "renderserveritem.h"

RenderServerItem::RenderServerItem()
{
    this->borderColor = "#000000";
    this->fillColor   = "#000000";
    this->zValue      = 1.0;
    this->visible     = true;
    this->tfOrigin.setX(0);
    this->tfOrigin.setY(0);
    this->referenceYTF = 0;
    this->roundCaps = false;
}

void RenderServerItem::setZValue(double z){
    this->zValue =z;
    if (z <= 0) this->visible = false;
    else this->visible = true;
}

qreal RenderServerItem::z() const {
    return this->zValue;
}

void RenderServerItem::setVisible(bool visible){
    this->visible = visible;
}

void RenderServerItem::setTransformOriginPoint(qreal x, qreal y){
    this->tfOrigin.setX(x);
    this->tfOrigin.setY(y);
}


void RenderServerItem::setBrush(const QBrush &brush){
    this->fillColor = brush.color().name(QColor::HexArgb);
}

void RenderServerItem::setPen(const QPen &pen){
    this->borderColor = pen.color().name(QColor::HexArgb);
    this->borderWidth = pen.widthF();
    this->roundCaps = (pen.capStyle() == Qt::RoundCap);
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

void RenderServerItem::scale(qreal scale) {
    Q_UNUSED(scale)
}

void RenderServerItem::moveBy(qreal dx, qreal dy){
    Q_UNUSED(dx)
    Q_UNUSED(dy)
}

void RenderServerItem::setReferenceYForTransformations(qreal refY){
    this->referenceYTF = refY;
}


QString RenderServerItem::getType() const {
    return this->itemType;
}

qreal RenderServerItem::x() const{
    return 0;
}


qreal RenderServerItem::y() const {
    return 0;
}

QPointF RenderServerItem::scaleAPointAroundTFOrigin(qreal x, qreal y, qreal scale) const{
    return this->scaleAPointAroundTFOrigin(QPointF(x,y),scale);
}

QPointF RenderServerItem::scaleAPointAroundTFOrigin(const QPointF &point, qreal scale) const{
    qreal y = this->referenceYTF - point.y();
    qreal oy = this->referenceYTF - this->tfOrigin.y();
    qreal newX = point.x()*scale - this->tfOrigin.x()*scale + this->tfOrigin.x();
    qreal newY = y*scale - oy*scale + oy;

    //qDebug() << "TRANSFORM SCALING. Input: " << point << ". Output: " << QPointF(newX, this->referenceYTF - newY)  << ". Parameters: SCALE: " << scale << ". Origin: " << tfOrigin;

    return QPointF(newX, this->referenceYTF - newY);
    //return QPointF(newX, newY);
}


RenderServerItem::~RenderServerItem(){

}
