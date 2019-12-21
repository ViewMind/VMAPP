#include "readingmanager.h"

ReadingManager::ReadingManager()
{
    description = "";
}

void ReadingManager::init(ConfigurationManager *c){

    ExperimentDataPainter::init(c);

    // Setting the background brush.
    canvas->setBackgroundBrush(QBrush(Qt::gray));

    // Setting the radious and the default positions.
    R = 0.01*canvas->width();

    // Default font
    questionFont = QFont("Mono",23,QFont::Bold);

    // Fixing the question font    
    //questionFont.setFamily("Courier New");
    //questionFont.setPointSize(20);
    questionFont.setFamily(c->getString(CONFIG_READING_FONT_NAME));
    questionFont.setPointSize(c->getInt(CONFIG_READING_FONT_SIZE));
    questionFont.setItalic(false);
    questionFont.setBold(false);

    // Calculating the char width with QFontMetrics.
    QFontMetrics metrics(questionFont);

    // Since the font is monospaced the width of A is the same as that of any other character.
    //charWidth = metrics.width(QChar('A'));
    charWidth = metrics.averageCharWidth();
}

bool ReadingManager::parseExpConfiguration(const QString &contents){

    ReadingParser parser;
    if (!parser.parseReadingDescription(contents)) {
        error = parser.getError();
        return false;
    }

    description = parser.getDescription();
    versionString = parser.getVersion();
    phrases = parser.getPhrases();
    expectedIDs = parser.getExpectedIDs();

    if (config->getBool(CONFIG_DEMO_MODE)) enableDemoMode();
    return true;

}

void ReadingManager::enableDemoMode(){
    while (phrases.size() > NUMBER_OF_TRIALS_IN_DEMO_MODE){
        phrases.removeLast();
    }
}

bool ReadingManager::drawPhrase(const QString &idStr){

    bool ok;
    qint32 id = idStr.toInt(&ok);
    if (!ok) return false;

    // Finding the phrase with the right id;
    qint32 selectedIndex = -1;
    for (qint32 i = 0; i < phrases.size(); i++){
        if (phrases.at(i).getID() == id){
            selectedIndex = i;
            break;
        }
    }

    if (selectedIndex == -1) {
        error = "Could not find phrase with id: " + idStr;
        return false;
    }

    drawPhrase(QS_PHRASE,selectedIndex,true);
    return true;

}

