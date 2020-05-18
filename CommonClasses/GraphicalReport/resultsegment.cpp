#include "resultsegment.h"

ResultSegment::ResultSegment()
{
    langCodes["Spanish"] = "es";
    langCodes["English"] = "en";
}

bool ResultSegment::loadSegment(const QString &conf, const QString &language){

    ConfigurationManager data;
    if (!data.loadConfiguration(conf,COMMON_TEXT_CODEC)){
        error = data.getError();
        return false;
    }

    QString langCode = langCodes[language];

    //qDebug() << "Loading" << conf << "for" << language;
    //qDebug() << data.getMap();

    title         = data.getString(RS_CODE_TITLE + langCode);
    explanation   = data.getString(RS_CODE_EXPLANATION + langCode);
    rangeText     = data.getString(RS_CODE_RANGE_TEXT + langCode);
    smallerBetter = data.getBool(RS_CODE_SMALLER_BETTER);
    multBy100     = data.getBool(RS_CODE_MULT_BY_100);
    nameIndex     = data.getString(RS_CODE_VALUE);

    barColorCode = BSCC_NONE;

    cuttoffValues.clear();
    QStringList temp = data.getStringList(RS_CODE_CUTOFF_VALUES);
    for (qint32 i = 0; i < temp.size(); i++){
        cuttoffValues << temp.at(i).toDouble();
    }

    return true;

}

void ResultSegment::setValue(const qreal &val){

    value = val;

    if (multBy100){
        displayValue = qRound(value*100);
    }
    else{
        displayValue = qRound(value);
    }

    int result = 0;
    result = 0;
    for (qint32 i = 0; i < cuttoffValues.size()-1; i++){
        if (displayValue < cuttoffValues.at(i)){
            break;
        }
        result = i;
    }

    // If larger is better then the indexes are inverted as 0 represents green which is good and 2 represents red.
    if (!smallerBetter){
        if (cuttoffValues.size() == 4){
            if (result == 0) result = 2;
            else if (result == 2) result = 0;
        }
        else{
            if (result == 0) result = 1;
            else if (result == 1) result = 0;
        }
    }

    // Setting the bar type;
    if (cuttoffValues.size() == 4){
        // 3 Segment bar
        switch (result) {
        case 0:
            barColorCode = BSCC_GREEN;
            break;
        case 1:
            barColorCode = BSCC_YELLOW;
            break;
        case 2:
            barColorCode = BSCC_RED;
            break;
        default:
            break;
        }
    }
    else{
        if (result == 0) barColorCode = BSCC_GREEN;
        else barColorCode = BSCC_RED;
    }

    segmentIndicator = result;

}

QString ResultSegment::getTitle() const{
    return title;
}

QString ResultSegment::getRangeText() const{
    return rangeText;
}

QString ResultSegment::getExplanation() const{
    return explanation;
}

QString ResultSegment::getDisplayValue() const{
    return QString::number(displayValue);
}

QString ResultSegment::getNameIndex() const{
    return nameIndex;
}

ResultSegment::BarSegmentColorCode ResultSegment::getBarSegmentColor() const{
    return barColorCode;
}

qint32 ResultSegment::getBarSegmentIndex() const{
    return segmentIndicator;
}

qint32 ResultSegment::getNumberOfSegments() const {
    return cuttoffValues.size()-1;
}

QString ResultSegment::code2String(const BarSegmentColorCode &bscc){
    switch (bscc) {
    case BSCC_GREEN:
        return "GREEN";
    case BSCC_NONE:
        return "NONE";
    case BSCC_RED:
        return "RED";
    case BSCC_YELLOW:
        return "YELLOW";
    }
    return "UNKNOWN";
}

QVariantMap ResultSegment::getMapForDisplay() const{
    QVariantMap map;

    map["vmTitleText"] = title;
    QString exp = explanation;
    exp = exp.replace("/n","<br>");
    map ["vmExpText"] = exp;
    map["vmRefText"] = rangeText;
    map["vmResValue"] = getDisplayValue();
    map["vmResBarIndicator"] = QString::number(segmentIndicator);
    map["vmHasTwoSections"] = (cuttoffValues.size() == 3);

return map;

}
