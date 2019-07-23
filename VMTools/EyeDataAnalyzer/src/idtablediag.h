#ifndef IDTABLEDIAG_H
#define IDTABLEDIAG_H

#include <QDialog>
#include <QFileDialog>
#include <QCheckBox>

namespace Ui {
class IDTableDiag;
}

class IDTableDiag : public QDialog
{
    Q_OBJECT

public:

    struct Columns{
        bool displayID;
        bool vmPid;
        bool vmHPid;
        QString dir;
    };

    explicit IDTableDiag(QWidget *parent = 0);
    ~IDTableDiag();

    Columns getTableColumns() const;

private slots:
    void on_pushButton_clicked();

    void on_pbOk_clicked();

    void on_pbCancel_clicked();

private:
    Ui::IDTableDiag *ui;
};

#endif // IDTABLEDIAG_H
