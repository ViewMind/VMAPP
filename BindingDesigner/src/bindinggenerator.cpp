#include "bindinggenerator.h"
#include "ui_bindinggenerator.h"

static const QString REPO_IMAGE = "../outputs/images";
static const QString REPO_TEXTS = "../outputs/texts";

BindingGenerator::BindingGenerator(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::BindingGenerator)
{
    ui->setupUi(this);
    scanPath();
}

BindingGenerator::~BindingGenerator()
{
    delete ui;
}

void BindingGenerator::on_pbGenFile_clicked()
{
    bool bc;
    bc = (ui->cbBindingType->currentText() == "BC");
    qint32 ntargets = ui->cbNumTargets->currentIndex()+2;
    QString filename = REPO_TEXTS + "/" + ui->cbBindingType->currentText() + "_" + ui->cbNumTargets->currentText() + ".dat";

    BindingFileGenerator bfg;
    bfg.generateFile(filename,bc,false,ntargets);

    scanPath();

    QMessageBox::information(this,"Done","Generated file: " + filename,QMessageBox::Ok);
}

void BindingGenerator::on_pbGenImages_clicked()
{
    QString filename = REPO_TEXTS + "/" + ui->listWidget->currentItem()->text();
    drawPictures(filename);
    QMessageBox::information(this,"Done","Image generation completed",QMessageBox::Ok);
}


void BindingGenerator::scanPath(){
    ui->listWidget->clear();
    QDir dir(REPO_TEXTS);
    if (!dir.exists()) qWarning() << "Source repo does not exist";
    QStringList files = dir.entryList(QStringList(),QDir::Files);
    for (qint32 i = 0; i < files.size(); i++){
        ui->listWidget->addItem(files.at(i));
    }
}

void BindingGenerator::drawPictures(const QString &input){

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

    QDir outputDir(REPO_IMAGE + "/" + basefile.baseName());
    if (outputDir.exists()) outputDir.removeRecursively();
    if (outputDir.exists()){
        qWarning() << "Could not delete the existing directory" << outputDir.absolutePath();
        return;
    }

    QDir baseDir(REPO_IMAGE);
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

        // Getting the file name before changing the showSlideType.
        QString filename = outputDir.absolutePath() + "/" + bmng.getTrial(currentTrial).name;
        if (showTypeSlide) filename = filename + "_show.png";
        else filename = filename + "_test.png";

        // Changing the type.
        showTypeSlide = !showTypeSlide;

        // Saving the file
        QPixmap pixmap = bmng.getImage();
        if (!pixmap.save(filename)){
            qWarning() << "Could not save" << filename;
            return;
        }
    }

}
