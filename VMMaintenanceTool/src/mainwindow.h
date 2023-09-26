#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>

#include "../../CommonClasses/DirTools/dircompare.h"
#include "messagelogger.h"
#include "langs.h"
#include "debugoptions.h"
#include "maintenancemanager.h"
#include "paths.h"
#include "supportcontact.h"
#include "supportdialog.h"

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
    void onProgressUpdate(qreal p, QString filename);
    void onNewMessage(qint32 type, QString message);
    void onMaintenanceFinished();

    void on_pbMainAction_clicked();

private:

    Ui::MainWindow *ui;
    MessageLogger *logger;
    MaintenanceManager maintainer;

};
#endif // MAINWINDOW_H
