#include "animationmanager.h"

AnimationManager::AnimationManager(QObject *parent) : QObject(parent)
{
    connect(&animationTimer,&QTimer::timeout,this,&AnimationManager::onAnimationTimeOut);
    noErrorsWhenAddingVariables = true;
}

QString AnimationManager::getError() const {
    return this->error;
}

void AnimationManager::onAnimationTimeOut(){

    QMap<QString,qint32> animationSignals = animationImage.animate();
    //QString toDbug = "2-R";

    QStringList endSignals;
    QStringList variableList = animationSignals.keys();
    for (qint32 j = 0; j < variableList.size(); j++){
        QString var = variableList.at(j);
        qint32  signal = animationSignals.value(var);
        if (signal == RenderServerItem::ANIMATION_END){
            endSignals << var;
        }
        else {
            // If notification were enabled a signal must be emitted. END signals are handled by the rest of the animation manager.
            if (animationPaths.contains(var)){
                qint32 index = animationPaths.value(var).animationIndex;
                qint32 nindex = animationPaths.value(var).path.at(index).notificationIndex;
                if (nindex != -1){
                    emit AnimationManager::reachedAnimationStop(var,nindex,signal);
                }
            }
        }
    }

    if (!endSignals.empty()){

        //qDebug() << "Got the end signals for " << endSignals;

        // We need to notify of the proper endpoints.
        for (qint32 i = 0; i < endSignals.size(); i++){

            QString variable = endSignals.at(i);
            QStringList parts = variable.split("-");
            qint32 itemID = parts.first().toInt();
            QString parameters = parts.last();

            if (!animationPaths.contains(variable)) continue;

            AnimationPath path = animationPaths.value(variable);

            qreal start = 0;
            qreal end = 0;
            qreal duration = 0;
            qreal delay = 0;

            qint32 animationPointToNotify = path.path.at(path.animationIndex).notificationIndex;

            if (path.animationInReverse){
                path.animationIndex--;
                if (path.animationIndex == -1){
                    path.animationIndex = 0;
                    path.animationInReverse = false;
                }
            }
            else {
                path.animationIndex++;
                if (path.animationIndex == path.path.size()){ // We need to go back.
                    path.animationIndex = path.animationIndex - 1;
                    path.animationInReverse = true;
                }
            }

            AnimationPathPoint app = path.path.at(path.animationIndex);
            if (path.animationInReverse){
                start = app.end;
                end   = app.start;
            }
            else {
                start = app.start;
                end   = app.end;
            }
            duration = app.duration;
            delay = app.delay;
            animationPaths[variable] = path;

            // If the notification is enabled for this point we send it out.
            if (animationPointToNotify != -1) {
                emit AnimationManager::reachedAnimationStop(variable,animationPointToNotify,RenderServerItem::ANIMATION_END);
            }

            RenderServerItem *item = animationImage.getItemByID(itemID);
            //if (variable == toDbug) qDebug() << "New animation point for item " << itemID << "PARAMETER" << parameters << " FROM " << start << "TO" << end << "LASTING" << duration << ". Current Item Index" << path.animationIndex << " Is In Reverse " << path.animationInReverse;
            item->addAnimationParameter(parameters,start,end,duration,ANIMATION_INTERVAL,delay);

        }
    }

    emit AnimationManager::animationUpdated();

}

void AnimationManager::setScene(const RenderServerScene &scene){
    animationImage.copyFrom(scene);
    animationImage.clearAnimationData();
    animationPaths.clear();
    animationTimer.stop();
    noErrorsWhenAddingVariables = true;
}


