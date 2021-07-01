#ifndef DATRECURSIVEFINDER_H
#define DATRECURSIVEFINDER_H

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QDebug>

class DatRecursiveFinder
{
public:
    DatRecursiveFinder();
    void startFrom(const QString &dir);
    QStringList getDatFileList() const {return fileList;}

private:

    void searchRecusively(const QString &directory);

    QStringList fileList;
};

#endif // DATRECURSIVEFINDER_H
