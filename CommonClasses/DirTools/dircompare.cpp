#include "dircompare.h"

DirCompare::DirCompare(QObject *parent) : QThread{parent} {

  connect(&ref,&DirRunner::finished,this,&DirCompare::onDirRunFinished);
  connect(&dir,&DirRunner::finished,this,&DirCompare::onDirRunFinished);

}

void DirCompare::onDirRunFinished(){

    dirsDone++;
    if (dirsDone == 2){
        // Both finished.
        this->start();
    }
}

void DirCompare::setDirs(const QString &refdir, const QString &checkdir){
    ref.setupListGeneration(refdir);
    dir.setupListGeneration(checkdir);

    QFileInfo baseref(refdir);
    this->baseRefDir = baseref.absoluteFilePath();

    QFileInfo basecheck(checkdir);
    this->baseCheckDir = basecheck.absoluteFilePath();
}

QStringList DirCompare::getFileList(FileListType flt) const{
    switch (flt) {
    case FLT_BAD_CHECSUM:
        return this->filesBadChecksum;
    case FLT_NOT_IN_CHECK:
        return this->filesNotInCheckDir;
    case FLT_NOT_IN_REF:
        return this->filesNotInRefDir;
    default:
        return this->filesSameAsRefDir;
    }
}

void DirCompare::compare(){
    dirsDone = 0;
    ref.start();
    dir.start();
}

void DirCompare::run(){

    QStringList refFileList = ref.getListOfPaths();
    QStringList checkFileList = dir.getListOfPaths();

    QSet<QString> checkFileSet(checkFileList.begin(),checkFileList.end());

    this->filesBadChecksum.clear();
    this->filesNotInCheckDir.clear();
    this->filesNotInRefDir.clear();
    this->filesSameAsRefDir.clear();

    qreal total = refFileList.size();


    // We need to search each file in ref directory in the check directory.
    for (qint32 i = 0; i < refFileList.size(); i++){

        // We take the reference file.
        QString reffile = refFileList.at(i);

        // We replace the root base with the check base.
        QString checkfile = reffile;
        checkfile.replace(this->baseRefDir,this->baseCheckDir);

        // Now we check if the file exists.
        //qDebug() << "Checking for" << checkfile;
        if (checkFileSet.contains(checkfile)){

            // We remove the file from the set.
            checkFileSet.remove(checkfile);

            QString refSum = computeFileCheckSum(reffile);
            QString checkSum = computeFileCheckSum(checkfile);
            if (refSum == checkSum){
                this->filesSameAsRefDir << checkfile;
            }
            else {
                this->filesBadChecksum << checkfile;
            }

        }
        else {
            // The file is missing in the set and so it is added to the proper list.
            this->filesNotInCheckDir << checkfile;
        }

        qreal p = i+1;
        p = p*100.0/total;
        emit DirCompare::updateProgress(p,reffile);

    }

    // Remaining files in the set are the files NOT in the ref dir.
    checkFileList.clear();
    this->filesNotInRefDir = checkFileSet.values();

}

QString DirCompare::computeFileCheckSum(const QString &filename){

    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        qDebug() << "Cannot read file" << filename;
        return "";
    }

    QCryptographicHash hash(CheckSumAlg);
    hash.addData(&file);
    QString ans(hash.result().toHex());
    file.close();

    //qDebug() << "Computed the checksum of file" << filename;

    return ans;

}
