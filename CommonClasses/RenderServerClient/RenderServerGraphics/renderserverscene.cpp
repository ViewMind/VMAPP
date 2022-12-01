#include "renderserverscene.h"

RenderServerScene::RenderServerScene(double x, double y, double width, double height) {
    Q_UNUSED(x)
    Q_UNUSED(y)
    this->sceneWidth = width;
    this->sceneHeight = height;
    this->backgroundColor = "#000000";
}

RenderServerScene::RenderServerScene(const RenderServerScene &scene){
    this->copyFrom(scene);
}

RenderServerScene::RenderServerScene(){
    this->sceneHeight = 0;
    this->sceneWidth = 0;
    this->backgroundColor = "#000000";
}

void RenderServerScene::setSceneRect(const QRectF &rect){
    this->setSceneRect(rect.x(),rect.y(),rect.width(),rect.height());
}

void RenderServerScene::setSceneRect(qreal x, qreal y, qreal w, qreal h){
    Q_UNUSED(x)
    Q_UNUSED(y)
    this->sceneWidth = w;
    this->sceneHeight = h;
}

bool RenderServerScene::isValid() const {
    return ((this->sceneHeight > 0) && (this->sceneWidth > 0));
}

void RenderServerScene::copyFrom(const RenderServerScene &scene){
    // Copy construnctor.
    this->sceneHeight = scene.width();
    this->sceneWidth  = scene.height();
    this->backgroundColor = scene.getBackgroundColorName();
    this->itemsInScene.clear();

    QList<RenderServerItem*> list = scene.getItemList();

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
        if (item->getType() == RenderServerItemTypeName::IMAGE){
            RenderServerImageItem *created_item = new RenderServerImageItem(item->getItemData());
            this->itemsInScene.append(created_item);
        }

    }

}

QList<RenderServerItem*> RenderServerScene::getItemList() const {
    return this->itemsInScene;
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
    RenderServerCircleItem *item = new RenderServerCircleItem(x,y,w,h);
    item->setPen(pen);
    item->setBrush(brush);
    item->setReferenceYForTransformations(this->sceneHeight);
    item->setItemID(static_cast<qint32>(this->itemsInScene.size()));
    this->itemsInScene.append(item);
    return item;
}

RenderServerRectItem* RenderServerScene::addRect(const QRectF &rect){
    return this->addRect(rect.x(),rect.y(),rect.width(),rect.height(), QPen(), QBrush());
}

RenderServerRectItem* RenderServerScene::addRect(qreal x, qreal y, qreal w, qreal h, const QPen &pen, const QBrush &brush){
    RenderServerRectItem *item = new RenderServerRectItem(x,y,w,h);
    item->setPen(pen);
    item->setBrush(brush);
    item->setReferenceYForTransformations(this->sceneHeight);
    item->setItemID(static_cast<qint32>(this->itemsInScene.size()));
    this->itemsInScene.append(item);
    return item;
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
    item->setItemID(static_cast<qint32>(this->itemsInScene.size()));
    this->itemsInScene.append(item);
    return item;
}

RenderServerTriangleItem *RenderServerScene::addTriangle(const QPolygonF &triangle){
    RenderServerTriangleItem *item = new RenderServerTriangleItem(triangle);
    item->setItemID(static_cast<qint32>(this->itemsInScene.size()));
    this->itemsInScene.append(item);    
    item->setReferenceYForTransformations(this->sceneHeight);
    return item;
}

RenderServerImageItem* RenderServerScene::addImage(const QString &fname, bool fitToWidth, qreal w, qreal h, qreal value_to_fit){
    RenderServerImageItem *item = new RenderServerImageItem(fname);
    item->setDimensions(fitToWidth,value_to_fit,w,h);
    item->setReferenceYForTransformations(this->sceneHeight);
    item->setItemID(static_cast<qint32>(this->itemsInScene.size()));
    this->itemsInScene.append(item);
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
    item->setItemID(static_cast<qint32>(this->itemsInScene.size()));
    this->itemsInScene.append(item);
    return item;
}


RenderServerLineItem* RenderServerScene::addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen){
    RenderServerLineItem *item = new RenderServerLineItem(x1,y1,x2,y2);
    item->setPen(pen);
    item->setReferenceYForTransformations(this->sceneHeight);
    item->setItemID(static_cast<qint32>(this->itemsInScene.size()));
    this->itemsInScene.append(item);
    return item;
}


RenderServerPacket RenderServerScene::render(bool skippable) const {
    RenderServerPacket p;

    p.setPacketType(RenderServerPacketType::TYPE_2D_RENDER);
    //qDebug() << "Sending the background color of" << this->backgroundColor;
    p.setPayloadField(RenderControlPacketFields::BG_COLOR,this->backgroundColor);

    QList<RenderServerItem*> list = this->getRenderServerOrder();

    for (qint32 i = 0; i < list.size(); i++){
       list.at(i)->render(&p);
    }

    if (skippable){ // Can be skipped if necessary.
        p.setPacketSkippable();
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

void RenderServerScene::clearAnimationData(){
    for (qint32 i = 0; i < itemsInScene.size(); i++){
        itemsInScene[i]->clearAnimationData();
    }
}

RenderServerItem* RenderServerScene::getItemByID(const qint32 &id) const{

    if ((id < 0) || (id >= itemsInScene.size())) return nullptr;
    return itemsInScene.at(id);

}

QMap<QString,qint32> RenderServerScene::animate() {

    QMap<QString,qint32> endParams;

    for (qint32 i = 0; i < itemsInScene.size(); i++){
        QMap<QString,qint32> ans = itemsInScene.at(i)->animate();
        if (ans.size() > 0){
            QStringList keys = ans.keys();
            for (qint32 j = 0; j < keys.size(); j++){
                endParams[keys.at(j)] = ans.value(keys.at(j));
            }
        }
    }

    return endParams;

}


void RenderServerScene::clear(){
    this->itemsInScene.clear();
}

RenderServerScene::~RenderServerScene(){
    this->itemsInScene.clear();
}
