#include "nbackmanager.h"

const qreal NBackManager::K_TARGET_R          =                        0.022; // of the width.
const qreal NBackManager::K_TARGET_OFFSET_X   =                        0.02;
const qreal NBackManager::K_TARGET_OFFSET_Y   =                        0.036;
const qreal NBackManager::K_CROSS_LINE_LENGTH =                        0.05;

const char * NBackManager::CONFIG_PAUSE_TEXT_LANG   = "pause_text_lang";
const char * NBackManager::CONFIG_IS_VS             = "is_VS";

const char * NBackManager::LANG_EN = "EN";
const char * NBackManager::LANG_ES = "ES";

const char * NBackManager::PAUSE_TEXT_SPANISH = "Presione \"G\" para continuar el estudio";
const char * NBackManager::PAUSE_TEXT_ENGLISH = "Press \"G\" to continue with study";

NBackManager::NBackManager(){
    currentlyLitUp = -1;
    connect(&litUpTimer,&QTimer::timeout,this,&NBackManager::onLightOffTimeout);
    numberOfExplanationScreens = NUMBER_OF_EXPLANATION_SLIDES;
}

void NBackManager::setDebugSequenceValue(qint32 next_target_box){
    if (DBUGBOOL(Debug::Options::RENDER_HITBOXES)){
        if (next_target_box == -1){
            gDebugSequenceValue->setZValue(-1);
        }
        else{
            gDebugSequenceValue->setZValue(1);
            gDebugSequenceValue->setText(QString::number(next_target_box));
        }
    }
}

bool NBackManager::parseExpConfiguration(const QString &contents){

    NBackParser parser;
    if (!parser.parseFieldingExperiment(contents,ScreenResolutionWidth,ScreenResolutionHeight)){
        error = parser.getError();
        return false;
    }

    nbackTrials  = parser.getParsedTrials();
    versionString   = parser.getVersionString();
    hitTargetBoxes  = parser.getHitTargetBoxes();
    drawTargetBoxes = parser.getDrawTargetBoxes();

    return true;

}

void NBackManager::ligthOffAllBoxes(){
    //qDebug() << "Turn Off";
    currentlyLitUp = -1;
    lastBoxLitUp   = -1;
    for (qint32 i = 0; i < graphicalTargetBoxes.size(); i++){
        graphicalTargetBoxes.at(i)->setBrush(QBrush(Qt::black));
    }
}

void NBackManager::onLightOffTimeout(){
    if (currentlyLitUp != -1){
        graphicalTargetBoxes.at(currentlyLitUp)->setBrush(QBrush(Qt::black));
        lastBoxLitUp = currentlyLitUp;
        currentlyLitUp = -1;
    }
}

bool NBackManager::lightUpBox(qint32 box_index){
    if (box_index < 0) return false;
    if (box_index >= graphicalTargetBoxes.size()) return false;
    if (box_index == currentlyLitUp) return false;

    // When all boxes are set to light up regardless whether it's the proper box or not, then this will prevent when staring at the same box for too long
    // for it to remain lit up.
    if (box_index == lastBoxLitUp) return false;

    graphicalTargetBoxes.at(box_index)->setBrush(QBrush(QColor(228,228,228)));
    currentlyLitUp = box_index;
    if (DBUGBOOL(Debug::Options::DBUG_MSG)){
        qDebug() << "DBUG: Lit UP Box" << box_index << "Last Box to Litup was" << lastBoxLitUp;
    }
    litUpTimer.start(LIGHT_OFF_TIME);
    return true;
}

void NBackManager::configure(const QVariantMap &config){
    if (config.value(CONFIG_PAUSE_TEXT_LANG).toString() == LANG_ES) pauseText = PAUSE_TEXT_SPANISH;
    else pauseText = PAUSE_TEXT_ENGLISH;

    if (config.value(CONFIG_IS_VS).toBool()){
        explanationListTextKey = STUDY_TEXT_KEY_NBACKVS;
    }
    else {
        explanationListTextKey = STUDY_TEXT_KEY_NBACKRT;
    }

}

void NBackManager::init(qreal display_resolution_width, qreal display_resolution_height){

    ExperimentDataPainter::init(display_resolution_width,display_resolution_height);
    clearCanvas();
    drawBackground();

    DrawingConstantsCalculator dcc;
    dcc.setTargetResolution(ScreenResolutionWidth,ScreenResolutionHeight);
    TARGET_R = dcc.getHorizontalRatio(K_TARGET_R)*ScreenResolutionWidth;

}

