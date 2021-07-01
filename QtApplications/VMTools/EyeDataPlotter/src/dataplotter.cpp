#include "dataplotter.h"

DataPlotter::DataPlotter()
{

}

void DataPlotter::plotPoints(const QString &outputFileName, const DataExtractor::DataPoints &datapoints){

    qreal W = datapoints.width;
    qreal H = datapoints.height;
    qreal R = 0.007*W;
    QImage image(W,H,QImage::Format_RGB888);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor transparent(0,0,0,0);

    painter.setBrush(QBrush(transparent));

    for (qint32 j = 0; j < datapoints.datapoints.size(); j++){

        DataExtractor::DataMatrix dp = datapoints.datapoints.at(j);

        for (qint32 i = 0; i < dp.size(); i++){

            if (dp.at(i).size() != 4) continue;

            QPointF pR(dp.at(i).at(0),dp.at(i).at(1));
            QPointF pL(dp.at(i).at(2),dp.at(i).at(3));

            // Drawing the dot
            painter.setPen(QPen(Qt::blue));
            painter.drawEllipse(pR,R,R);
            painter.setPen(QPen(Qt::red));
            painter.drawEllipse(pL,R,R);

        }

    }

    image.save(outputFileName + ".png");

}
