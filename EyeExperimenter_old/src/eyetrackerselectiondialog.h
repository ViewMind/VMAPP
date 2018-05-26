#ifndef EYETRACKERSELECTIONDIALOG_H
#define EYETRACKERSELECTIONDIALOG_H

#include <QDialog>

namespace Ui {
class EyeTrackerSelectionDialog;
}

class EyeTrackerSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EyeTrackerSelectionDialog(QWidget *parent = 0);
    ~EyeTrackerSelectionDialog();
    qint32 getSelectedEyeTracker() const;

private:
    Ui::EyeTrackerSelectionDialog *ui;
};

#endif // EYETRACKERSELECTIONDIALOG_H
