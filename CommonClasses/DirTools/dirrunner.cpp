#include "dirrunner.h"

DirRunner::DirRunner() {
    fileToFind = "";
    dirToFind = "";
    rootDir = "";
    fileFound = false;
}

void DirRunner::setupFindFile(const QString &fileToFind, const QString &rootDir){
    this->rootDir = rootDir;
    this->fileToFind = fileToFind;
    this->dirToFind = "";
}

void DirRunner::setupListGeneration(const QString &rootDir){
    this->rootDir = rootDir;
    this->fileToFind = "";
    this->dirToFind = "";
}

void DirRunner::setupFindDir(const QString &dirToFind, const QString &rootDir){
    this->rootDir = rootDir;
    this->fileToFind = "";
    this->dirToFind = dirToFind;
}

QStringList DirRunner::getListOfPaths() const {
    return this->listOfPaths;
}

QString DirRunner::getPathForSearched() const {
    if (this->listOfPaths.empty()) return "";
    if ((!this->fileFound) && (this->fileToFind != "")) return ""; // Was searching for a file and it was not found.
    if ((!this->dirFound) && (this->dirToFind != "")) return ""; // Was searching for a dir and it was not found.
    return this->listOfPaths.last();
}

void DirRunner::run(){

    this->fileFound = false;
    this->dirFound = false;
    this->listOfPaths.clear();

    //qDebug() << this->rootDir;

    recursiveDirectoryList(this->rootDir,&listOfPaths);

}

void DirRunner::recursiveDirectoryList(const QString &dir, QStringList *list){

    if (this->fileFound) return;
    if (this->dirFound) return;

    QDir dirobj(dir);
    QFileInfoList entries = dirobj.entryInfoList(QStringList(),QDir::NoDotAndDotDot|QDir::AllDirs|QDir::Files);

    //qDebug() << dirobj.absolutePath() << entries;

    for (qint32 i = 0; i < entries.size(); i++){
        QString path = entries.at(i).absoluteFilePath();
        if (entries.at(i).isDir()){

            if (this->dirToFind != ""){
                if (path.endsWith(this->dirToFind)){
                    list->append(path);
                    this->dirFound = true;
                    return;
                }
            }

            recursiveDirectoryList(path,list);
        }
        else {
            if (this->fileToFind != "") {
                if (path.endsWith(this->fileToFind)){
                    list->append(path);
                    this->fileFound = true;
                    return;
                }
            }
            list->append(path);
        }

    }

}
