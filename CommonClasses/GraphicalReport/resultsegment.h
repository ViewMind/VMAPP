#ifndef RESULTSEGMENT_H
#define RESULTSEGMENT_H

#include <QHash>

#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

#define  CONF_LOAD_RDINDEX     ":/CommonClasses/GraphicalReport/report_text/00_index_of_cognitive_impairment"
#define  CONF_LOAD_EXECPROC    ":/CommonClasses/GraphicalReport/report_text/01_executive_processes"
#define  CONF_LOAD_WORKMEM     ":/CommonClasses/GraphicalReport/report_text/02_working_memory"
#define  CONF_LOAD_RETMEM      ":/CommonClasses/GraphicalReport/report_text/03_retrieval_memory"
#define  CONF_LOAD_BINDIND2    ":/CommonClasses/GraphicalReport/report_text/04_binding_index_2"
#define  CONF_LOAD_BINDIND3    ":/CommonClasses/GraphicalReport/report_text/05_binding_index_3"
#define  CONF_LOAD_BEHAVE      ":/CommonClasses/GraphicalReport/report_text/06_behavioral_conduct"

#define  RS_CODE_TITLE          "title_"
#define  RS_CODE_EXPLANATION    "explanation_"
#define  RS_CODE_RANGE_TEXT     "range_text_"
#define  RS_CODE_CUTOFF_VALUES  "cutoff_values"
#define  RS_CODE_SMALLER_BETTER "smaller_better"
#define  RS_CODE_MULT_BY_100    "mult_by_100"
#define  RS_CODE_VALUE          "associated_value"

class ResultSegment
{
public:
    ResultSegment();
    typedef enum {BSCC_NONE,BSCC_RED,BSCC_GREEN,BSCC_YELLOW} BarSegmentColorCode;
    bool loadSegment(const QString &conf, const QString &language);

    void setValue(const qreal &val);

    QString getDisplayValue() const;
    QString getTitle() const;
    QString getRangeText() const;
    QString getExplanation() const;
    QString getNameIndex() const;

    BarSegmentColorCode getBarSegmentColor() const;
    qint32 getBarSegmentIndex() const;
    qint32 getNumberOfSegments() const;

    QVariantMap getMapForDisplay() const;

    static QString code2String(const BarSegmentColorCode &bscc);

    QString getError() const {return error;}

private:

    QString error;

    QString title;
    QString explanation;
    QString rangeText;
    QString nameIndex;
    QList<qreal> cuttoffValues;
    bool smallerBetter;
    bool multBy100;

    qreal value;
    qint32 displayValue;

    QHash<QString,QString> langCodes;

    qint32 segmentIndicator;
    BarSegmentColorCode barColorCode;

};

#endif // RESULTSEGMENT_H
