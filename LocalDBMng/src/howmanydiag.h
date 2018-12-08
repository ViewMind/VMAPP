#ifndef HOWMANYDIAG_H
#define HOWMANYDIAG_H

#include <QDialog>

namespace Ui {
class HowManyDiag;
}

class HowManyDiag : public QDialog
{
    Q_OBJECT

public:
    explicit HowManyDiag(QWidget *parent = 0);
    qint32 getHowMany() const {return howMany;}
    ~HowManyDiag();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::HowManyDiag *ui;
    qint32 howMany;
};

#endif // HOWMANYDIAG_H
