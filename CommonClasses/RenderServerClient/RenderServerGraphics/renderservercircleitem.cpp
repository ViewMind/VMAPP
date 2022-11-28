#include "renderservercircleitem.h"

RenderServerCircleItem::RenderServerCircleItem(qreal x, qreal y, qreal w, qreal h):RenderServerItem(){
    Q_UNUSED(h)
    qreal R = w/2;
    qreal center_x = x + R;
    qreal center_y = y + R;

    QStringList renderItemList = itemData.value(RenderControlPacketFields::RENDER_LIST).toStringList();

    renderItemList << RenderControlPacketFields::X
    << RenderControlPacketFields::Y
    << RenderControlPacketFields::COLOR
    << RenderControlPacketFields::BORDER_COLOR
    << RenderControlPacketFields::BORDER_WIDTH
    << RenderControlPacketFields::RADIOUS;

    itemData[RenderControlPacketFields::RENDER_LIST] = renderItemList;

    QVariantList xl; xl << center_x;
    QVariantList yl; yl << center_y;
    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;
    itemData[RenderControlPacketFields::RADIOUS] = R;

    itemData[RenderControlPacketFields::TYPE]      = GL2DItemType::TYPE_CIRCLE;
    itemData[RenderControlPacketFields::TYPE_NAME] = RenderServerItemTypeName::CIRCLE;

    updateBRect();
}

RenderServerCircleItem::RenderServerCircleItem(const QVariantMap &idata): RenderServerItem(idata) {
    updateBRect();
}

// Since the interface is that of the QGraphicsEllipse Item, the x and y coordinates
// used are that of the top left corner of the item. So they need to be compesated.
void RenderServerCircleItem::setPos(qreal x, qreal y){

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    xl[0] = x + itemData.value(RenderControlPacketFields::RADIOUS).toReal();
    yl[0] = y + itemData.value(RenderControlPacketFields::RADIOUS).toReal();

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;

    updateBRect();
}

qreal RenderServerCircleItem::x() const {    
    return itemData.value(RenderControlPacketFields::X).toList().first().toReal() - itemData.value(RenderControlPacketFields::RADIOUS).toReal();
}

qreal RenderServerCircleItem::y() const {
    return itemData.value(RenderControlPacketFields::Y).toList().first().toReal() - itemData.value(RenderControlPacketFields::RADIOUS).toReal();
}

void RenderServerCircleItem::updateBRect(){

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    qreal R = itemData.value(RenderControlPacketFields::RADIOUS).toReal();
    qreal center_x = xl[0].toReal();
    qreal center_y = yl[0].toReal();

    bRect.setLeft(center_x - R);
    bRect.setTop(center_y - R);
    bRect.setWidth(2*R);
    bRect.setHeight(2*R);
}

void RenderServerCircleItem::scale(qreal scale){

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    qreal R = itemData.value(RenderControlPacketFields::RADIOUS).toReal();
    qreal center_x = xl[0].toReal();
    qreal center_y = yl[0].toReal();

    // Assuming scale change maitaining it's center.
    if (scale < 0) return;
    R = R*scale;
    QPointF newCenter = scaleAPointAroundTFOrigin(center_x,center_y,scale);
    xl[0] = newCenter.x();
    yl[0] = newCenter.y();

    itemData[RenderControlPacketFields::RADIOUS] = R;
    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;

    updateBRect();
}

void RenderServerCircleItem::moveBy(qreal dx, qreal dy) {

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    xl[0] = xl[0].toReal() + dx;
    yl[0] = yl[0].toReal() + dy;

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;

    updateBRect();
}
