#ifndef SHOWSCREEN_H
#define SHOWSCREEN_H

#include <QWidget>
#include <QKeyEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QDebug>
#include <QMessageBox>
#include "../../CommonClasses/Experiments/bindingmanager.h"

class ShowScreen: public QWidget
{
    Q_OBJECT
public:
    ShowScreen(QWidget *parent = 0);
    bool setup(const QString &filename);
    bool generateImages(const QString &base, const QString &imageDir);

protected:

    // Controls
    void keyPressEvent(QKeyEvent *event);

private:

    // The pointer to the GraphicsView where the drawing and showing will take place.
    QGraphicsView *gview;

    ConfigurationManager config;
    BindingManager manager;
    qint32 currentTrial;
    bool showTypeSlide;

    bool done;

    void next();
    void previous();
    void draw();

};

#endif // SHOWSCREEN_H
