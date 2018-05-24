#ifndef VIEWMINDUIDEV_H
#define VIEWMINDUIDEV_H

#include <QMainWindow>

namespace Ui {
class ViewMindUiDev;
}

class ViewMindUiDev : public QMainWindow
{
    Q_OBJECT

public:
    explicit ViewMindUiDev(QWidget *parent = 0);
    ~ViewMindUiDev();

private:
    Ui::ViewMindUiDev *ui;
};

#endif // VIEWMINDUIDEV_H
