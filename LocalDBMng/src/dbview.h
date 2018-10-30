#ifndef DBVIEW_H
#define DBVIEW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>

#include "../../CommonClasses/LocalInformationManager/localinformationmanager.h"

#define ITEM_ACTION_BOOL   0
#define ITEM_ACTION_STRING 1
#define ITEM_ACTION_PASS   2
#define ROLE               2000

namespace Ui {
class DBView;
}

class DBView : public QMainWindow
{
    Q_OBJECT

public:
    explicit DBView(QWidget *parent = 0);
    ~DBView();

private slots:
    void on_pbBrowse_clicked();

    void on_checkBox_toggled(bool checked);

    void on_lvDoctors_itemClicked(QListWidgetItem *item);

    void on_pbDeleteSelected_clicked();

    void on_pbApplyChanges_clicked();

private:
    Ui::DBView *ui;
    LocalInformationManager *lim;
    ConfigurationManager config;

    void fillDoctorList();
};

#endif // DBVIEW_H
