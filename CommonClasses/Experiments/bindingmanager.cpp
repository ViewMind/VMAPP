#include "bindingmanager.h"

BindingManager::BindingManager()
{
    canvas = nullptr;
}

void BindingManager::drawFlags(const BindingSlide &slide){

    canvas->clear();

    // Each target is drawn according to its values.

    for (qint32 i = 0; i < slide.size(); i++){
        QGraphicsRectItem *back  = canvas->addRect(0,0,drawStructure.FlagSideH,drawStructure.FlagSideV);
        QGraphicsRectItem *vrect = canvas->addRect(0,0,
                                                   drawStructure.FlagSideH-2*drawStructure.HLBorder,
                                                   drawStructure.FlagSideV-2*drawStructure.VSBorder);
        QGraphicsRectItem *hrect = canvas->addRect(0,0,
                                                   drawStructure.FlagSideH-2*drawStructure.HSBorder,
                                                   drawStructure.FlagSideV-2*drawStructure.VLBorder);
        back->setPos(slide.at(i).x,slide.at(i).y);
        // Calculating the other position based on the base flag position
        vrect->setPos(slide.at(i).x+drawStructure.HLBorder,slide.at(i).y+drawStructure.VSBorder);
        hrect->setPos(slide.at(i).x+drawStructure.HSBorder,slide.at(i).y+drawStructure.VLBorder);

        // Changing the colors.
        back->setBrush(QBrush(slide.at(i).back));
        vrect->setBrush(QBrush(slide.at(i).cross));
        hrect->setBrush(QBrush(slide.at(i).cross));
        vrect->setPen(QPen(slide.at(i).cross));
        hrect->setPen(QPen(slide.at(i).cross));

    }

}


bool BindingManager::drawFlags(const QString &trialName, bool show){

    // Getting the selected trial
    qint32 id = -1;
    for (qint32 i = 0; i < trials.size(); i++){
        if (trials.at(i).name == trialName){
            id = i;
            break;
        }
    }
    if (id == -1) return false;
    if (show) drawFlags(trials.at(id).show);
    else drawFlags(trials.at(id).test);
    return true;

}

void BindingManager::init(ConfigurationManager *c){
    ExperimentDataPainter::init(c);

    canvas->setBackgroundBrush(QBrush(Qt::gray));

    qint32 WScreen, HScreen;
    WScreen = canvas->width();
    HScreen = canvas->height();

    // Calculating the points for the cross
    qreal delta = 0.01*WScreen;
    line0.setP1(QPointF(WScreen/2 - delta,HScreen/2 - delta));
    line0.setP2(QPointF(WScreen/2 + delta,HScreen/2 + delta));
    line1.setP1(QPointF(WScreen/2 + delta,HScreen/2 - delta));
    line1.setP2(QPointF(WScreen/2 - delta,HScreen/2 + delta));

}

void BindingManager::drawCenter(qint32 currentTrial){
    canvas->clear();

    //currentTrial++;
    // Patch to avoid changing the logic of the working Binding experiment.
    //if (currentTrial >= trials.size()) currentTrial = trials.size()-1;

    if (trials.at(currentTrial).number == -1){
        canvas->addLine(line0,QPen(QBrush(Qt::red),2));
        canvas->addLine(line1,QPen(QBrush(Qt::red),2));
    }
    else{
        QGraphicsTextItem *number = canvas->addText(QString::number(trials.at(currentTrial).number),QFont("Mono",30));
        number->setPos((SCREEN_W - number->boundingRect().width())/2,(SCREEN_H - number->boundingRect().height())/2);
    }
}

void BindingManager::drawTrial(qint32 currentTrial, bool show){

    if (show){
        //qWarning() << "Drawing show for" << currentTrial;
        drawFlags(trials.at(currentTrial).show);
    }
    else{
        //qWarning() << "Drawing test for" << currentTrial;
        drawFlags(trials.at(currentTrial).test);
    }

}

void BindingManager::enableDemoMode(){
    while (trials.size() > NUMBER_OF_TRIALS_IN_DEMO_MODE){
        trials.removeLast();
    }
}

