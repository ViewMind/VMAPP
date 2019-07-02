#ifndef BINDINGFIXATIONDRAWER_H
#define BINDINGFIXATIONDRAWER_H

#include "../../CommonClasses/common.h"
#include "../../CommonClasses/Experiments/bindingmanager.h"
#include <QThread>
#include <QDebug>

#define RESOLUTION_H  768
#define RESOLUTION_W  1366

#define COL_TRIAL_NAME 4
#define COL_EYE        8
#define COL_X          14
#define COL_Y          15
#define COL_BC_OR_UC   16
#define COL_CONDITION  17
#define COL_TYPE       18
#define COL_COUNT      19
#define COL_TYPE_REC   "RECOGNITION"
#define COL_TYPE_ENC   "ENCODING"
#define COL_COND_CONTROL   "CONTROL"
#define COL_COND_DCL       "DCL"


class BindingFixationDrawer: public QThread
{
    Q_OBJECT

public:
    BindingFixationDrawer();
    void setCSVFile(const QString &csv) {csvfilename = csv;}
    void run();

signals:
    void updatePercent(qint32 percent);

private:
    QString csvfilename;
    struct Point {
        qreal x;
        qreal y;
        QString condition;
        bool show;
        bool isbc;
        QString trialname;
    };

    void drawPointOnCanvas(QPainter *painter, QList<Point> points, qreal R);


};

#endif // BINDINGFIXATIONDRAWER_H
