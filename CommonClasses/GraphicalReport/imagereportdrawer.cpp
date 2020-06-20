#include "imagereportdrawer.h"

ImageReportDrawer::ImageReportDrawer()
{
    canvas = new QGraphicsScene(0,0,PAGE_WIDTH,PAGE_HEIGHT);
    canvas->setBackgroundBrush(QBrush(Qt::white));

    qreal a = RESULTS_MARKER_TRIANG_DIM;
    downArrow  << QPointF(-a/2,0) << QPointF(a/2,0) << QPointF(0,a) << QPointF(-a/2,0);
    upArrow << QPointF(-a/2,0) << QPointF(a/2,0) << QPointF(0,-a) << QPointF(-a/2,0);

}


bool ImageReportDrawer::drawReport(const QVariantMap &ds, ConfigurationManager *c,const QString &bindingCode, const QString &bindingDescription){

    canvas->clear();
    QString language = c->getString(CONFIG_REPORT_LANGUAGE);
    langData = loadReportText(c->getString(CONFIG_REPORT_LANGUAGE));
    loadFonts();

    //----------------------------------- CHECKING WHICH DATA NEEDS TO BE ADDED ----------------------------------------------
    bool doReading = false;
    if (ds.contains(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION)){
        QString str = ds.value(CONFIG_RESULTS_READ_PREDICTED_DETERIORATION).toString();
        if ((str != "0") && (str != "nan")) doReading = true;
    }

    bool doNBack = false;
    if (ds.contains(CONFIG_RESULTS_NBACKRT_NUM_FIX_ENC)){
        QString str = ds.value(CONFIG_RESULTS_NBACKRT_NUM_FIX_ENC).toString();
        if ((str != "0") && (str != "nan")) doNBack = true;
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

    DiagonosisLogic diagClassFinder;
    diagClassFinder.reset();

    if (doReading){
        QStringList toLoad;
        toLoad << CONF_LOAD_RDINDEX << CONF_LOAD_EXECPROC << CONF_LOAD_WORKMEM << CONF_LOAD_RETMEM;
        for (qint32 i = 0; i < toLoad.size(); i++){
            ResultSegment rs;
            rs.loadSegment(toLoad.at(i),language);
            rs.setValue(ds.value(rs.getNameIndex()).toDouble());
            diagClassFinder.setResultSegment(rs);
            d.rs = rs;
            data2Show << d;
            //qDebug() << "Adding" << reportItems.last();
        }
    }

    if (doMemEnc){
        if (bindingCode == "I"){
            ResultSegment rs;
            if (bindingDescription == "2"){
                rs.loadSegment(CONF_LOAD_BINDIND2,language);
            }
            else if (bindingDescription == "3"){
                rs.loadSegment(CONF_LOAD_BINDIND3,language);
            }
            rs.setValue(ds.value(rs.getNameIndex()).toDouble());
            diagClassFinder.setResultSegment(rs);
            d.rs = rs;
            data2Show << d;
        }
        else if (bindingCode == "B"){
            ResultSegment rs;
            rs.loadSegment(CONF_LOAD_BEHAVE,language);
            QVariantList l = ds.value(rs.getNameIndex()).toList();

            //First value is BC ans and second is UC ans
            if (l.size() != 2) rs.setValue(-1);
            else rs.setValue(l.first().toDouble());

            diagClassFinder.setResultSegment(rs);
            d.rs = rs;
            data2Show << d;
        }
    }

    if (doNBack){
        QStringList toLoad;
        toLoad << CONF_LOAD_NBRT_FIX_ENC << CONF_LOAD_NBRT_FIX_RET << CONF_LOAD_NBRT_INHIB_PROC << CONF_LOAD_NBRT_SEQ_COMPLETE
                  << CONF_LOAD_NBRT_TARGET_HIT << CONF_LOAD_NBRT_MEAN_RESP_TIME << CONF_LOAD_NBRT_MEAN_SAC_AMP;
        for (qint32 i = 0; i < toLoad.size(); i++){
            ResultSegment rs;
            rs.loadSegment(toLoad.at(i),language);
            rs.setValue(ds.value(rs.getNameIndex()).toDouble());
            diagClassFinder.setResultSegment(rs);
            d.rs = rs;
            data2Show << d;
            //qDebug() << "Adding" << reportItems.last();
        }
    }

    //----------------------------------------- BACKGROUNDS ------------------------------------------------

    // Backgrounds
    canvas->addRect(0,0,PAGE_WIDTH,BANNER_HEIGHT,QPen(),QBrush(QColor(COLOR_BANNER)));

    // Adding the logo
    if (!c->getBool(CONFIG_REPORT_NO_LOGO)){
        QImage logo(":/CommonClasses/GraphicalReport/report_text/viewmind.png");
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


    QStringList tempStringList = langData.getString(DR_CONFG_EXPLANATION_PT2).split("<br>");

    QString html = "<span " + fontDescription + "font-family:" + expFont.family() + "\";>";
    html = html + tempStringList.last() + "</span>";

    exp->setHtml(html);

    yOffset1 = BAR_PATIENT_BAR_HEIGHT + BANNER_HEIGHT + PANEL_MARGIN_TOP;
    exp->setPos(PANEL_MARGIN_LEFT,yOffset1);

    //qreal colorTableXOffset = PANEL_MARGIN_LEFT + exp->boundingRect().width() + PANEL_HOR_BUFFER_SPACE;
    qreal colorTableXOffset = PANEL_MARGIN_LEFT;
    QStringList DescColors = langData.getStringList(DR_CONFG_COLOR_EXPLANATION);
    QStringList ColorScale;
    ColorScale << COLOR_BLUE << COLOR_GREEN << COLOR_YELLOW << COLOR_RED;

    QFont fontSquares = fonts.value(FONT_BOOK);
    fontSquares.setPointSize(FONT_SIZE_SQUARES);

    qreal xPos = colorTableXOffset;
    yOffset1 = yOffset1 + PANEL_MARGIN_TOP + exp->boundingRect().height();
    for (qint32 i = 0; i < ColorScale.size(); i++){

        QPen pen(QColor(ColorScale.at(i)));

        // Adding the square
        QGraphicsRectItem *rect = canvas->addRect(0,0,PANEL_SQUARE_SIDE,PANEL_SQUARE_SIDE,pen,QBrush(QColor(ColorScale.at(i))));
        rect->setPos(xPos ,yOffset1);

        // Adding the text.
        QGraphicsTextItem *desc = canvas->addText(DescColors.at(i),fontSquares);
        desc->setDefaultTextColor(COLOR_FONT_EXPLANATION);
        desc->setPos(xPos + rect->boundingRect().width() + PANEL_SQUARE_TO_TEXT_SPACE, yOffset1 + (rect->boundingRect().height() - desc->boundingRect().height())/2);
        xPos = xPos + rect->boundingRect().width() + PANEL_SQUARE_TO_TEXT_SPACE + desc->boundingRect().width() + PANEL_SQUARE_BUFFER_SPACE;

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
        d.gname = canvas->addText(d.rs.getTitle(),resultTitleFont);
        d.gname->setDefaultTextColor(COLOR_FONT_RESULTS);
        d.grange = canvas->addText(d.rs.getRangeText(),expFont);
        d.grange->setDefaultTextColor(COLOR_FONT_RESULTS);
        d.gvalue = canvas->addText(d.rs.getDisplayValue(),numberFont);
        d.gvalue->setDefaultTextColor(COLOR_FONT_RESULTS);
        d.gclarification = canvas->addText(d.rs.getExplanation(),clarificationFont);
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
        if (i == data2Show.size()-1){
            yOffset = lastLineY;
        }

    }

    //----------------------------------------- DIAGNOSIS BOX ------------------------------------------------
    // Drawing the actual box.
    qreal rectWidth = PAGE_WIDTH-BAR_PATIENT_BAR_MARGIN_LEFT-BAR_PATIENT_BAR_MARGIN_RIGHT;

    qreal topBoxMargin = yOffset+0.06*RESULTS_DIAG_BOX_HEIGHT;

    QGraphicsRectItem *diagnosisBox = canvas->addRect(0,0,rectWidth,RESULTS_DIAG_BOX_HEIGHT,
                                                      QPen(QBrush(COLOR_FONT_RESULTS),6),QBrush());
    diagnosisBox->setPos(BAR_PATIENT_BAR_MARGIN_LEFT,yOffset);

    qreal diagMarginK = 0.03;
    qreal diagXStart = BAR_PATIENT_BAR_MARGIN_LEFT + diagMarginK*rectWidth;
    qreal diagContentWidth = rectWidth*(1.0-2.0*diagMarginK);

    // Adding the diagnosis box title.
    QGraphicsTextItem *diagBoxTitle = canvas->addText(langData.getString(DR_CONFG_DIAGNOSIS_TITLE));
    QFont diagBoxTitleFont = fonts.value(FONT_BOLD);
    diagBoxTitleFont.setPointSize(FONT_SIZE_DIAG_TITLE);
    diagBoxTitle->setFont(diagBoxTitleFont);
    diagBoxTitle->setDefaultTextColor(COLOR_FONT_RESULTS);
    diagBoxTitle->setPos(diagXStart,topBoxMargin);

    // Adding the diagnosis text
    qreal diagTopMargin = topBoxMargin + diagBoxTitle->boundingRect().height() + 0.06*RESULTS_DIAG_BOX_HEIGHT;
    QString diagTextContent = langData.getString(DR_CONFG_DIAG_CLASS + diagClassFinder.getDiagnosisClass());
    //diagTextContent = langData.getString("class_2");  // FOR DEBUG
    diagTextContent = cleanNewLines(diagTextContent);
    QGraphicsTextItem *diagText = canvas->addText(diagTextContent);
    QFont diagTextFont = fonts.value(FONT_MEDIUM);
    diagTextFont.setPixelSize(FONT_SIZE_DIAG_TEXT);
    diagText->setFont(diagTextFont);
    diagText->setTextWidth(diagContentWidth);
    diagText->setDefaultTextColor(COLOR_FONT_RESULTS);
    diagText->setPos(diagXStart,diagTopMargin);

    // Adding the divider.
    qreal yposDiagDiv = diagTopMargin + diagText->boundingRect().height() + 0.04*RESULTS_DIAG_BOX_HEIGHT;
    QGraphicsLineItem *diagDivLine = canvas->addLine(diagXStart,yposDiagDiv,diagXStart+diagContentWidth,yposDiagDiv,QPen(QBrush(COLOR_FONT_RESULTS),6));

    // Adding the disclaimer
    qreal yDisclaimer = yposDiagDiv + diagDivLine->boundingRect().height() + 0.04*RESULTS_DIAG_BOX_HEIGHT;
    QString disclaimerContent = langData.getString(DR_CONFG_DISCLAIMER);
    disclaimerContent = cleanNewLines(disclaimerContent);
    QFont disclaimerFont = fonts.value(FONT_LIGHT);
    disclaimerFont.setPointSize(FONT_SIZE_DIAG_DISCLAIMER);
    disclaimerFont.setItalic(true);
    QGraphicsTextItem *disclaimerText = canvas->addText(disclaimerContent);
    disclaimerText->setFont(disclaimerFont);
    disclaimerText->setTextWidth(diagContentWidth);
    disclaimerText->setPos(diagXStart,yDisclaimer);


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
    qint32 resBarSize = d.rs.getNumberOfSegments();

    if (resBarSize == 3) colorScale << COLOR_GREEN << COLOR_YELLOW << COLOR_RED;
    else if (resBarSize == 4) colorScale << COLOR_BLUE << COLOR_GREEN << COLOR_YELLOW << COLOR_RED;
    else if (resBarSize == 2) colorScale << COLOR_GREEN << COLOR_RED;
    else qWarning() << "Res Bar Size is neither 4, 3 or 2 it is" << resBarSize;

    qreal R = RESULTS_SEGBAR_HEIGHT/2;
    QList<qreal> barSegments;
    qreal barSegWidth = (qreal)RESULTS_SEGBAR_WIDTH/(resBarSize+1);
    for (qint32 i = 0; i < resBarSize; i++) barSegments << barSegWidth;
    qreal indicator = d.rs.getBarSegmentIndex();

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
    QString base = ":/CommonClasses/GraphicalReport/report_text/";
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

    QString path = ":/CommonClasses/GraphicalReport/report_text/";

    if ((lang != CONFIG_P_LANG_ES) && (lang != CONFIG_P_LANG_EN)) lang = CONFIG_P_LANG_EN;
    path = path + lang;

    if (!langData.loadConfiguration(path,COMMON_TEXT_CODEC)){
        qWarning() << "Error loading lang" << langData.getError();
    }

    return langData;

}

QString ImageReportDrawer::cleanNewLines(QString text){
    text = text.replace("\n"," ");
    text = text.replace("\r"," ");
    text = text.replace("  "," ");
    return text;
}
