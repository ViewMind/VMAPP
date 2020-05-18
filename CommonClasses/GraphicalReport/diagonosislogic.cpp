#include "diagonosislogic.h"

DiagonosisLogic::DiagonosisLogic()
{

}

void DiagonosisLogic::setResultSegment(const ResultSegment &rs){
    ResultSegment::BarSegmentColorCode bscc = rs.getBarSegmentColor();
    if (rs.getNameIndex() == CONFIG_RESULTS_READ_PREDICTED_DETERIORATION){
        idxIDC = bscc;
    }
    else if (rs.getNameIndex() == CONFIG_RESULTS_EXECUTIVE_PROCESSES){
        idxEXP = bscc;
    }
    else if (rs.getNameIndex() == CONFIG_RESULTS_WORKING_MEMORY){
        idxWM = bscc;
    }
    else if (rs.getNameIndex() == CONFIG_RESULTS_RETRIEVAL_MEMORY){
        idxRM = bscc;
    }
    else if (rs.getNameIndex() == CONFIG_RESULTS_BINDING_CONVERSION_INDEX){
        idxIDBA = bscc;
    }
    else if (rs.getNameIndex() == CONFIG_RESULTS_BEHAVIOURAL_RESPONSE){
        idxBeh = bscc;
    }

}

QString DiagonosisLogic::getDiagnosisClass() const{
    if ((idxIDC == ResultSegment::BSCC_GREEN) &&
            (idxEXP == ResultSegment::BSCC_GREEN) &&
            (idxIDBA == ResultSegment::BSCC_GREEN) &&
            (idxRM == ResultSegment::BSCC_GREEN) &&
            (idxWM == ResultSegment::BSCC_GREEN)) return "0";

    if ((idxIDC == ResultSegment::BSCC_RED) &&
            ((idxEXP == ResultSegment::BSCC_GREEN) || (idxEXP == ResultSegment::BSCC_YELLOW)) &&
            (idxIDBA == ResultSegment::BSCC_GREEN) &&
            ((idxWM == ResultSegment::BSCC_GREEN)  || (idxWM == ResultSegment::BSCC_YELLOW)) ) return "1";

    if ((idxIDC == ResultSegment::BSCC_RED) &&
            ((idxEXP == ResultSegment::BSCC_RED) || (idxWM == ResultSegment::BSCC_RED)) &&
            (idxIDBA == ResultSegment::BSCC_GREEN) ) return "2";

    if ((idxIDC == ResultSegment::BSCC_GREEN) &&
            ((idxEXP == ResultSegment::BSCC_GREEN) || (idxEXP == ResultSegment::BSCC_YELLOW)) &&
            (idxIDBA == ResultSegment::BSCC_RED) &&
            ((idxWM == ResultSegment::BSCC_GREEN)  || (idxWM == ResultSegment::BSCC_YELLOW)) ) return "3";

    if ((idxIDC == ResultSegment::BSCC_RED) &&
            ((idxEXP == ResultSegment::BSCC_RED) || (idxWM == ResultSegment::BSCC_RED)) &&
            (idxIDBA == ResultSegment::BSCC_RED) ) return "4";

    return "NA";
}

void DiagonosisLogic::reset(){
    idxIDC  = ResultSegment::BSCC_NONE;
    idxBeh  = ResultSegment::BSCC_NONE;
    idxEXP  = ResultSegment::BSCC_NONE;
    idxIDBA = ResultSegment::BSCC_NONE;
    idxRM   = ResultSegment::BSCC_NONE;
    idxWM   = ResultSegment::BSCC_NONE;
}


QString DiagonosisLogic::getDebugString(const QString &joiner){
    QStringList ans;
    ans << "Behavioural: " + ResultSegment::code2String(idxBeh);
    ans << "Exec. Proc.: " + ResultSegment::code2String(idxEXP);
    ans << "IDBA: " + ResultSegment::code2String(idxIDBA);
    ans << "IDC: " + ResultSegment::code2String(idxIDC);
    ans << "Ret. Mem.: " + ResultSegment::code2String(idxRM);
    ans << "Work. Mem: " + ResultSegment::code2String(idxWM);
    return ans.join(joiner);
}
