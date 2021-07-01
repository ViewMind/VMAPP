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

    if (data.containsKeyword(RS_ROUND_FOR_DISPLAY)){
        roundForDisplay = data.getBool(RS_ROUND_FOR_DISPLAY);
    }
    else{
        roundForDisplay = true;
    }

    barColorCode = BSCC_NONE;

    cuttoffValues.clear();
    QStringList temp = data.getStringList(RS_CODE_CUTOFF_VALUES);

    //qDebug() << "Value list" << temp;

    for (qint32 i = 0; i < temp.size(); i++){
        cuttoffValues << temp.at(i).toDouble();
    }

    return true;

}

void ResultSegment::setValue(const qreal &val){

    value = val;
    qreal compareValue;

    if (multBy100){
        if (roundForDisplay){
            qint32 temp = qRound(value*100);
            displayValue = QString::number(temp);
            compareValue = temp;
        }
        else{
            displayValue = QString::number(value*100,'f',2);
            compareValue = value*100;
        }
    }
    else{
        if (roundForDisplay){
            qint32 temp = qRound(value);
            displayValue = QString::number(temp);
            compareValue = temp;
        }
        else{
            displayValue = QString::number(value,'f',2);
            compareValue = value;
        }
    }

    //qDebug() << "DISPLAY VALUE" << displayValue << val << roundForDisplay;

    int result = 0;
    result = 0;
    for (qint32 i = 0; i < cuttoffValues.size()-1; i++){
        if (compareValue < cuttoffValues.at(i)){
            break;
        }
        result = i;
    }

    // If larger is better then the indexes are inverted as 0 represents green which is good and 2 represents red.
    if (!smallerBetter){
        result = (cuttoffValues.size() - 2) - result;
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
    else if (cuttoffValues.size() == 5){
        switch (result) {
        case 0:
            barColorCode = BSCC_BLUE;
            break;
        case 1:
            barColorCode = BSCC_GREEN;
            break;
        case 2:
            barColorCode = BSCC_YELLOW;
            break;
        case 3:
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
    return displayValue;
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
    case BSCC_BLUE:
        return "BLUE";
    }
    return "UNKNOWN";
}

QVariantMap ResultSegment::getMapForDisplay() const{
    QVariantMap map;

    map["vmTitleText"] = title;
    QString exp = explanation;
    exp = exp.replace("/n","<br>");
    map["vmExpText"] = exp;
    map["vmRefText"] = rangeText;
    map["vmResValue"] = getDisplayValue();
    map["vmResBarIndicator"] = QString::number(segmentIndicator);
    map["vmNumOfSegements"] = cuttoffValues.size() -1;
    map["vmIsStudyTitle"] = false;

    return map;

}
