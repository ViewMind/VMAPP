#ifndef BINDINGGENERATOR_H
#define BINDINGGENERATOR_H

#include <QMainWindow>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>

#include "../../CommonClasses/ConfigurationManager/configurationmanager.h"
#include "../../CommonClasses/Experiments/bindingmanager.h"

#include "bindingfilegenerator.h"

namespace Ui {
class BindingGenerator;
}

class BindingGenerator : public QMainWindow
{
    Q_OBJECT

public:
    explicit BindingGenerator(QWidget *parent = 0);
    ~BindingGenerator();

private slots:
    void on_pbGenFile_clicked();
    void on_pbGenImages_clicked();

private:
    Ui::BindingGenerator *ui;
    void scanPath();
    void drawPictures(const QString &input);

};

#endif // BINDINGGENERATOR_H
