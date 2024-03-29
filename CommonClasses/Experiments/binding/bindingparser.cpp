#include "bindingparser.h"

/*************************************************************************************************
 * The intermediate maxtrix (CSV File) required by the EyeMatrixGenerator for the Binding
 * experiments requires information obtainted from parsing the experiment description.
 * Since the BindingManager contains Desktop and GUI related classes, and these are not supported
 * when compiling for console mode only (I.E. the server) the parsing and the drawing,
 * for the Binding experiment is separated
 * ***********************************************************************************************/


//// Dimensions for drawing the target flags. The dimensions are in mm so that they can be drawn the same size independently of the screen resolution.
//// The parser creates the structure that defines how the values will be drawn. That's why it is here.
///// TODO: Replace scaled values with % of screen size to make the CONFIG_XK and CONFIG_YK dissappear.
//const qreal BindingParser::LARGE_BINDING_TARGET_SIDE =              45.25;
//const qreal BindingParser::LARGE_BINDING_TARGET_HS =                1.0;
//const qreal BindingParser::LARGE_BINDING_TARGET_HL =                11.75;
//const qreal BindingParser::LARGE_BINDING_TARGET_VS =                6.00;
//const qreal BindingParser::LARGE_BINDING_TARGET_VL =                16.0;

//const qreal BindingParser::LARGE_BINDING_GRID_SPACING_X_2FLAGS =    128.0;
//const qreal BindingParser::LARGE_BINDING_GRID_SPACING_X_3FLAGS =    64.0;
//const qreal BindingParser::LARGE_BINDING_GRID_SPACING_Y =           48.0;

//const qreal BindingParser::SMALL_BINDING_TARGET_SIDE =              10.0;
//const qreal BindingParser::SMALL_BINDING_TARGET_HS =                0.25;
//const qreal BindingParser::SMALL_BINDING_TARGET_HL =                2.6;
//const qreal BindingParser::SMALL_BINDING_TARGET_VS =                1.33;
//const qreal BindingParser::SMALL_BINDING_TARGET_VL =                3.54;

//const qreal BindingParser::SMALL_BINDING_GRID_SPACING_X_2FLAGS =    20.0;
//const qreal BindingParser::SMALL_BINDING_GRID_SPACING_X_3FLAGS =    20.0;
//const qreal BindingParser::SMALL_BINDING_GRID_SPACING_Y =           20.0;

//// XK and YK, originally represented  the px to mm ratio, horizontally and vertically respectively in a monitor
//// They are now left as scaling and adjustment constants to make the squares have the right size in each
//// eyetracking helmet to use.

//#ifdef EYETRACKER_HTCVIVEPRO
//const qreal BindingParser::CONFIG_XK =                              0.25;
//const qreal BindingParser::CONFIG_YK =                              0.25;
//#endif

//#ifdef EYETRACKER_GAZEPOINT
//const qreal BindingParser::CONFIG_XK =                              0.25;
//const qreal BindingParser::CONFIG_YK =                              0.25;
//#endif


BindingParser::BindingParser()
{
}


BindingParser::BindingTrial BindingParser::getTrialByName(const QString &id) const{
    BindingTrial trial;
    trial.name = "";
    for (qint32 i = 0; i < trials.size(); i++){
        if (trials.at(i).name == id){
            return trials.at(i);
        }
    }
    return trial;
}

