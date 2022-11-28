#include "renderserveritem.h"

RenderServerItem::RenderServerItem()
{
    itemData[RenderControlPacketFields::BORDER_COLOR]       = "#000000";
    itemData[RenderControlPacketFields::COLOR]              = "#000000";
    itemData[RenderControlPacketFields::Z]                  = 1.0;
    itemData[RenderControlPacketFields::VISIBLE]            = true;
    itemData[RenderControlPacketFields::TF_ORIGIN]          = QPointF(0,0);
    itemData[RenderControlPacketFields::REF_Y]              = 0;
    itemData[RenderControlPacketFields::USE_ROUND_CAPS]     = false;
    itemData[RenderControlPacketFields::BORDER_WIDTH]       = 0;
    itemData[RenderControlPacketFields::TYPE]               = -1;
    itemData[RenderControlPacketFields::TYPE_NAME]          = "";
    itemData[RenderControlPacketFields::DISPLAY_ONLY_FLAG]  = false;
    itemData[RenderControlPacketFields::PARAMETER_ANIMATION_LIST] = QVariantList();
    itemData[RenderControlPacketFields::ID] = -1;
    QStringList renderItemList;
    renderItemList << RenderControlPacketFields::TYPE << RenderControlPacketFields::DISPLAY_ONLY_FLAG; // These two parameters are required by ALL items.
    itemData[RenderControlPacketFields::RENDER_LIST] = renderItemList;
}

RenderServerItem::RenderServerItem(const QVariantMap &idata){
    QStringList keys = idata.keys();
    this->itemData.clear();
    for (qint32 i = 0; i < keys.size(); i++){
        QString key = keys.at(i);
        this->itemData[key] = idata.value(key);
    }
}

QVariantMap RenderServerItem::getItemData() const {
    return this->itemData;
}

void RenderServerItem::setDisplayOnly(bool disp_only){
    itemData[RenderControlPacketFields::DISPLAY_ONLY_FLAG] = disp_only;
}

void RenderServerItem::setZValue(double z){
    itemData[RenderControlPacketFields::Z] = z;
    if (z <= 0) itemData[RenderControlPacketFields::VISIBLE] = false;
    else itemData[RenderControlPacketFields::VISIBLE] = true;
}

qreal RenderServerItem::z() const {
    return itemData[RenderControlPacketFields::Z].toReal();
}

void RenderServerItem::setVisible(bool visible){
    itemData[RenderControlPacketFields::VISIBLE] = visible;
}

void RenderServerItem::setTransformOriginPoint(qreal x, qreal y){
    QPointF tfOrigin = itemData[RenderControlPacketFields::TF_ORIGIN].toPointF();
    tfOrigin.setX(x);
    tfOrigin.setY(y);
    itemData[RenderControlPacketFields::TF_ORIGIN] = tfOrigin;
}


void RenderServerItem::setBrush(const QBrush &brush){
    itemData[RenderControlPacketFields::COLOR] = brush.color().name(QColor::HexArgb);
}

void RenderServerItem::setPen(const QPen &pen){
    itemData[RenderControlPacketFields::BORDER_COLOR] = pen.color().name(QColor::HexArgb);
    itemData[RenderControlPacketFields::BORDER_WIDTH] = pen.widthF();
    itemData[RenderControlPacketFields::USE_ROUND_CAPS] = (pen.capStyle() == Qt::RoundCap);
}

QRectF RenderServerItem::boundingRect() const {
    return bRect;
}

bool RenderServerItem::isVisible() const {
    return itemData.value(RenderControlPacketFields::VISIBLE).toBool();
}


void RenderServerItem::setPos(qreal x, qreal y){
    Q_UNUSED(x)
    Q_UNUSED(y)
}

void RenderServerItem::render(RenderServerPacket *packet) const{
    QVariantList list;
    if (packet->containsPayloadField(RenderControlPacketFields::SPEC_LIST)){
        list = packet->getPayloadField(RenderControlPacketFields::SPEC_LIST).toList();
    }

    QStringList renderItemList = itemData.value(RenderControlPacketFields::RENDER_LIST).toStringList();

    //    qDebug() << "Adding packet";
    //    qDebug() << "======";
    //    qDebug() << itemData;
    //    qDebug() << "======";
    //    qDebug() << renderItemList;

    // Copying all relevant specs.
    QVariantMap spec;
    for (qint32 i = 0; i < renderItemList.size(); i++){
        QString key = renderItemList.at(i);
        spec[key] = itemData.value(key);
    }

    //qDebug() << spec;

    list << spec;
    packet->setPayloadField(RenderControlPacketFields::SPEC_LIST,list);

}

