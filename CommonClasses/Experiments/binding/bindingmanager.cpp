#include "bindingmanager.h"

const char * BindingManager::CONFIG_IS_BC = "is_bc";
const char * BindingManager::CONFIG_N_TARGETS = "number_of_targets";

BindingManager::BindingManager()
{    
    numberOfExplanationScreens = 4;
    // explanationListTextKey =
}



void BindingManager::drawFlags(const BindingParser::BindingSlide &primary, const BindingParser::BindingSlide &secondary, const RenderFlagType &rtf){

    clearCanvas();

    if (rtf == RFT_NORMAL){
        // Just draw the primary.
        renderSingleFlag(primary,SRL_NORMAL);
    }
    else if (rtf == RFT_ENCODING_SIDE){
        // We render the primary only on the left side.
        renderSingleFlag(primary,SRL_LEFT);
    }
    else {
        // We render the primary on the left and the secondary on the right.
        renderSingleFlag(primary,SRL_LEFT,true);
        renderSingleFlag(secondary,SRL_RIGHT);
    }

}

void BindingManager::renderSingleFlag(const BindingParser::BindingSlide &slide, ScreenRenderLocation srl = ScreenRenderLocation::SRL_NORMAL, bool renderArrow){

    QList<RenderServerItem*> items;

//    // FOR DEBUGGING.
//    RenderServerRectItem *debugRect = canvas.addRect(gridBoundingRect);
//    debugRect->setBrush(QBrush(Qt::white));

//    qDebug() << "Grid bounding rect cx" << gridBoundingRect.x() + gridBoundingRect.width()/2
//             << "cy" << gridBoundingRect.y() + gridBoundingRect.height()/2
//             << gridBoundingRect;

    // The grid bounding rect is added for reference.
    RenderServerRectItem * brect = canvas.addRect(gridBoundingRect);
    brect->setBrush(QBrush(QColor(canvas.getBackgroundColorName())));
    if (srl == SRL_NORMAL) brect->setPen(QPen(Qt::gray)); // This will render it invisible as the color is the same as the background.   
    else brect->setPen(QPen(Qt::black));
    items << brect;

    for (qint32 i = 0; i < slide.size(); i++){

        RenderServerRectItem *back  = canvas.addRect(0,0,parser.getDrawStructure().FlagSideH,parser.getDrawStructure().FlagSideV);
        RenderServerRectItem *vrect = canvas.addRect(0,0,
                                                   parser.getDrawStructure().FlagSideH-2*parser.getDrawStructure().HLBorder,
                                                   parser.getDrawStructure().FlagSideV-2*parser.getDrawStructure().VSBorder);
        RenderServerRectItem *hrect = canvas.addRect(0,0,
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

        items << back << vrect << hrect;

    }

    RenderServerItemGroup *group = canvas.createItemGroup(items);

    if (srl != ScreenRenderLocation::SRL_NORMAL){

        // The first point is to set the transform origin point to the center of the flag.
        qreal x_center = ScreenResolutionWidth/2;
        qreal y_center = ScreenResolutionHeight/2;

        //qDebug() << "SCREEN CENTER" << x_center << y_center;

        // The scale point is in the center of the screen.
        group->setTransformOriginPoint(x_center,y_center);
        group->setScale(DUAL_RENDERING_SCALE_FACTOR);


        // Moving to the left or to the right accordingly leaving the proper margins.
        qreal dx;
        if (srl == ScreenRenderLocation::SRL_LEFT){
            dx = -x_center + gridBoundingRect.width()*DUAL_RENDERING_SCALE_FACTOR/2 + ScreenResolutionWidth*DUAL_RENDERING_MARGIN_PERCENT_OF_W;
        }
        else {
            dx = x_center - gridBoundingRect.width()*DUAL_RENDERING_SCALE_FACTOR/2 - ScreenResolutionWidth*DUAL_RENDERING_MARGIN_PERCENT_OF_W;
        }

        group->moveBy(dx,0);

        // Drawing arrow indication if requested.
        if (renderArrow){

            // Computing the space between both blocks.
            qreal slide_width          = gridBoundingRect.width()*DUAL_RENDERING_SCALE_FACTOR;
            qreal left_margin_x        = ScreenResolutionWidth*DUAL_RENDERING_MARGIN_PERCENT_OF_W;
            qreal space_between_slides = ScreenResolutionWidth - 2*left_margin_x- 2*slide_width;
            qreal arrow_width          = space_between_slides*(1-2*DUAL_RENDERING_AIR_ARROW);
            qreal arrow_height         = DUAL_RENDERING_ARROW_HTOW_RATIO*arrow_width;

            QPolygonF poly_arrow_head;
            poly_arrow_head.append(QPointF(x_center - arrow_width/2,y_center-arrow_height/2));

            poly_arrow_head.append(QPointF(x_center + arrow_width/2,y_center));

            poly_arrow_head.append(QPointF(x_center - arrow_width/2,y_center+arrow_height/2));

            // qDebug() << "ARROW POINTS" << poly_arrow_head;

            // Computing all the arrow paramters.
            QColor arrowColor = Qt::cyan;

            // Add the triangle polygon to the scene
            //QGraphicsPolygonItem* arrow_head = canvas.addPolygon(poly_arrow_head);
            RenderServerTriangleItem* arrow_head = canvas.addTriangle(poly_arrow_head);
            arrow_head->setBrush(QBrush(arrowColor));
            arrow_head->setPen(QPen(arrowColor));

        }

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

    clearCanvas(); // We need to do this heare as render single flag no longer calls it.
    if (show) renderSingleFlag(parser.getTrials().at(id).show);
    else renderSingleFlag(parser.getTrials().at(id).test);

    return true;

}

void BindingManager::init(qreal display_resolution_width, qreal display_resolution_height){
    ExperimentDataPainter::init(display_resolution_width,display_resolution_height);

    canvas.setBackgroundBrush(QBrush(Qt::gray));

    // Old code. These are obviously the same values as the parameters.
    qreal WScreen, HScreen;
    WScreen = canvas.width();
    HScreen = canvas.height();

    // Calculating the points for the cross
    qreal delta = 0.05*WScreen;
    line0.setP1(QPointF(WScreen/2 - delta,HScreen/2 - delta));
    line0.setP2(QPointF(WScreen/2 + delta,HScreen/2 + delta));
    line1.setP1(QPointF(WScreen/2 + delta,HScreen/2 - delta));
    line1.setP2(QPointF(WScreen/2 - delta,HScreen/2 + delta));

}

qint32 BindingManager::size() const {
    if (shortModeEnabled){
        return NUMBER_OF_TRIALS_IN_SHORT_MODE;
        //return 2;
    }
    else {
        return static_cast<qint32>(parser.getTrialList().size());
    }
}

void BindingManager::configure(const QVariantMap &configuration){
    numberOfTargets = configuration.value(CONFIG_N_TARGETS).toInt();
    if (configuration.value(CONFIG_IS_BC).toBool()){
        explanationListTextKey = STUDY_TEXT_KEY_BINDING_BC;
        isBC = true;
    }
    else {
        explanationListTextKey = STUDY_TEXT_KEY_BINDING_UC;
        isBC = false;
    }
}

void BindingManager::drawCenter(){
    clearCanvas();
    canvas.addLine(line0,QPen(QBrush(Qt::red),4));
    canvas.addLine(line1,QPen(QBrush(Qt::red),4));
}

void BindingManager::drawTrial(qint32 currentTrial, bool show, bool enableRenderDualMode){

    BindingParser::BindingSlide primary;
    BindingParser::BindingSlide secondary;

    RenderFlagType rft = RFT_NORMAL;

    //qDebug() << "BindingManager::drawTrial. Number of Trials: " << parser.getTrials().size() << ". Accessing trial " << currentTrial;

    if (enableRenderDualMode){

        if (show) rft = RFT_ENCODING_SIDE;
        else rft = RFT_SIDE_BY_SIDE;

        primary = parser.getTrials().at(currentTrial).show;

        if (!show){
            secondary = parser.getTrials().at(currentTrial).test;
        }

    }
    else {
        if (show){
            primary = parser.getTrials().at(currentTrial).show;
        }
        else {
            primary = parser.getTrials().at(currentTrial).test;
        }
    }

    drawFlags(primary,secondary,rft);

}


bool BindingManager::parseExpConfiguration(const QString &contents){    
    bool ans = parser.parseBindingExperiment(contents,ScreenResolutionWidth,ScreenResolutionHeight);
    versionString = parser.getVersionString();
    expectedIDs = parser.getExpectedIDs();
    error = parser.getError();
    gridBoundingRect = parser.getGridBoundingRect();
    return ans;
}

void BindingManager::renderStudyExplanationScreen(qint32 screen_index){

    //qDebug() << "Rendering Study Explantion Binding: isBC" << isBC << "Screen Index" << screen_index << "Number of Targets" << numberOfTargets;

    if (isBC){
        if (screen_index == SE_BC_ENCODING_DIFF){
            if (numberOfTargets == 2){
                drawTrial(SE_BC_2_DIFF_TRIAL,true,true);
            }
            else {
                drawTrial(SE_BC_3_DIFF_TRIAL,true,true);
            }
        }
        else if (screen_index == SE_BC_DECODING_DIFF){
            if (numberOfTargets == 2){
                drawTrial(SE_BC_2_DIFF_TRIAL,false,true);
            }
            else {
                drawTrial(SE_BC_3_DIFF_TRIAL,false,true);
            }
        }
        else if (screen_index == SE_BC_ENCODING_SAME){
            if (numberOfTargets == 2){
                drawTrial(SE_BC_2_SAME_TRIAL,true,true);
            }
            else {
                drawTrial(SE_BC_3_SAME_TRIAL,true,true);
            }

        }
        else if (screen_index == SE_BC_DECODING_SAME){
            if (numberOfTargets == 2){
                drawTrial(SE_BC_2_SAME_TRIAL,false,true);
            }
            else {
                drawTrial(SE_BC_3_SAME_TRIAL,false,true);
            }
        }
    }
    else {
        if (screen_index == SE_UC_ENCODING_DIFF){
            if (numberOfTargets == 2){
                drawTrial(SE_UC_2_DIFF_TRIAL,true,true);
            }
            else {
                drawTrial(SE_UC_3_DIFF_TRIAL,true,true);
            }
        }
        else if (screen_index == SE_UC_DECODING_DIFF){
            if (numberOfTargets == 2){
                drawTrial(SE_UC_2_DIFF_TRIAL,false,true);
            }
            else {
                drawTrial(SE_UC_3_DIFF_TRIAL,false,true);
            }
        }
        else if (screen_index == SE_UC_ENCODING_SAME){
            if (numberOfTargets == 2){
                drawTrial(SE_UC_2_SAME_TRIAL,true,true);
            }
            else {
                drawTrial(SE_UC_3_SAME_TRIAL,true,true);
            }

        }
        else if (screen_index == SE_UC_DECODING_SAME){
            if (numberOfTargets == 2){
                drawTrial(SE_UC_2_SAME_TRIAL,false,true);
            }
            else {
                drawTrial(SE_UC_3_SAME_TRIAL,false,true);
            }
        }
    }

}


