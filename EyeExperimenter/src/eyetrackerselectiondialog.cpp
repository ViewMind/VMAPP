#include "eyetrackerselectiondialog.h"
#include "ui_eyetrackerselectiondialog.h"

EyeTrackerSelectionDialog::EyeTrackerSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EyeTrackerSelectionDialog)
{
    ui->setupUi(this);
}

qint32 EyeTrackerSelectionDialog::getSelectedEyeTracker() const{
    return ui->cboxEyeTracker->currentIndex();
}

EyeTrackerSelectionDialog::~EyeTrackerSelectionDialog()
{
    delete ui;
}
