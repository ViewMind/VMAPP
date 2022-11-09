#ifndef RENDERSERVERSCENE_H
#define RENDERSERVERSCENE_H

/**
 * @brief The RenderServerScene class
 * The idea for the class is to provide all the interfaces of QGraphicsScene just as is, but rendering really only creates the
 * proper render package to send to the remote render server.
 * However all interfaces are maintained the same, even when some parameters are not used.
 * This is done to minimize code change.
 */

#include <QString>
#include <QList>
#include <QFont>

#include "renderservercircleitem.h"
#include "renderserverrectitem.h"
#include "renderserverlineitem.h"
#include "renderservertriangleitem.h"
#include "renderserverarrowitem.h"
#include "renderservertextitem.h"

class RenderServerScene
{
public:

    RenderServerScene(double x, double y, double width, double height);

    RenderServerCircleItem * addEllipse(qreal x, qreal y, qreal w, qreal h, const QPen &pen = QPen(), const QBrush &brush = QBrush());
    RenderServerRectItem * addRect(qreal x, qreal y, qreal w, qreal h, const QPen &pen = QPen(), const QBrush &brush = QBrush());
    RenderServerTextItem * addText(const QString &text, const QFont &font = QFont());
    RenderServerTextItem * addSimpleText(const QString &text, const QFont &font = QFont());
    RenderServerLineItem * addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen = QPen());
    RenderServerArrowItem * addArrow(qreal x1, qreal y1, qreal x2, qreal y2, qreal hH, qreal hL, const QColor &color);
    RenderServerPacket render() const;

    qreal width() const;
    qreal height() const;

    void clear();

private:
    qreal sceneWidth;
    qreal sceneHeight;
    QString backgroundColor;
    QList <RenderServerItem*> itemsInScene;
};

#endif // RENDERSERVERSCENE_H
