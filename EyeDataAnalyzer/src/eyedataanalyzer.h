#ifndef EYEDATAANALYZER_H
#define EYEDATAANALYZER_H

#include <QMainWindow>
#include <QFileDialog>
#include <QLineEdit>

#define   PROGRAM_NAME      "EyeDataAnalyzer"
#define   PROGRAM_VERSION   "1.2.3"
#define   WORK_DIR          "outputs"

#include "../../CommonClasses/HTMLWriter/htmlwriter.h"
#include "../../CommonClasses/DataAnalysis/rawdataprocessor.h"
#include "../../CommonClasses/PNGWriter/imagereportdrawer.h"
#include "fixationdrawer.h"

namespace Ui {
class EyeDataAnalyzer;
}

class EyeDataAnalyzer : public QMainWindow
{
    Q_OBJECT

public:
    explicit EyeDataAnalyzer(QWidget *parent = 0);
    ~EyeDataAnalyzer();

private slots:
    void on_pbBrowsePatientDir_clicked();

    void on_leTimeFilterThreshold_editingFinished();

    void on_leMovingWindowMaxDispersion_editingFinished();

    void on_leMinimumFixationLength_editingFinished();

    void on_leSampleFrequency_editingFinished();

    void on_leLatencyEscapeRadious_editingFinished();

    void on_leDistanceToMonitor_editingFinished();

    void on_leMarginTargetHit_editingFinished();

    void on_leXToMM_editingFinished();

    void on_leYToMM_editingFinished();

    void on_comboLang_currentIndexChanged(const QString &arg1);

    void on_cbUseLogo_stateChanged(int arg1);

    void on_comboEyeSelection_currentIndexChanged(int index);

    void on_leDoctorsName_editingFinished();

    void on_lePatientName_editingFinished();

    void on_lePatientAge_editingFinished();

    void on_pbAnalyzeData_clicked();

    void onProcessorMessage(const QString &msg, qint32 type);

    void on_pbDrawFixations_clicked();

    void on_pbGeneratePNG_clicked();

private:
    Ui::EyeDataAnalyzer *ui;
    ConfigurationManager configuration;
    LogInterface log;
    HTMLWriter htmlWriter;
    QHash<QString,FixationList> lastFixations;
    QHash<QString,QString> processedFiles;

    // Fills the ui with loaded configuration
    void fillUi();

    // Fills the list widget with the currently selected directory.
    void listDatFilesInPatientDirectory();

    // Validation for numeric values
    void validateAndSave(QLineEdit *le, const QString &parameter, bool positive, bool real);
    QString validNumber(const QString &str, bool positive = true, bool isReal = false);
};

#endif // EYEDATAANALYZER_H
