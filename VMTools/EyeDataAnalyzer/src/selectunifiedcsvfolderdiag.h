#ifndef SELECTUNIFIEDCSVFOLDERDIAG_H
#define SELECTUNIFIEDCSVFOLDERDIAG_H

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class SelectUnifiedCSVFolderDiag;
}

class SelectUnifiedCSVFolderDiag : public QDialog
{
    Q_OBJECT

public:
    explicit SelectUnifiedCSVFolderDiag(QWidget *parent = 0);
    ~SelectUnifiedCSVFolderDiag();

    qint32 getDisplayIDCode() const;
    QString getDirectory() const;

private slots:
    void on_pbBrowse_clicked();

    void on_pbOK_clicked();

    void on_pbCancel_clicked();

private:
    Ui::SelectUnifiedCSVFolderDiag *ui;
};

#endif // SELECTUNIFIEDCSVFOLDERDIAG_H
