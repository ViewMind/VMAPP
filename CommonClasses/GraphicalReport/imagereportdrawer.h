#ifndef IMAGEREPORTDRAWER_H
#define IMAGEREPORTDRAWER_H

#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/common.h"
#include "resultsegment.h"
#include "diagonosislogic.h"

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QDebug>
#include <QFontDatabase>
#include <QFont>
#include <QtMath>
#include <QImage>
#include <QPainter>
#include <QDate>
#include <QThread>
#include <QTextBlock>
#include <QTextBlockFormat>
#include <QTextCursor>

// Resource location
#define  RESOURCE_LOCATION            ":/../../EyeExperimenter/src/report_text/"

// Language configuration file keywords
#define  DR_CONFG_PATIENT_NAME        "patient_name"
#define  DR_CONFG_DOCTOR_NAME         "doctor_name"
#define  DR_CONFG_PATIENT_AGE         "patient_age"
#define  DR_CONFG_DATE                "date"
#define  DR_CONFG_DATE_FORMAT         "date_format"
#define  DR_CONFG_EXPLANATION_PT1     "explanation_pt1"
#define  DR_CONFG_EXPLANATION_PT2     "explanation_pt2"
#define  DR_CONFG_EXPLANATION_BOLD    "explanation_bold"
#define  DR_CONFG_COLOR_EXPLANATION   "color_explanation"
#define  DR_CONFG_MOTTO               "motto"
#define  DR_CONFG_DISCLAIMER          "disclaimer"
#define  DR_CONFG_DIAG_CLASS          "class_"
#define  DR_CONFG_DIAGNOSIS_TITLE     "diag_class_title"
#define  DR_CONFG_RESULT_TITLE        "result_title"
#define  DR_CONFG_STUDY_READING       "reading_study"
#define  DR_CONFG_STUDY_NBACKRT       "nbackrt_study"
#define  DR_CONFG_STUDY_COLORS        "colors_study"
#define  DR_CONFG_STUDY_GONOGO        "gonogo_study"

// Drawing Scaling Control
#define  MASTER_SCALE                  1//10

// Dimensions
#define  PAGE_WIDTH                    595*MASTER_SCALE
#define  PAGE_HEIGHT                   842*MASTER_SCALE
#define  BANNER_HEIGHT                 98*MASTER_SCALE
#define  BANNER_LOGO_HEIGHT            31*MASTER_SCALE
#define  BANNER_LOGO_WIDTH             146*MASTER_SCALE
#define  BANNER_MARGIN_TOP_LOGO        44*MASTER_SCALE
#define  BANNER_MARGIN_BOTTOM_LOGO     33*MASTER_SCALE
#define  BANNER_MARGIN_LEFT_LOGO       38*MASTER_SCALE
#define  BANNER_MARGIN_MOTTO_RIGHT     38*MASTER_SCALE
#define  BANNER_MOTTO_LINE_WIDTH       2*MASTER_SCALE
#define  BAR_PATIENT_BAR_HEIGHT        48*MASTER_SCALE
#define  BAR_PATIENT_BAR_MARGIN_LEFT   38*MASTER_SCALE
#define  BAR_PATIENT_BAR_MARGIN_RIGHT  38*MASTER_SCALE

#define  SEPARATOR_LINE_WIDTH          1*MASTER_SCALE

#define  PANEL_HEIGHT                  66*MASTER_SCALE
#define  PANEL_EXP_TEXT_WIDTH          280*MASTER_SCALE
#define  PANEL_SQUARES_EXP_WIDTH       183*MASTER_SCALE
#define  PANEL_MARGIN_LEFT             38*MASTER_SCALE
#define  PANEL_MARGIN_RIGHT            38*MASTER_SCALE
#define  PANEL_HOR_BUFFER_SPACE        49*MASTER_SCALE
#define  PANEL_SQUARE_BUFFER_SPACE     15*MASTER_SCALE

#define  PANEL_MARGIN_TOP              10*MASTER_SCALE
#define  PANEL_MARGIN_BOTTOM           45*MASTER_SCALE
#define  PANEL_SQUARE_SIDE             14*MASTER_SCALE
#define  PANEL_SQUARE_TO_TEXT_SPACE    6*MASTER_SCALE
#define  BAR_RESULTS_HEIGHT            31*MASTER_SCALE
#define  RESULTS_MARGIN_TOP            15*MASTER_SCALE
#define  RESULTS_RESULT_NAME_VBUFFER   5*MASTER_SCALE
#define  RESULTS_MARGIN_BOTTOM         48*MASTER_SCALE
#define  RESULTS_MARGIN_LEFT           38*MASTER_SCALE
#define  RESULTS_MARGIN_RIGHT          38*MASTER_SCALE

