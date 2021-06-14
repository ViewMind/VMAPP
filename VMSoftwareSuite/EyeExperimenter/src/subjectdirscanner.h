#ifndef SUBJECTDIRSCANNER_H
#define SUBJECTDIRSCANNER_H

#include <QDir>
#include "../../../CommonClasses/RawDataContainer/viewminddatacontainer.h"
#include "eyexperimenter_defines.h"

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
    static const char * FILE_PATH;

    SubjectDirScanner();

    // Sets up basinc info on what to scan.
    void setup(const QString &workdir, const QString &loggedUser);

    // Scans the setup directory for evaluations belongin to the setup user
    QList<QVariantMap> scanSubjectDirectoryForEvalutionsFrom();

    // The function searches for binding study files that are missing study, either UC or BC by the evaluator, in the set up direcotry, and that match the study configuration
    // Returns the newest file, if it finds it. Empty string otherwise.
    QString findIncompleteBindingStudies(const QString &missing_study, const QVariantMap study_configuration);

    // Searches for a perception study
    QString findIncompletedPerceptionStudy(const qint32 missing_part, const QVariantMap &study_configuration);

    // Returns the last error message.
    QString getError() const;

    // Returns the workDirectory that was set in setup.
    QString getSetDirectory() const;

    // Sorting method is a simple BubbleSort because the number of data should never be large.
    static QVariantMap sortSubjectDataListByOrder(QList<QVariantMap> list);

private:
    QString workDirectory;
    QString loggedInUser;
    QString error;


};

#endif // SUBJECTDIRSCANNER_H
