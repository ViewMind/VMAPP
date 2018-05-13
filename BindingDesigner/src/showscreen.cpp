#include "showscreen.h"

ShowScreen::ShowScreen(QWidget *parent) : QWidget(parent)
{
    // Making this window frameless and making sure it stays on top.
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::X11BypassWindowManagerHint|Qt::Window);
    this->setGeometry(0,0,SCREEN_W,SCREEN_H);

    // Creating a graphics widget and adding it to the layout
    gview = new QGraphicsView(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(gview);

    gview->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    gview->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    config.addKeyValuePair(CONFIG_XPX_2_MM,0.25);
    config.addKeyValuePair(CONFIG_YPX_2_MM,0.25);
    manager.init(&config);
    gview->setScene(manager.getCanvas());

}


bool ShowScreen::setup(const QString &filename){
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)){
        qWarning() << "Could not read the file";
        return false;
    }
    QTextStream reader(&file);
    QString contents = reader.readAll();
    if (!manager.parseExpConfiguration(contents)){
        qWarning() << "Error parsing: " << manager.getError();
        return false;
    }
    currentTrial = -1;
    showTypeSlide = true;
    next();
    return true;
}

void ShowScreen::next(){
    done = false;
    if (showTypeSlide) {
        if (currentTrial < manager.getNumberOfTrials()-1) {
            currentTrial++;
        }
        else {
            done = true;
            return;
        }
    }
    draw();
    showTypeSlide = !showTypeSlide;
}

void ShowScreen::previous(){
    if (!showTypeSlide) {
        if (currentTrial > 0) currentTrial--;
        else return;
    }
    draw();
    showTypeSlide = !showTypeSlide;
}

void ShowScreen::draw(){
    manager.drawTrial(currentTrial,showTypeSlide);

    // Adding the identification on top
    QString information = QString::number(currentTrial) + ") " + manager.getTrial(currentTrial).name;
    if (showTypeSlide) information = information + ". SHOW Slide";
    else information = information + ". TEST Slide";
    QGraphicsTextItem *text = manager.getCanvas()->addText(information);
    text->setPos(20,0);
}

void ShowScreen::keyPressEvent(QKeyEvent *event){

    switch (event->key()){
    case Qt::Key_D:
        next();
        break;
    case Qt::Key_A:
        previous();
        break;
    case Qt::Key_Escape:
        this->hide();
        break;
    }

}

bool ShowScreen::generateImages(const QString &base, const QString &imageDir){
    QDir outputDir(base + "/" + imageDir);
    if (outputDir.exists()) outputDir.removeRecursively();
    if (outputDir.exists()){
        qWarning() << "Could not delete the existing directory" << outputDir.absolutePath();
        return false;
    }

    QDir baseDir(base);
    if (!baseDir.mkdir(imageDir)) {
        qWarning() << "Could not create the image dir" << outputDir.absolutePath();
        return false;
    }

    currentTrial = -1;
    showTypeSlide = true;
    done = false;

    while (!done){
        next();
        QPixmap pixmap = manager.getImage();
        QString filename = outputDir.absolutePath() + "/" + manager.getTrial(currentTrial).name;
        if (showTypeSlide) filename = filename + "_show.png";
        else filename = filename + "_test.png";
        if (!pixmap.save(filename)){
            qWarning() << "Could not save" << filename;
            return false;
        }
    }

    return true;

}
