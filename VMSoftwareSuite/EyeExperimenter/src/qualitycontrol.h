#ifndef QUALITYCONTROL_H
#define QUALITYCONTROL_H

#include <QThread>
#include "../../../CommonClasses/RawDataContainer/viewminddatacontainer.h"
#include "eyexperimenter_defines.h"

class QualityControl: public QThread
{
    Q_OBJECT

public:
    QualityControl();

    void run();

    void setVMContainterFile(const QString &file);

    QStringList getStudyList() const;

    QVariantMap getGraphDataAndReferenceDataForStudy(const QString &study);

    QString getError() const;

    QString getSetFileName() const;

private:

    ViewMindDataContainer rawdata;

    QString originalFileName;

    QString error;

    QVariantMap qualityControlData;

    QStringList availableStudies;

    static const char * GRAPH_DATA;
    static const char * REF_DATA;

    bool computeQualityControlVectors(const QString &studyType, const QString &metaStudyName);
};

#endif // QUALITYCONTROL_H
