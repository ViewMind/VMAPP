#ifndef FILEDLDEV_H
#define FILEDLDEV_H

#include <QMainWindow>
#include <QDebug>
#include <QString>
#include "../../../CommonClasses/FileDownloader/filedownloader.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FileDLDev; }
QT_END_NAMESPACE

class FileDLDev : public QMainWindow
{
    Q_OBJECT

public:
    FileDLDev(QWidget *parent = nullptr);
    ~FileDLDev();

public slots:
    void onDLProgress(qreal p, qreal hours, qreal minutes, qreal seconds);
    void onDLComplete(bool ok);

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

private:
    Ui::FileDLDev *ui;
    FileDownloader dl;
};
#endif // FILEDLDEV_H
