#include "filedownloader.h"

FileDownloader::FileDownloader()
{
    downLoadInProgress = false;
    connect(&dlControl,&QNetworkAccessManager::finished,this,&FileDownloader::downloadComplete);
}

bool FileDownloader::download(const QString &url, const QString &destinationFile){

    if (downLoadInProgress){
        return false;
    }

    error = "";

    //qDebug() << "FILE DOWNLOADER. Setting dest as '" + destinationFile + "' for URL: " << url;

    downloadDestination = destinationFile;
    downLoadInProgress = true;

    // Start the download.
    QUrl qurl(url);
    QNetworkRequest request(qurl);
    dlControl.get(request);

    return true;
}

QString FileDownloader::getError() const{
    return error;
}

QString FileDownloader::getDownloadDestination() const{
    return downloadDestination;
}

void FileDownloader::downloadComplete(QNetworkReply * netreply){

    // All received bytes are saved to a file.
    downLoadInProgress = false;

    QFile receivedFile(downloadDestination);
    if (!receivedFile.open(QFile::WriteOnly)){
        error = "Could not open " + receivedFile.fileName() + " for writing";
        netreply->deleteLater();
        emit FileDownloader::downloadCompleted(false);
        return;
    }

    QDataStream fileWriter(&receivedFile);
    QByteArray rawdata = netreply->readAll();
    fileWriter.writeRawData(rawdata.constData(), static_cast<qint32>(rawdata.size()));
    receivedFile.close();

    netreply->deleteLater();
    emit FileDownloader::downloadCompleted(true);
}

FileDownloader::~FileDownloader() {
}