void ReadingManager::drawPhrase(QuestionState qstate, qint32 currentQuestion, bool simpleDraw){

    //qWarning() << "Drawing the phrase";

    if ( ((qstate == QS_PHRASE) || (qstate == QS_INFORMATION)) && !simpleDraw){

        //qWarning() << "Showing the phrase";

        // Should simply bring the escape point and phrase forward, while hiding the start point.
        phraseToShow->setZValue(10);
        escapePoint->setZValue(10);
        startPoint->setZValue(-10);

        // Chaging the current target point to escape point
        currentTargetX = escapeX+R;
        currentTargetY = escapeY+R;

        // Nothing more to do.
        return;

    }

    // Any other state the phrase needs to be drawn.
    clearCanvas();

    QString mainPhrase;
    qreal xpos, ypos;
    qreal WScreen,HScreen;
    WScreen = canvas->width();
    HScreen = canvas->height();

    if (qstate != QS_QUESTION) mainPhrase = phrases.at(currentQuestion).getPhrase();
    else mainPhrase = phrases.at(currentQuestion).getFollowUpQuestion();

    phraseToShow = canvas->addSimpleText(mainPhrase,questionFont);
    phraseToShow->setPen(QPen(Qt::black));
    phraseToShow->setBrush(QBrush(Qt::black));
    xpos = (WScreen - phraseToShow->boundingRect().width())/2;
    ypos = (HScreen - phraseToShow->boundingRect().height())/2;
    phraseToShow->setPos(xpos,ypos);

    // The start and escape point are drawn
    if (qstate == QS_POINT){

        // The escape point should allways be in the bottom right of the screen.
        escapePoint = canvas->addEllipse(0,0,2*R,2*R,QPen(QBrush(Qt::black),2),QBrush(Qt::red));

        // Caculating escape point coordinates.
        escapeX = WScreen - (xpos + phraseToShow->boundingRect().width());
        escapeX = xpos + phraseToShow->boundingRect().width() + escapeX*config->getReal(CONFIG_READING_ESCAPE_POINT_XY_K);
        escapeY = HScreen - (ypos + phraseToShow->boundingRect().height());
        escapeY = ypos + phraseToShow->boundingRect().height() + escapeY*config->getReal(CONFIG_READING_ESCAPE_POINT_XY_K);
        escapePoint->setPos(escapeX,escapeY);

        // Y position calculation for the start reading point
        qreal yposp = ypos + phraseToShow->boundingRect().height()/2;

        // Creating a screen to hide the phrase.
        canvas->addRect(0,0,WScreen,HScreen,QPen(Qt::gray),QBrush(Qt::gray));

        // This dot should appear about in the middle of the first letter.
        startPoint = canvas->addEllipse(0,0,2*R,2*R,QPen(QBrush(Qt::black),2),QBrush(Qt::red));
        startPoint->setPos(xpos-R,yposp-R);

        // Hiding both the phrase AND the escape point
        escapePoint->setZValue(-10);
        escapePoint->setZValue(-10);

        // The current advance point is the start point so...
        currentTargetX = xpos;
        currentTargetY = yposp;

        return;
    }

    if (qstate == QS_QUESTION){

        // The options should be drawn.
        QList<QGraphicsSimpleTextItem*> list;
        validClickAreas.clear();

        // First the text items are created.
        qreal summedWidth = 0;
        qint32 NOptions = phrases.at(currentQuestion).getNofOptions();
        for (qint32 i = 1; i < phrases.at(currentQuestion).size(); i++){
            QString option = phrases.at(currentQuestion).getFollowUpAt(i);
            QGraphicsSimpleTextItem *item = canvas->addSimpleText(option,questionFont);
            item->setPen(QPen(Qt::black));
            item->setBrush(QBrush(Qt::red));
            list << item;
            summedWidth = item->boundingRect().width() + summedWidth + 2*AIR;
        }

        // Now the options are drawn, surrounded by a thick, red rectangle and equally spaced, 10% Screen height, below the phrase
        qreal spaceBetweenOptions = (WScreen - summedWidth)/(NOptions+1);
        qreal x = 0;
        qreal y = ypos + phraseToShow->boundingRect().height() + 0.1*HScreen;
        for (qint32 i = 0; i < NOptions; i++){

            // Drawing the rectangle on the bottom of the option;
            x = x + spaceBetweenOptions;

            //qWarning() << "Drawing rectangle i" << i << "at" << x << "for option" << list.at(i)->text();
            qint32 rW = static_cast<qint32>(list.at(i)->boundingRect().width() + 2*AIR);
            qint32 rH = static_cast<qint32>(list.at(i)->boundingRect().height() + 2*AIR);
            QRect r(0,0,rW,rH);
            QGraphicsRectItem *ri =  canvas->addRect(r);
            ri->setPen(QPen(QBrush(Qt::red),2));
            ri->setPos(x,y);

            // Translation is required to the rectangle is positioned correctly on the screen.
            r.translate(static_cast<qint32>(x),static_cast<qint32>(y));
            validClickAreas << r;

            // The text is positioned in the center of the rectangle and brought to the forefront.
            list.at(i)->setPos(x+AIR,y+AIR);
            list.at(i)->setZValue(10);

            // Adding the displacement of x for size of the complete rectangle.
            x = x + list.at(i)->boundingRect().width() + 2*AIR;

        }
        //qWarning() << "Done drawing options";
    }

}

qint32 ReadingManager::isPointContainedInAClickArea(const QPoint &point){
    for (qint32 i = 0; i < validClickAreas.size(); i++){
        if (validClickAreas.at(i).contains(point)){
            return i;
        }
    }
    return -1;
}

qint32 ReadingManager::getCharIndex(qint32 x){
    if ((x > phraseToShow->pos().x()) && (x < phraseToShow->pos().x() + phraseToShow->boundingRect().width())){
        return static_cast<qint32>((x - phraseToShow->pos().x())/charWidth);
    }
    return -1;
}



