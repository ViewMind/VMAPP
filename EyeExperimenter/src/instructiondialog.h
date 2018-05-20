#ifndef INSTRUCTIONDIALOG_H
#define INSTRUCTIONDIALOG_H

#include <QDialog>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QImage>
#include <QVBoxLayout>
#include <QKeyEvent>

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/Experiments/experimentdatapainter.h"

class InstructionDialog : public QDialog
{
public:
    InstructionDialog(QWidget *parent = 0);
    void setInstruction(const QString &inst);
    QPixmap getPixmap() const;

protected:
    void keyPressEvent(QKeyEvent *e);

private:
    QGraphicsView *gview;
};

#endif // INSTRUCTIONDIALOG_H
