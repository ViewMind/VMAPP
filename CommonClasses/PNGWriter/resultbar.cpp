#include "resultbar.h"

ResultBar::ResultBar()
{
}

bool ResultBar::setResultType(const QString &resultType){

    resType = resultType;
    values.clear();
    if (resultType == CONFIG_RESULTS_ATTENTIONAL_PROCESSES){
        values << 450 << 550 << 650 << 750;
        largerBetter = false;        
    }
    else if (resultType == CONFIG_RESULTS_EXECUTIVE_PROCESSES){
        values << -4 << 18 << 40 << 62;
        largerBetter = false;
    }
    else if (resultType == CONFIG_RESULTS_WORKING_MEMORY){
        values << 50 << 60 << 70 << 80;
        largerBetter = true;
    }
    else if (resultType == CONFIG_RESULTS_RETRIEVAL_MEMORY){
        values << 7 << 15 << 23 << 31;
        largerBetter = true;
    }
    else if (resultType == CONFIG_RESULTS_MEMORY_ENCODING){
        values << -0.95 << 0.05 << 1.05;
        largerBetter = true;
    }
    else if (resultType == CONFIG_RESULTS_BEHAVIOURAL_RESPONSE){
        values << -1 << 0 << -1;
        largerBetter = true;
    }
    else{
        return false;
    }

    return true;
}

void ResultBar::setValue(const QVariant &varvalue){

    // The first step is an interpretation of the value
    qreal calcvalue;

    if (resType == CONFIG_RESULTS_BEHAVIOURAL_RESPONSE){
        QStringList list = varvalue.toStringList();
        qint32 BC = list.first().toUInt();
        value = BC;
           if (BC > 10){
            // This is the yellow section.
            calcvalue = -0.5;
        }
        else{
            calcvalue = 0.5;
        }
    }
    else {
        value = varvalue.toReal();
        calcvalue = value;
    }

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

