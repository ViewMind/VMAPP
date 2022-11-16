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

RenderServerItem::RenderServerItem(const QVariantMap &itemData){
    this->borderColor       =  itemData[RenderControlPacketFields::BORDER_COLOR].toString();
    this->fillColor         =  itemData[RenderControlPacketFields::COLOR].toString()       ;
    this->zValue            =  itemData[RenderControlPacketFields::Z].toReal()             ;
    this->visible           =  itemData[RenderControlPacketFields::VISIBLE].toBool()       ;
    this->tfOrigin          =  itemData[RenderControlPacketFields::TF_ORIGIN].toPointF()   ;
    this->referenceYTF      =  itemData[RenderControlPacketFields::REF_Y].toReal()         ;
    this->roundCaps         =  itemData[RenderControlPacketFields::USE_ROUND_CAPS].toBool();
    this->itemType          =  itemData[RenderControlPacketFields::TYPE].toString();
    this->borderWidth       =  itemData[RenderControlPacketFields::BORDER_WIDTH].toReal();
}

QVariantMap RenderServerItem::getItemData() const {
    QVariantMap itemData;
    itemData[RenderControlPacketFields::BORDER_COLOR]   = this->borderColor;
    itemData[RenderControlPacketFields::COLOR]          = this->fillColor;
    itemData[RenderControlPacketFields::Z]              = this->zValue;
    itemData[RenderControlPacketFields::VISIBLE]        = this->visible;
    itemData[RenderControlPacketFields::TF_ORIGIN]      = this->tfOrigin;
    itemData[RenderControlPacketFields::REF_Y]          = this->referenceYTF;
    itemData[RenderControlPacketFields::USE_ROUND_CAPS] = this->roundCaps;
    itemData[RenderControlPacketFields::BORDER_WIDTH]   = this->borderWidth;
    itemData[RenderControlPacketFields::TYPE]           = this->itemType;

    return itemData;
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
