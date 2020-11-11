#include "perceptionmanager.h"

PerceptionManager::PerceptionManager()
{

}



bool PerceptionManager::parseExpConfiguration(const QString &contents){
    PerceptionParser parser;
    if (!parser.parsePerceptionExperiment(contents)){
        error = "Parsing Perception Experiment: " + parser.getError();
        return false;
    }


    perceptionTrials = parser.getTrials();

    if (config->getBool(CONFIG_DEMO_MODE)) enableDemoMode();

    return true;

}


void PerceptionManager::init(ConfigurationManager *c){
    ExperimentDataPainter::init(c);
    clearCanvas();
    canvas->setBackgroundBrush(QBrush(Qt::gray));

    noText = "NO";
    if (c->getString(CONFIG_REPORT_LANGUAGE) == CONFIG_P_LANG_ES) yesText = "SI";
    else yesText = "YES";


    // Generating the bounding rects.
    QFont yesOrNo = QFont("Arial",YES_NO_FONT_SIZE,QFont::Bold);

    // Fixing the question font

    QGraphicsTextItem *yes = canvas->addText(yesText,yesOrNo);

    qreal box_w = yes->boundingRect().width()*AIR_YES_NO_TEXT_W;
    qreal box_h = yes->boundingRect().height()*AIR_YES_NO_TEXT_H;

    qreal margin = ScreenResolutionWidth*MARGIN_YES_NO_TEXT;
    qreal x_yes_box = margin;
    qreal y_box = ScreenResolutionHeight/2 - box_h/2;
    qreal x_no_box  = ScreenResolutionWidth - margin - box_w;

    yesAndNoTargetBoxes.clear();
    yesAndNoTargetBoxes <<  QRectF(x_no_box,y_box,box_w,box_h);  // 0 is NO
    yesAndNoTargetBoxes <<  QRectF(x_yes_box,y_box,box_w,box_h); // 1 is YES.


}

qint32 PerceptionManager::size() const{
    return perceptionTrials.size();
}


qreal PerceptionManager::sizeToProcess() const{
    return perceptionTrials.size();
}

QList<QRectF> PerceptionManager::getYesAndNoTargetBoxes() const{
    return yesAndNoTargetBoxes;
}

PerceptionManager::PerceptionTrialState  PerceptionManager::getPerceptionTrialState() const{
    return currentTrialState;
}


void PerceptionManager::enableDemoMode(){
    while (perceptionTrials.size() > NUMBER_OF_TRIALS_IN_DEMO_MODE){
        perceptionTrials.removeLast();
    }
}

QString PerceptionManager::getCurrentTrialHeader() const{
    if ((currentTrial < 0) || (currentTrial >= perceptionTrials.size())) return "";
    return perceptionTrials.at(currentTrial).toString();
}

void PerceptionManager::prepareForTrialSet(){
    currentTrial = -1;
    currentTrialState = PTS_YES_OR_NO;
}

bool PerceptionManager::drawNextTrialState(){
    switch(currentTrialState){
    case PTS_CROSS_ONLY:
        currentTrialState = PTS_TRIANGLES;
        drawTrial(currentTrial);
        break;
    case PTS_TRIANGLES:
        currentTrialState = PTS_YES_OR_NO;
        drawTrial(currentTrial);
        break;
    case PTS_YES_OR_NO:
        currentTrialState = PTS_CROSS_ONLY;
        currentTrial++;
        if (currentTrial < perceptionTrials.size()){
            drawTrial(currentTrial);
        }
        else return true;
        break;
    }

    return false;
}

