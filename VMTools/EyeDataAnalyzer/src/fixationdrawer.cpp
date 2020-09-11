#include "fixationdrawer.h"

FixationDrawer::FixationDrawer()
{
    manager = nullptr;
}

bool FixationDrawer::prepareToDrawFixations(const QString &expID, ConfigurationManager *c, const QString &expDescription, const QString &outDir){

    if (manager != nullptr)
        delete manager;

    if (expID == CONFIG_P_EXP_READING){
        manager = new ReadingManager();
    }
    else if ((expID == CONFIG_P_EXP_BIDING_BC) || (expID == CONFIG_P_EXP_BIDING_UC)){
        manager = new BindingManager();
    }
    else if (expID == CONFIG_P_EXP_FIELDING){
        manager = new FieldingManager();
    }
    else if (expID == CONFIG_P_EXP_NBACKRT){
        manager = new FieldingManager();
    }
    else if (expID == CONFIG_P_EXP_GONOGO){
        manager = new GoNoGoManager();
    }
    else{
        error = "Unkonwn experiment ID: " + expID;
        return false;
    }

    // Common initialization of the manager
    manager->init(c);
    if (!manager->parseExpConfiguration(expDescription)){
        error = "PARSE ERROR: " + manager->getError();
        return false;
    }

    experimentID = expID;
    outputImageDirectory = outDir;

    return true;

}

bool FixationDrawer::drawFixations(const FixationList &flist){

    if ((experimentID == CONFIG_P_EXP_BIDING_BC) || (experimentID == CONFIG_P_EXP_BIDING_UC)){

        BindingManager *m = (BindingManager *)manager;
        for (qint32 i = 0; i < flist.trialID.size(); i++){
            QString trialName = flist.trialID.at(i).first();
            QString isTrial = flist.trialID.at(i).last();
            if (m->drawFlags(trialName,isTrial == "0")){
                // Constructing the image name
                QString imageName = trialName + "_" + isTrial;
                //... and drawing the fixations on the image.
                drawFixationOnImage(imageName,flist.left.at(i),flist.right.at(i));
            }
        }

    }
    else if (experimentID == CONFIG_P_EXP_READING){
        ReadingManager *m = (ReadingManager *)manager;
        //qWarning() << "Draw fixation on Reading";
        for (qint32 i = 0; i < flist.trialID.size(); i++){
            QString imgID = flist.trialID.at(i).first();
            if (m->drawPhrase(imgID)){
                drawFixationOnImage(imgID,flist.left.at(i),flist.right.at(i));
            }
        }

    }
    else if (experimentID == CONFIG_P_EXP_FIELDING){
        FieldingManager *m = (FieldingManager *)manager;
        m->drawBackground();
        for (qint32 i = 0; i < flist.trialID.size(); i++){

            QString imgID = flist.trialID.at(i).first();
            QString trialID = flist.trialID.at(i).last();

            qint32 imgNumber = imgID.toInt();
            if (imgNumber < 4){
                m->setDrawState(FieldingManager::DS_CROSS_TARGET);
                if (!m->setTargetPositionFromTrialName(trialID,imgNumber-1)){
                    error = "FIELDING: Could not find trial with name " + trialID + " for drawing the fixations";
                    return false;
                }
            }
            else{
                if (imgNumber == 5){
                    m->setDrawState(FieldingManager::DS_3);
                }
                else if (imgNumber == 6){
                    m->setDrawState(FieldingManager::DS_2);
                }
                else{
                    m->setDrawState(FieldingManager::DS_1);
                }
            }
            // Constructing the image name
            QString imageName = trialID + "_" + imgID;
            //... and drawing the fixations on the image.
            drawFixationOnImage(imageName,flist.left.at(i),flist.right.at(i));
        }
    }
    else if (experimentID == CONFIG_P_EXP_NBACKRT){
        FieldingManager *m = (FieldingManager *)manager;
        m->drawBackground();

        for (qint32 i = 0; i < flist.trialID.size(); i++){

            QString imgID = flist.trialID.at(i).first();
            QString trialID = flist.trialID.at(i).last();

            qint32 imgNumber = imgID.toInt();
            if (imgNumber < 4){
                m->setDrawState(FieldingManager::DS_CROSS_TARGET);
                if (!m->setTargetPositionFromTrialName(trialID,imgNumber-1)){
                    error = "FIELDING: Could not find trial with name " + trialID + " for drawing the fixations";
                    return false;
                }
            }
            else{
                m->setDrawState(FieldingManager::DS_TARGET_BOX_ONLY);
            }
            // Constructing the image name
            QString imageName = trialID + "_" + imgID;
            //... and drawing the fixations on the image.
            drawFixationOnImage(imageName,flist.left.at(i),flist.right.at(i));
        }
    }
    else if (experimentID == CONFIG_P_EXP_GONOGO){
        GoNoGoManager *m = (GoNoGoManager *)manager;

        for (qint32 i = 0; i < flist.trialID.size(); i++){

            QString trialID = flist.trialID.at(i).first();

            m->drawTrialByID(trialID);

            // Constructing the image name
            QString imageName = trialID;
            //... and drawing the fixations on the image.
            drawFixationOnImage(imageName,flist.left.at(i),flist.right.at(i));
        }
    }
    else {
        error = "Unknown experiment for which to draw fixations: " + experimentID;
        return false;
    }

    return true;

}


void FixationDrawer::drawFixationOnImage(const QString &outputBaseFileName, const Fixations &l, const Fixations &r){

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