void NBackManager::drawBackground(){

    qreal centerX = ScreenResolutionWidth/2;
    qreal centerY = ScreenResolutionHeight/2;

    DrawingConstantsCalculator dcc;
    dcc.setTargetResolution(ScreenResolutionWidth,ScreenResolutionHeight);

    // Background
    this->clearCanvas();

    graphicalTargetBoxes.clear();
    canvas.addRect(0,0,ScreenResolutionWidth,ScreenResolutionHeight,QPen(),QBrush(Qt::black));

    for (qint32 i = 0; i < drawTargetBoxes.size(); i++){

        if (DBUGBOOL(Debug::Options::RENDER_HITBOXES)){

            // And Draws the Target Boxes.
            QRectF hitBoxToDraw = hitTargetBoxes.at(i);

            qDebug() << "DBUG NBACK HITBOX" << i << hitBoxToDraw;

            RenderServerRectItem *rect2 = canvas.addRect(0,0,hitBoxToDraw.width(),hitBoxToDraw.height(),
                                                       QPen(QBrush(Qt::green),2),
                                                       QBrush(Qt::black));

            rect2->setPos(hitBoxToDraw.x(),hitBoxToDraw.y());

        }

        RenderServerRectItem *rect = canvas.addRect(0,0,drawTargetBoxes.at(i).width(),drawTargetBoxes.at(i).height(),
                                                  QPen(QBrush(Qt::white),6),
                                                  QBrush(Qt::black));
        rect->setPos(drawTargetBoxes.at(i).x(),drawTargetBoxes.at(i).y());

        if (DBUGBOOL(Debug::Options::RENDER_HITBOXES)){

            // Debug Message Draws The Number On the Hit Boxes
            RenderServerTextItem *item = canvas.addText(QString::number(i));
            item->setDefaultTextColor(QColor(Qt::red));
            item->setFont(QFont("Mono",46));
            item->setPos(drawTargetBoxes.at(i).x(),drawTargetBoxes.at(i).y());
        }

        graphicalTargetBoxes << rect;
    }

    // Adding the cross
    qreal halfLine = ScreenResolutionWidth*dcc.getHorizontalRatio(K_CROSS_LINE_LENGTH)/2.0;
    gCrossLine0 = canvas.addLine(centerX-halfLine,centerY,
                                  centerX+halfLine,centerY,
                                  QPen(QBrush(Qt::white),4));
    gCrossLine1 = canvas.addLine(centerX,centerY-halfLine,
                                  centerX,centerY+halfLine,
                                  QPen(QBrush(Qt::white),4));

    gCrossLine0->setZValue(-1);
    gCrossLine1->setZValue(-1);

    // Adding the target
    gTarget = renderTargetCircle();

    // Adding the text
    QFont letterFont;
    letterFont.setFamily("Courier New");
    letterFont.setPointSize(50);
    letterFont.setBold(true);

    gText1 = canvas.addSimpleText("1",letterFont);
    qreal x = (ScreenResolutionWidth - gText1->boundingRect().width())/2;
    qreal y = (ScreenResolutionHeight - gText1->boundingRect().height())/2;
    gText1->setPos(x,y);
    gText1->setPen(QPen(Qt::white));
    gText1->setBrush(QBrush(Qt::white));

    gText2 = canvas.addSimpleText("2",letterFont);
    gText2->setPos(x,y);
    gText2->setPen(QPen(Qt::white));
    gText2->setBrush(QBrush(Qt::white));

    gText3 = canvas.addSimpleText("3",letterFont);
    gText3->setPos(x,y);
    gText3->setPen(QPen(Qt::white));
    gText3->setBrush(QBrush(Qt::white));

    if (DBUGBOOL(Debug::Options::RENDER_HITBOXES)){
        gDebugSequenceValue = canvas.addSimpleText("-1",letterFont);
        gDebugSequenceValue->setPos(x,y);
        gDebugSequenceValue->setPen(QPen(Qt::white));
        gDebugSequenceValue->setBrush(QBrush(Qt::white));
        gDebugSequenceValue->setZValue(-1);
    }

}

