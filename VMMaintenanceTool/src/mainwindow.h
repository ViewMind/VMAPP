#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>

#include "../../CommonClasses/DirTools/dircompare.h"
#include "messagelogger.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:



    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDirRunFinished();
    void onDirCompareFinished();
    void onDirCompareProgressUpdate(qreal p, QString filename);

    void on_pbMainAction_clicked();

private:

    typedef enum { DM_UPDATE_MODE, DM_NORMAL_MODE, DM_PROGRESS_MODE} DisplayMode;

    Ui::MainWindow *ui;

    MessageLogger *logger;

    DirCompare dirComparer;
    DirRunner drunner;

    void setDisplayMode(DisplayMode dm);

};
#endif // MAINWINDOW_H
