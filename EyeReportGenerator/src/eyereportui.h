#ifndef EYEREPORTUI_H
#define EYEREPORTUI_H

#include <QMainWindow>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QProcess>
#include <QTimer>

//#include "../../CommonClasses/common.h"
#include "../../CommonClasses/LogInterface/loginterface.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/ImageExplorer/imageexplorer.h"
#include "eyedataprocessingthread.h"

#define   EYE_REP_GEN_VERSION                           "1.3"

// Comment for normal function. Otherwise just generates a report
// #define  TEST_REPORT

namespace Ui {
class EyeReportUI;
}

class EyeReportUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit EyeReportUI(QWidget *parent = 0);
    ~EyeReportUI();

private slots:
    void onPThreadFinished();
    void onUpdatePBar(qint32 value);
    void onAppendMsg(const QString &msg, qint32 type);

private:
    ConfigurationManager configuration;
    Ui::EyeReportUI *ui;
    LogInterface log;
    EyeDataProcessingThread pthread;

    void testReport();

    void loadArguments();
    void addToConfigFromCmdLine(const QString &field, const QString &value);

};

#endif // EYEREPORTUI_H
