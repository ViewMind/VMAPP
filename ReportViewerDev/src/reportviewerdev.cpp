#include "reportviewerdev.h"
#include "ui_reportviewerdev.h"

ReportViewerDev::ReportViewerDev(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ReportViewerDev)
{
    ui->setupUi(this);
    explorer = new ImageExplorer();
    explorer->show();

    explorer->setImageFile("/home/ariela/Pictures/gundam2.png");
}

ReportViewerDev::~ReportViewerDev()
{
    delete ui;
}
