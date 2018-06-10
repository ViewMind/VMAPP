#ifndef EVENTDETECTION_H
#define EVENTDETECTION_H

#include <QWidget>
#include <QDebug>
#include <QPushButton>

class EventDetection : public QWidget
{
    Q_OBJECT
public:
    explicit EventDetection(QWidget *parent = nullptr);
    void configure(qint32 x, qint32 y, qreal w, qreal h, const QString &message);

signals:
    void buttonPressed();

public slots:
    void onButtonPressed();

private:
    QPushButton *btnDone;

};

#endif // EVENTDETECTION_H
