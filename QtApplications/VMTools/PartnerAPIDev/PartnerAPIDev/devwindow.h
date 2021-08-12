#ifndef DEVWINDOW_H
#define DEVWINDOW_H

#include <QMainWindow>
#include <QMessageAuthenticationCode>

QT_BEGIN_NAMESPACE
namespace Ui { class DevWindow; }
QT_END_NAMESPACE

#include <QJsonObject>
#include <QTimer>
#include "../../../CommonClasses/RestAPIController/orbitpartnerapi.h"
#include "../../../CommonClasses/debug.h"

class DevWindow : public QMainWindow
{
    Q_OBJECT

public:
    DevWindow(QWidget *parent = nullptr);
    ~DevWindow();

private slots:
    void on_pbTest_clicked();
    void onFinished();
    void onGotReply();

private:
    Ui::DevWindow *ui;
    OrbitPartnerAPI orbit;
    RESTAPIController apiclient;
};
#endif // DEVWINDOW_H
