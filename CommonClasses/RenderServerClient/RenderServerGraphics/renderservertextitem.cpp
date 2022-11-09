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
    this->itemType = "Text";
    this->borderWidth = 20;
    this->fontSize = 10;
    this->lineSpacing = 1;
    this->alignment = ALIGN_CENTER;

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
    float char_h = static_cast<float>(this->fontSize);
    float char_w = static_cast<float>(DEFAULT_LETTER_RATIO)*char_h;

    float normal = 10.0f/char_w;
    float weight = static_cast<float>(font.weight());

    // Weight scale goes from 100 to 900 where the middle is 500 which is my normal.
    // So this computes a constant which tells me how to multiply the char with in order to obtain
    // the line width for the text.
    this->borderWidth = char_w*normal*weight/500.0f;

    qDebug() << "Setting font border width to " << this->borderWidth;

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

    qDebug() << "The text width is" << brw << " The text height is" << brh << " so to center in " << this->x << this->y << " we define top left as " << top << left;

    this->bRect.setTop(top);
    this->bRect.setLeft(left);
    this->bRect.setWidth(brw);
    this->bRect.setHeight(brh);

}


void RenderServerTextItem::computeTopLeftInRenderServer() {



}
