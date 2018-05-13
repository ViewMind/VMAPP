#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
#include <QTextStream>
#include <QFile>
#include <QMessageBox>

#include "showscreen.h"
#include "bindingfilegenerator.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void scanPath();

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    Ui::MainWindow *ui;

    QString outputImageRepo;
    QString sourcePath;

    BindingFileGenerator bfg;
    ShowScreen *screen;

    struct BFGSelection{
        bool bound;
        qint32 ntargets;
        bool numbered;
        qint32 state;
        QString getFilename() const{
            QString ans;
            if (bound) ans = "bc_";
            else ans = "uc_";
            ans = ans + QString::number(ntargets) + "_";
            if (numbered) ans = ans + "n";
            else ans = ans + "x";
            ans = ans + ".dat";
            return ans;
        }
    };

    BFGSelection selector;

};

#endif // MAINWINDOW_H
