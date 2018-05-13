#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Setting up the paths:
    outputImageRepo = "C:/Users/Viewmind/Documents/QtProjects/BindingDesigner/00_image_outputs";
    sourcePath      = "C:/Users/Viewmind/Documents/QtProjects/BindingDesigner/src/texts";

    screen = new ShowScreen();
    screen->hide();

    scanPath();

    // Default values.
    selector.state = 0;
    selector.bound = true;
    selector.ntargets = 2;
    selector.numbered = false;
    ui->label->setText("Current selection: 02x");

}

void MainWindow::keyPressEvent(QKeyEvent *e){

    switch (e->key()){
    case Qt::Key_0:
        if (selector.state == 0){
            selector.bound = true;
            ui->label->setText("Current selection: 0");
            selector.state++;
        }
        break;
    case Qt::Key_1:
        if (selector.state == 0){
            selector.bound = false;
            ui->label->setText("Current selection: 1");
            selector.state++;
        }
        break;
    case Qt::Key_2:
        if (selector.state == 1){
            selector.ntargets = 2;
            ui->label->setText(ui->label->text() + "2");
            selector.state++;
        }
        break;
    case Qt::Key_3:
        if (selector.state == 1){
            selector.ntargets = 3;
            ui->label->setText(ui->label->text() + "3");
            selector.state++;
        }
        break;
    case Qt::Key_4:
        if (selector.state == 1){
            selector.ntargets = 4;
            ui->label->setText(ui->label->text() + "4");
            selector.state++;
        }
        break;
    case Qt::Key_X:
        if (selector.state == 2){
            selector.numbered = false;
            ui->label->setText(ui->label->text() + "x");
            selector.state = 0;
        }
        break;
    case Qt::Key_N:
        if (selector.state == 2){
            selector.numbered = true;
            ui->label->setText(ui->label->text() + "n");
            selector.state = 0;
        }
        break;
    case Qt::Key_G:
        if (selector.state == 0){
            QString filename = sourcePath + "/" + selector.getFilename();
            bfg.generateFile(filename,selector.bound,selector.numbered,selector.ntargets);
            scanPath();
            QMessageBox::information(this,"Done","Generation completed",QMessageBox::Ok);
        }
        break;
    case Qt::Key_S:
        if (selector.state == 0){
            QString filename = sourcePath + "/" + ui->listWidget->currentItem()->text();
            if (!screen->setup(filename)) return;
            //screen->show();
            QFileInfo basefile(filename);
            if (screen->generateImages(outputImageRepo,basefile.baseName())){
                QMessageBox::information(this,"Done","Image generation completed",QMessageBox::Ok);
            }
        }
        break;
    }

}

void MainWindow::scanPath(){
    ui->listWidget->clear();
    QDir dir(sourcePath);
    if (!dir.exists()) qWarning() << "Source repo does not exist";
    QStringList files = dir.entryList(QStringList(),QDir::Files);
    for (qint32 i = 0; i < files.size(); i++){
        ui->listWidget->addItem(files.at(i));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
