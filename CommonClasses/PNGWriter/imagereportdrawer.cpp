#include "imagereportdrawer.h"

ImageReportDrawer::ImageReportDrawer()
{
    canvas = new QGraphicsScene(0,0,PAGE_WIDTH,PAGE_HEIGHT);
    canvas->setBackgroundBrush(QBrush(Qt::white));

    qreal a = RESULTS_MARKER_TRIANG_DIM;
    downArrow  << QPointF(-a/2,0) << QPointF(a/2,0) << QPointF(0,a) << QPointF(-a/2,0);
    upArrow << QPointF(-a/2,0) << QPointF(a/2,0) << QPointF(0,-a) << QPointF(-a/2,0);

}


bool ImageReportDrawer::drawReport(const QVariantMap &ds, ConfigurationManager *c,const QString &bindingCode){

    canvas->clear();
    langData = loadReportText(c->getString(CONFIG_REPORT_LANGUAGE));
    loadFonts();

    //----------------------------------- CHECKING WHICH DATA NEEDS TO BE ADDED ----------------------------------------------
    bool doReading = false;
    if (ds.contains(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION)){
        QString str = ds.value(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION).toString();
        if ((str != "0") && (str != "nan")) doReading = true;
    }

    bool doMemEnc = false;
    if (bindingCode == "I"){
        if (ds.contains(CONFIG_RESULTS_BINDING_CONVERSION_INDEX)){
            QString str = ds.value(CONFIG_RESULTS_BINDING_CONVERSION_INDEX).toString();
            if ((str != "0") && (str != "nan")) doMemEnc = true;
        }
    }
    else if (bindingCode == "B"){
        if (ds.contains(CONFIG_RESULTS_BEHAVIOURAL_RESPONSE)){
            QString str = ds.value(CONFIG_RESULTS_BEHAVIOURAL_RESPONSE).toString();
            if ((str != "0") && (str != "nan")) doMemEnc = true;
        }
    }

    //----------------------------------- GENERATING SHOW STRUCTURES ----------------------------------------------
    ShowData data2Show;
    ShowDatum d;

    /// TODO eliminate the dual fonts.

    if (doReading){
        d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(0);
        d.clarification = "";
        d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(0);
        d.resultBar.setResultType(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION);
        d.resultBar.setValue(ds.value(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION));
        d.value = d.resultBar.getValue();
        data2Show << d;

        d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(1);
        d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(1);
        d.clarification = langData.getStringList(DR_CONFG_RES_CLARIFICATION).at(0);
        d.resultBar.setResultType(CONFIG_RESULTS_EXECUTIVE_PROCESSES);
        d.resultBar.setValue(ds.value(CONFIG_RESULTS_EXECUTIVE_PROCESSES));
        d.value = d.resultBar.getValue();
        data2Show << d;

        d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(2);
        d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(2);
        d.clarification = langData.getStringList(DR_CONFG_RES_CLARIFICATION).at(1);
        d.resultBar.setResultType(CONFIG_RESULTS_WORKING_MEMORY);
        d.resultBar.setValue(ds.value(CONFIG_RESULTS_WORKING_MEMORY));
        d.value = d.resultBar.getValue();
        data2Show << d;

        d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(3);
        d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(3);
        d.clarification = langData.getStringList(DR_CONFG_RES_CLARIFICATION).at(2);
        d.resultBar.setResultType(CONFIG_RESULTS_RETRIEVAL_MEMORY);
        d.resultBar.setValue(ds.value(CONFIG_RESULTS_RETRIEVAL_MEMORY));
        d.value = d.resultBar.getValue();
        data2Show << d;
    }

    if (doMemEnc){
        if (bindingCode == "I"){
            d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(4);
            d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(4);
            d.clarification = langData.getStringList(DR_CONFG_RES_CLARIFICATION).at(3);
            d.resultBar.setResultType(CONFIG_RESULTS_BINDING_CONVERSION_INDEX);
            d.resultBar.setValue(ds.value(CONFIG_RESULTS_BINDING_CONVERSION_INDEX));
            d.value = d.resultBar.getValue();
            data2Show << d;
        }
        else if (bindingCode == "B"){
            d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(5);
            d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(5);
            d.clarification = langData.getStringList(DR_CONFG_RES_CLARIFICATION).at(4);
            d.resultBar.setResultType(CONFIG_RESULTS_BINDING_CONVERSION_INDEX);
            d.resultBar.setValue(ds.value(CONFIG_RESULTS_BINDING_CONVERSION_INDEX));
            d.value = d.resultBar.getValue();
            data2Show << d;
        }
    }

    //----------------------------------------- BACKGROUNDS ------------------------------------------------

    // Backgrounds
    canvas->addRect(0,0,PAGE_WIDTH,BANNER_HEIGHT,QPen(),QBrush(QColor(COLOR_BANNER)));

    // Adding the logo
    if (!c->getBool(CONFIG_REPORT_NO_LOGO)){
        QImage logo(":/CommonClasses/PNGWriter/report_text/viewmind.png");
        logo = logo.scaled(QSize(BANNER_LOGO_WIDTH,BANNER_LOGO_HEIGHT),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        QGraphicsPixmapItem* logoItem = new QGraphicsPixmapItem(QPixmap::fromImage(logo));
        canvas->addItem(logoItem);
        logoItem->setPos(BANNER_MARGIN_LEFT_LOGO,BANNER_MARGIN_TOP_LOGO);
    }

    // Adding the Motto.
    QFont mottoFont = fonts.value(FONT_MEDIUM);
    mottoFont.setPointSize(FONT_SIZE_BANNER);
    QGraphicsTextItem *motto = canvas->addText(langData.getString(DR_CONFG_MOTTO),mottoFont);
    motto->setDefaultTextColor(QColor(COLOR_FONT_BANNER));
    qreal xmotto = PAGE_WIDTH-BANNER_MARGIN_MOTTO_RIGHT-motto->boundingRect().width();
    motto->setPos(xmotto,BANNER_MARGIN_TOP_LOGO);

    // Adding the line beside the motto.
    qreal xmottobar = xmotto - PANEL_SQUARE_TO_TEXT_SPACE;
    qreal bar_overshoot = motto->boundingRect().height()*0.05;

    QPen mottoLinePen(QColor(COLOR_FONT_BANNER));
    mottoLinePen.setWidth(BANNER_MOTTO_LINE_WIDTH);
    canvas->addLine(xmottobar,motto->pos().y()-bar_overshoot,
                    xmottobar,motto->pos().y()+bar_overshoot+motto->boundingRect().height(),
                    mottoLinePen);


//  Rect is now white, so there is no sense in drawing it.
//    QGraphicsRectItem *bar1 = canvas->addRect(0,0,PAGE_WIDTH,BAR_PATIENT_BAR_HEIGHT,QPen(),QBrush(QColor(COLOR_BAR_GRAY)));
//    bar1->setPos(0,BANNER_HEIGHT);

    QGraphicsLineItem *lineSep1 = canvas->addLine(0,0,PAGE_WIDTH-BAR_PATIENT_BAR_MARGIN_LEFT-BAR_PATIENT_BAR_MARGIN_RIGHT,0,QPen(QBrush(COLOR_LINE_SEP),10));
    lineSep1->setPos(BAR_PATIENT_BAR_MARGIN_LEFT,BANNER_HEIGHT);

    QGraphicsLineItem *lineSep2 = canvas->addLine(0,0,PAGE_WIDTH-BAR_PATIENT_BAR_MARGIN_LEFT-BAR_PATIENT_BAR_MARGIN_RIGHT,0,QPen(QBrush(COLOR_LINE_SEP),10));
    lineSep2->setPos(BAR_PATIENT_BAR_MARGIN_LEFT,BANNER_HEIGHT+BAR_PATIENT_BAR_HEIGHT);

//    QGraphicsRectItem *panel = canvas->addRect(0,0,PAGE_WIDTH,PANEL_HEIGHT,QPen(),QBrush(QColor(COLOR_LIGHT_GRAY)));
//    panel->setPos(0,BANNER_HEIGHT+BAR_PATIENT_BAR_HEIGHT);

    //QGraphicsRectItem *bar2 = canvas->addRect(0,0,PAGE_WIDTH,BAR_RESULTS_HEIGHT,QPen(),QBrush(QColor(COLOR_BAR_GRAY)));
    qint32 resultTitleBarYStart = BANNER_HEIGHT+BAR_PATIENT_BAR_HEIGHT+PANEL_HEIGHT;

    QGraphicsLineItem *lineSep3 = canvas->addLine(0,0,PAGE_WIDTH-BAR_PATIENT_BAR_MARGIN_LEFT-BAR_PATIENT_BAR_MARGIN_RIGHT,0,QPen(QBrush(COLOR_LINE_SEP),10));
    lineSep3->setPos(BAR_PATIENT_BAR_MARGIN_LEFT,resultTitleBarYStart);

    QGraphicsLineItem *lineSep4 = canvas->addLine(0,0,PAGE_WIDTH-BAR_PATIENT_BAR_MARGIN_LEFT-BAR_PATIENT_BAR_MARGIN_RIGHT,0,QPen(QBrush(COLOR_LINE_SEP),10));
    lineSep4->setPos(BAR_PATIENT_BAR_MARGIN_LEFT,resultTitleBarYStart+BAR_RESULTS_HEIGHT);

    QFont resultTitleFont = fonts.value(FONT_BOLD);
    resultTitleFont.setPointSize(FONT_SIZE_RESULT_TITLE);
    QGraphicsTextItem *r = canvas->addText(langData.getString(DR_CONFG_RESULT_TITLE),resultTitleFont);
    r->setDefaultTextColor(COLOR_FONT_DARK_GRAY);
    r->setPos ((PAGE_WIDTH - r->boundingRect().width())/2,
               (BAR_RESULTS_HEIGHT  - r->boundingRect().height())/2 +resultTitleBarYStart);


    //----------------------------------------- UPPERBAR TEXT ------------------------------------------------

    // Upperbar text, adding all data first.
    QFont upperBarField = fonts.value(FONT_BOLD);
    upperBarField.setPointSize(FONT_SIZE_UPPERBAR);
    QFont upperBarInfo  = fonts.value(FONT_BOOK);
    upperBarInfo.setPointSize(FONT_SIZE_UPPERBAR);

    QGraphicsTextItem *patient      = canvas->addText(langData.getString(DR_CONFG_PATIENT_NAME) + ": ",upperBarField);
    patient->setDefaultTextColor(QColor(COLOR_FONT_DARK_GRAY));
    QGraphicsTextItem *doctor       = canvas->addText(langData.getString(DR_CONFG_DOCTOR_NAME) + ": ",upperBarField);
    doctor->setDefaultTextColor(QColor(COLOR_FONT_DARK_GRAY));
    QGraphicsTextItem *age          = canvas->addText(langData.getString(DR_CONFG_PATIENT_AGE) + ": ",upperBarField);
    age->setDefaultTextColor(QColor(COLOR_FONT_DARK_GRAY));
    QGraphicsTextItem *date         = canvas->addText(langData.getString(DR_CONFG_DATE) + ": ",upperBarField);
    date->setDefaultTextColor(QColor(COLOR_FONT_DARK_GRAY));

    QGraphicsTextItem *patient_data = canvas->addText(ds.value(CONFIG_PATIENT_NAME).toString(),upperBarInfo);
    patient_data->setDefaultTextColor(QColor(COLOR_FONT_DARK_GRAY));

    // Doctor name, might contain UID. It is necessary to remove it.
    QString drname = ds.value(CONFIG_DOCTOR_NAME).toString();
    if (drname.contains('(')){
        QStringList parts = drname.split('(');
        drname = parts.first();
        drname = drname.trimmed();
    }

    QGraphicsTextItem *doctor_data  = canvas->addText(drname,upperBarInfo);
    doctor_data->setDefaultTextColor(QColor(COLOR_FONT_DARK_GRAY));
    QGraphicsTextItem *age_data     = canvas->addText(ds.value(CONFIG_PATIENT_AGE).toString(),upperBarInfo);
    age_data->setDefaultTextColor(QColor(COLOR_FONT_DARK_GRAY));
    QGraphicsTextItem *date_data    = canvas->addText(ds.value(CONFIG_REPORT_DATE).toString(),upperBarInfo);
    date_data->setDefaultTextColor(QColor(COLOR_FONT_DARK_GRAY));

    qreal yOffset1 = BANNER_HEIGHT + (BAR_PATIENT_BAR_HEIGHT -  patient->boundingRect().height()*2)/2;
    qreal yOffset2 = yOffset1 + patient->boundingRect().height();

    patient->setPos(BAR_PATIENT_BAR_MARGIN_LEFT,yOffset1);
    patient_data->setPos(BAR_PATIENT_BAR_MARGIN_LEFT + patient->boundingRect().width(),yOffset1);

    doctor->setPos(BAR_PATIENT_BAR_MARGIN_LEFT,yOffset2);
    doctor_data->setPos(BAR_PATIENT_BAR_MARGIN_LEFT + doctor->boundingRect().width(),yOffset2);

    qreal xOffset = PAGE_WIDTH - BAR_PATIENT_BAR_MARGIN_RIGHT - qMax(age->boundingRect().width() + age_data->boundingRect().width(),
                                                                     date->boundingRect().width() + date_data->boundingRect().width());

    age->setPos(xOffset,yOffset1);
    age_data->setPos(xOffset + age->boundingRect().width(),yOffset1);
    date->setPos(xOffset,yOffset2);
    date_data->setPos(xOffset + date->boundingRect().width(),yOffset2);

    //----------------------------------------- PANEL CONTENT ------------------------------------------------

    QFont expFont = fonts.value(FONT_BOOK);
    expFont.setPointSize(FONT_SIZE_EXPTEXT);
    QGraphicsTextItem *exp = canvas->addText("",expFont);

    QString fontDescription = "style = \" color: " + QString(COLOR_FONT_EXPLANATION)
            + "; font-size: " + QString::number((qint32)FONT_SIZE_EXPTEXT) + "pt;";

    QString html = "<span " + fontDescription + "font-family:" + expFont.family() + "\";>";
    html = html + langData.getString(DR_CONFG_EXPLANATION_PT1) + "</span>";
    html = html + "<span " + fontDescription + "font-family:" + fonts.value(FONT_BLACK).family() + "\";> "
            + langData.getString(DR_CONFG_EXPLANATION_BOLD) + " </span>";
    html = html + "<span " + fontDescription + "font-family:" + expFont.family() + "\";>";
    html = html + langData.getString(DR_CONFG_EXPLANATION_PT2) + "</span>";

    exp->setHtml(html);
    exp->setTextWidth(PANEL_EXP_TEXT_WIDTH);

    yOffset1 = BAR_PATIENT_BAR_HEIGHT + BANNER_HEIGHT + PANEL_MARGIN_TOP;
    exp->setPos(PANEL_MARGIN_LEFT,yOffset1);

    qreal colorTableXOffset = PANEL_MARGIN_LEFT + exp->boundingRect().width() + PANEL_HOR_BUFFER_SPACE;
    QStringList DescColors = langData.getStringList(DR_CONFG_COLOR_EXPLANATION);
    QStringList ColorScale;
    ColorScale << COLOR_GREEN << COLOR_YELLOW << COLOR_RED;

    QFont fontSquares = fonts.value(FONT_BOOK);
    fontSquares.setPointSize(FONT_SIZE_SQUARES);
    yOffset1 = BAR_PATIENT_BAR_HEIGHT + BANNER_HEIGHT + PANEL_MARGIN_TOP;

    for (qint32 i = 0; i < ColorScale.size(); i++){

        QPen pen(QColor(ColorScale.at(i)));

        // Adding the square
        QGraphicsRectItem *rect = canvas->addRect(0,0,PANEL_SQUARE_SIDE,PANEL_SQUARE_SIDE,pen,QBrush(QColor(ColorScale.at(i))));
        rect->setPos(colorTableXOffset,yOffset1);

        // Adding the text.
        QGraphicsTextItem *desc = canvas->addText(DescColors.at(i),fontSquares);
        desc->setDefaultTextColor(COLOR_FONT_EXPLANATION);
        qreal y = yOffset1 + (rect->boundingRect().height() - desc->boundingRect().height())/2;
        desc->setPos(colorTableXOffset+rect->boundingRect().width() + PANEL_SQUARE_TO_TEXT_SPACE,y);

        yOffset1 = yOffset1 + rect->boundingRect().height() + PANEL_SQUARE_BUFFER_SPACE;

    }

    //----------------------------------------- RESULT LIST ------------------------------------------------

    QFont numberFont = fonts.value(FONT_LIGHT);
    numberFont.setPointSize(FONT_SIZE_RESULT_VALUE);

    QFont resultNameFont = fonts.value(FONT_MEDIUM);
    resultNameFont.setPointSize(FONT_SIZE_RESULT_NAME);

    QFont clarificationFont = fonts.value(FONT_LIGHT);
    clarificationFont.setPointSize(FONT_SIZE_RESULT_EXP);

    QFont paramRange     = fonts.value(FONT_BOOK);
    paramRange.setPointSize(FONT_SIZE_RESULT_RANGE);

    qreal dataXOffset = RESULTS_MARGIN_LEFT;
    qreal maxNameHeight = 0;
    qreal spaceToLine = RESULTS_SEP_LINE_SPACE;
    qreal maxParamHeight = 0;
    qreal maxClarificationHeight = 0;

    for (qint32 i = 0; i < data2Show.size(); i++){

        ShowDatum d = data2Show.at(i);
        d.gname = canvas->addText(d.name,resultTitleFont);
        d.gname->setDefaultTextColor(COLOR_FONT_RESULTS);
        d.grange = canvas->addText(d.range,expFont);
        d.grange->setDefaultTextColor(COLOR_FONT_RESULTS);
        d.gvalue = canvas->addText(d.value,numberFont);
        d.gvalue->setDefaultTextColor(COLOR_FONT_RESULTS);
        d.gclarification = canvas->addText(d.clarification,clarificationFont);
        d.gclarification->setDefaultTextColor(COLOR_FONT_RESULTS);

        data2Show[i] = d;
        maxClarificationHeight = qMax(maxClarificationHeight,d.gclarification->boundingRect().height());
        maxNameHeight = qMax(maxNameHeight,d.gname->boundingRect().height());
        maxParamHeight = qMax(maxParamHeight,d.grange->boundingRect().height());

    }

    qreal yOffset = BANNER_HEIGHT + BAR_PATIENT_BAR_HEIGHT + BAR_RESULTS_HEIGHT + PANEL_HEIGHT + RESULTS_MARGIN_TOP;

    qreal barStart = RESULTS_MARGIN_LEFT + RESULTS_NAME_COL_WIDTH + RESULTS_HOR_BUFFER_SPACE;

    // Radious
    qreal R = RESULTS_SEGBAR_HEIGHT/2;

    // This last adjustment ensures that the first bar is properly centered vertically
    qreal lastLineY = yOffset - RESULTS_MARGIN_TOP + RESULTS_SEP_LINE_SPACE;

    for (qint32 i = 0; i < data2Show.size(); i++){

        ShowDatum d = data2Show.at(i);
        d.gname->setPos(dataXOffset,yOffset);
        d.gclarification->setPos(dataXOffset,yOffset + maxNameHeight + RESULTS_RESULT_NAME_VBUFFER);
        d.grange->setPos(dataXOffset,yOffset + maxClarificationHeight + maxNameHeight + 2*RESULTS_RESULT_NAME_VBUFFER);

        // Y for the separation line.
        qreal sepLineY = maxNameHeight + yOffset + maxParamHeight + maxClarificationHeight + spaceToLine + 3*RESULTS_RESULT_NAME_VBUFFER;

        // Y for the number
        qreal y = ((sepLineY - lastLineY) - d.gvalue->boundingRect().height())/2 + lastLineY;
        d.gvalue->setPos(PAGE_WIDTH - dataXOffset - d.gvalue->boundingRect().width(),y);

        // Y value for the bars.
        qreal bary = d.gvalue->boundingRect().height()/2 + d.gvalue->pos().y() - R;

        drawSegmentBarLengthsAndIndicators(d,bary,barStart);

        // Drawing the separator line
        canvas->addLine(dataXOffset, sepLineY, d.gvalue->pos().x() + d.gvalue->boundingRect().width(),sepLineY);

        // Calculating values for the next iteration
        yOffset = sepLineY + spaceToLine;
        lastLineY = sepLineY;

    }


    //----------------------------------------- SAVING PIC ------------------------------------------------

    // Doing a quick save.
    qreal scale = 1;
    QImage image(PAGE_WIDTH*scale,PAGE_HEIGHT*scale,QImage::Format_ARGB32);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    canvas->render(&painter);
    return image.save(c->getString(CONFIG_IMAGE_REPORT_PATH));

}

void ImageReportDrawer::drawSegmentBarLengthsAndIndicators(const ShowDatum &d, qreal yBarLocation, qreal xStart){

    // Calculating the segment bars.
    QStringList colorScale;
    qint32 resBarSize = d.resultBar.getValues().size();

    if (resBarSize == 4) colorScale << COLOR_GREEN << COLOR_YELLOW << COLOR_RED;
    else if (resBarSize == 3) colorScale << COLOR_GREEN << COLOR_RED;
    else qWarning() << "Res Bar Size is neither 3 or 4 it is" << resBarSize;

    qreal R = RESULTS_SEGBAR_HEIGHT/2;
    QList<qreal> barSegments;
    qreal barSegWidth = (qreal)RESULTS_SEGBAR_WIDTH/resBarSize;
    for (qint32 i = 0; i < resBarSize-1; i++) barSegments << barSegWidth;
    qreal indicator = d.resultBar.getSegmentBarIndex();

    // Calculating the indicator, which should be in the middle of the corresponding segment.
    qreal indicatorOffset = (2*indicator + 1)*barSegWidth/2;

    // The ones in the borders need to be shorte to account for the semicircle at the end.
    barSegments[0] = barSegments.first() - R;
    barSegments[barSegments.size()-1] = barSegments.last() - R;

    // Actually drawing everything, starting with the bars
    qreal x = xStart+R;
    QColor first, last;
    for (qint32 i = 0; i < barSegments.size(); i++){
        QColor color(colorScale.at(i));
        QPen pen(color);
        QBrush brush(color);

        if (i == 0) first = color;
        else if (i == barSegments.size()-1) last = color;

        QGraphicsRectItem *rect = canvas->addRect(0,0,barSegments.at(i),RESULTS_SEGBAR_HEIGHT,pen,brush);
        rect->setPos(x,yBarLocation);
        x = x + barSegments.at(i);
    }

    // Now the bar endings.
    QGraphicsEllipseItem *end1 = canvas->addEllipse(0,0,2*R,2*R,QPen(first),QBrush(first));
    end1->setPos(xStart,yBarLocation);
    QGraphicsEllipseItem *end2 = canvas->addEllipse(0,0,2*R,2*R,QPen(last),QBrush(last));
    end2->setPos(x-R,yBarLocation);

    // Finally the indicator
    qreal xindicator = xStart + indicatorOffset;
    QGraphicsPolygonItem * up = canvas->addPolygon(upArrow,QPen(),QBrush(Qt::black));
    up->setPos(xindicator,yBarLocation+RESULTS_SEGBAR_HEIGHT+up->boundingRect().height());
    QGraphicsPolygonItem * down = canvas->addPolygon(downArrow,QPen(),QBrush(Qt::black));
    down->setPos(xindicator,yBarLocation-down->boundingRect().height());

}

void ImageReportDrawer::loadFonts(){

    QStringList fontFileList;
    QString base = ":/CommonClasses/PNGWriter/report_text/";
    fontFileList << FONT_BOLD
                 << FONT_BOOK
                 << FONT_MEDIUM
                 << FONT_LIGHT
                 << FONT_BLACK;

    for (qint32 i = 0; i < fontFileList.size(); i++){
        int id = QFontDatabase::addApplicationFont(base+fontFileList.at(i));
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        QFont font(family);
        font.setPointSize(9);
        fonts[fontFileList.at(i)] = font;
    }

}

ConfigurationManager ImageReportDrawer::loadReportText(QString lang){

    ConfigurationManager langData;

    QString path = ":/CommonClasses/PNGWriter/report_text/";

    if ((lang != CONFIG_P_LANG_ES) && (lang != CONFIG_P_LANG_EN)) lang = CONFIG_P_LANG_EN;
    path = path + lang;

    ConfigurationManager::CommandVerifications cv;
    ConfigurationManager::Command cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_LIST;
    cmd.fields << ConfigurationManager::VT_STRING;
    cv[DR_CONFG_COLOR_EXPLANATION] = cmd;

    cmd.clear();
    cv[DR_CONFG_DATE] = cmd;

    cmd.clear();
    cv[DR_CONFG_DOCTOR_NAME] = cmd;

    cmd.clear();
    cv[DR_CONFG_EXPLANATION_PT1] = cmd;

    cmd.clear();
    cv[DR_CONFG_EXPLANATION_PT2] = cmd;

    cmd.clear();
    cv[DR_CONFG_MOTTO] = cmd;

    cmd.clear();
    cv[DR_CONFG_PATIENT_AGE] = cmd;

    cmd.clear();
    cv[DR_CONFG_PATIENT_NAME] = cmd;

    cmd.clear();
    cv[DR_CONFG_RESULT_TITLE] = cmd;

    cmd.clear();
    cv[DR_CONFG_DATE_FORMAT] = cmd;

    cmd.clear();
    cv[DR_CONFG_EXPLANATION_BOLD] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_LIST;
    cmd.fields << ConfigurationManager::VT_STRING ;
    cv[DR_CONFG_RESULTS_NAME] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_LIST;
    cmd.fields << ConfigurationManager::VT_STRING;
    cv[DR_CONFG_RESULTS_NAME] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_LIST;
    cmd.fields << ConfigurationManager::VT_STRING;
    cv[DR_CONFG_RESULT_RANGES] = cmd;

    cmd.clear();
    cmd.type = ConfigurationManager::VT_LIST;
    cmd.fields << ConfigurationManager::VT_STRING;
    cv[DR_CONFG_RES_CLARIFICATION] = cmd;


    if (!langData.setupVerification(cv)){
        qWarning() << "Setup Verification Error" << langData.getError();
    }
    if (!langData.loadConfiguration(path,COMMON_TEXT_CODEC)){
        qWarning() << "Error loading lang" << langData.getError();
    }

    return langData;

}
