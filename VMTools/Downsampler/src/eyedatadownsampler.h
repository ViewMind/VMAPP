#ifndef EYEDATADOWNSAMPLER_H
#define EYEDATADOWNSAMPLER_H

#include <QMainWindow>
#include <QMimeData>
#include <QIntValidator>
#include <QMessageBox>

#include "datasetdownsampler.h"
#include "frequencyanddispersionsweeper.h"

QT_BEGIN_NAMESPACE
namespace Ui { class EyeDataDownSampler; }
QT_END_NAMESPACE

class EyeDataDownSampler : public QMainWindow
{
    Q_OBJECT

public:
    EyeDataDownSampler(QWidget *parent = nullptr);
    ~EyeDataDownSampler() override;

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private slots:
    void on_mdSweeper_finished();
    void on_pbMDAndFrequencySweep_clicked();

private:
    Ui::EyeDataDownSampler *ui;    
    FrequencyAndDispersionSweeper::SweepParameters sweepParams;
    FrequencyAndDispersionSweeper sweeper;

};
#endif // EYEDATADOWNSAMPLER_H