bool BindingManager::parseExpConfiguration(const QString &contents){

    bool legacyDescription = false;

    // Generating the contents from the phrases
    QStringList lines = contents.split('\n',QString::KeepEmptyParts);

    // Checking for old interpretation marking
    QStringList tokens;
    tokens = lines.first().split(' ',QString::SkipEmptyParts);

    // Making any character to signify old form
    bool largeTargets = false;
    if (tokens.size() == 2){
        if (tokens.last() == BINDING_LARGE_TARGET_MARK){
            largeTargets = true;
        }
    }

    usesNumbers = false;

    if (!config->containsKeyword(CONFIG_XPX_2_MM)){
        error = "Configuration file requires the horizontal pixel to mm ratio: x_px_mm";
        return false;
    }

    if (!config->containsKeyword(CONFIG_YPX_2_MM)){
        error = "Configuration file requires the vertical pixel to mm ratio: y_px_mm";
        return false;
    }    

    if (tokens.size() == 13) {
        // This is a legacy binding description.
        numberOfTargets = 2;
        largeTargets = true;
        legacyDescription = true;
    }
    else{

        // Getting the number of targets in the trial
        numberOfTargets = tokens.first().toInt();

        if ((numberOfTargets < 2) || (numberOfTargets > 4)){
            error = "Invalid number of targets in the first line " + lines.first();
            return false;
        }

    }

    // Defining the values to draw the target.
    drawStructure.xpos.clear();
    drawStructure.ypos.clear();

    if ((numberOfTargets == 2) && (largeTargets)){
        // Legacy values for targets.
        drawStructure.FlagSideH = 181;
        drawStructure.FlagSideV = 181;
        drawStructure.HSBorder = 4;
        drawStructure.HLBorder = 47;
        drawStructure.VSBorder = 24;
        drawStructure.VLBorder = 64;
        drawStructure.xpos << 165 << 677;
        drawStructure.ypos << 102 << 294 << 486;
    }
    else{

        drawStructure.FlagSideH = BINDING_TARGET_SIDE/config->getReal(CONFIG_XPX_2_MM);
        drawStructure.FlagSideV = BINDING_TARGET_SIDE/config->getReal(CONFIG_YPX_2_MM);
        drawStructure.HSBorder  = BINDING_TARGET_HS/config->getReal(CONFIG_XPX_2_MM);
        drawStructure.VLBorder  = BINDING_TARGET_VL/config->getReal(CONFIG_XPX_2_MM);
        drawStructure.HLBorder  = BINDING_TARGET_HL/config->getReal(CONFIG_YPX_2_MM);
        drawStructure.VSBorder  = BINDING_TARGET_VS/config->getReal(CONFIG_YPX_2_MM);

        qreal Wg = BINDING_TARGET_GRID/config->getReal(CONFIG_XPX_2_MM);
        qreal Hg = BINDING_TARGET_GRID/config->getReal(CONFIG_YPX_2_MM);
        qreal Sx = BINDING_TARGET_SIDE/config->getReal(CONFIG_XPX_2_MM);
        qreal Sy = BINDING_TARGET_SIDE/config->getReal(CONFIG_YPX_2_MM);
        qint32 Xog = (SCREEN_W - Wg)/2;
        qint32 Yog = (SCREEN_H - Hg)/2;
        qint32 Gx = Wg/3;
        qint32 Gy = Hg/3;

        // The grid is 3x3.
        for (qint32 i = 0; i < 3; i++){
            drawStructure.xpos << Xog + ((2*i+1)*Gx - Sx)/2;
        }

        for (qint32 j = 0; j < 3; j++){
            drawStructure.ypos << Yog + ((2*j+1)*Gy - Sy)/2;
        }

    }

    // Doing a Legacy parse.
    if (legacyDescription){
        return legacyParser(contents);
    }

    // Clear structure.
    trials.clear();
    qint32 i = 1;
    while (i < lines.size()){

        if (i + 6 >= lines.size()){
            if (!lines.at(i).isEmpty()){
                error = "The end of the file was not reached, however, there aren't 6 lines left @ line " + lines.at(i);
                return false;
            }
            else {
                i++;
                continue;
            }
        }

        tokens = lines.at(i).split(' ',QString::SkipEmptyParts);

        // Name [x|number] and [s|d]
        if (tokens.size() != 3){
            error = "Name line should have 3 and only 3 items @ line: " + lines.at(i);
            return false;
        }
        BindingTrial trial;
        trial.name = tokens.at(0);
        if (tokens.at(1) == 'x'){            
            if (usesNumbers){
                error = "Transition numbers were found, but then a x  @ line:" + lines.at(i);
                return false;
            }
            trial.number = -1;
        }
        else{
            usesNumbers = true;
            bool ok;
            trial.number = tokens.at(1).toUInt(&ok);
            if (!ok){
                error = "Invalid number in trial name @ line: " + lines.at(i);
                return false;
            }            
        }

        if (tokens.at(2) == "s") trial.isSame = true;
        else if (tokens.at(2) == "d") trial.isSame = false;
        else{
            error = "Trial type should be s or d in trial name @ line: " + lines.at(i);
            return false;
        }

        // Positions for the SHOW slide.
        if (!parseFlagPositions(lines.at(i+1),&trial,true)) return false;

        // Back colors for the SHOW slide
        if (!parseColors(lines.at(i+2),&trial,true,true)) return false;

        // Cross colors for the SHOW slide
        if (!parseColors(lines.at(i+3),&trial,false,true)) return false;

        // Positions for the TEST slide
        if (!parseFlagPositions(lines.at(i+4),&trial,false)) return false;

        // Back colors for the TEST slide
        if (!parseColors(lines.at(i+5),&trial,true,false)) return false;

        // Cross colors for the TEST slide
        if (!parseColors(lines.at(i+6),&trial,false,false)) return false;

        trials << trial;
        i = i + 7;

    }

    if (config->getBool(CONFIG_DEMO_MODE)) enableDemoMode();

    return true;
}

