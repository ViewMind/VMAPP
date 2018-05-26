#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QDir>
#include "eye_experimenter_defines.h"
#include "../../CommonClasses/common.h"
#include "../../CommonClasses/Experiments/readingmanager.h"
#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = 0, ConfigurationManager *c = 0);
    ~SettingsDialog();


private slots:
    void on_pbOk_clicked();

    void on_cbBindingDefault_stateChanged(int arg1);

private:
    Ui::SettingsDialog *ui;
    ConfigurationManager *config;
    QString bool2String(bool boolean) const { if (boolean) return "true"; else return "false";}
};

#endif // SETTINGSDIALOG_H
