#ifndef DIAGONOSISLOGIC_H
#define DIAGONOSISLOGIC_H

#include "../../CommonClasses/common.h"
#include "resultsegment.h"

class DiagonosisLogic
{
public:
    DiagonosisLogic();
    void reset();
    void setResultSegment(const ResultSegment &rs);
    QString getDiagnosisClass() const;

    QString getDebugString (const QString &joiner);

private:
    ResultSegment::BarSegmentColorCode idxIDC;   // Congnitive Impairment Index
    ResultSegment::BarSegmentColorCode idxIDBA;  // Index of Beta Amiloid
    ResultSegment::BarSegmentColorCode idxEXP;   // Executive Processes
    ResultSegment::BarSegmentColorCode idxWM;    // Working Memory
    ResultSegment::BarSegmentColorCode idxRM;    // Retrieval Memory
    ResultSegment::BarSegmentColorCode idxBeh;   // Behavioural Index

};

#endif // DIAGONOSISLOGIC_H
