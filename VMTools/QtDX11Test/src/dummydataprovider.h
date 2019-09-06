#ifndef DUMMYDATAPROVIDER_H
#define DUMMYDATAPROVIDER_H

#include <QObject>
#include <QApplication>
#include <QTimer>
#include <QScreen>
#include <QDebug>

class DummyDataProvider : public QObject
{
    Q_OBJECT
public:
    DummyDataProvider();
    void start();
    void stop();

signals:
    void newPositionData(qint32 rx, qint32 ry, qint32 lx, qint32 ly);

private slots:
    void on_timeOut();

private:
    QTimer sampleTimer;

};

#endif // DUMMYDATAPROVIDER_H
