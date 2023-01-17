#ifndef QUALITYCONTROL_H
#define QUALITYCONTROL_H

#include <QThread>
#include "../../CommonClasses/RawDataContainer/viewminddatacontainer.h"
#include "eyexperimenter_defines.h"

class QualityControl: public QThread
{
    Q_OBJECT

public:
    QualityControl();

    void run() override;

    void setVMContainterFile(const QString &file, const QVariantMap &updatedQC);

    QVariantList getStudyList() const;

    QVariantMap getGraphDataAndReferenceDataForStudy(const QString &study);

    QString getError() const;

    QString getSetFileName() const;

    bool setDiscardReasonAndComment(const QString &discard_reson_code, const QString &comment);

    // Should only be used for debugging.
    void disableCheckSumVerification();

    bool checkFileIntegrity();


private:

    ViewMindDataContainer rawdata;

    QString originalFileName;

    QString error;

    QVariantMap qualityControlData;

    QVariantMap updatedQCParameters;

    QStringList availableStudies;
    QList<bool> listOfStudy3DFlag;

    bool verifyFileIntegrity;

    static const char * GRAPH_DATA;
    static const char * LOWER_REF_DATA;
    static const char * UPPER_REF_DATA;
    static const char * QC_INDEX;
    static const char * QC_OK;
    static const char * QC_THRESHOLD;

    bool computeQualityControlVectors(const QString &studyType, const QString &metaStudyName);
    bool computeQualityControlVectorsFor3DStudies(const QString &studyType, const QString &metaStudyName);
    bool computeQualityControlVectorsFor2DStudies(const QString &studyType, const QString &metaStudyName);

};

#endif // QUALITYCONTROL_H