bool BindingParser::parseBindingExperiment(const QString &contents, qreal ScreenResolutionWidth, qreal ScreenResolutionHeight){

    // Splitting into lines.
    QStringList lines = contents.split('\n',Qt::KeepEmptyParts);

    // Checking for old interpretation marking
    /// TODO: Remove this when it is safe.
    QStringList tokens;
    tokens = lines.first().split(' ',Qt::SkipEmptyParts);

    int horizontalGridPoints = 2; // Initialization done just o avoid a warning

    // Getting the number of targets in the trial
    numberOfTargets = tokens.first().toInt();

    // Last value, if it exists should be the version string
    if (tokens.size() > 1){
        versionString = tokens.last();
    }

    // Even tough its the same value, there is no real relation between the grid size and the number of targets selected.
    // For clarity a different variable is used.
    horizontalGridPoints = numberOfTargets;

    if ((numberOfTargets != 2) && (numberOfTargets != 3)){
        error = "Invalid number of targets in the first line " + lines.first();
        return false;
    }

    // Defining the values to draw the target.
    drawStructure.xpos.clear();
    drawStructure.ypos.clear();

    // Loading the draw structure
    DrawValues drawValues = loadDrawStructure(ScreenResolutionWidth,ScreenResolutionHeight);

    // The mm values are transformed to pixels and the math is only once to define the possible target positions.
    drawStructure.FlagSideH = drawValues.sideh*ScreenResolutionWidth;
    drawStructure.FlagSideV = drawValues.sidev*ScreenResolutionHeight;
    drawStructure.HSBorder  = drawValues.hs*ScreenResolutionWidth;
    drawStructure.VLBorder  = drawValues.vl*ScreenResolutionHeight;
    drawStructure.HLBorder  = drawValues.hl*ScreenResolutionWidth;
    drawStructure.VSBorder  = drawValues.vs*ScreenResolutionHeight;

    //qDebug() << "Flag Side H" << drawStructure.FlagSideH << "V" << drawStructure.FlagSideV << "@" << ScreenResolutionWidth << "x" << ScreenResolutionHeight;

    qreal Gxpx = drawValues.gx*ScreenResolutionWidth;
    qreal Gypx = drawValues.gy*ScreenResolutionHeight;

    //        qDebug() << "Draw Values for" << numberOfTargets << ". Small: " << useSmall;
    //        qDebug() << "Resolution" << ScreenResolutionWidth << ScreenResolutionHeight;
    //        qDebug() << "FSH" << drawStructure.FlagSideH/ScreenResolutionWidth;
    //        qDebug() << "FSV" << drawStructure.FlagSideV/ScreenResolutionHeight;
    //        qDebug() << "HSB" << drawStructure.HSBorder/ScreenResolutionWidth;
    //        qDebug() << "VLB" << drawStructure.VLBorder/ScreenResolutionHeight;
    //        qDebug() << "HLB" << drawStructure.HLBorder/ScreenResolutionWidth;
    //        qDebug() << "VSB" << drawStructure.VSBorder/ScreenResolutionHeight;
    //        qDebug() << "GX"  << (Gxpx)/ScreenResolutionWidth;
    //        qDebug() << "GY"  << (Gypx)/ScreenResolutionHeight;

    qreal Sx = drawValues.sideh*ScreenResolutionWidth;
    qreal Sy = drawValues.sidev*ScreenResolutionHeight;

    // Total horizontal space required by the placement grid
    qreal Wx = Gxpx*(horizontalGridPoints-1) + Sx;
    // Total vertical space required by the placement grid
    qreal Wy = Gypx*2 + Sy;

    qreal Xog = (ScreenResolutionWidth - Wx)/2;
    qreal Yog = (ScreenResolutionHeight - Wy)/2;

    // Stored so it can be called upon by other code.
    gridBoundingRect = QRectF(Xog,Yog,Wx,Wy);

    for (qint32 i = 0; i < horizontalGridPoints; i++){
        drawStructure.xpos << Xog + (i*Gxpx);
    }

    for (qint32 j = 0; j < 3; j++){
        drawStructure.ypos << Yog + (j*Gypx);
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

        tokens = lines.at(i).split(' ',Qt::SkipEmptyParts);

        // Name [x|number] and [s|d]
        if (tokens.size() != 3){
            error = "Name line should have 3 and only 3 items @ line: " + lines.at(i);
            return false;
        }
        BindingTrial trial;
        trial.name = tokens.at(0);

        QStringList expectedID;
        expectedID << trial.name << "0";
        expectedIDs << expectedID;
        expectedID.clear();
        expectedID << trial.name << "1";
        expectedIDs << expectedID;

        // The token position at 1 was used to describe what was drawn before the trial. It's no longer used.
        // However the code was not changed because the description files were not changed.

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

        // qWarning() << "ADDING :" << trial.toString();

        i = i + 7;

    }

    return true;
}


bool BindingParser::parseFlagPositions(const QString &line, BindingTrial *trial,bool show){
    QStringList tokens = line.split(' ',Qt::SkipEmptyParts);
    if (tokens.size() != numberOfTargets){
        error = "Number of positions should be " + QString::number(numberOfTargets) + " @ line: " + line;
        return false;
    }

    for (qint32 i = 0; i < tokens.size(); i++){
        BindingFlag flag;
        bool ok;
        qint32 pcode = tokens.at(i).toInt(&ok);
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
        //        qWarning() << "For " << i << "Position" << flag.x << "from" << drawStructure.xpos << "is" << drawStructure.xpos.at(flag.x)
        //                   << "and" << "Position" << flag.y << "from" << drawStructure.ypos << "is" << drawStructure.ypos.at(flag.y);
        flag.x = static_cast<qint32>(drawStructure.xpos.at(flag.x));
        flag.y = static_cast<qint32>(drawStructure.ypos.at(flag.y));

        if (show) trial->show.append(flag);
        else trial->test.append(flag);
    }

    return true;
}


bool BindingParser::parseColors(const QString &line, BindingTrial *trial, bool background, bool show){
    QStringList tokens = line.split(' ',Qt::SkipEmptyParts);
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

/**
 * @brief BindingParser::loadDrawStructure
 * @details All the constants returned by this function where computed by using the old values to draw the squares in a 1920 x 1080 monitor
 * about 22 inches. These provided a ratio to the value to the width and height of the resoltuion. The values were left as is.
 * Using these values the drawing can be parametrized ONLY by the resolution.
 * @param targetW: The width of the resolution where the flags will be drawn
 * @param targetH: The height of the resolution where the flags will be drawn
 * @return The drawing contstants.
 */

BindingParser::DrawValues BindingParser::loadDrawStructure(qreal targetW, qreal targetH){

    DrawValues drawValues;

    // For computing the actual draw constants, based on resolution.
    DrawingConstantsCalculator dcc;
    dcc.setTargetResolution(targetW,targetH);

    drawValues.sideh = dcc.getHorizontalRatio(0.0942708);
    drawValues.sidev = dcc.getVerticalRatio(0.167593);
    drawValues.hl    = dcc.getHorizontalRatio(0.0244792);
    drawValues.hs    = dcc.getHorizontalRatio(0.00208333);
    drawValues.vl    = dcc.getVerticalRatio(0.0592593);
    drawValues.vs    = dcc.getVerticalRatio(0.0222222);

    if (numberOfTargets == 2){
        drawValues.gx = dcc.getHorizontalRatio(0.266667);
        drawValues.gy = dcc.getVerticalRatio(0.177778);
    }
    else{
        // Assuming 3 targets
        drawValues.gx = dcc.getHorizontalRatio(0.133333);
        drawValues.gy = dcc.getVerticalRatio(0.177778);
    }

    return drawValues;
}