void NBackManager::setDrawState(DrawState ds){
    switch (ds){
    case DS_1:
        gText1->setZValue(1);
        gText2->setZValue(-1);
        gText3->setZValue(-1);
        gCrossLine0->setZValue(-1);
        gCrossLine1->setZValue(-1);
        gTarget->setZValue(-1);
        break;
    case DS_2:
        gText1->setZValue(-1);
        gText2->setZValue(1);
        gText3->setZValue(-1);
        gCrossLine0->setZValue(-1);
        gCrossLine1->setZValue(-1);
        gTarget->setZValue(-1);
        break;
    case DS_3:
        gText1->setZValue(-1);
        gText2->setZValue(-1);
        gText3->setZValue(1);
        gCrossLine0->setZValue(-1);
        gCrossLine1->setZValue(-1);
        gTarget->setZValue(-1);
        break;
    case DS_CROSS:
        gText1->setZValue(-1);
        gText2->setZValue(-1);
        gText3->setZValue(-1);
        gCrossLine0->setZValue(1);
        gCrossLine1->setZValue(1);
        gTarget->setZValue(-1);
        break;
    case DS_CROSS_TARGET:
        gText1->setZValue(-1);
        gText2->setZValue(-1);
        gText3->setZValue(-1);
        gCrossLine0->setZValue(1);
        gCrossLine1->setZValue(1);
        gTarget->setZValue(1);
        break;
    case DS_TARGET_BOX_ONLY:
        gText1->setZValue(-1);
        gText2->setZValue(-1);
        gText3->setZValue(-1);
        gCrossLine0->setZValue(-1);
        gCrossLine1->setZValue(-1);
        gTarget->setZValue(-1);
        break;
    }
}

bool NBackManager::setTargetPositionFromTrialName(const QString &trial, qint32 image){
    qint32 id = -1;
    for (qint32 i = 0; i < nbackTrials.size(); i++){
        if (nbackTrials.at(i).id == trial){
            id = i;
            break;
        }
    }

    if (id == -1) return false;
    setTargetPosition(id,image);
    return true;
}

void NBackManager::setTargetPosition(qint32 trial, qint32 image){

    // Check to fix a small bug when using a sequence of six, when changing to a new image image number will be 6 and the following instruction will break.
    if (image >= nbackTrials.at(trial).sequence.size()) return;

    setTargetPositionByRectangleIndex(nbackTrials.at(trial).sequence.at(image));
}

QPoint NBackManager::getTargetPoint(qint32 trial, qint32 image) const{
    QRectF r = drawTargetBoxes.at(nbackTrials.at(trial).sequence.at(image));
    return QPoint(static_cast<qint32>(r.x()),static_cast<qint32>(r.y()));
}

QList<qint32> NBackManager::getExpectedTargetSequenceForTrial(qint32 trial, qint32 targetNum) const{
    QList<qint32> hits;
    QList<qint32> sequence = nbackTrials.at(trial).sequence.mid(0,targetNum);

    for (qsizetype i = sequence.size()-1; i >= 0; i--){
        hits << sequence.at(i);
    }
    return hits;
}

QString NBackManager::getFullSequenceAsString(qint32 trial){
    QList<qint32> sequence = nbackTrials.at(trial).sequence;
    QStringList strnumbers;
    for (qint32 i = 0; i < sequence.size(); i++){
        strnumbers << QString::number(sequence.at(i));
    }
    return strnumbers.join(" ");
}

bool NBackManager::isPointInTargetBox(qreal x, qreal y, qint32 targetBox) const{
    //return FieldingParser::isHitInTargetBox(hitTargetBoxes,targetBox,x,y);
    return hitTargetBoxes.at(targetBox).contains(x,y);
}

qint32 NBackManager::pointIsInWhichTargetBox(qreal x, qreal y) const{
    for (qint32 i = 0; i < hitTargetBoxes.size(); i++){
        if (hitTargetBoxes.at(i).contains(x,y)){
            return i;
        }
    }
    return -1;
}

qint32 NBackManager::size() const {
    if (shortModeEnabled){
        return NUMBER_OF_TRIALS_IN_SHORT_MODE;
    }
    else {
        return static_cast<qint32>(nbackTrials.size());
    }
}

