#include "resultbar.h"

ResultBar::ResultBar()
{
    bscc = BSCC_NONE;
}

void ResultBar::ResultBarCodes::reset(){
    idxIDC = BSCC_NONE;
    idxBeh = BSCC_NONE;
    idxEXP = BSCC_NONE;
    idxIDBA = BSCC_NONE;
    idxRM = BSCC_NONE;
    idxWM = BSCC_NONE;
}

void ResultBar::ResultBarCodes::setIDX(const ResultBar &resbar){

    //qDebug() << "Setting result bar IDX of type" << resultType << " with color " << code2String(bscc);

    BarSegmentColorCode bscc = resbar.getBarSegmentColorCode();
    if (resbar.getResultConfigured() == CONFIG_RESULTS_READ_PREDICTED_DETERIORATION){
        idxIDC = bscc;
    }
    else if (resbar.getResultConfigured() == CONFIG_RESULTS_EXECUTIVE_PROCESSES){
        idxEXP = bscc;
    }
    else if (resbar.getResultConfigured() == CONFIG_RESULTS_WORKING_MEMORY){
        idxWM = bscc;
    }
    else if (resbar.getResultConfigured() == CONFIG_RESULTS_RETRIEVAL_MEMORY){
        idxRM = bscc;
    }
    else if (resbar.getResultConfigured() == CONFIG_RESULTS_BINDING_CONVERSION_INDEX){
        idxIDBA = bscc;
    }
    else if (resbar.getResultConfigured() == CONFIG_RESULTS_BEHAVIOURAL_RESPONSE){
        idxBeh = bscc;
    }

    //qDebug() << "After setting IDX:";
    //qDebug() << this->toString();
}

QString ResultBar::ResultBarCodes::code2String(const BarSegmentColorCode &bscc){
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

QString ResultBar::ResultBarCodes::getDiagnosisClass() const {    

    //qDebug() << "idc exp idba rm wm" << idxIDC << idxEXP << idxIDBA << idxRM << idxWM;

    if ((idxIDC == BSCC_GREEN) &&
            (idxEXP == BSCC_GREEN) &&
            (idxIDBA == BSCC_GREEN) &&
            (idxRM == BSCC_GREEN) &&
            (idxWM == BSCC_GREEN)) return "0";

    if ((idxIDC == BSCC_RED) &&
            ((idxEXP == BSCC_GREEN) || (idxEXP == BSCC_YELLOW)) &&
            (idxIDBA == BSCC_GREEN) &&
            ((idxWM == BSCC_GREEN)  || (idxWM == BSCC_YELLOW)) ) return "1";

    if ((idxIDC == BSCC_RED) &&
            ((idxEXP == BSCC_RED) || (idxWM == BSCC_RED)) &&
            (idxIDBA == BSCC_GREEN) ) return "2";

    if ((idxIDC == BSCC_GREEN) &&
            ((idxEXP == BSCC_GREEN) || (idxEXP == BSCC_YELLOW)) &&
            (idxIDBA == BSCC_RED) &&
            ((idxWM == BSCC_GREEN)  || (idxWM == BSCC_YELLOW)) ) return "3";

    if ((idxIDC == BSCC_RED) &&
            ((idxEXP == BSCC_RED) || (idxWM == BSCC_RED)) &&
            (idxIDBA == BSCC_RED) ) return "4";

    return "NA";
}

QString ResultBar::ResultBarCodes::toString(const QString &joiner) const {

    QStringList ans;
    ans << "Behavioural: " + code2String(idxBeh);
    ans << "Exec. Proc.: " + code2String(idxEXP);
    ans << "IDBA: " + code2String(idxIDBA);
    ans << "IDC: " + code2String(idxIDC);
    ans << "Ret. Mem.: " + code2String(idxRM);
    ans << "Work. Mem: " + code2String(idxWM);
    return ans.join(joiner);
}

bool ResultBar::setResultType(const QString &resultType){

    resType = resultType;
    values.clear();

    //qDebug() << "Setting result type to" << resultType;

    if (resultType == CONFIG_RESULTS_READ_PREDICTED_DETERIORATION){
        values << 0 << 30 << 60;
        largerBetter = true;
    }
    else if (resultType == CONFIG_RESULTS_EXECUTIVE_PROCESSES){
        // values << -4 << 18 << 40 << 62;
        largerBetter = false;
        // Values are inverted so that larger values are better.
        //values << 50 << 60 << 70 << 80;
        values << 28 << 36 << 44 << 51;
        largerBetter = false;
    }
    else if (resultType == CONFIG_RESULTS_WORKING_MEMORY){
        values << 50 << 60 << 70 << 80;
        largerBetter = true;
    }
    else if (resultType == CONFIG_RESULTS_RETRIEVAL_MEMORY){
        values << 1 << 5 << 9 << 13;
        largerBetter = true;
    }
    else if (resultType == CONFIG_RESULTS_BINDING_CONVERSION_INDEX){
        values << 0 << 26 << 52;
        largerBetter = true;
    }
    else if (resultType == CONFIG_RESULTS_BEHAVIOURAL_RESPONSE){
        largerBetter = true;
        values << 12 << 22 << 32;
    }
    else{
        return false;
    }

    return true;
}

QString ResultBar::getValue() const{
    return QString::number(value,'f',0);
}

void ResultBar::reportValueConversion(qreal rvalue){
    if (resType == CONFIG_RESULTS_READ_PREDICTED_DETERIORATION){
        value = qRound(rvalue*100);
    }
    //    else if (resType == CONFIG_RESULTS_EXECUTIVE_PROCESSES){
    //        value = (100-value);
    //    }
    else if (resType == CONFIG_RESULTS_BINDING_CONVERSION_INDEX){
        value = qRound(rvalue*100);
    }
    else value = qRound(rvalue);
}

void ResultBar::setValue(const QVariant &varvalue){

    qreal realValue;

    // The first step is an interpretation of the value
    if (resType == CONFIG_RESULTS_BEHAVIOURAL_RESPONSE){
        QVariantList bc_uc = varvalue.toList();
        //First value is BC ans and second is UC ans
        if (bc_uc.size() != 2) value = -1;
        else realValue = bc_uc.first().toDouble();
    }
    else realValue = varvalue.toReal();

    // Correcting the values.
    reportValueConversion(realValue);

    qreal calcvalue;
    calcvalue = value;
    int result = 0;
    result = 0;
    for (qint32 i = 0; i < values.size()-1; i++){
        if (calcvalue < values.at(i)){
            break;
        }
        result = i;
    }

    // If larger is better then the indexes are inverted as 0 represents green which is good and 2 represents red.
    if (largerBetter){
        if (values.size() == 4){
            if (result == 0) result = 2;
            else if (result == 2) result = 0;
        }
        else{
            if (result == 0) result = 1;
            else if (result == 1) result = 0;
        }
    }



    segmentBarIndex = result;
    // qDebug() << "Segment Bar Index" << result << " values size " << values.size();

    // Setting the bar type;
    if (values.size() == 4){
        // 3 Segment bar
        switch (segmentBarIndex) {
        case 0:
            bscc = BSCC_GREEN;
            break;
        case 1:
            bscc = BSCC_YELLOW;
            break;
        case 2:
            bscc = BSCC_RED;
            break;
        default:
            break;
        }
    }
    else{
        if (segmentBarIndex == 0) bscc = BSCC_GREEN;
        else bscc = BSCC_RED;
    }

    //qDebug() << "Segment Bar Index: " << ResultBarCodes::code2String(bscc);

}

