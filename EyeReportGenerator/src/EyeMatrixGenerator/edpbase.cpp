#include "edpbase.h"

EDPBase::EDPBase(ConfigurationManager *c)
{
    config = c;
    error = "";
    skipImageGeneration = false;
    mwa.parameters.maxDispersion = 30;
    mwa.parameters.minimumFixationLength = 50;
    mwa.parameters.sampleFrequency = 120;
}

bool EDPBase::configure(const QString &fileName, const QString &managerConfigData){

    QFileInfo info (fileName);
    workingDirectory = info.absolutePath();
    QFileInfo pname(workingDirectory);

    // Creating the image output directory if it doesn't exist
    outputImageDirectory = workingDirectory + "/" + info.baseName();
    QDir dir(workingDirectory);
    dir.mkdir(outputImageDirectory);
    subjectIdentifier = pname.baseName();
    outputFile = workingDirectory + "/" + info.baseName() + ".csv";

    if (!manager->parseExpConfiguration(managerConfigData)){
        error = manager->getError();
        return  false;
    }

    // Used for the progress bar.
    numberToProcess = manager->sizeToProcess();

    return true;
}

/************************** Auxiliary functions*******************************/
qreal EDPBase::averageColumnOfMatrix(const DataMatrix &data, qint32 col,
                                              qint32 startRow, qint32 endRow){
    qreal avg = 0;
    for (qint32 i = startRow; i <= endRow; i++){
        avg = avg + data.at(i).at(col);
    }
    qreal size = endRow - startRow + 1;
    return avg/size;

}

qint32 EDPBase::countZeros(const DataMatrix &data,
                                    qint32 col,
                                    qint32 startRow,
                                    qint32 endRow,qreal tol) const{

    qint32 counter = 0;
    for (qint32 i = startRow; i <= endRow; i++){
        if (data.at(i).at(col) <= tol) counter++;
    }
    return 0;

}

void EDPBase::drawFixationOnImage(const QString &outputBaseFileName,
                                  const Fixations &l,
                                  const Fixations &r){

    // Doing checks: Output image directory must exist otherwise nothing is done.
    QDir dirCheck(outputImageDirectory);
    if (!dirCheck.exists()) return;

    // Drawing the graphics scene onto the painter.
    QImage image(manager->getCanvas()->width(),manager->getCanvas()->height(),QImage::Format_RGB888);
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    manager->getCanvas()->render(&painter);

    // Adding the fixations on top of it.
    // Doing the plotting for the left eye.
    QFont font("Mono",12);
    qreal R = 0.01*image.width();
    qreal lineH = 0.1*image.height();

    QColor transparent(0,0,0,0);

    painter.setFont(font);
    painter.setBrush(QBrush(transparent));

    for (qint32 i = 0; i < l.size(); i++){
        QPointF p(l.at(i).x,l.at(i).y);

        // Drawing the dot
        painter.setPen(QPen(Qt::blue));
        painter.drawEllipse(p,R,R);

        // Drawing a line
        painter.drawLine(p.x(),p.y(),p.x(),p.y()-lineH);

        // Drawing the text.
        painter.drawText(p.x(),p.y()-lineH,2*R,2*R,Qt::AlignCenter,QString::number(i+1));

        // Duration position.
        // Duration position.
        painter.save();
        painter.translate(p.x()-10,p.y()-2*lineH);
        painter.rotate(-90);
        painter.drawText(0,0,4*R,2*R,Qt::AlignCenter,QString::number(l.at(i).duration));
        painter.restore();

    }

    for (qint32 i = 0; i < r.size(); i++){
        QPointF p(r.at(i).x,r.at(i).y);

        // Drawing the dot
        painter.setPen(QPen(Qt::red));
        painter.drawEllipse(p,R,R);

        // Drawing a line
        painter.drawLine(p.x(),p.y(),p.x(),p.y()+lineH);

        // Drawing the text.
        painter.drawText(p.x(),p.y()+lineH,2*R,2*R,Qt::AlignCenter,QString::number(i+1));

        // Duration position.
        painter.save();
        painter.translate(p.x()-10,p.y()+2*lineH);
        painter.rotate(-90);
        painter.drawText(0,0,4*R,2*R,Qt::AlignCenter,QString::number(r.at(i).duration));
        painter.restore();

    }

    if (image.isNull()) return;

    //qWarning() << "Saving to" << outputImageDirectory + "/" + outputBaseFileName + ".jpg";

    image.save(outputImageDirectory + "/" + outputBaseFileName + ".jpg");

}