void NBackManager::drawPauseScreen(){

    this->clearCanvas();
    qreal xpos, ypos;

    QFont font;
    font = QFont("Mono",50);

    // Chaging the current target point to escape point
    RenderServerTextItem *phraseToShow = canvas.addSimpleText(pauseText,font);
    phraseToShow->setPen(QPen(QColor(Qt::white)));
    phraseToShow->setBrush(QBrush(QColor(Qt::white)));
    xpos = (canvas.width() - phraseToShow->boundingRect().width())/2;
    ypos = (canvas.height() - phraseToShow->boundingRect().height())/2;
    phraseToShow->setPos(xpos,ypos);
    phraseToShow->setZValue(1);

}


void NBackManager::renderStudyExplanationScreen(qint32 screen_index){

    QList<qint32> sequence;
    QMap<qint32,qint32> sequence_to_render;
    if (screen_index >= STUDY_EXPLANTION_TARGET_4){
        // Second example sequence
        sequence << 2 << 5 << 0;
    }
    else {
        // Third example sequence
        sequence << 0 << 3 << 1;
    }

    QColor arrow(Qt::red);

    canvas.clear();
    drawBackground();

    if (screen_index == STUDY_EXPLANTION_TARGET_1){
        setDrawState(DrawState::DS_CROSS_TARGET);
        setTargetPositionByRectangleIndex(sequence.at(0));
    }
    else if (screen_index == STUDY_EXPLANTION_TARGET_2){
        setDrawState(DrawState::DS_CROSS_TARGET);
        setTargetPositionByRectangleIndex(sequence.at(1));
    }
    else if (screen_index == STUDY_EXPLANTION_TARGET_3){
        setDrawState(DrawState::DS_CROSS_TARGET);
        setTargetPositionByRectangleIndex(sequence.at(2));
    }

    else if (screen_index == STUDY_EXPLANTION_LOOK_3){
        setDrawState(DrawState::DS_TARGET_BOX_ONLY);
        sequence_to_render[3] = sequence.at(2);
        renderPhantomTargets(sequence_to_render);
        renderStudyArrows(-1,sequence.at(2),arrow,true);
    }
    else if (screen_index == STUDY_EXPLANTION_LOOK_2){
        setDrawState(DrawState::DS_TARGET_BOX_ONLY);
        sequence_to_render[3] = sequence.at(2);
        sequence_to_render[2] = sequence.at(1);
        renderPhantomTargets(sequence_to_render);
        renderStudyArrows(sequence.at(2),sequence.at(1),arrow);
    }
    else if (screen_index == STUDY_EXPLANTION_LOOK_1){
        setDrawState(DrawState::DS_TARGET_BOX_ONLY);
        sequence_to_render[3] = sequence.at(2);
        sequence_to_render[2] = sequence.at(1);
        sequence_to_render[1] = sequence.at(0);
        renderPhantomTargets(sequence_to_render);
        renderStudyArrows(sequence.at(1),sequence.at(0),arrow);
    }

    else if (screen_index == STUDY_EXPLANTION_TARGET_4){
        setDrawState(DrawState::DS_CROSS_TARGET);
        setTargetPositionByRectangleIndex(sequence.at(0));
    }
    else if (screen_index == STUDY_EXPLANTION_TARGET_5){
        setDrawState(DrawState::DS_CROSS_TARGET);
        setTargetPositionByRectangleIndex(sequence.at(1));
    }
    else if (screen_index == STUDY_EXPLANTION_TARGET_6){
        setDrawState(DrawState::DS_CROSS_TARGET);
        setTargetPositionByRectangleIndex(sequence.at(2));
    }

    else if (screen_index == STUDY_EXPLANTION_LOOK_6){
        setDrawState(DrawState::DS_TARGET_BOX_ONLY);
        sequence_to_render[3] = sequence.at(2);
        renderPhantomTargets(sequence_to_render);
        renderStudyArrows(-1,sequence.at(2),arrow,true);
    }
    else if (screen_index == STUDY_EXPLANTION_LOOK_5){
        setDrawState(DrawState::DS_TARGET_BOX_ONLY);
        sequence_to_render[3] = sequence.at(2);
        sequence_to_render[2] = sequence.at(1);
        renderPhantomTargets(sequence_to_render);
        renderStudyArrows(sequence.at(2),sequence.at(1),arrow);
    }
    else if (screen_index == STUDY_EXPLANTION_LOOK_4){
        setDrawState(DrawState::DS_TARGET_BOX_ONLY);
        sequence_to_render[3] = sequence.at(2);
        sequence_to_render[2] = sequence.at(1);
        sequence_to_render[1] = sequence.at(0);
        renderPhantomTargets(sequence_to_render);
        renderStudyArrows(sequence.at(1),sequence.at(0),arrow);
    }
    else {
        setDrawState(DrawState::DS_CROSS);
    }

}

