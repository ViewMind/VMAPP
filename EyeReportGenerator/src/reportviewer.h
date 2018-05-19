#ifndef REPORTVIEWER_H
#define REPORTVIEWER_H

#include <QWidget>
#include <QFile>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QGraphicsPixmapItem>
#include "../../../CommonClasses/common.h"

class ReportViewer: public QWidget
{
public:
    ReportViewer(QWidget *parent = 0);
    void loadReport(const QString &report);

private:
    QGraphicsView *gview;

};

#endif // REPORTVIEWER_H
