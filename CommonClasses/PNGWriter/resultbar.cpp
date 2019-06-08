#include "resultbar.h"

ResultBar::ResultBar()
{
}

bool ResultBar::setResultType(const QString &resultType){

    resType = resultType;
    values.clear();
    if (resultType == CONFIG_RESULTS_READ_PREDICTED_DETERIORATION){
        values << 0 << 0.30 << 0.60;
        largerBetter = true;
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
    else if (resultType == CONFIG_RESULTS_BINDING_CONVERSION_INDEX){
        values << 0 << 0.26 << 0.52;
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

    value = varvalue.toReal();
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

