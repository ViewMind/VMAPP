#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QPainter>
#include <QImage>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Setup
    qreal W = 1024;
    qreal H = 768;
    QGraphicsScene scene(0,0,W,H);
    QImage image(W,H,QImage::Format_RGB888);
    QPainter painter(&image);
    scene.render(&painter);


    // Drawing
    painter.setBrush(QBrush(QColor(255,0,0)));
    painter.drawRect(0,0,200,100);
    painter.setBrush(QBrush(QColor(0,255,0)));
    painter.drawRect(200,100,200,100);


    image.save("mytest.png");
    qWarning() << "DONE";

    return a.exec();
}
