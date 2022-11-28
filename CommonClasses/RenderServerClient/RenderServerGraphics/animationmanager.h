#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include <QObject>
#include <QTimer>
#include "renderserverscene.h"
#include "renderserveritem.h"

class AnimationManager : public QObject
{
    Q_OBJECT
public:
    explicit AnimationManager(QObject *parent = nullptr);

    /**
     * @brief setScene - Sets the image to animate and clear all animation variables.
     * @param scene - The scene to animate.
     */
    void setScene(const RenderServerScene &scene);

    /**
     * @brief addVariableStop - Adds a point along the path that for the animation of a variable in an object.
     * @details The animation works by moving variables from one value to another, of a given render server item, at fixed time intervals.
     * This allows to add stops one by one in order to craft any paths. At any point when the variable reaches the point, if the notify flag
     * was set true, a signal is emitted. In order for a value to be animated, at least two points need to be added.
     * @param itemID - The unique index for the item.
     * @param variable - The variable to animate.
     * @param value - The value of the variable in the path.
     * @param duration - How long does it take (in ms) to go from the previous value to the one added now. The very first value for the variable is ignored.
     * @param notify - Whether a signal is emitted when the variable reaches the end of this animation point.
     * @param delay - Constant time to wait before starting the animation of the next interval.
     * @return
     */
    bool addVariableStop(const qint32 &itemID, const QString &parameter, qreal value, qreal duration, bool notify, qreal delay = 0);

    void startAnimation();
    void stopAnimation();

    RenderServerPacket getCurrentFrame() const;
    RenderServerScene getCurrentFrameAsScene() const;
    RenderServerItem * getItemByID(qint32 id);

    QString getError() const;

signals:
    void animationUpdated();
    void reachedAnimationStop(const QString &variable, qint32 animationStopIndex, qint32 animationSignalType);

private slots:
    void onAnimationTimeOut();

private:

    static const qint32 ANIMATION_INTERVAL = 23; // milliseconds.

    typedef struct {
        qreal start;
        qreal end;
        qreal duration;
        qreal delay;
        qint32 notificationIndex;
    } AnimationPathPoint;

    typedef struct {
        QList<AnimationPathPoint> path;
        QList<qreal> values;
        QList<qreal> durations;
        QList<qreal> delays;
        QList<bool>  notifications;
        qint32 animationIndex;
        bool   animationInReverse;
    } AnimationPath;

    RenderServerScene animationImage;

    QMap< QString,AnimationPath > animationPaths;

    QTimer animationTimer;
    QString error;
    bool noErrorsWhenAddingVariables;

    AnimationPath newAnimationPath() const;

    void makeAnimationPaths();

};

#endif // ANIMATIONMANAGER_H
