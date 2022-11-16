#include "renderserverscene.h"

RenderServerScene::RenderServerScene(double x, double y, double width, double height) {
    Q_UNUSED(x)
    Q_UNUSED(y)
    this->sceneWidth = width;
    this->sceneHeight = height;
    this->backgroundColor = "#000000";
}

RenderServerScene::RenderServerScene(RenderServerScene *scene){
    this->copyFrom(scene);
}


void RenderServerScene::copyFrom(RenderServerScene *scene){
    // Copy construnctor.
    this->sceneHeight = scene->width();
    this->sceneWidth  = scene->height();
    this->backgroundColor = scene->getBackgroundColorName();
    this->itemsInScene.clear();

    QList<RenderServerItem*> list = scene->getItemList();

    for (qint32 i = 0; i < list.size(); i++){

        RenderServerItem * item = list.at(i);
        if (item->getType() == RenderServerItemTypeName::ARROW){
            RenderServerArrowItem *created_item = new RenderServerArrowItem(item->getItemData());
            this->itemsInScene.append(created_item);
        }
        if (item->getType() == RenderServerItemTypeName::RECT){
            RenderServerRectItem *created_item = new RenderServerRectItem(item->getItemData());
            this->itemsInScene.append(created_item);
        }
        if (item->getType() == RenderServerItemTypeName::TEXT){
            RenderServerTextItem *created_item = new RenderServerTextItem(item->getItemData());
            this->itemsInScene.append(created_item);
        }
        if (item->getType() == RenderServerItemTypeName::CIRCLE){
            RenderServerCircleItem *created_item = new RenderServerCircleItem(item->getItemData());
            this->itemsInScene.append(created_item);
        }
        if (item->getType() == RenderServerItemTypeName::LINE){
            RenderServerLineItem *created_item = new RenderServerLineItem(item->getItemData());
            this->itemsInScene.append(created_item);
        }
        if (item->getType() == RenderServerItemTypeName::TRIANGLE){
            RenderServerTriangleItem *created_item = new RenderServerTriangleItem(item->getItemData());
            this->itemsInScene.append(created_item);
        }

    }

}

qreal RenderServerScene::width() const{
    return this->sceneWidth;
}

qreal RenderServerScene::height() const{
    return this->sceneHeight;
}

RenderServerItemGroup* RenderServerScene::createItemGroup(const QList<RenderServerItem *> items){
    return new RenderServerItemGroup(items);
}

RenderServerCircleItem* RenderServerScene::addEllipse(const QRectF brect, const QPen &pen, const QBrush &brush){
    return this->addEllipse(brect.x(),brect.y(),brect.width(),brect.height(),pen,brush);
}


RenderServerCircleItem* RenderServerScene::addEllipse(qreal x, qreal y, qreal w, qreal h, const QPen &pen, const QBrush &brush){
    // Since we are adding a circle only the width is considere as the diameter and the height is ignored
    RenderServerCircleItem *circle = new RenderServerCircleItem(x,y,w,h);
    circle->setPen(pen);
    circle->setBrush(brush);
    circle->setReferenceYForTransformations(this->sceneHeight);
    this->itemsInScene.append(circle);
    return circle;
}

RenderServerRectItem* RenderServerScene::addRect(const QRectF &rect){
    return this->addRect(rect.x(),rect.y(),rect.width(),rect.height(), QPen(), QBrush());
}

RenderServerRectItem* RenderServerScene::addRect(qreal x, qreal y, qreal w, qreal h, const QPen &pen, const QBrush &brush){
    RenderServerRectItem *rect = new RenderServerRectItem(x,y,w,h);
    rect->setPen(pen);
    rect->setBrush(brush);
    rect->setReferenceYForTransformations(this->sceneHeight);
    this->itemsInScene.append(rect);
    return rect;
}

void RenderServerScene::setBackgroundBrush(const QBrush &brush){
    this->backgroundColor = brush.color().name();
}

QString RenderServerScene::getBackgroundColorName() const {
    return this->backgroundColor;
}


RenderServerTextItem* RenderServerScene::addSimpleText(const QString &text, const QFont &font){
    return this->addText(text,font);
}

RenderServerTextItem* RenderServerScene::addText(const QString &text, const QFont &font){
    Q_UNUSED(font)
    RenderServerTextItem *item = new RenderServerTextItem(text);
    item->setFont(font);
    item->setReferenceYForTransformations(this->sceneHeight);
    this->itemsInScene.append(item);
    return item;
}

RenderServerTriangleItem *RenderServerScene::addTriangle(const QPolygonF &triangle){
    RenderServerTriangleItem *item = new RenderServerTriangleItem(triangle);
    this->itemsInScene.append(item);
    item->setReferenceYForTransformations(this->sceneHeight);
    return item;
}

RenderServerArrowItem* RenderServerScene::addArrow(qreal x1, qreal y1, qreal x2, qreal y2, qreal hH, qreal hL, const QColor &color){
    RenderServerArrowItem *item = new RenderServerArrowItem(x1,y1,x2,y2,hL,hH,true);
    item->setPen(QPen(color));
    item->setBrush(QBrush(color));
    item->setReferenceYForTransformations(this->sceneHeight);
    this->itemsInScene.append(item);
    return item;
}

RenderServerLineItem* RenderServerScene::addLine(const QLineF line, const QPen &pen){
    RenderServerLineItem *item = new RenderServerLineItem(line.x1(),line.y1(),line.x2(),line.y2());
    item->setPen(pen);
    item->setReferenceYForTransformations(this->sceneHeight);
    this->itemsInScene.append(item);
    return item;
}


RenderServerLineItem* RenderServerScene::addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen){
    RenderServerLineItem *item = new RenderServerLineItem(x1,y1,x2,y2);
    item->setPen(pen);
    item->setReferenceYForTransformations(this->sceneHeight);
    this->itemsInScene.append(item);
    return item;
}


RenderServerPacket RenderServerScene::render() const {
    RenderServerPacket p;

    p.setPacketType(RenderServerPacketType::TYPE_2D_RENDER);
    //qDebug() << "Sending the background color of" << this->backgroundColor;
    p.setPayloadField(RenderControlPacketFields::BG_COLOR,this->backgroundColor);

    QList<RenderServerItem*> list = this->getRenderServerOrder();

    for (qint32 i = 0; i < list.size(); i++){
       list.at(i)->render(&p);
    }

    return p;
}


QList<RenderServerItem*> RenderServerScene::getRenderServerOrder() const {

    QList<RenderServerItem*> torender;

    for (qint32 i = 0; i < this->itemsInScene.size(); i++){
        if (this->itemsInScene.at(i)->isVisible()){

            RenderServerItem *item = this->itemsInScene.at(i);


            if (torender.isEmpty()){
                torender << item;
            }
            else if (torender.first()->z() > item->z()){
                torender.insert(0,item);
            }
            else {
                bool inserted = false;
                for (qint32 j = 0; j < torender.size()-1; j++){
                    if ((torender.at(j)->z() <= item->z()) && (torender.at(j+1)->z() > item->z())){
                        inserted = true;
                        torender.insert(j,item);
                        break;
                    }
                }
                if (!inserted){
                    torender << item;
                }
            }

        }
    }

//    for (qint32 i = 0; i < torender.size(); i++){
//        qDebug() << "Item of type " << torender.at(i)->getType() << " with z value of " << torender.at(i)->z();
//    }

    return torender;

}


void RenderServerScene::clear(){
    this->itemsInScene.clear();
}