#define  RESULTS_SEP_LINE_SPACE        5*MASTER_SCALE
#define  RESULTS_NAME_COL_WIDTH        257*MASTER_SCALE
#define  RESULTS_NUMBER_COL_WIDTH      83*MASTER_SCALE
#define  RESULTS_HOR_BUFFER_SPACE      15*MASTER_SCALE
#define  RESULTS_SEGBAR_HEIGHT         9*MASTER_SCALE
#define  RESULTS_SEGBAR_WIDTH          146*MASTER_SCALE
#define  RESULTS_MARKER_TRIANG_DIM     7*MASTER_SCALE
#define  RESULTS_DIAG_BOX_HEIGHT       120*MASTER_SCALE


// Color definitions
//#define  COLOR_BANNER                  "#628dae"
//#define  COLOR_BAR_GRAY                "#505150"
#define  COLOR_LIGHT_GRAY              "#f1f0f0"
#define  COLOR_BANNER                  "#ffffff"
#define  COLOR_BAR_GRAY                "#ffffff"
//#define  COLOR_LIGHT_GRAY              "#ffffff"

#define  COLOR_GREEN                   "#049948"
#define  COLOR_YELLOW                  "#e5b42c"
#define  COLOR_RED                     "#cb2026"
#define  COLOR_BLUE                    "#00557f"

#define  COLOR_LINE_SEP                "#4f504f"

#define  COLOR_FONT_WHITE              "#ffffff"
#define  COLOR_FONT_DARK_GRAY          "#505150"
#define  COLOR_FONT_EXPLANATION        "#646565"
#define  COLOR_FONT_RESULTS            "#505150"
#define  COLOR_FONT_BANNER             "#628dae"

// Font Definitions
#define  FONT_MEDIUM                   "GothamMedium.otf"
#define  FONT_BOLD                     "GothamBold.otf"
#define  FONT_BOOK                     "GothamBook.otf"
#define  FONT_LIGHT                    "GothamLight.otf"
#define  FONT_BLACK                    "GothamBlackRegular.otf"

// Font Sizes
#define  FONT_SCALE                    0.75*MASTER_SCALE
#define  FONT_SIZE_BANNER              9*FONT_SCALE
#define  FONT_SIZE_UPPERBAR            9*FONT_SCALE
#define  FONT_SIZE_EXPTEXT             10*FONT_SCALE
#define  FONT_SIZE_SQUARES             8*FONT_SCALE
#define  FONT_SIZE_RESULT_TITLE        15*FONT_SCALE
#define  FONT_SIZE_RESULT_NAME         14*FONT_SCALE
#define  FONT_SIZE_RESULT_VALUE        24*FONT_SCALE
#define  FONT_SIZE_RESULT_RANGE        10*FONT_SCALE
#define  FONT_SIZE_RESULT_EXP          9*FONT_SCALE
#define  FONT_SIZE_DIAG_TEXT           11*FONT_SCALE
#define  FONT_SIZE_DIAG_DISCLAIMER     9*FONT_SCALE
#define  FONT_SIZE_DIAG_TITLE          12*FONT_SCALE


class ImageReportDrawer
{

public:
    ImageReportDrawer();
    bool drawReport(const QVariantMap &ds, ConfigurationManager *c, qint32 studyPage, const QString &bindingCode = "I", const QString &bindingDescription = "2");

    static ConfigurationManager loadReportText(QString lang);
    static QString cleanNewLines(QString text);

private:

    QGraphicsScene *canvas;
    ConfigurationManager langData;
    QHash<QString,QFont> fonts;

    struct ShowDatum{
        ResultSegment rs;
        QGraphicsTextItem *gname;
        QGraphicsTextItem *grange;
        QGraphicsTextItem *gvalue;
        QGraphicsTextItem *gclarification;
    };

    typedef QList<ShowDatum> ShowData;

    // Polygons for the arrow indicators
    QPolygonF upArrow;
    QPolygonF downArrow;

    // Loading of auxiliary data files.

    void loadFonts();

    // The return value are as follows 1 per segment and the last one is the indicator value.
    void drawSegmentBarLengthsAndIndicators(const ShowDatum &d, qreal yBarLocation, qreal xStart);

    QString getValueToPrint(const QString &v, qint32 decPoints);

};

#endif // IMAGEREPORTDRAWER_H