void NBackManager::renderPhantomTargets(QMap<qint32, qint32> rectangle_indexes){

    QFont letterFont;
    letterFont.setFamily("Courier New");
    letterFont.setPointSize(100);
    letterFont.setBold(true);

    QList<qint32> target_numbers = rectangle_indexes.keys();

    for (qint32 i = 0; i < target_numbers.size(); i++){

        qint32 target_box = rectangle_indexes[target_numbers.at(i)];

        RenderServerCircleItem * circle = renderTargetCircle();
        circle->setPen(QPen(Qt::gray));
        circle->setBrush(QBrush(QColor(Qt::red).lighter(180)));
        QRectF r = drawTargetBoxes.at(target_box);
        qreal pos_x = r.x() + (r.width() - circle->boundingRect().width())/2;
        qreal pos_y = r.y() + (r.height() - circle->boundingRect().height())/2;
        circle->setPos(pos_x,pos_y);


        RenderServerTextItem * indicator = canvas.addSimpleText(QString::number(target_numbers.at(i)),letterFont);
        //qDebug() << "Circle position for indicator is" << circle->boundingRect();

        // Want the center of the circle to coincide with the center of the tex.
        QRectF textBoundingRect = indicator->boundingRect();
        qreal textX = pos_x+TARGET_R - textBoundingRect.width()/2;
        qreal textY = pos_y+TARGET_R - textBoundingRect.height()/2;

        indicator->setPos(textX,textY); // Should be the center of the circle
        indicator->setBrush(QBrush(Qt::red));

    }

}

void NBackManager::setTargetPositionByRectangleIndex(qint32 rectangle_index){
    QRectF r = drawTargetBoxes.at(rectangle_index);
    gTarget->setPos(r.x() + (r.width() - gTarget->boundingRect().width())/2,
                   r.y() + (r.height() - gTarget->boundingRect().height())/2);

}

RenderServerCircleItem * NBackManager::renderTargetCircle(){
    //qDebug() << "Adding target cicle";
    RenderServerCircleItem * circle = canvas.addEllipse(0,0,TARGET_R*2,TARGET_R*2,QPen(),QBrush(Qt::red));
    return circle;
}

void NBackManager::renderStudyArrows(qint32 source_target, qint32 dest_target, const QColor &color, bool drawStartMark){

    Q_UNUSED(drawStartMark)

    qreal xo, yo;
    qreal xd, yd;

    if (source_target >= 0){
        QRectF r = drawTargetBoxes.at(source_target);
        xo = r.x() + r.width()/2;
        yo = r.y() + r.height()/2;
    }
    else {
        // Screen center.
        xo = ScreenResolutionWidth/2;
        yo = ScreenResolutionHeight/2;
    }

    QRectF r = drawTargetBoxes.at(dest_target);
    xd = r.x() + r.width()/2;
    yd = r.y() + r.height()/2;

    // The arrow is shortned
    QList<qreal> deltas = LineMath::PointDeltaToChangeSegmentLength(xo,yo,xd,yd,-2*TARGET_R);
    qreal Dx = deltas.first();
    qreal Dy = deltas.last();

    deltas = LineMath::PointDeltaToChangeSegmentLength(xo,yo,xd,yd,TARGET_R);
    qreal Tx = deltas.first();
    qreal Ty = deltas.last();

    qreal arrowWidth = 10;
    qreal arrowHeadHeightMultiplier = 4;
    qreal arrowHeadHeight = arrowWidth*arrowHeadHeightMultiplier;
    qreal arrowHeadLength = 2*arrowHeadHeight;

    RenderServerArrowItem * arrow =  canvas.addArrow(xo+Tx,yo+Ty,xd+Dx,yd+Dy,arrowHeadHeightMultiplier,arrowHeadLength,color);
    arrow->setPen(QPen(color,arrowWidth));

//    QGraphicsArrow arrow;
//    arrow.setArrowColor(color);
//    if (drawStartMark){
//        arrow.setArrowStartCircleParameters(3,color.lighter(130));
//    }
//    arrow.render(canvas,xo+Tx,yo+Ty,xd+Dx,yd+Dy,0.5);

}
