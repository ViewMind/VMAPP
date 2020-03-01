#ifndef RESULTBAR_H
#define RESULTBAR_H

#include "../common.h"

class ResultBar
{
public:

    typedef enum {BSCC_NONE,BSCC_RED,BSCC_GREEN,BSCC_YELLOW} BarSegmentColorCode;

    struct ResultBarCodes {

    public:
        void reset();
        void setIDX(const ResultBar &resbar);
        QString toString(const QString &joiner  = ",") const;
        QString getDiagnosisClass() const;

        static QString code2String(const BarSegmentColorCode &bscc);

    private:
        BarSegmentColorCode idxIDC;   // Congnitive Impairment Index
        BarSegmentColorCode idxIDBA;  // Index of Beta Amiloid
        BarSegmentColorCode idxEXP;   // Executive Processes
        BarSegmentColorCode idxWM;    // Working Memory
        BarSegmentColorCode idxRM;    // Retrieval Memory
        BarSegmentColorCode idxBeh;   // Behavioural Index

    };

    ResultBar();
    bool setResultType(const QString &resultType);
    void setValue(const QVariant &varvalue);
    qint32 getSegmentBarIndex() const {return segmentBarIndex;}
    QString getValue() const;
    qint32 getValueAsNumber() const {return value;}
    QList<qint32> getSegmentBarCutOffValues() const {return segmentBarCuttOffValues;}
    bool isLargerBetter() const {return largerBetter;}
    bool hasTwoSections() const {return (segmentBarCuttOffValues.size() == 3);}
    QString getResultConfigured() const {return resType;}
    BarSegmentColorCode getBarSegmentColorCode() const { return bscc; }


private:
    QString resType;
    QList<qint32> segmentBarCuttOffValues;
    qint32 segmentBarIndex;
    qint32 value;
    bool largerBetter;
    BarSegmentColorCode bscc;
    void reportValueConversion(qreal rvalue);

};

#endif // RESULTBAR_H
