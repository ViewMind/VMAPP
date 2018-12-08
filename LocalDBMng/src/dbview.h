#ifndef DBVIEW_H
#define DBVIEW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QTime>

#include "../../CommonClasses/LocalInformationManager/localinformationmanager.h"

#include "howmanydiag.h"

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

    void on_actionInsert_Random_Patients_triggered();

private:
    Ui::DBView *ui;
    LocalInformationManager lim;
    //ConfigurationManager config;

    void fillDoctorList();

    QString getRandomFirstName(const QStringList &someRandomNames) const { int i = qrand() % someRandomNames.size(); return someRandomNames.at(i).split(" ").first(); }
    QString getRandomLastName(const QStringList &someRandomNames) const { int i = qrand() % someRandomNames.size(); return someRandomNames.at(i).split(" ").last(); }



};

#endif // DBVIEW_H
