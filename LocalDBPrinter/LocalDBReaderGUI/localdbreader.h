#ifndef LOCALDBREADER_H
#define LOCALDBREADER_H

#include <QMainWindow>
#include <QFileDialog>

#include "../../CommonClasses/LocalInformationManager/localinformationmanager.h"

namespace Ui {
class LocalDBReader;
}

class LocalDBReader : public QMainWindow
{
    Q_OBJECT

public:
    explicit LocalDBReader(QWidget *parent = 0);
    ~LocalDBReader();

private slots:
    void on_pbSelectAndGo_clicked();

private:
    Ui::LocalDBReader *ui;
};

#endif // LOCALDBREADER_H
