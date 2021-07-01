#ifndef EYESELECTOR_H
#define EYESELECTOR_H

#include <QRectF>
#include <QList>
#include <QPoint>
#include <QPointF>
#include <QDebug>


class EyeSelector
{
public:

    // Continuous behaviours means that the targetDataPoints inside a targetBox must be continuos, If there is even one data point outside of it, the state machine resets.
    // In Cumulative mode, the state machine ONLY resets when the data point is inside another target other than the current target.
    typedef enum {EBS_CONTINOUS, EBS_CUMULATIVE} EyeSelectorBehaviour;

    EyeSelector();

    void setTargetCountForSelection(qint32 tcs);
    void setTargetBoxes(QList<QRectF> tboxes);
    void setBehaviour(EyeSelectorBehaviour esb);

    // Will return the index of the Target Box that was selected, having made it through the state machine.
    // -1 otherwise. Once a selection is maded the state machine is reset.
    qint32 isSelectionMade(qreal x, qreal y);

    // Overload functions for convenience.
    qint32 isSelectionMade(QPoint point);
    qint32 isSelectionMade(QPointF point);

    void reset();


private:

    qint32 targetCountForSelection;
    qint32 pointCounter;
    qint32 currentSelection;
    QList<QRectF> targetBoxes;
    EyeSelectorBehaviour eyeSelectorBehaviour;

};

#endif // EYESELECTOR_H
