#ifndef LANGUAGESELECTIONDIALOG_H
#define LANGUAGESELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class LanguageSelectionDialog;
}

class LanguageSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LanguageSelectionDialog(QWidget *parent = nullptr);
    ~LanguageSelectionDialog();

    QString getLanguage() const;

private slots:
    void on_pbOk_clicked();

private:
    Ui::LanguageSelectionDialog *ui;
};

#endif // LANGUAGESELECTIONDIALOG_H
