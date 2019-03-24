#include "datrecursivefinder.h"

DatRecursiveFinder::DatRecursiveFinder()
{

}

void DatRecursiveFinder::startFrom(const QString &dir){
    searchRecusively(dir);
}

void DatRecursiveFinder::searchRecusively(const QString &directory){

    QDir cdir(directory);
    QStringList filters; filters << "*.dat";
    QStringList files = cdir.entryList(filters,QDir::Files);
    QStringList dirs  = cdir.entryList(QStringList(),QDir::Dirs | QDir::NoDotAndDotDot);

    for (qint32 i = 0; i < files.size(); i++){
        fileList << directory + "/" + files.at(i);
    }

    for (qint32 i = 0; i < dirs.size(); i++){
        searchRecusively(directory + "/" + dirs.at(i));
    }
}
