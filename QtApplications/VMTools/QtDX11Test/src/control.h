#ifndef CONTROL_H
#define CONTROL_H

#include <QObject>
#include <QDebug>

#include "d3dmanager.h"
#include "targettest.h"
#include "dummydataprovider.h"

class Control : public QObject
{
    Q_OBJECT
public:
    explicit Control(QObject *parent = nullptr);
    ~Control();

    Q_INVOKABLE void startTest();
    Q_INVOKABLE void stopTest();

signals:

public slots:

    void newPositionData(int rx, int ry, int lx, int ly);

private:

    D3DManager *d3dManager;
    TargetTest targetTest;
    DummyDataProvider mouseTracker;

};

#endif // CONTROL_H
