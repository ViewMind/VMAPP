#ifndef REPORTVIEWERDEV_H
#define REPORTVIEWERDEV_H

#include <QMainWindow>

#include "../../CommonClasses/ImageExplorer/imageexplorer.h"

namespace Ui {
class ReportViewerDev;
}

class ReportViewerDev : public QMainWindow
{
    Q_OBJECT

public:
    explicit ReportViewerDev(QWidget *parent = 0);
    ~ReportViewerDev();

private:
    Ui::ReportViewerDev *ui;
    ImageExplorer *explorer;

};

#endif // REPORTVIEWERDEV_H
