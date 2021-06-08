#ifndef SUBJECTDIRSCANNER_H
#define SUBJECTDIRSCANNER_H

#include <QDir>
#include "../../../CommonClasses/RawDataContainer/rawdatacontainer.h"

class SubjectDirScanner
{
public:

    // These are the fields of a Variant Map for each study, returned by scanning a patient directory.
    // They are a simplified version of the structure in the RawData container for sorting, display
    // and selection purposes.

    static const char * STATUS;
    static const char * CODE;
    static const char * DATE;
    static const char * TYPE;
    static const char * DISPLAY_ID;
    static const char * SUBJECT_INSTITUTION_ID;
    static const char * SUBJECT_NAME;
    static const char * EVALUATOR_NAME;
    static const char * EVALUATOR_ID;
    static const char * MEDIC_NAME;
    static const char * MEDIC_ID;
    static const char * ORDER_CODE;

    SubjectDirScanner();

    static QList<QVariantMap> scanSubjectDirectoryForEvalutionsFrom(const QString & directory, const QString &evalutor_username, QStringList *errorLst);

    // Sorting method is a simple BubbleSort because the number of data should never be mare than maybe a dozen or so.
    static void sortSubjectDataListByOrder(QList<QVariantMap> *list);

};

#endif // SUBJECTDIRSCANNER_H
