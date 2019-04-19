#include "bargrapher.h"

const qreal BarGrapher::PROYECTION_FACTOR = 0.707106;
const qreal BarGrapher::LEFT_MARGIN  = 0.1;
const qreal BarGrapher::RIGHT_MARGIN = 0.04;
const qreal BarGrapher::TOP_MARGIN   = 0.08;

BarGrapher::BarGrapher()
{

}


void BarGrapher::configureGraphText(qreal width, qreal height, const QStringList &xItems, const QString &ylabel, const QString &title){
    graphTitle = title;
    W = width;
    H = height;
    itemDescriptions = xItems;
    yAxisText = ylabel;
}


QImage BarGrapher::generateGraph(const DataSet &dataSet){

    QImage graph(W,H,QImage::Format_RGB888);
    error = "";

    // Making sure that the data is coherent.
    if (dataSet.isEmpty()){
        error = "No data was presented";
        return graph;
    }
    if (dataSet.size() > 2){
        error = "Cannot have more than two data sets, passed " + QString::number(dataSet.size());
        return graph;
    }

    qint32 numItems = dataSet.first().size();
    for (qint32 i = 0; i < dataSet.size(); i++){
        if (numItems != dataSet.at(i).size()){
            error = "All data sets must be the same size: " + QString::number(numItems) + " however data set " + QString::number(i) + " has " + QString::number(dataSet.at(i).size());
            return graph;
        }
    }

    if (!itemDescriptions.isEmpty()){
        if (numItems != itemDescriptions.size()){
            error = "The number of item descriptios (" + QString::number(itemDescriptions.size()) + ") must coincide with the numer of points in the data set (" + QString::number(numItems) + ")";
            return graph;
        }
    }
    else{
        for (qint32 i = 0; i < numItems; i++){
            itemDescriptions << QString::number(i);
        }
    }

    // The font to be used
    QFont textFont("Mono");
    textFont.setPixelSize(10);
    //QFont titleFont("Mono",16,QFont::Bold);

    // Calculating margins to get the effective graph area.
    qreal XAxisLength = (1.0 - LEFT_MARGIN - RIGHT_MARGIN)*W;
    qreal DW = XAxisLength/(qreal)(numItems);

    // Calculating the effective graph area due to the text items.
    qreal Th = 0.1*H;
    qreal GH = (1.0-TOP_MARGIN)*H - Th;
    qreal xOffset = LEFT_MARGIN*W;
    qreal yOffset = TOP_MARGIN*H;

    // --------------- Commence drawing -----------------------
    QPainter painter(&graph);

    // The background
    painter.setBrush(QBrush(QColor(226,226,226)));
    painter.drawRect(0,0,W,H);

    // Drawing the axis;
    QPen pen;
    pen.setColor(QColor(0,0,0));
    pen.setWidth(2);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::black));

    painter.drawLine(xOffset,yOffset+GH,xOffset + XAxisLength,yOffset+GH);
    painter.drawLine(xOffset,yOffset,xOffset,yOffset+GH);

    // Drawing the X Axis text
    painter.setFont(textFont);
    for (qint32 i = 0; i < numItems; i++){
        QRectF rectF(xOffset + i*DW,yOffset+GH,DW,Th);
        painter.drawText(rectF,itemDescriptions.at(i));
    }

    return graph;
}


QImage BarGrapher::mergeImagesInGrid(const QList<QList<QImage> > &graphs){
    Q_UNUSED(graphs)
    QImage res;
    return res;
}
