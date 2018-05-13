#include "readingmanager.h"

ReadingManager::ReadingManager()
{
    description = "";
}

void ReadingManager::init(ConfigurationManager *c){

    ExperimentDataPainter::init(c);

    // Setting the background brush.
    canvas->setBackgroundBrush(QBrush(Qt::white));

    // Setting the radious and the default positions.
    R = 0.01*canvas->width();

    // Default font
    questionFont = QFont("Mono",23,QFont::Bold);

    // Fixing the question font
    questionFont.setFamily("Courier New");
    questionFont.setPointSize(20);
    questionFont.setItalic(false);
    questionFont.setBold(false);

    // Calculating the char width with QFontMetrics.
    QFontMetrics metrics(questionFont);

    // Since the font is monospaced the width of A is the same as that of any other character.
    charWidth = metrics.width(QChar('A'));
}

bool ReadingManager::parseExpConfiguration(const QString &contents){


    // Getting the description if available.
    QString experimentData = contents;
    qint32 commentToken = contents.indexOf(READING_COMMENT_STRING);
    if (commentToken != -1){
        qint32 end = commentToken+QString(READING_COMMENT_STRING).length();
        description = contents.mid(0,commentToken);
        experimentData = contents.mid(end+1);
    }

    // Generating the contents from the phrases
    QStringList lines = experimentData.split('\n',QString::KeepEmptyParts);

    for (qint32 i = 0; i < lines.size(); i++){

        QString line = lines.at(i);

        line = line.trimmed();
        if (line.isEmpty()) continue;

        Phrase p;

        QStringList idAndQuestion = line.split(':',QString::SkipEmptyParts);
        if (idAndQuestion.size() != 2){
            error = "There should only be 1 instance of : in the question " + line + " @ " + QString::number(i+1);
            return false;
        }

        bool ok;
        p.setID(idAndQuestion.first().toInt(&ok));
        if (!ok){
            error = "Invalid id (it is not an integer) " + idAndQuestion.first() + " @ " + QString::number(i+1);
            return false;
        }

        QStringList qAndA = idAndQuestion.last().split("|",QString::SkipEmptyParts);
        p.setPhrase(qAndA.first().trimmed());

        if (qAndA.size() == 2){
            QStringList options = qAndA.last().split(',',QString::SkipEmptyParts);
            for (qint32 j = 0; j < options.size(); j++){
                p.appendToFollowUp(options.at(j).trimmed());
            }
        }
        else if (qAndA.size() != 1){
            error = "There should only at most 1 instance of | in the question " + line + " @ " + QString::number(i+1);
            return false;
        }

        phrases << p;

    }

    return true;

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

    //qWarning() << "Drawing selected index" << selectedIndex;
    //qWarning() << "Canvas size" << canvas->width() << canvas->height();
    drawPhrase(QS_PHRASE,selectedIndex,true);
    //qWarning() << "Drawn";
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
    canvas->clear();
    QString mainPhrase;
    qint32 xpos, ypos;
    qint32 WScreen,HScreen;
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
        escapeX = xpos + phraseToShow->boundingRect().width() + escapeX/2;
        escapeY = HScreen - (ypos + phraseToShow->boundingRect().height());
        escapeY = ypos + phraseToShow->boundingRect().height() + escapeY/2;
        escapePoint->setPos(escapeX,escapeY);

        // Y position calculation for the start reading point
        qint32 yposp = ypos + phraseToShow->boundingRect().height()/2;

        // Creating a screen to hide the phrase.
        canvas->addRect(0,0,WScreen,HScreen,QPen(),QBrush(Qt::white));

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
        qint32 summedWidth = 0;
        qint32 NOptions = phrases.at(currentQuestion).getNofOptions();
        for (qint32 i = 1; i < phrases.at(currentQuestion).size(); i++){
            QString option = phrases.at(currentQuestion).getFollowUpAt(i);
            //qWarning() << "Option" << i << "is" << option;
            QGraphicsSimpleTextItem *item = canvas->addSimpleText(option,questionFont);
            item->setPen(QPen(Qt::black));
            item->setBrush(QBrush(Qt::red));
            list << item;
            summedWidth = item->boundingRect().width() + summedWidth + 2*AIR;
        }

        // Now the options are drawn, surrounded by a thick, red rectangle and equally spaced, 10% Screen height, below the phrase
        qint32 spaceBetweenOptions = (WScreen - summedWidth)/(NOptions+1);
        qint32 x = 0;
        qint32 y = ypos + phraseToShow->boundingRect().height() + 0.1*HScreen;
        for (qint32 i = 0; i < NOptions; i++){

            // Drawing the rectangle on the bottom of the option;

            //qWarning() << "Drawing rectangle" << i+1;

            x = x + spaceBetweenOptions;
            QRect r(0,0,list.at(i)->boundingRect().width() + 2*AIR,list.at(i)->boundingRect().height() + 2*AIR);
            QGraphicsRectItem *ri =  canvas->addRect(r);
            ri->setPen(QPen(QBrush(Qt::red),2));
            ri->setPos(x,y);

            // Translation is required to the rectangle is positioned correctly on the screen.
            r.translate(x,y);
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
        return (x - phraseToShow->pos().x())/charWidth;
    }
    return -1;
}



