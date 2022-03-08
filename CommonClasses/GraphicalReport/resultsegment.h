#ifndef RESULTSEGMENT_H
#define RESULTSEGMENT_H

#include <QHash>

#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

#define  CONF_LOAD_RDINDEX                ":/CommonClasses/GraphicalReport/report_text/00_index_of_cognitive_impairment"
#define  CONF_LOAD_EXECPROC               ":/CommonClasses/GraphicalReport/report_text/01_executive_processes"
#define  CONF_LOAD_WORKMEM                ":/CommonClasses/GraphicalReport/report_text/02_working_memory"
#define  CONF_LOAD_RETMEM                 ":/CommonClasses/GraphicalReport/report_text/03_retrieval_memory"
#define  CONF_LOAD_BINDIND2               ":/CommonClasses/GraphicalReport/report_text/04_binding_index_2"
#define  CONF_LOAD_BINDIND3               ":/CommonClasses/GraphicalReport/report_text/05_binding_index_3"
#define  CONF_LOAD_BEHAVE                 ":/CommonClasses/GraphicalReport/report_text/06_behavioral_conduct"
#define  CONF_LOAD_NBRT_FIX_ENC           ":/CommonClasses/GraphicalReport/report_text/07_nbackrt_num_fix_enc"
#define  CONF_LOAD_NBRT_FIX_RET           ":/CommonClasses/GraphicalReport/report_text/08_nbackrt_num_fix_ret"
#define  CONF_LOAD_NBRT_INHIB_PROC        ":/CommonClasses/GraphicalReport/report_text/09_nbackrt_inhib_proc"
#define  CONF_LOAD_NBRT_SEQ_COMPLETE      ":/CommonClasses/GraphicalReport/report_text/10_nbackrt_seq_complete"
#define  CONF_LOAD_NBRT_TARGET_HIT        ":/CommonClasses/GraphicalReport/report_text/11_nbackrt_target_hit"
#define  CONF_LOAD_NBRT_MEAN_RESP_TIME    ":/CommonClasses/GraphicalReport/report_text/12_nbackrt_mean_resp_time"
#define  CONF_LOAD_NBRT_MEAN_SAC_AMP      ":/CommonClasses/GraphicalReport/report_text/13_nbackrt_mean_sac_amp"
#define  CONF_LOAD_GNG_DMT_FACILITATE     ":/CommonClasses/GraphicalReport/report_text/14_gng_dmt_facilitate"
#define  CONF_LOAD_GNG_DMT_INTERFERENCE   ":/CommonClasses/GraphicalReport/report_text/15_gng_dmt_interference"
#define  CONF_LOAD_GNG_PIP_FACILITATE     ":/CommonClasses/GraphicalReport/report_text/16_gng_pip_facilitate"
#define  CONF_LOAD_GNG_PIP_INTERFERENCE   ":/CommonClasses/GraphicalReport/report_text/17_gng_pip_interference"
#define  CONF_LOAD_GNG_SPEED_PROCESSING   ":/CommonClasses/GraphicalReport/report_text/18_gng_speed_processing"


#define  RS_CODE_TITLE          "title_"
#define  RS_CODE_EXPLANATION    "explanation_"
#define  RS_CODE_RANGE_TEXT     "range_text_"
#define  RS_CODE_CUTOFF_VALUES  "cutoff_values"
#define  RS_CODE_SMALLER_BETTER "smaller_better"
#define  RS_CODE_MULT_BY_100    "mult_by_100"
#define  RS_CODE_VALUE          "associated_value"
#define  RS_ROUND_FOR_DISPLAY   "round_for_display"

class ResultSegment
{
public:
    ResultSegment();
    typedef enum {BSCC_NONE,BSCC_RED,BSCC_GREEN,BSCC_YELLOW, BSCC_BLUE} BarSegmentColorCode;
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
    bool roundForDisplay;

    qreal value;
    QString displayValue;

    QHash<QString,QString> langCodes;

    qint32 segmentIndicator;
    BarSegmentColorCode barColorCode;

};

#endif // RESULTSEGMENT_H
