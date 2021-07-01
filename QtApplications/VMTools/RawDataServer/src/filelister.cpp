#include "filelister.h"

FileLister::FileLister()
{

}


void FileLister::listFileInDirectory(const QString &dir){
    fileList.clear();
    addFilesToListFromDir(dir);
}

QStringList FileLister::getFileListNoRootDir() const{
    QStringList ans;
    for (qint32 i = 0; i < fileList.size(); i++){
        QStringList pathParts = fileList.at(i).split("/");
        if (pathParts.size() > 1){
            pathParts.removeFirst();
            ans << pathParts.join("/");
        }
        else ans << fileList.at(i);
    }
    return ans;
}

void FileLister::addFilesToListFromDir(const QString &dir){

    QStringList dirs  = QDir(dir).entryList(QStringList(),QDir::Dirs|QDir::NoDotAndDotDot,QDir::Name);
    QStringList files = QDir(dir).entryList(QStringList(),QDir::Files,QDir::Name);

    for (qint32 i = 0; i < files.size(); i++){
        fileList << dir + "/" + files.at(i);
    }

    for (qint32 i = 0; i < dirs.size(); i++){
        QString path = dir + "/" + dirs.at(i);
        addFilesToListFromDir(path);
    }

}
