#include "bindingmanager.h"

BindingManager::BindingManager()
{
    canvas = nullptr;
}

void BindingManager::drawFlags(const BindingParser::BindingSlide &slide){

    canvas->clear();

    // Each target is drawn according to its values.

    for (qint32 i = 0; i < slide.size(); i++){
        QGraphicsRectItem *back  = canvas->addRect(0,0,parser.getDrawStructure().FlagSideH,parser.getDrawStructure().FlagSideV);
        QGraphicsRectItem *vrect = canvas->addRect(0,0,
                                                   parser.getDrawStructure().FlagSideH-2*parser.getDrawStructure().HLBorder,
                                                   parser.getDrawStructure().FlagSideV-2*parser.getDrawStructure().VSBorder);
        QGraphicsRectItem *hrect = canvas->addRect(0,0,
                                                   parser.getDrawStructure().FlagSideH-2*parser.getDrawStructure().HSBorder,
                                                   parser.getDrawStructure().FlagSideV-2*parser.getDrawStructure().VLBorder);
        back->setPos(slide.at(i).x,slide.at(i).y);
        // Calculating the other position based on the base flag position
        vrect->setPos(slide.at(i).x+parser.getDrawStructure().HLBorder,slide.at(i).y+parser.getDrawStructure().VSBorder);
        hrect->setPos(slide.at(i).x+parser.getDrawStructure().HSBorder,slide.at(i).y+parser.getDrawStructure().VLBorder);

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
    for (qint32 i = 0; i < parser.getTrials().size(); i++){
        if (parser.getTrials().at(i).name == trialName){
            id = i;
            break;
        }
    }
    if (id == -1) return false;
    if (show) drawFlags(parser.getTrials().at(id).show);
    else drawFlags(parser.getTrials().at(id).test);
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

void BindingManager::drawCenter(){
    canvas->clear();
    canvas->addLine(line0,QPen(QBrush(Qt::red),2));
    canvas->addLine(line1,QPen(QBrush(Qt::red),2));
}

void BindingManager::drawTrial(qint32 currentTrial, bool show){

    if (show){
        //qWarning() << "Drawing show for" << currentTrial;
        drawFlags(parser.getTrials().at(currentTrial).show);
    }
    else{
        //qWarning() << "Drawing test for" << currentTrial;
        drawFlags(parser.getTrials().at(currentTrial).test);
    }

}

void BindingManager::enableDemoMode(){    
    parser.demoModeList(NUMBER_OF_TRIALS_IN_DEMO_MODE);
}

bool BindingManager::parseExpConfiguration(const QString &contents){    
    bool ans = parser.parseBindingExperiment(contents,config,ScreenResolutionWidth,ScreenResolutionHeight,NUMBER_OF_TRIALS_IN_DEMO_MODE);
    versionString = parser.getVersionString();
    error = parser.getError();
    return ans;
}