void PerceptionManager::drawTrial(qint32 trial_index){

    if ((trial_index < 0 ) || (trial_index >=  perceptionTrials.size())) return;

    PerceptionParser::PerceptionTrial trial = perceptionTrials.at(trial_index);

    // Basic math to compute squre side.
    // The width and the height is the same. This assumes the relationship with screen height and pixel height is more or less the same than the relationship of screen width to width in pixels.
    qreal effective_height = ScreenResolutionHeight*(1.0 - 2*K_WIDTH_MARGIN);

    // A grid has white space of the same dimension as the used space. Hence if there are N squares, the effective lenght is divided by N+(N-1) = 2*N-1.
    qreal square_side = effective_height/(2*NUMBER_OF_ELEMENTS_PER_ROW -1);
    qreal effective_width = square_side*(2*NUMBER_OF_ELEMENTS_PER_COL - 1);

    // The offset of the squre at (row,col) 0,0
    qreal x_offset = (ScreenResolutionWidth-effective_width)/2;
    qreal y_offset = (ScreenResolutionHeight-effective_height)/2;

    clearCanvas();

    QPen linePen(QBrush(Qt::black),2);
    qreal half_a = square_side/2;

    // The center cross. Where c and r are both 2.
    qreal x = x_offset + 2*2*square_side;
    qreal y = y_offset + 2*2*square_side;
    canvas->addLine(x,y+half_a,x+square_side,y+half_a,linePen);
    canvas->addLine(x+half_a,y,x+half_a,y+square_side,linePen);

    if (currentTrialState == PTS_TRIANGLES){

        for (qint32 r = 0; r < trial.trialDescription.size(); r++){
            for (qint32 c = 0; c < trial.trialDescription.at(r).size(); c++){

                qreal x = x_offset + 2*c*square_side;
                qreal y = y_offset + 2*r*square_side;

                if (trial.trialDescription.at(r).at(c) != PerceptionParser::TT_X){
                    QPolygonF triangle = getTrianglePoligonAt(x,y,square_side,trial.trialDescription.at(r).at(c));
                    canvas->addPolygon(triangle,linePen);
                }
            }
        }

    }
    else if (currentTrialState == PTS_YES_OR_NO){

        QFont yesOrNo = QFont("Arial",YES_NO_FONT_SIZE,QFont::Bold);

        // Fixing the question font

        QGraphicsTextItem *yes = canvas->addText(yesText,yesOrNo);
        QGraphicsTextItem *no = canvas->addText(noText,yesOrNo);

        yesAndNoBoxes.clear();
        yesAndNoBoxes << canvas->addRect(yesAndNoTargetBoxes.first(),linePen);  // 0 is NO.
        yesAndNoBoxes << canvas->addRect(yesAndNoTargetBoxes.last(),linePen);   // 1 is Yes.

        yes->setPos(yesAndNoTargetBoxes.last().left() + (yesAndNoTargetBoxes.last().width()  - yes->boundingRect().width())/2,
                    yesAndNoTargetBoxes.last().top() + (yesAndNoTargetBoxes.last().height() - yes->boundingRect().height())/2 );
        yes->setZValue(2);

        no->setPos(yesAndNoTargetBoxes.first().left() + (yesAndNoTargetBoxes.first().width()-no->boundingRect().width())/2,
                   yesAndNoTargetBoxes.first().top() +  (yesAndNoTargetBoxes.first().height()-no->boundingRect().height())/2);
        no->setZValue(2);

    }


}

QPolygonF PerceptionManager::getTrianglePoligonAt(qreal x, qreal y, qreal a, PerceptionParser::TriangleType triangle_type){
    QPolygonF triangle;

    // In an equilateral triangle the height is sqrt(3)/2 it's side.
    qreal h = 0.87*a;
    qreal c_off = (a-h)/2;
    qreal half_a = a/2;

    switch(triangle_type){
    case PerceptionParser::TT_DOWN:
        triangle << QPointF(x,y+c_off) << QPointF(x+half_a,y+a-c_off) << QPointF(x+a,y+c_off);
        break;
    case PerceptionParser::TT_UP:
        triangle << QPointF(x+half_a,y+c_off) << QPointF(x+a,y+a-c_off) << QPointF(x,y+a-c_off);
        break;
    case PerceptionParser::TT_RIGHT:
        triangle << QPointF(x+c_off,y) << QPointF(x+a-c_off,y+half_a) << QPointF(x+c_off,y+a);
        break;
    case PerceptionParser::TT_LEFT:
        triangle << QPointF(x+c_off,y+half_a) << QPointF(x+a-c_off,y) << QPointF(x+a-c_off,y+a);
        break;
    }

    return triangle;
}


void PerceptionManager::highlightSelection(qreal x, qreal y){

    if (currentTrialState != PTS_YES_OR_NO) return;

    for (qint32 i = 0; i < yesAndNoBoxes.size(); i++){
        yesAndNoBoxes.at(i)->setBrush(QBrush(QColor(Qt::gray)));
        yesAndNoBoxes.at(i)->setZValue(1);
        if (yesAndNoTargetBoxes.at(i).contains(x,y)){
            yesAndNoBoxes.at(i)->setBrush(QBrush(QColor(Qt::gray).lighter(150)));
        }
    }

}

