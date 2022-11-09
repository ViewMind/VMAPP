#include "renderserverscene.h"

RenderServerScene::RenderServerScene(double x, double y, double width, double height) {
    Q_UNUSED(x)
    Q_UNUSED(y)
    this->sceneWidth = width;
    this->sceneHeight = height;
    this->backgroundColor = "#000000";
}

qreal RenderServerScene::width() const{
    return this->sceneWidth;
}

qreal RenderServerScene::height() const{
    return this->sceneHeight;
}


RenderServerCircleItem* RenderServerScene::addEllipse(qreal x, qreal y, qreal w, qreal h, const QPen &pen, const QBrush &brush){
    // Since we are adding a circle only the width is considere as the diameter and the height is ignored
    RenderServerCircleItem *circle = new RenderServerCircleItem(x,y,w,h);
    circle->setPen(pen);
    circle->setBrush(brush);
    this->itemsInScene.append(circle);
    return circle;
}

RenderServerRectItem* RenderServerScene::addRect(qreal x, qreal y, qreal w, qreal h, const QPen &pen, const QBrush &brush){
    RenderServerRectItem *rect = new RenderServerRectItem(x,y,w,h);
    rect->setPen(pen);
    rect->setBrush(brush);
    this->itemsInScene.append(rect);
    return rect;
}

RenderServerTextItem* RenderServerScene::addSimpleText(const QString &text, const QFont &font){
    return this->addText(text,font);
}

RenderServerTextItem* RenderServerScene::addText(const QString &text, const QFont &font){
    Q_UNUSED(font)
    RenderServerTextItem *item = new RenderServerTextItem(text);
    item->setFont(font);
    this->itemsInScene.append(item);
    return item;
}

RenderServerArrowItem* RenderServerScene::addArrow(qreal x1, qreal y1, qreal x2, qreal y2, qreal hH, qreal hL, const QColor &color){
    RenderServerArrowItem *item = new RenderServerArrowItem(x1,y1,x2,y2,hL,hH,true);
    item->setPen(QPen(color));
    item->setBrush(QBrush(color));
    this->itemsInScene.append(item);
    return item;
}

RenderServerLineItem* RenderServerScene::addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen){
    RenderServerLineItem *item = new RenderServerLineItem(x1,y1,x2,y2);
    item->setPen(pen);
    this->itemsInScene.append(item);
    return item;
}

RenderServerPacket RenderServerScene::render() const{
    RenderServerPacket p;

    p.setPacketType(RenderServerPacketType::TYPE_2D_RENDER);
    p.setPayloadField(RenderControlPacketFields::BG_COLOR,this->backgroundColor);

    for (qint32 i = 0; i < this->itemsInScene.size(); i++){
        if (this->itemsInScene.at(i)->isVisible()){
            this->itemsInScene.at(i)->render(&p);
        }
        else {
            qDebug() << "Item " << i << " of type " << this->itemsInScene.at(i)->getType() << " is INVISIBLE";
        }
    }

    return p;
}

void RenderServerScene::clear(){
    this->itemsInScene.clear();
}
