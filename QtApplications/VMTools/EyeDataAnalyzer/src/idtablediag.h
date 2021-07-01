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
        bool PUID;
        bool HPUID;
        bool DBPUID;
        bool DID;
        QString dir;
    };

    explicit IDTableDiag(QWidget *parent = 0);
    ~IDTableDiag();

    void setInstitutions(const QHash<QString, QString> &imap);

    Columns getTableColumns() const;

private slots:
    void on_pbOk_clicked();

    void on_pbCancel_clicked();

private:
    Ui::IDTableDiag *ui;
};

#endif // IDTABLEDIAG_H
