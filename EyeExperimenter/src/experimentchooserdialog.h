#ifndef EXPERIMENTCHOOSERDIALOG_H
#define EXPERIMENTCHOOSERDIALOG_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include "../../CommonClasses/Experiments/common.h"

namespace Ui {
class ExperimentChooserDialog;
}

class ExperimentChooserDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExperimentChooserDialog(QWidget *parent = 0);
    ~ExperimentChooserDialog();

    // Gets the selected experiment.
    QList<qint32> getSelectedSequence() const;

    void setPatientData(const QString &name, quint8 age, const QString &email);

    // The patient name.
    QString getPatientName() const;

    // The patient's age.
    QString getPatientsAge() const;

    // The email
    QString getEmail() const;

private:
    Ui::ExperimentChooserDialog *ui;
};

#endif // EXPERIMENTCHOOSERDIALOG_H