bool BindingManager::parseFlagPositions(const QString &line, BindingTrial *trial,bool show){
    QStringList tokens = line.split(' ',QString::SkipEmptyParts);
    if (tokens.size() != numberOfTargets){
        error = "Number of positions should be " + QString::number(numberOfTargets) + " @ line: " + line;
        return false;
    }

    for (qint32 i = 0; i < tokens.size(); i++){
        BindingFlag flag;
        bool ok;
        qint32 pcode = tokens.at(i).toUInt(&ok);
        if (!ok){
            error = "Invalid postion code @ line: " + line;
            return false;
        }
        flag.x = qFloor(pcode/10);
        flag.y = pcode - flag.x*10;

        if (flag.x >= drawStructure.xpos.size()){
            error = "The x index in the pcode " + tokens.at(i) + " should be lower than "
                    + QString::number(drawStructure.xpos.size()) + " @ line: " + line;
            return false;
        }
        if (flag.y >= drawStructure.ypos.size()){
            error = "The y index in the pcode " + tokens.at(i) + " should be lower than "
                    + QString::number(drawStructure.ypos.size()) + " @ line: " + line;
            return false;
        }
        flag.x = drawStructure.xpos.at(flag.x);
        flag.y = drawStructure.ypos.at(flag.y);
        if (show) trial->show.append(flag);
        else trial->test.append(flag);
    }

    return true;
}


bool BindingManager::parseColors(const QString &line, BindingTrial *trial, bool background, bool show){
    QStringList tokens = line.split(' ',QString::SkipEmptyParts);
    if (tokens.size() != numberOfTargets){
        error = "Number of colors should be " + QString::number(numberOfTargets) + " @ line: " + line;
        return false;
    }

    BindingSlide slide;
    if (show) slide = trial->show;
    else slide = trial->test;

    //qWarning() << "Num colors" << tokens.size() << "num of targets" << slide.size();

    for (qint32 i = 0; i < tokens.size(); i++){
        QString color_str = "#" + tokens.at(i);
        QColor color(color_str);
        if (color.isValid()){
            if (background) slide[i].back = color;
            else slide[i].cross = color;
        }
        else{
            error = "Invalid color " + tokens.at(i) + " @ line: " + line;
            return false;
        }
    }

    if (show) trial->show = slide;
    else trial->test = slide;
    return true;
}

bool BindingManager::legacyParser(const QString &contents){

    QStringList lines = contents.split("\n",QString::SkipEmptyParts);
    trials.clear();

    QMap<QString,qint32> dic;
    dic["u"] = 0;
    dic["m"] = 1;
    dic["l"] = 2;

    for (qint32 i = 0; i < lines.size(); i++){

        QString line = lines.at(i);

        if (line.isEmpty()) continue;

        QStringList tokens = line.split(" ",QString::SkipEmptyParts);

        if (tokens.size() != 13){
            error = "Line " + line + " does not contain 13 words";
            return false;
        }

        BindingTrial trial;
        trial.name = tokens.at(0);
        trial.number = -1;
        trial.isSame = trial.name.contains(STR_SAME);

        BindingFlag sl, sr, tl, tr;

        sl.back = QColor("#" + tokens.at(2));
        sl.cross = QColor("#" + tokens.at(3));
        sl.x = drawStructure.xpos.at(0);
        sl.y = drawStructure.ypos.at(dic.value(tokens.at(1)));

        sr.back = QColor("#" + tokens.at(5));
        sr.cross = QColor("#" + tokens.at(6));
        sr.x = drawStructure.xpos.at(1);
        sr.y = drawStructure.ypos.at(dic.value(tokens.at(4)));

        tl.back = QColor("#" + tokens.at(8));
        tl.cross = QColor("#" + tokens.at(9));
        tl.x = drawStructure.xpos.at(0);
        tl.y = drawStructure.ypos.at(dic.value(tokens.at(7)));

        tr.back = QColor("#" + tokens.at(11));
        tr.cross = QColor("#" + tokens.at(12));
        tr.x = drawStructure.xpos.at(1);
        tr.y = drawStructure.ypos.at(dic.value(tokens.at(10)));

        trial.show << sl << sr;
        trial.test << tl << tr;

        trials << trial;

    }

    return true;

}

