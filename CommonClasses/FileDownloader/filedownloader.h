#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QElapsedTimer>

/**
 * @brief The FileDownloader class is used to simply set a URL which points to a file and save said file to specified disk destination.
 */

class FileDownloader : public QObject
{
    Q_OBJECT
public:
    FileDownloader();
    ~FileDownloader() override;

    bool download(const QString &url, const QString &destinationFile);

    QString getError() const;

    QString getDownloadDestination() const;

signals:
    void downloadCompleted(bool allOk);
    void downloadProgress(qreal progress, qreal hours, qreal minutes, qreal seconds);

private slots:
    void downloadComplete(QNetworkReply * netreply);
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QString error;
    QString downloadDestination;
    bool downLoadInProgress;
    QNetworkAccessManager dlControl;
    QNetworkReply * downloadMonitor;
    qreal previousProgress;
    QElapsedTimer timer;
    qint64 acc_ms;

};

#endif // FILEDOWNLOADER_H
