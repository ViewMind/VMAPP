#include "renderserverlineitem.h"

RenderServerLineItem::RenderServerLineItem(qreal x1, qreal y1, qreal x2, qreal y2)
{

    QStringList renderItemList = itemData.value(RenderControlPacketFields::RENDER_LIST).toStringList();

    renderItemList << RenderControlPacketFields::X
    << RenderControlPacketFields::Y
    << RenderControlPacketFields::BORDER_COLOR
    << RenderControlPacketFields::BORDER_WIDTH
    << RenderControlPacketFields::USE_ROUND_CAPS;

    itemData[RenderControlPacketFields::RENDER_LIST] = renderItemList;


    QVariantList x; x << x1 << x2;
    QVariantList y; y << y1 << y2;
    itemData[RenderControlPacketFields::X] = x;
    itemData[RenderControlPacketFields::Y] = y;
    itemData[RenderControlPacketFields::BORDER_WIDTH] = 1;

    itemData[RenderControlPacketFields::TYPE]      = GL2DItemType::TYPE_LINE;
    itemData[RenderControlPacketFields::TYPE_NAME] = RenderServerItemTypeName::LINE;


    updateBRect();
}


RenderServerLineItem::RenderServerLineItem(const QVariantMap &idata): RenderServerItem(idata) {
    updateBRect();
}


void RenderServerLineItem::setPos(qreal x, qreal y){
    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    xl[0] = x;
    yl[0] = y;

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;

    updateBRect();
}

void RenderServerLineItem::updateBRect(){

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    qreal x1 = xl.first().toReal();
    qreal x2 = xl.last().toReal();
    qreal y1 = yl.first().toReal();
    qreal y2 = yl.last().toReal();

    qreal max_x = qMax(x1,x2);
    qreal min_x = qMin(x1,x2);
    qreal max_y = qMax(y1,y2);
    qreal min_y = qMin(y1,y2);

    bRect.setTop(min_y);
    bRect.setLeft(min_x);
    bRect.setWidth(max_x - min_x);
    bRect.setHeight(max_y - min_y);

}

qreal RenderServerLineItem::x() const {
    return itemData.value(RenderControlPacketFields::X).toList().first().toReal();
}

qreal RenderServerLineItem::y() const {
    return itemData.value(RenderControlPacketFields::Y).toList().first().toReal();
}

void RenderServerLineItem::scale(qreal scale){

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    qreal x1 = xl.first().toReal();
    qreal x2 = xl.last().toReal();
    qreal y1 = yl.first().toReal();
    qreal y2 = yl.last().toReal();


    QPointF p1 = scaleAPointAroundTFOrigin(x1,y1,scale);
    QPointF p2 = scaleAPointAroundTFOrigin(x2,y2,scale);
    xl[0] = p1.x();
    yl[0] = p1.y();
    xl[1] = p2.x();
    yl[1] = p2.y();


    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;


    updateBRect();
}

void RenderServerLineItem::moveBy(qreal dx, qreal dy){
    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    for (qint32 i = 0; i < 2; i++){
        xl[i] = xl.value(i).toReal() + dx;
        yl[i] = yl.value(i).toReal() + dy;
    }

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;
    updateBRect();
}
