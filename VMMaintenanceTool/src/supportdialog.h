#ifndef SUPPORTDIALOG_H
#define SUPPORTDIALOG_H

#include <QDialog>
#include <QMovie>
#include "langs.h"
#include "paths.h"
#include "messagelogger.h"
#include "supportcontact.h"

namespace Ui {
class SupportDialog;
}

class SupportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SupportDialog(QWidget *parent = nullptr);
    ~SupportDialog();

    void setLoggger(MessageLogger *logger);
    QString getSupportContactResult() const;

private slots:
    void on_pushButton_clicked();
    void on_pbCancel_clicked();
    void onSCContactFinished();

private:
    Ui::SupportDialog *ui;
    SupportContact sc;
    MessageLogger *logger;

    void contactSupport();

};

#endif // SUPPORTDIALOG_H
