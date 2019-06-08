#include "resultbar.h"

ResultBar::ResultBar()
{
}

bool ResultBar::setResultType(const QString &resultType){

    resType = resultType;
    values.clear();
    if (resultType == CONFIG_RESULTS_READ_PREDICTED_DETERIORATION){
        values << 0 << 30 << 60;
        largerBetter = true;
    }
    else if (resultType == CONFIG_RESULTS_EXECUTIVE_PROCESSES){
        //values << -4 << 18 << 40 << 62;
        //largerBetter = false;
        // Values are inverted so that larger values are better.
        values << 38 << 60 << 82 << 104;
        largerBetter = true;
    }
    else if (resultType == CONFIG_RESULTS_WORKING_MEMORY){
        values << 50 << 60 << 70 << 80;
        largerBetter = true;
    }
    else if (resultType == CONFIG_RESULTS_RETRIEVAL_MEMORY){
        values << 7 << 15 << 23 << 31;
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
    return ReportValueConversion(resType,value);
}

QString ResultBar::ReportValueConversion(const QString &resultType, qreal value){
    if (resultType == CONFIG_RESULTS_READ_PREDICTED_DETERIORATION){
        qint32 p = value*100;
        return QString::number(p);
    }
    else if (resultType == CONFIG_RESULTS_EXECUTIVE_PROCESSES){
        qint32 p = (100-value);
        return QString::number(p);
    }
    else if (resultType == CONFIG_RESULTS_WORKING_MEMORY){
        qint32 p = value;
        return QString::number(p);
    }
    else if (resultType == CONFIG_RESULTS_RETRIEVAL_MEMORY){
        qint32 p = value;
        return QString::number(p);
    }
    else if (resultType == CONFIG_RESULTS_BINDING_CONVERSION_INDEX){
        qint32 p = value*100;
        return QString::number(p);
    }
    else if (resultType == CONFIG_RESULTS_BEHAVIOURAL_RESPONSE){
        qint32 p = value;
        return QString::number(p);
    }
    return "";
}

void ResultBar::setValue(const QVariant &varvalue){

    // The first step is an interpretation of the value
    if (resType == CONFIG_RESULTS_BEHAVIOURAL_RESPONSE){
        QVariantList bc_uc = varvalue.toList();
        //First value is BC ans and second is UC ans
        if (bc_uc.size() != 2) value = -1;
        else value = bc_uc.first().toDouble();
    }
    else value = varvalue.toReal();

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
}

