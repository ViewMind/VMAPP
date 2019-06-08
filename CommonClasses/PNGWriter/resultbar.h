#ifndef RESULTBAR_H
#define RESULTBAR_H

#include "../common.h"

class ResultBar
{
public:
    ResultBar();
    bool setResultType(const QString &resultType);
    void setValue(const QVariant &varvalue);
    qint32 getSegmentBarIndex() const {return segmentBarIndex;}
    QString getValue() const;
    qreal getValueAsReal() const {return value;}
    QList<qreal> getValues() const {return values;}
    bool isLargerBetter() const {return largerBetter;}
    bool hasTwoSections() const {return (values.size() == 3);}
    QString getResultConfigured() const {return resType;}

    static QString ReportValueConversion(const QString &resultType, qreal value);

private:
    QString resType;
    QList<qreal> values;
    qint32 segmentBarIndex;
    qreal value;
    bool largerBetter;

};

#endif // RESULTBAR_H
