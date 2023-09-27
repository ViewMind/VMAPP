#ifndef DIRCOMPARE_H
#define DIRCOMPARE_H

#include <QThread>
#include <QSet>
#include <QCryptographicHash>

#include "dirrunner.h"

/**
 * @brief The DirCompare class - Compares two directories by comparing the files (via checksume) and finding out the differences
 */

class DirCompare : public QThread
{
    Q_OBJECT

public:

    typedef enum {FLT_NOT_IN_REF, FLT_NOT_IN_CHECK, FLT_BAD_CHECSUM, FLT_SAME} FileListType;

    explicit DirCompare(QObject *parent = nullptr);

    /**
     * @brief setDirs - Sets the dir to compare.
     * @param refdir - The reference dir. The differences listed consider this the "correct" directory.
     * @param checkdir - The directory to check.
     */
    void setDirs(const QString &refdir, const QString &checkdir);

    /**
     * @brief compare - Starts the comparison in a separate thread.
     */
    void compare();

    void run();

    /**
     * @brief getFileList - Returns each of the file lists generated byt he comparison
     * @param flt - The type of file list to return.
     * @return The file list selected. The comparison searches for files in reference not in the check directory,
     *  files in the check directory no in reference, files that have the same relative paths but different checksums
     *  and files tha are the same in both.
     */
    QStringList getFileList(FileListType flt) const;

    /**
     * @brief runInThread - Runs the comparison in the calling thread. Either call compare or calll this, after calling setDirs.
     */
    void runInThread();

signals:
    void updateProgress(qreal p, QString fileBeingChecked);

private slots:
    void onDirRunFinished();

private:

    DirRunner ref;
    DirRunner dir;

    QString baseRefDir;
    QString baseCheckDir;

    QStringList filesNotInCheckDir;
    QStringList filesNotInRefDir;
    QStringList filesBadChecksum;
    QStringList filesSameAsRefDir;

    // Used to know when both dirs are done.
    int dirsDone;

    // The algorithm used
    const QCryptographicHash::Algorithm CheckSumAlg = QCryptographicHash::Sha1;

    QString computeFileCheckSum(const QString& filename);
    void doComparison();

};

#endif // DIRCOMPARE_H
