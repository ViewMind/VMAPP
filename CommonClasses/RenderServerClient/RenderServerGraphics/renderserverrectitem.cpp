#include "renderserverrectitem.h"

RenderServerRectItem::RenderServerRectItem(qreal x, qreal y, qreal width, qreal height):RenderServerItem() {

    QStringList renderItemList = itemData.value(RenderControlPacketFields::RENDER_LIST).toStringList();

    renderItemList << RenderControlPacketFields::X
                   << RenderControlPacketFields::Y
                   << RenderControlPacketFields::COLOR
                   << RenderControlPacketFields::BORDER_COLOR
                   << RenderControlPacketFields::BORDER_WIDTH
                   << RenderControlPacketFields::WIDTH
                   << RenderControlPacketFields::HEIGHT;

    itemData[RenderControlPacketFields::RENDER_LIST] = renderItemList;

    QVariantList xl; xl << x;
    QVariantList yl; yl << y;
    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;
    itemData[RenderControlPacketFields::WIDTH] = width;
    itemData[RenderControlPacketFields::HEIGHT] = height;

    itemData[RenderControlPacketFields::TYPE]      = GL2DItemType::TYPE_RECT;
    itemData[RenderControlPacketFields::TYPE_NAME] = RenderServerItemTypeName::RECT;

}

void RenderServerRectItem::setPos(qreal x, qreal y){
    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    xl[0] = x;
    yl[0] = y;

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;
}

void RenderServerRectItem::updateBRect() {
    bRect.setTop(itemData[RenderControlPacketFields::Y].toList().first().toReal());
    bRect.setLeft(itemData[RenderControlPacketFields::X].toList().first().toReal());
    bRect.setWidth(itemData[RenderControlPacketFields::WIDTH].toReal());
    bRect.setHeight(itemData[RenderControlPacketFields::HEIGHT].toReal());
}

RenderServerRectItem::RenderServerRectItem(const QVariantMap &idata): RenderServerItem(idata) {
}

void RenderServerRectItem::scale(qreal scale){

    // Scales to it's center.

    if (scale < 0) return;

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();


    QPointF newTL = scaleAPointAroundTFOrigin(xl[0].toReal(),yl[0].toReal(),scale);
    itemData[RenderControlPacketFields::HEIGHT] = itemData[RenderControlPacketFields::HEIGHT].toReal()*scale;
    itemData[RenderControlPacketFields::WIDTH] = itemData[RenderControlPacketFields::WIDTH].toReal()*scale;
    yl[0] = newTL.y();
    xl[0] = newTL.x();

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;

}

qreal RenderServerRectItem::x() const {
    return itemData.value(RenderControlPacketFields::X).toList().first().toReal();
}

qreal RenderServerRectItem::y() const {
    return itemData.value(RenderControlPacketFields::Y).toList().first().toReal();
}

void RenderServerRectItem::moveBy(qreal dx, qreal dy){
    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();


    xl[0] = xl.value(0).toReal() + dx;
    yl[0] = yl.value(0).toReal() + dy;


    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;
}
