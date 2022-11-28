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
#include <QTimer>

#include "renderservercircleitem.h"
#include "renderserverrectitem.h"
#include "renderserverlineitem.h"
#include "renderservertriangleitem.h"
#include "renderserverarrowitem.h"
#include "renderservertextitem.h"
#include "renderservertriangleitem.h"
#include "renderserverimageitem.h"
#include "renderserveritemgroup.h"

class RenderServerScene
{

public:

    RenderServerScene(double x, double y, double width, double height);    
    RenderServerScene(const RenderServerScene &anotherScene);
    RenderServerScene();
    virtual ~RenderServerScene();

    void copyFrom(const RenderServerScene &scene);
    bool isValid() const;

    void setSceneRect(const QRectF &rect);
    void setSceneRect(qreal x, qreal y, qreal w, qreal h);

    RenderServerCircleItem * addEllipse(QRectF brect, const QPen &pen = QPen(), const QBrush &brush = QBrush());
    RenderServerCircleItem * addEllipse(qreal x, qreal y, qreal w, qreal h, const QPen &pen = QPen(), const QBrush &brush = QBrush());
    RenderServerRectItem * addRect(qreal x, qreal y, qreal w, qreal h, const QPen &pen = QPen(), const QBrush &brush = QBrush());
    RenderServerRectItem * addRect(const QRectF &rect);
    RenderServerTextItem * addText(const QString &text, const QFont &font = QFont());
    RenderServerTextItem * addSimpleText(const QString &text, const QFont &font = QFont());
    RenderServerLineItem * addLine(const QLineF, const QPen &pen = QPen());
    RenderServerLineItem * addLine(qreal x1, qreal y1, qreal x2, qreal y2, const QPen &pen = QPen());
    RenderServerArrowItem * addArrow(qreal x1, qreal y1, qreal x2, qreal y2, qreal hH, qreal hL, const QColor &color);
    RenderServerTriangleItem *addTriangle(const QPolygonF &triangle);
    RenderServerImageItem * addImage(const QString &fname, bool fitToWidth, qreal w, qreal h, qreal value_to_fit);
    RenderServerItemGroup * createItemGroup (const QList<RenderServerItem*> items);
    RenderServerPacket render() const;

    // Animation functions.
    void clearAnimationData();
    RenderServerItem * getItemByID(const qint32 &id) const;
    QMap<QString, qint32> animate();

    void setBackgroundBrush(const QBrush &brush);
    QString getBackgroundColorName() const;

    qreal width() const;
    qreal height() const;

    void clear();

    QList<RenderServerItem *> getItemList() const;

private:
    qreal sceneWidth;
    qreal sceneHeight;
    QString backgroundColor;
    QList <RenderServerItem*> itemsInScene;

    /**
     * @brief getRenderServerOrder - Sets the render order by considering the z value of each item. Filters all non visible items.
     * @details All visible items (z > 0) are ordered by their z values from higher to lower using bubblesort.
     */
    QList<RenderServerItem*> getRenderServerOrder() const;
};

#endif // RENDERSERVERSCENE_H
