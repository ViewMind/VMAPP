#include "filedownloader.h"

FileDownloader::FileDownloader()
{
    downLoadInProgress = false;
    downloadMonitor = nullptr;
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

    // This allows us to know how much of the file is being downloaded.
    downloadMonitor = dlControl.get(request);
    connect(downloadMonitor, &QNetworkReply::downloadProgress,this,&FileDownloader::onDownloadProgress);
    previousProgress = 0;

    acc_ms = 0;
    timer.start();

    return true;
}

QString FileDownloader::getError() const{
    return error;
}

QString FileDownloader::getDownloadDestination() const{
    return downloadDestination;
}

void FileDownloader::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal){
    qreal num = bytesReceived;
    qreal den = bytesTotal;
    qreal p = num*100.0/den;

    // We should only notify whatever object is connected when there is a least a 1% increase
    if ((p - previousProgress) < 1) return;
    previousProgress = p;

    // We estimate how many milliseconds remaining there are until the download finishes.
    qreal ms = timer.elapsed();
    qreal ms_remaining =ms*(den-num)/num;

    //qDebug() << "It took" << s << "ms to get" << num << "bytes. So for the remaining " << (den-num) << " bytes it will take" << ss;

    qreal seconds = ms_remaining/1000;
    qreal minutes = qFloor(seconds/60);
    seconds = seconds - minutes*60;
    qreal hours = qFloor(minutes/60);
    minutes = minutes - hours*60;

    qDebug() << hours << minutes << seconds;

    emit FileDownloader::downloadProgress(p,hours,minutes,seconds);


}

void FileDownloader::downloadComplete(QNetworkReply * netreply){

    // All received bytes are saved to a file.
    downLoadInProgress = false;
    disconnect(downloadMonitor, &QNetworkReply::downloadProgress,this,&FileDownloader::onDownloadProgress);

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