void RenderServerItem::scale(qreal scale) {
    Q_UNUSED(scale)
}

void RenderServerItem::moveBy(qreal dx, qreal dy){
    Q_UNUSED(dx)
    Q_UNUSED(dy)
}

void RenderServerItem::setReferenceYForTransformations(qreal refY){
    itemData[RenderControlPacketFields::REF_Y] = refY;
}


QString RenderServerItem::getType() const {
    return itemData[RenderControlPacketFields::TYPE_NAME].toString();
}

void RenderServerItem::setItemID(const qint32 &id){
    itemData[RenderControlPacketFields::ID] = id;
}

qint32 RenderServerItem::getItemID() const{
    return itemData[RenderControlPacketFields::ID].toInt();
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

    qreal referenceYTF = this->itemData.value(RenderControlPacketFields::REF_Y).toReal();
    QPointF tfOrigin = this->itemData.value(RenderControlPacketFields::TF_ORIGIN).toPointF();

    qreal y = referenceYTF - point.y();
    qreal oy = referenceYTF - tfOrigin.y();
    qreal newX = point.x()*scale - tfOrigin.x()*scale + tfOrigin.x();
    qreal newY = y*scale - oy*scale + oy;

    //qDebug() << "TRANSFORM SCALING. Input: " << point << ". Output: " << QPointF(newX, this->referenceYTF - newY)  << ". Parameters: SCALE: " << scale << ". Origin: " << tfOrigin;

    return QPointF(newX, referenceYTF - newY);
    //return QPointF(newX, newY);
}


QString RenderServerItem::addAnimationParameter(const QString &name, qreal start, qreal end, qreal duration, qreal animationInterval, qreal delay){

    if (this->getItemID() == -1) return "Item does not have ID set"; // ID is required to know which parameter ended.

    // Animation paratemters can be positioned as items on a list. We need to search for the dot.
    QString paramName = "";
    qint32 listIndex = -1;
    bool ok = false;

    QStringList nameParts = name.split(".",Qt::SkipEmptyParts);

    if (nameParts.size() == 2){
        // Name and index.
        paramName = nameParts.first();
        listIndex = nameParts.last().toInt(&ok);
        if (listIndex < 0) return "Bad index on name. List index is less than zero"; // BAd index.
        if (!ok) return "Bad index. List index could not be interpreted to an integer"; // Not an integer.
    }
    else if (nameParts.size() == 1){
        paramName = name;
    }
    else return "Bad name for parameter. Too many '.'"; // Splitting a name should only provide 1 or 2 parts.

    if (!itemData.contains(paramName)) return "The parameter name '" + paramName + "' does not exist"; // The parameter doesn't exist.

    // Regular animation parameter.
    if (listIndex == -1){
        ok = false;
        itemData.value(paramName).toReal(&ok);
        if (!ok) return "The parameter specified cannot be treated as a real value";  // The item can't be treated as a real value.
    }
    // Indexed paameter in list.
    else {

        if (!itemData.value(paramName).canConvert<QVariantList>()) return "Especified parameter cannot be treated as list."; // Parameter name does not reference a list.
        QVariantList list = itemData.value(paramName).toList();
        if (list.size() <= listIndex) return "Bad index of " + QString::number(listIndex) + " but list is only " + QString::number(list.size());  // Bad or inexistent index.
        ok = false;
        list.value(listIndex).toReal(&ok);
        if (!ok) return "The parameter specified at the specified index cannot be treated as a real value";  // The item can't be treated as a real value.
    }


    // If we got here we can add the animation parameter.
    QVariantMap animation;

    // We need to compute the steps.
    qreal steps = duration/animationInterval;
    if (steps < 0){
        return "Animation duration is too short";
    }

    qreal delaySteps = 0;
    if (delay > 0){
        delaySteps = delay/animationInterval;
    }

    animation[AnimationVars::END] = end;
    animation[AnimationVars::START] = start;
    animation[AnimationVars::INDEX] = listIndex;
    animation[AnimationVars::PARAMETER] = paramName;
    animation[AnimationVars::STEPCOUNTER] = 0;
    animation[AnimationVars::STEPS] = steps + delaySteps;
    animation[AnimationVars::DELAY_STEPS] = delaySteps;
    animation[AnimationVars::DELTA] = (end - start)/steps;

    //qDebug() << "RenderServerItem: Created an animation point from " << start << " to " << end << "that lasts" << duration << " and has " << steps << "steps and a delta of" << animation.value(AnimationVars::DELTA).toReal();

    QVariantMap map = itemData[RenderControlPacketFields::PARAMETER_ANIMATION_LIST].toMap();
    map.insert(name,animation);
    itemData[RenderControlPacketFields::PARAMETER_ANIMATION_LIST] = map;

    return "";

}

