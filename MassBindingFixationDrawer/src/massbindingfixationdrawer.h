#ifndef MASSBINDINGFIXATIONDRAWER_H
#define MASSBINDINGFIXATIONDRAWER_H

#include <QMainWindow>
#include "bindingfixationdrawer.h"

namespace Ui {
class MassBindingFixationDrawer;
}

class MassBindingFixationDrawer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MassBindingFixationDrawer(QWidget *parent = 0);
    ~MassBindingFixationDrawer();

private slots:
    void on_finished();

    void on_pbGo_clicked();

private:
    Ui::MassBindingFixationDrawer *ui;

    BindingFixationDrawer *fdrawer;
};

#endif // MASSBINDINGFIXATIONDRAWER_H
