#include "instructiondialog.h"

InstructionDialog::InstructionDialog(QWidget *parent): QDialog(parent)
{
    // Making this window frameless and making sure it stays on top.
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint|Qt::Window);
    this->setGeometry(0,0,SCREEN_W,SCREEN_H);

    // Creating a graphics widget and adding it to the layout
    gview = new QGraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(gview);

    gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    gview->setScene(new QGraphicsScene(0,0,SCREEN_W,SCREEN_H));

}

void InstructionDialog::setInstruction(const QString &inst){

    QImage image(inst);
    image = image.scaled(QSize(SCREEN_W,SCREEN_H),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QPixmap pixmap = QPixmap::fromImage(image);
    gview->scene()->addPixmap(pixmap);

}

QPixmap InstructionDialog::getPixmap() const{
    QPixmap image(gview->scene()->width(),gview->scene()->height());
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    gview->scene()->render(&painter);
    return image;
}

void InstructionDialog::keyPressEvent(QKeyEvent *e){
    Q_UNUSED(e);
    this->accept();
}
