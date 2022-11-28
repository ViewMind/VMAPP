#include "renderserverimageitem.h"

RenderServerImageItem::RenderServerImageItem(const QString &fname):RenderServerItem() {

    QStringList renderItemList = itemData.value(RenderControlPacketFields::RENDER_LIST).toStringList();

    renderItemList << RenderControlPacketFields::X
                   << RenderControlPacketFields::Y
                   << RenderControlPacketFields::WIDTH
                   << RenderControlPacketFields::HEIGHT
                   << RenderControlPacketFields::FIT_TO_WIDTH
                   << RenderControlPacketFields::FILE_NAME;

    itemData[RenderControlPacketFields::RENDER_LIST] = renderItemList;


    QVariantList x; x << 0;
    QVariantList y; y << 0;
    itemData[RenderControlPacketFields::X] = x;
    itemData[RenderControlPacketFields::Y] = y;
    itemData[RenderControlPacketFields::IMG_H] = 0;
    itemData[RenderControlPacketFields::IMG_W] = 0;
    itemData[RenderControlPacketFields::FILE_NAME] = fname;

    itemData[RenderControlPacketFields::TYPE]      = GL2DItemType::TYPE_IMAGE;
    itemData[RenderControlPacketFields::TYPE_NAME] = RenderServerItemTypeName::IMAGE;
}

RenderServerImageItem::RenderServerImageItem(const QVariantMap &idata): RenderServerItem(idata) {
    updateBRect();
}

void RenderServerImageItem::setDimensions(bool fitToWidth, qreal value_to_fit ,qreal w, qreal h){
    itemData[RenderControlPacketFields::FIT_TO_WIDTH] = fitToWidth;
    itemData[RenderControlPacketFields::VALUE_TO_FIT] = value_to_fit;

    itemData[RenderControlPacketFields::WIDTH] = value_to_fit;
    itemData[RenderControlPacketFields::HEIGHT] = value_to_fit;

    qreal K = w/h;
    if (fitToWidth){
        itemData[RenderControlPacketFields::IMG_W] = value_to_fit;
        itemData[RenderControlPacketFields::IMG_H] = value_to_fit/K;
    }
    else {
        itemData[RenderControlPacketFields::IMG_H] = value_to_fit;
        itemData[RenderControlPacketFields::IMG_W] = value_to_fit*K;
    }

}

void RenderServerImageItem::setPos(qreal x, qreal y){
    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    xl[0] = x;
    yl[0] = y;

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;

    updateBRect();
}

void RenderServerImageItem::scale(qreal scale){

    // Scales to it's center.

    if (scale < 0) return;

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    qreal x_tl = xl[0].toReal();
    qreal y_tl = yl[0].toReal();

    QPointF newTL = scaleAPointAroundTFOrigin(x_tl,y_tl,scale);
    itemData[RenderControlPacketFields::IMG_H] = itemData[RenderControlPacketFields::IMG_H].toReal()*scale;
    itemData[RenderControlPacketFields::IMG_W] = itemData[RenderControlPacketFields::IMG_W].toReal()*scale;
    yl[0] = newTL.y();
    xl[0] = newTL.x();

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;

    updateBRect();

}

void RenderServerImageItem::moveBy(qreal dx, qreal dy){
    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    xl[0] = xl.value(0).toReal() + dx;
    yl[0] = yl.value(0).toReal() + dy;

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;
    updateBRect();
}

qreal RenderServerImageItem::x() const {
    return itemData.value(RenderControlPacketFields::X).toList().first().toReal();
}

qreal RenderServerImageItem::y() const {
    return itemData.value(RenderControlPacketFields::Y).toList().first().toReal();
}

void RenderServerImageItem::updateBRect(){

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();
    qreal imgW = itemData.value(RenderControlPacketFields::IMG_W).toReal();
    qreal imgH = itemData.value(RenderControlPacketFields::IMG_H).toReal();


    bRect.setTop(yl[0].toReal());
    bRect.setLeft(xl[0].toReal());
    bRect.setWidth(imgW);
    bRect.setHeight(imgH);
}
