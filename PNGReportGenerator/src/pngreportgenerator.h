#ifndef PNGREPORTGENERATOR_H
#define PNGREPORTGENERATOR_H

#include <QMainWindow>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>

#include "imagereportdrawer.h"

namespace Ui {
class PNGReportGenerator;
}

class PNGReportGenerator : public QMainWindow
{
    Q_OBJECT

public:
    explicit PNGReportGenerator(QWidget *parent = 0);
    ~PNGReportGenerator();

private slots:
    void on_pbBrowse_clicked();

    void on_pbGenerate_clicked();

private:
    Ui::PNGReportGenerator *ui;
};

#endif // PNGREPORTGENERATOR_H