QMap<QString, qint32> RenderServerItem::animate() {

    QVariantMap alist = itemData[RenderControlPacketFields::PARAMETER_ANIMATION_LIST].toMap();
    QString id = itemData.value(RenderControlPacketFields::ID).toString();
    QStringList pnames = alist.keys();

    QMap<QString, qint32> signalsTriggered;

    //qDebug() << "Animating item with the following parameters" << pnames;

    for (qint32 i = 0; i < pnames.size(); i++){

        QVariantMap anim = alist[pnames.at(i)].toMap();

        QString signalVariableName = id + "-" + pnames.at(i);

        qint32  stepcounter = anim.value(AnimationVars::STEPCOUNTER).toInt();
        qint32  endstep     = anim.value(AnimationVars::STEPS).toInt();
        qint32  delaySteps  = anim.value(AnimationVars::DELAY_STEPS).toInt();
        qreal   start       = anim.value(AnimationVars::START).toReal();
        qreal   end         = anim.value(AnimationVars::END).toReal();
        qreal   delta       = anim.value(AnimationVars::DELTA).toReal();
        qint32  index      = anim.value(AnimationVars::INDEX).toInt();
        QString p          = anim.value(AnimationVars::PARAMETER).toString();

        if (stepcounter < delaySteps){ // Until we finish the delay we don't move.
            delta = 0;
        }
        else if ((stepcounter == delaySteps) && (stepcounter > 0)){
            signalsTriggered[signalVariableName] = ANIMATION_DELAY_END;
        }

        // Get the current value.
        qreal pvalue = 0;
        if (index == -1){
            pvalue = itemData.value(p).toReal();
        }
        else {
            pvalue = itemData.value(p).toList().at(index).toReal();
        }

        bool ended = false;

        if (stepcounter == 0){
            // First step. We set it to start.
            if (delaySteps > 0){
                signalsTriggered[signalVariableName] = ANIMATION_BEGIN_DELAY;
            }
            else {
                signalsTriggered[signalVariableName] = ANIMATION_BEGIN_NO_DELAY;
            }
            pvalue = start;
        }
        else if (stepcounter == (endstep-1)){
            // Last step we set the value to end.
            pvalue = end;
            signalsTriggered[signalVariableName] = ANIMATION_END;
            ended = true;
        }
        else {
            // A step in the middle.
            pvalue = pvalue + delta;
        }

        // We set the new value.
        if (index == -1){
            itemData[p] = pvalue;
        }
        else {
            QVariantList list = itemData.value(p).toList();
            list[index] = pvalue;
            itemData[p] = list;
        }

        //qDebug() << "Changed" << pnames.at(i) << "to new value of" << pvalue;
        stepcounter++;

        // Now we check end conditions.
        if (ended){
            // We are done. We take out the item and add to signal list.
            alist.remove(pnames.at(i));
        }
        else {
            anim[AnimationVars::STEPCOUNTER] = stepcounter;
            alist[pnames.at(i)] = anim;
        }

    }

    // We make sure to store the animation list as is.
    itemData[RenderControlPacketFields::PARAMETER_ANIMATION_LIST] = alist;

    return signalsTriggered;
}

QString RenderServerItem::AnimationSignalType2String(qint32 type){
    switch (type) {
    case ANIMATION_BEGIN_DELAY:
        return "AnimationBeginDelay";
    case ANIMATION_BEGIN_NO_DELAY:
        return "AnimationBeginNoDelay";
    case ANIMATION_DELAY_END:
        return "AnimationDelayEnd";
    case ANIMATION_END:
        return "AnimationEnd";
    default:
        return "UnknownSignalType " + QString::number(type);
    }
}


void RenderServerItem::clearAnimationData(){
    itemData[RenderControlPacketFields::PARAMETER_ANIMATION_LIST].clear();
}

RenderServerItem::~RenderServerItem(){

}
