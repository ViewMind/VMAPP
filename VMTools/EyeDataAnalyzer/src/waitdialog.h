#ifndef WAITDIALOG_H
#define WAITDIALOG_H

#include <QDialog>

namespace Ui {
class WaitDialog;
}

class WaitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WaitDialog(QWidget *parent = 0);
    ~WaitDialog();
    void setMessage(const QString &msg);
    void setProgressBarVisibility(bool visibility);

public slots:
     void setProgressBarValue(qreal value);

private:
    Ui::WaitDialog *ui;
};

#endif // WAITDIALOG_H
