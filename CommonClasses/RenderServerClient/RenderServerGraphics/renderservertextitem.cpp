#include "renderservertextitem.h"

RenderServerTextItem::RenderServerTextItem(const QString &text):RenderServerItem() {

    bRect.setTop(0);
    bRect.setLeft(0);
    bRect.setWidth(100);
    bRect.setHeight(100);

    QStringList renderItemList = itemData.value(RenderControlPacketFields::RENDER_LIST).toStringList();

    renderItemList << RenderControlPacketFields::X
    << RenderControlPacketFields::Y
    << RenderControlPacketFields::COLOR
    << RenderControlPacketFields::BORDER_COLOR
    << RenderControlPacketFields::BORDER_WIDTH
    << RenderControlPacketFields::TEXT
    << RenderControlPacketFields::FONT_SIZE
    << RenderControlPacketFields::TEXT_ALIGNMENT;

    itemData[RenderControlPacketFields::RENDER_LIST] = renderItemList;

    QVariantList x; x << 0;
    QVariantList y; y << 0;
    itemData[RenderControlPacketFields::X] = x;
    itemData[RenderControlPacketFields::Y] = y;
    itemData[RenderControlPacketFields::WIDTH] = 100;
    itemData[RenderControlPacketFields::HEIGHT] = 100;
    itemData[RenderControlPacketFields::TEXT] = text;

    itemData[RenderControlPacketFields::FONT_SIZE] = 10;
    itemData[RenderControlPacketFields::LINE_SPACING] = 1;
    itemData[RenderControlPacketFields::TEXT_ALIGNMENT] = QString(ALIGN_CENTER);

    itemData[RenderControlPacketFields::TYPE]      = GL2DItemType::TYPE_TEXT;
    itemData[RenderControlPacketFields::TYPE_NAME] = RenderServerItemTypeName::TEXT;

}

RenderServerTextItem::RenderServerTextItem(const QVariantMap &idata):RenderServerItem(idata){
    computeBoundingRect();
}

void RenderServerTextItem::setPos(qreal x, qreal y){
    QVariantList xl = itemData.value(RenderControlPacketFields::X).toList();
    QVariantList yl = itemData.value(RenderControlPacketFields::Y).toList();

    xl[0] = x;
    yl[0] = y;

    itemData[RenderControlPacketFields::X] = xl;
    itemData[RenderControlPacketFields::Y] = yl;
    computeBoundingRect();
}

void RenderServerTextItem::setText(const QString &text){
    itemData[RenderControlPacketFields::TEXT] = text;
    computeBoundingRect();
}

void RenderServerTextItem::setAlignment(const QString &alignment){
    itemData[RenderControlPacketFields::TEXT_ALIGNMENT] = alignment;
    computeBoundingRect();
}

void RenderServerTextItem::setFont(const QFont &font){
    itemData[RenderControlPacketFields::FONT_SIZE] = font.pointSizeF();
    qreal fontSize = font.pointSizeF();

    // Computing the border with according the following formula
    qreal char_h = fontSize;
    qreal char_w = DEFAULT_LETTER_RATIO*char_h;

    qreal normal = 10.0/char_w;
    qreal weight = static_cast<qreal>(font.weight());

    // Weight scale goes from 100 to 900 where the middle is 500 which is my normal.
    // So this computes a constant which tells me how to multiply the char with in order to obtain
    // the line width for the text.
    itemData[RenderControlPacketFields::BORDER_WIDTH] = char_w*normal*weight/500.0;

    // qDebug() << "Setting font border width to " << borderWidth;

    computeBoundingRect();
}

void RenderServerTextItem::setDefaultTextColor(const QColor &col){
    itemData[RenderControlPacketFields::COLOR] = col.name();
    itemData[RenderControlPacketFields::BORDER_COLOR] = col.name();
}


qreal RenderServerTextItem::x() const {
    return itemData.value(RenderControlPacketFields::X).toList().first().toReal();
}

qreal RenderServerTextItem::y() const {
    return itemData.value(RenderControlPacketFields::Y).toList().first().toReal();
}

void RenderServerTextItem::computeBoundingRect() {

    qreal fontSize = itemData[RenderControlPacketFields::FONT_SIZE].toReal();
    qreal lineSpacing = itemData[RenderControlPacketFields::LINE_SPACING].toReal();
    QString text = itemData[RenderControlPacketFields::TEXT].toString();

    qreal h = fontSize;
    qreal w = DEFAULT_LETTER_RATIO*h;

    int longestLine = 0;

    qreal dy = h;
    if (lineSpacing > 1){
        dy = h*lineSpacing;
    }

    // First pass couns the number of lines and the number of characters per line
    int ncharsInLine = 0;
    QList<int> charPerLines;

    for (int i =0; i < text.size(); i++){
        if (text.at(i) == '\n'){
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

    // qDebug() << "The text width is" << brw << " The text height is" << brh << " so to center in " << x << y << " we define top left as " << top << left;

    bRect.setTop(this->y());
    bRect.setLeft(this->x());
    bRect.setWidth(brw);
    bRect.setHeight(brh);

    itemData[RenderControlPacketFields::WIDTH] = brw;
    itemData[RenderControlPacketFields::HEIGHT] = brh;

}

