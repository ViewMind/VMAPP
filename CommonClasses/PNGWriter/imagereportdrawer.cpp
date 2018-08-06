#include "imagereportdrawer.h"

ImageReportDrawer::ImageReportDrawer()
{
    canvas = new QGraphicsScene(0,0,PAGE_WIDTH,PAGE_HEIGHT);
    canvas->setBackgroundBrush(QBrush(Qt::white));

    qreal a = RESULTS_MARKER_TRIANG_DIM;
    downArrow  << QPointF(-a/2,0) << QPointF(a/2,0) << QPointF(0,a) << QPointF(-a/2,0);
    upArrow << QPointF(-a/2,0) << QPointF(a/2,0) << QPointF(0,-a) << QPointF(-a/2,0);

}


void ImageReportDrawer::drawReport(ConfigurationManager *dataSet, ConfigurationManager *c){

    canvas->clear();

    if (!loadLanguageConfiguration(c->getString(CONFIG_REPORT_LANGUAGE))) return;
    loadFonts();

    //----------------------------------- GENERATING SHOW STRUCTURES ----------------------------------------------
    ShowData data2Show;
    ShowDatum d;

    /// TODO simplify the code in order to take advantage of the math done in flowControl
    /// TODO eliminate the dual fonts.

    if (dataSet->containsKeyword(CONFIG_RESULTS_ATTENTIONAL_PROCESSES)){
        d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(0);
        d.clarification = "";
        d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(0);
        qreal totalFixations = dataSet->getReal(CONFIG_RESULTS_ATTENTIONAL_PROCESSES);
        d.value = QString::number(totalFixations,'f',0);
        d.stopValues.clear();
        d.stopValues << 450 << 550 << 650 << 750;
        d.largerBetter = false;
        d.calcValue = -1;
        data2Show << d;

        d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(1);
        d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(1);
        d.clarification = langData.getStringList(DR_CONFG_RES_CLARIFICATION).at(0);
        d.value = QString::number(dataSet->getReal(CONFIG_RESULTS_EXECUTIVE_PROCESSES),'f',0);
        d.stopValues.clear();
        d.stopValues << -4 << 18 << 40 << 62;
        d.largerBetter = false;
        d.calcValue = -1;
        data2Show << d;

        d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(2);
        d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(2);
        d.clarification = langData.getStringList(DR_CONFG_RES_CLARIFICATION).at(1);
        d.value = QString::number(dataSet->getReal(CONFIG_RESULTS_WORKING_MEMORY),'f',0);
        d.stopValues.clear();
        d.stopValues << 50 << 60 << 70 << 80;
        d.largerBetter = true;
        d.calcValue = -1;
        data2Show << d;

        d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(3);
        d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(3);
        d.clarification = langData.getStringList(DR_CONFG_RES_CLARIFICATION).at(2);
        d.value = QString::number(dataSet->getReal(CONFIG_RESULTS_RETRIEVAL_MEMORY),'f',0);
        d.stopValues.clear();
        d.stopValues << 7 << 15 << 23 << 31;
        d.largerBetter = true;
        d.calcValue = -1;
        data2Show << d;
    }

    if (dataSet->containsKeyword(CONFIG_RESULTS_MEMORY_ENCODING)){
        d.name  = langData.getStringList(DR_CONFG_RESULTS_NAME).at(4);
        d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(4);
        d.clarification = langData.getStringList(DR_CONFG_RES_CLARIFICATION).at(3);
        d.value = QString::number(dataSet->getReal(CONFIG_RESULTS_MEMORY_ENCODING),'f',3);
        d.stopValues.clear();
        d.stopValues << -0.95 << 0.05 << 1.05;
        d.largerBetter = true;
        d.calcValue = -1;
        data2Show << d;

        // First one should be BC and the second one is UC. Need to compute the faux value in order to draw the data.
        QStringList results = dataSet->getStringList(CONFIG_RESULTS_BEHAVIOURAL_RESPONSE);

        d.name = langData.getStringList(DR_CONFG_RESULTS_NAME).at(5);
        d.range = langData.getStringList(DR_CONFG_RESULT_RANGES).at(5);
        d.clarification = langData.getStringList(DR_CONFG_RES_CLARIFICATION).at(4);
        d.value = results.first();
        d.stopValues.clear();
        d.stopValues << -1 << 0 << 1;
        d.largerBetter = true;
        d.calcValue = true;

        qint32 BC = results.first().toUInt();
        //qint32 UC = results.last().toUInt();

        if (BC > 10){
            // This is the yellow section.
            d.calcValue = -0.5;
        }
        else{
            d.calcValue = 0.5;
        }

        data2Show << d;

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
    motto->setDefaultTextColor(QColor(COLOR_FONT_WHITE));
    qreal xmotto = PAGE_WIDTH-BANNER_MARGIN_MOTTO_RIGHT-motto->boundingRect().width();
    motto->setPos(xmotto,BANNER_MARGIN_TOP_LOGO);

    // Adding the line beside the motto.
    qreal xmottobar = xmotto - PANEL_SQUARE_TO_TEXT_SPACE;
    qreal bar_overshoot = motto->boundingRect().height()*0.05;

    QPen mottoLinePen(Qt::white);
    mottoLinePen.setWidth(BANNER_MOTTO_LINE_WIDTH);
    canvas->addLine(xmottobar,motto->pos().y()-bar_overshoot,
                    xmottobar,motto->pos().y()+bar_overshoot+motto->boundingRect().height(),
                    mottoLinePen);



    QGraphicsRectItem *bar1 = canvas->addRect(0,0,PAGE_WIDTH,BAR_PATIENT_BAR_HEIGHT,QPen(),QBrush(QColor(COLOR_BAR_GRAY)));
    bar1->setPos(0,BANNER_HEIGHT);

    QGraphicsRectItem *panel = canvas->addRect(0,0,PAGE_WIDTH,PANEL_HEIGHT,QPen(),QBrush(QColor(COLOR_LIGHT_GRAY)));
    panel->setPos(0,BANNER_HEIGHT+BAR_PATIENT_BAR_HEIGHT);

    QGraphicsRectItem *bar2 = canvas->addRect(0,0,PAGE_WIDTH,BAR_RESULTS_HEIGHT,QPen(),QBrush(QColor(COLOR_BAR_GRAY)));
    qint32 resultTitleBarYStart = BANNER_HEIGHT+BAR_PATIENT_BAR_HEIGHT+PANEL_HEIGHT;
    bar2->setPos(0,resultTitleBarYStart);

    QFont resultTitleFont = fonts.value(FONT_BOLD);
    resultTitleFont.setPointSize(FONT_SIZE_RESULT_TITLE);
    QGraphicsTextItem *r = canvas->addText(langData.getString(DR_CONFG_RESULT_TITLE),resultTitleFont);
    r->setDefaultTextColor(COLOR_FONT_WHITE);
    r->setPos ((PAGE_WIDTH - r->boundingRect().width())/2,
               (BAR_RESULTS_HEIGHT  - r->boundingRect().height())/2 +resultTitleBarYStart);


    //----------------------------------------- UPPERBAR TEXT ------------------------------------------------

    // Upperbar text, adding all data first.
    QFont upperBarField = fonts.value(FONT_BOLD);
    upperBarField.setPointSize(FONT_SIZE_UPPERBAR);
    QFont upperBarInfo  = fonts.value(FONT_BOOK);
    upperBarInfo.setPointSize(FONT_SIZE_UPPERBAR);

    QGraphicsTextItem *patient      = canvas->addText(langData.getString(DR_CONFG_PATIENT_NAME) + ": ",upperBarField);
    patient->setDefaultTextColor(QColor(COLOR_FONT_WHITE));
    QGraphicsTextItem *doctor       = canvas->addText(langData.getString(DR_CONFG_DOCTOR_NAME) + ": ",upperBarField);
    doctor->setDefaultTextColor(QColor(COLOR_FONT_WHITE));
    QGraphicsTextItem *age          = canvas->addText(langData.getString(DR_CONFG_PATIENT_AGE) + ": ",upperBarField);
    age->setDefaultTextColor(QColor(COLOR_FONT_WHITE));
    QGraphicsTextItem *date         = canvas->addText(langData.getString(DR_CONFG_DATE) + ": ",upperBarField);
    date->setDefaultTextColor(QColor(COLOR_FONT_WHITE));

    QGraphicsTextItem *patient_data = canvas->addText(dataSet->getString(CONFIG_PATIENT_NAME),upperBarInfo);
    patient_data->setDefaultTextColor(QColor(COLOR_FONT_WHITE));
    QGraphicsTextItem *doctor_data  = canvas->addText(dataSet->getString(CONFIG_DOCTOR_NAME),upperBarInfo);
    doctor_data->setDefaultTextColor(QColor(COLOR_FONT_WHITE));
    QGraphicsTextItem *age_data     = canvas->addText(dataSet->getString(CONFIG_PATIENT_AGE),upperBarInfo);
    age_data->setDefaultTextColor(QColor(COLOR_FONT_WHITE));
    QGraphicsTextItem *date_data    = canvas->addText(QDate::currentDate().toString(langData.getString(DR_CONFG_DATE_FORMAT)),upperBarInfo);
    date_data->setDefaultTextColor(QColor(COLOR_FONT_WHITE));

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

    QString generatedReport = c->getString(CONFIG_PATIENT_REPORT_DIR) + "/" + QString(FILE_REPORT_NAME) + QDateTime::currentDateTime().toString("_yyyy_MM_dd")  + ".png";
    // Saving the path to the configuration
    c->addKeyValuePair(CONFIG_IMAGE_REPORT_PATH,generatedReport);
    image.save(generatedReport);

}

void ImageReportDrawer::drawSegmentBarLengthsAndIndicators(const ShowDatum &d, qreal yBarLocation, qreal xStart){

    // Calculating the segment bars.
    QStringList colorScale;

    qreal vreal;
    if (d.calcValue == -1)
        vreal = d.value.toDouble();
    else
        vreal = d.calcValue;

    if (d.stopValues.size() == 4) colorScale << COLOR_GREEN << COLOR_YELLOW << COLOR_RED;
    else if (d.stopValues.size() == 3) colorScale << COLOR_GREEN << COLOR_YELLOW;

    qreal R = RESULTS_SEGBAR_HEIGHT/2;
    qreal fullRange = d.stopValues.last() - d.stopValues.first();

    QList<qreal> barSegments;
    qreal segment_sum = 0;
    for (qint32 i = 1; i < d.stopValues.size()-1; i++){
        qreal segment = (d.stopValues.at(i) - d.stopValues.at(i-1))/fullRange;
        barSegments << segment*RESULTS_SEGBAR_WIDTH;
        segment_sum = segment_sum + segment;
    }
    barSegments << (1-segment_sum)*RESULTS_SEGBAR_WIDTH;

    // Calculating the indicator, which should be in the middle of the corresponding segment.
    qreal indicatorOffset;

    if (d.stopValues.size() == 4){
        if (d.largerBetter){
            if (vreal < d.stopValues.at(1)){
                indicatorOffset = barSegments.at(0) + barSegments.at(1) + barSegments.at(2)*0.5;
            }
            else if (vreal < d.stopValues.at(2)){
                indicatorOffset = barSegments.at(0) + barSegments.at(1)*0.5;
            }
            else{
                indicatorOffset = barSegments.at(0)*0.5;
            }
        }
        else{
            if (vreal < d.stopValues.at(1)){
                indicatorOffset = barSegments.at(0)*0.5;
            }
            else if (vreal < d.stopValues.at(2)){
                indicatorOffset = barSegments.at(0) + barSegments.at(1)*0.5;
            }
            else{
                indicatorOffset = barSegments.at(0) + barSegments.at(1) + barSegments.at(2)*0.5;
            }
        }
    }
    else{
        // Two segment bar.
        if (d.largerBetter){
            if (vreal > d.stopValues.at(1)) indicatorOffset = barSegments.at(0)*0.5;
            else indicatorOffset = barSegments.at(0) + barSegments.at(1)*0.5;
        }
        else{
            if (vreal < d.stopValues.at(1)) indicatorOffset = barSegments.at(0)*0.5;
            else indicatorOffset = barSegments.at(0) + barSegments.at(1)*0.5;
        }
    }


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

bool ImageReportDrawer::loadLanguageConfiguration(const QString &lang){

    QString path = ":/CommonClasses/PNGWriter/report_text/";
    if ((lang != "Spanish") && (lang != "English")) return false;
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
    bool ans = langData.loadConfiguration(path,COMMON_TEXT_CODEC);

    if (!ans){
        qWarning() << "Error loading lang" << langData.getError();
    }

    return ans;

}
