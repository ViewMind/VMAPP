#include <QApplication>
#include "../../CommonClasses/DataAnalysis/BarGrapher/bargrapher.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Setup
    qreal W = 1024;
    qreal H = 768;

    // A fictional data set;
    QList<qreal> ds1;
    ds1 << 50 << 500 << 368 << 198;

    QStringList descs;
    descs << "Item 1";
    descs << "A somewhat long and complex description";
    descs << "Item 2";
    descs << "Item Another";

    // The actual bar graph
    BarGrapher bg;
    bg.configureGraphText(W,H,descs,"Some sort of label","The title of this damn thing");
    BarGrapher::DataSet ds;
    ds << ds1;
    QImage result = bg.generateGraph(ds);

    qWarning() << "ERROR" << bg.getError();

//    QImage image(W,H,QImage::Format_RGB888);
//    QPainter painter(&image);

//    // Drawing
//    painter.setBrush(QBrush(QColor(255,0,0)));
//    painter.drawRect(0,0,200,100);
//    painter.setBrush(QBrush(QColor(0,255,0)));
//    painter.drawRect(200,100,200,100);

//    // Second image
//    QImage image2(W,H,QImage::Format_RGB888);
//    QPainter painter2(&image2);
//    painter2.setBrush(QBrush(QColor(0,0,255)));
//    painter2.drawRect(0,0,200,100);
//    painter2.setBrush(QBrush(QColor(255,255,0)));
//    painter2.drawRect(200,100,200,100);

//    // Cascading images vertically.
//    QImage result(W,2*H,QImage::Format_RGB888);
//    QPainter sumpainter(&result);
//    sumpainter.drawImage(0,0,image);
//    sumpainter.drawImage(0,H,image2);

    result.save("mytest.png");

    //image.save("mytest.png");
    qWarning() << "DONE";

    return a.exec();
}
