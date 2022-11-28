#include "renderservertriangleitem.h"

RenderServerTriangleItem::RenderServerTriangleItem(const QPolygonF &triangle): RenderServerItem()
{

    QPolygonF tri;

    if (triangle.size() < 3) {
        tri.append(QPointF(0,0));
        tri.append(QPointF(0,0));
        tri.append(QPointF(0,0));
    }
    else {
        tri = triangle;
    }

    QStringList renderItemList = itemData.value(RenderControlPacketFields::RENDER_LIST).toStringList();

    renderItemList << RenderControlPacketFields::X
    << RenderControlPacketFields::Y
    << RenderControlPacketFields::COLOR
    << RenderControlPacketFields::BORDER_COLOR
    << RenderControlPacketFields::BORDER_WIDTH;

    itemData[RenderControlPacketFields::RENDER_LIST] = renderItemList;

    QVariantList x; x << tri.at(0).x() << tri.at(1).x() << tri.at(2).x();
    QVariantList y; y << tri.at(0).y() << tri.at(1).y() << tri.at(2).y();
    itemData[RenderControlPacketFields::X] = x;
    itemData[RenderControlPacketFields::Y] = y;

    itemData[RenderControlPacketFields::TYPE]      = GL2DItemType::TYPE_TRIANGLE;
    itemData[RenderControlPacketFields::TYPE_NAME] = RenderServerItemTypeName::TRIANGLE;

}

RenderServerTriangleItem::RenderServerTriangleItem(const QVariantMap &idata): RenderServerItem(idata) {
    updateBRect();
}

void RenderServerTriangleItem::setPos (qreal x, qreal y){
    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();
    qreal dx = x - xl[0].toReal();
    qreal dy = y - yl[0].toReal();
    moveBy(dx,dy);
}



void RenderServerTriangleItem::scale(qreal scale){

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    QPointF p1 = scaleAPointAroundTFOrigin(xl[0].toReal(),yl[0].toReal(),scale);
    QPointF p2 = scaleAPointAroundTFOrigin(xl[1].toReal(),yl[1].toReal(),scale);
    QPointF p3 = scaleAPointAroundTFOrigin(xl[2].toReal(),yl[2].toReal(),scale);

    xl[0] = p1.x();
    yl[0] = p1.y();
    xl[1] = p2.x();
    yl[1] = p2.y();
    xl[2] = p3.x();
    yl[3] = p3.y();

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;
	 
    updateBRect();
}

void RenderServerTriangleItem::moveBy(qreal dx, qreal dy){
    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    for (qint32 i = 0; i < 3; i++){
        xl[i] = xl.value(i).toReal() + dx;
        yl[i] = yl.value(i).toReal() + dy;
    }

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;
    updateBRect();
}

qreal RenderServerTriangleItem::x() const {
    return itemData.value(RenderControlPacketFields::X).toList().first().toReal();
}

qreal RenderServerTriangleItem::y()  const {
    return itemData.value(RenderControlPacketFields::Y).toList().first().toReal();
}


void RenderServerTriangleItem::updateBRect(){

    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    qreal x1 = xl.first().toReal();
    qreal x2 = xl[1].toReal();
    qreal x3 = xl.last().toReal();
    qreal y1 = yl.first().toReal();
    qreal y2 = yl[1].toReal();
    qreal y3 = yl.last().toReal();

    qreal max_x = qMax(x1,qMax(x2,x3));
    qreal min_x = qMin(x1,qMin(x2,x3));
    qreal max_y = qMax(y1,qMax(y2,y3));
    qreal min_y = qMin(y1,qMin(y2,y3));

    bRect.setTop(min_y);
    bRect.setLeft(min_x);
    bRect.setWidth(max_x - min_x);
    bRect.setHeight(max_y - min_y);

}