bool AnimationManager::addVariableStop(const qint32 &itemID, const QString &parameter, qreal value, qreal duration , bool notify, qreal delay){

    if (!noErrorsWhenAddingVariables) return false; // If there was an error we do nothing else.

    QString variable = QString::number(itemID) + "-" + parameter;

    // We can only add variables to the scene once we have at least two.
    if (!animationPaths.contains(variable)){
        AnimationPath path = newAnimationPath();
        path.durations << 0;
        path.values << value;
        path.notifications << notify;
        path.delays << delay;
        animationPaths[variable] = path;
        return true;
    }

    //if (dela)

    // We already have at least one point.
    AnimationPath path = animationPaths.value(variable);
    path.durations << duration;
    path.values << value;
    path.notifications << notify;
    path.delays << delay;

    qreal start = path.values.at( path.values.size()-2);
    qreal end   = path.values.last();

    RenderServerItem *item = animationImage.getItemByID(itemID);
    if (item == nullptr){
        error = "Tried to add animation step to non existent item '"  + QString::number(itemID) + "'";
        return false;
    }

    QString e = item->addAnimationParameter(parameter,start,end,duration,ANIMATION_INTERVAL,delay);
    if (e != ""){
        error = "Failed in adding variable stop '" + variable + "' to item " + QString::number(itemID) + ". Reason: " + e;
        return false;
    }

    animationPaths[variable] = path;
    return true;

}

RenderServerPacket AnimationManager::getCurrentFrame() const {
    return animationImage.render();
}

RenderServerScene AnimationManager::getCurrentFrameAsScene() const {
    return animationImage;
}

RenderServerItem *  AnimationManager::getItemByID(qint32 id) {
    return animationImage.getItemByID(id);
}

void AnimationManager::startAnimation(){

    QStringList keys = animationPaths.keys();
    animationImage.clearAnimationData();
    makeAnimationPaths();

    // Notify Start
    QMap<QString,qint32> variablesToNotifyAtStart;

    for (qint32 i = 0; i < keys.size(); i++){

        QString variable = keys.at(i);
        QStringList parts = variable.split("-");
        qint32 itemID = parts.first().toInt();
        QString parameters = parts.last();

        AnimationPath path = animationPaths.value(variable);
        AnimationPathPoint app = path.path.first();
        qreal start = app.start;
        qreal end   = app.end;
        qreal d     = app.duration;
        qreal delay = app.delay;

        RenderServerItem *item = animationImage.getItemByID(itemID);
        //qDebug() << "Animation start for " << itemID << "PARAMETER" << parameters << " FROM " << start << "TO" << end << "LASTING" << d;
        item->addAnimationParameter(parameters,start,end,d,ANIMATION_INTERVAL,delay);

        if (app.notificationIndex != -1){
            if (app.delay > 0){
                variablesToNotifyAtStart[variable] = RenderServerItem::ANIMATION_BEGIN_DELAY;
            }
            else {
                variablesToNotifyAtStart[variable] = RenderServerItem::ANIMATION_BEGIN_NO_DELAY;
            }
        }

    }

    QStringList toNotifyNames = variablesToNotifyAtStart.keys();
    for (qint32 i = 0; i < toNotifyNames.size(); i++){
        QString varName = toNotifyNames.at(i);
        emit AnimationManager::reachedAnimationStop(varName,0,variablesToNotifyAtStart.value(varName));
    }

    animationImage.animate();
    animationTimer.start(ANIMATION_INTERVAL);
    emit AnimationManager::animationUpdated();

}

void AnimationManager::makeAnimationPaths() {

    QStringList keys = animationPaths.keys();

    for (qint32 j = 0; j < keys.size(); j++){
        QString variable = keys.at(j);

        AnimationPath path = animationPaths.value(variable);
        path.animationInReverse = false;
        path.animationIndex = 0;
        path.path.clear();

        for (qint32 i = 0; i < path.values.size()-1; i++){
            AnimationPathPoint app;
            app.duration = path.durations.at(i+1);
            app.end   = path.values.at(i+1);
            app.start = path.values.at(i);
            app.delay = path.delays.at(i+1);
            app.notificationIndex = -1;

            //qDebug() << "VARIABLE" << variable << j  << path.notifications;

            if (path.notifications.at(i)){
                app.notificationIndex = i;
            }

            path.path << app;
        }

        animationPaths[variable] = path;

    }

}

void AnimationManager::stopAnimation(){
    animationTimer.stop();
}

AnimationManager::AnimationPath AnimationManager::newAnimationPath() const {
    AnimationPath path;
    path.animationIndex = -1;
    path.animationInReverse = false;
    return path;
}

