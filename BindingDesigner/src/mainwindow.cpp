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

    qWarning() << "Me apretaron!!";

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
        qWarning() << "ACA ESTOY!!!!";
        if (selector.state == 0){
            QString filename = sourcePath + "/" + ui->listWidget->currentItem()->text();
            drawPictures(filename);
            QMessageBox::information(this,"Done","Image generation completed",QMessageBox::Ok);

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

void MainWindow::drawPictures(const QString &input){

    BindingManager bmng;
    ConfigurationManager config;
    config.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,768);
    config.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,1366);
    config.addKeyValuePair(CONFIG_XPX_2_MM,0.25);
    config.addKeyValuePair(CONFIG_YPX_2_MM,0.25);
    bmng.init(&config);

    QFileInfo basefile(input);

    QFile file(input);
    file.open(QFile::ReadOnly);
    QTextStream reader(&file);
    QString contents = reader.readAll();
    file.close();
    if (!bmng.parseExpConfiguration(contents)){
        qWarning() << "Error parsing: " << bmng.getError();
        return;
    }

    int currentTrial = -1;
    bool showTypeSlide = true;

    QDir outputDir(outputImageRepo + "/" + basefile.baseName());
    if (outputDir.exists()) outputDir.removeRecursively();
    if (outputDir.exists()){
        qWarning() << "Could not delete the existing directory" << outputDir.absolutePath();
        return;
    }

    QDir baseDir(outputImageRepo);
    if (!baseDir.mkdir(basefile.baseName())) {
        qWarning() << "Could not create the image dir" << outputDir.absolutePath();
        return;
    }

    currentTrial = -1;
    showTypeSlide = true;
    bool done = false;

    while (!done){

        // Advancing the trial
        done = false;
        if (showTypeSlide) {
            if (currentTrial < bmng.size()-1) {
                currentTrial++;
            }
            else {
                done = true;
                return;
            }
        }

        // Drawing the slide
        bmng.drawTrial(currentTrial,showTypeSlide);

        // Adding the identification on top
        QString information = QString::number(currentTrial) + ") " + bmng.getTrial(currentTrial).name;
        if (showTypeSlide) information = information + ". SHOW Slide";
        else information = information + ". TEST Slide";
        QGraphicsTextItem *text = bmng.getCanvas()->addText(information);
        text->setPos(20,0);

        // Changing the type.
        showTypeSlide = !showTypeSlide;

        // Saving the file
        QPixmap pixmap = bmng.getImage();
        QString filename = outputDir.absolutePath() + "/" + bmng.getTrial(currentTrial).name;
        if (showTypeSlide) filename = filename + "_show.png";
        else filename = filename + "_test.png";
        if (!pixmap.save(filename)){
            qWarning() << "Could not save" << filename;
            return;
        }
    }



}

MainWindow::~MainWindow()
{
    delete ui;
}
