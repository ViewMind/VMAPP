#ifndef FILELISTER_H
#define FILELISTER_H

#include <QDir>

class FileLister
{
public:
    FileLister();
    void listFileInDirectory(const QString &dir);
    QStringList getFileList() const {return fileList;}
    QStringList getFileListNoRootDir() const;

private:

    void addFilesToListFromDir(const QString &dir);
    QStringList fileList;

};

#endif // FILELISTER_H
