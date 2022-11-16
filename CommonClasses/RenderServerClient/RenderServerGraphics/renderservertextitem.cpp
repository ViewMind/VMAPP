#include "renderservertextitem.h"

RenderServerTextItem::RenderServerTextItem(const QString &text)
{
    this->text = text;
    this->x = 0;
    this->y = 0;
    this->bRect.setTop(this->y);
    this->bRect.setLeft(this->x);
    this->bRect.setWidth(100);
    this->bRect.setHeight(100);
    this->itemType = RenderServerItemTypeName::TEXT;
    this->borderWidth = 20;
    this->fontSize = 10;
    this->lineSpacing = 1;
    this->alignment = ALIGN_CENTER;
}

RenderServerTextItem::RenderServerTextItem(const QVariantMap &itemData):RenderServerItem(itemData){

    this->x = itemData.value(RenderControlPacketFields::X).toReal();
    this->y = itemData.value(RenderControlPacketFields::Y).toReal();
    this->fontSize = itemData.value(RenderControlPacketFields::FONT_SIZE).toReal();
    this->lineSpacing = itemData.value(RenderControlPacketFields::LINE_SPACING).toReal();
    this->alignment = itemData.value(RenderControlPacketFields::TEXT_ALIGNMENT).toChar().toLatin1();
    this->text = itemData.value(RenderControlPacketFields::TEXT).toString();
    this->computeBoundingRect();

}

QVariantMap RenderServerTextItem::getItemData() const {
    QVariantMap itemData = RenderServerItem::getItemData();

    itemData[RenderControlPacketFields::X] = this->x;
    itemData[RenderControlPacketFields::Y] = this->y;
    itemData[RenderControlPacketFields::FONT_SIZE] = this->fontSize;
    itemData[RenderControlPacketFields::LINE_SPACING] = this->lineSpacing;
    itemData[RenderControlPacketFields::TEXT_ALIGNMENT] = this->alignment;
    itemData[RenderControlPacketFields::TEXT] = this->text;

    return itemData;
}

void RenderServerTextItem::setPos(qreal x, qreal y){
    this->x = x;
    this->y = y;
    this->computeBoundingRect();
}

void RenderServerTextItem::render(RenderServerPacket *packet) const{
    QVariantList t;
    if (packet->containsPayloadField(RenderControlPacketFields::SPEC_LIST)){
        t = packet->getPayloadField(RenderControlPacketFields::SPEC_LIST).toList();
    }

    QVariantMap spec;
    QVariantList x,y;
    x << this->boundingRect().left();
    y << this->boundingRect().top();

    spec[RenderControlPacketFields::TYPE] = GL2DItemType::TYPE_TEXT;
    spec[RenderControlPacketFields::X] = x;
    spec[RenderControlPacketFields::Y] = y;
    spec[RenderControlPacketFields::COLOR] = this->fillColor;
    spec[RenderControlPacketFields::BORDER_COLOR] = this->borderColor;
    spec[RenderControlPacketFields::BORDER_WIDTH] = this->borderWidth;
    spec[RenderControlPacketFields::TEXT] = this->text;
    spec[RenderControlPacketFields::FONT_SIZE] = this->fontSize;


    t << spec;
    packet->setPayloadField(RenderControlPacketFields::SPEC_LIST,t);

}


void RenderServerTextItem::setText(const QString &text){
    this->text = text;
}

void RenderServerTextItem::setFont(const QFont &font){
    this->fontSize = font.pointSizeF();

    // Computing the border with according the following formula
    qreal char_h = this->fontSize;
    qreal char_w = DEFAULT_LETTER_RATIO*char_h;

    qreal normal = 10.0/char_w;
    qreal weight = static_cast<qreal>(font.weight());

    // Weight scale goes from 100 to 900 where the middle is 500 which is my normal.
    // So this computes a constant which tells me how to multiply the char with in order to obtain
    // the line width for the text.
    this->borderWidth = char_w*normal*weight/500.0;

    // qDebug() << "Setting font border width to " << this->borderWidth;

    this->computeBoundingRect();
}

void RenderServerTextItem::setDefaultTextColor(const QColor &col){
    this->fillColor = col.name();
    this->borderColor = col.name();
}

void RenderServerTextItem::computeBoundingRect() {

    qreal h = this->fontSize;
    qreal w = DEFAULT_LETTER_RATIO*h;

    int longestLine = 0;

    qreal dy = h;
    if (this->lineSpacing > 1){
        dy = h*this->lineSpacing;
    }

    // First pass couns the number of lines and the number of characters per line
    int ncharsInLine = 0;
    QList<int> charPerLines;


    for (int i =0; i < this->text.size(); i++){
        if (this->text.at(i) == '\n'){
            charPerLines << ncharsInLine;
            if (ncharsInLine > longestLine) longestLine = ncharsInLine;
            ncharsInLine = 0;
        }
        else ncharsInLine++;
    }

    // The last line needs to be added after the cycle.
    charPerLines << ncharsInLine; // Adding the last line
    if (ncharsInLine > longestLine) longestLine = ncharsInLine; // And also checking if it's not the largest one

    // Now we compute the top left assuming that the x,y positio is the center mass of the text.
    qreal brw = longestLine*w;
    qreal brh = dy*static_cast<qreal>(charPerLines.size());

    qreal left = this->x - brw/2;
    qreal top  = this->y - brh/2;

    // qDebug() << "The text width is" << brw << " The text height is" << brh << " so to center in " << this->x << this->y << " we define top left as " << top << left;

    this->bRect.setTop(top);
    this->bRect.setLeft(left);
    this->bRect.setWidth(brw);
    this->bRect.setHeight(brh);

}


void RenderServerTextItem::computeTopLeftInRenderServer() {



}
