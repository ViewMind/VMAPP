#ifndef QGRAPHICSEXPERIMENT_H
#define QGRAPHICSEXPERIMENT_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QDebug>

#include "../../../CommonClasses/Experiments/qgraphicsarrow.h"

QT_BEGIN_NAMESPACE
namespace Ui { class QGraphicsExperiment; }
QT_END_NAMESPACE

class QGraphicsExperiment : public QMainWindow
{
    Q_OBJECT

public:
    QGraphicsExperiment(QWidget *parent = nullptr);
    ~QGraphicsExperiment() override;

private:
    Ui::QGraphicsExperiment *ui;
    QGraphicsScene *canvas;

    void RunTests();

};
#endif // QGRAPHICSEXPERIMENT_H
