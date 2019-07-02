#include "bindingfixationdrawer.h"

BindingFixationDrawer::BindingFixationDrawer()
{

}

void BindingFixationDrawer::run(){

    ConfigurationManager config;
    config.addKeyValuePair(CONFIG_RESOLUTION_WIDTH,RESOLUTION_W);
    config.addKeyValuePair(CONFIG_RESOLUTION_HEIGHT,RESOLUTION_H);
    config.addKeyValuePair(CONFIG_XPX_2_MM,0.25);
    config.addKeyValuePair(CONFIG_YPX_2_MM,0.25);

    QString outputdir = "outputs";
    QDir odir(outputdir);
    if (odir.exists()) odir.removeRecursively();
    QDir(".").mkdir(outputdir);

    QDir odir2(outputdir);
    if (!odir2.exists()){
        qWarning() << "Could not create output directory";
        return;
    }

    BindingManager bc;
    BindingManager uc;

    bc.init(&config);
    uc.init(&config);

    // ================== Loading bc description
    QFile bcfile(":/EyeExperimenter/src/experiment_data/bc.dat");
    if (!bcfile.open(QFile::ReadOnly)){
        qWarning() << "Could not open BC File";
        return;
    }
    QTextStream bcreader(&bcfile);
    QString bcexp = bcreader.readAll();
    bcfile.close();

    if (!bc.parseExpConfiguration(bcexp)){
        qWarning() << "Error parsing BC file: " << bc.getError();
        return;
    }

    QStringList bcnamelist;
    for (qint32 i = 0; i < bc.getExpectedIDs().size(); i++){
        QString id = bc.getExpectedIDs().at(i).first();
        if (!bcnamelist.contains(id)) bcnamelist << id;
    }

    // ================== Loading uc description
    QFile ucfile(":/EyeExperimenter/src/experiment_data/uc.dat");
    if (!ucfile.open(QFile::ReadOnly)){
        qWarning() << "Could not open uc File";
        return;
    }
    QTextStream ucreader(&ucfile);
    QString ucexp = ucreader.readAll();
    ucfile.close();

    if (!uc.parseExpConfiguration(ucexp)){
        qWarning() << "Error parsing uc file: " << uc.getError();
        return;
    }

    QStringList ucnamelist;
    for (qint32 i = 0; i < uc.getExpectedIDs().size(); i++){
        QString id = uc.getExpectedIDs().at(i).first();
        if (!ucnamelist.contains(id)) ucnamelist << id;
    }

    // ================== Loading CSV file
    QFile csvfile(csvfilename);
    if (!csvfile.open(QFile::ReadOnly)){
        qWarning() << "Error reading csv file" << csvfilename;
        return;
    }

    QTextStream csvreader(&csvfile);
    QString csvcontens = csvreader.readAll();
    csvfile.close();

    QStringList lines = csvcontens.split("\n");
    csvcontens = "";
    qWarning() << "Number of lines is" << lines.size();

    QHash<QString, QList<Point> > todraw;
    qWarning() << "Expected number of pictures to draw" <<  (bcnamelist.size() + ucnamelist.size())*4;

    QStringList namesuffixes;
    namesuffixes << "_L_ENC_" << "_L_REC_" << "_R_ENC_" << "_R_REC_";

    QList<bool> bcFlag;
    QList<bool> ucFlag;

    for (qint32 i = 0; i < bcnamelist.size(); i++){
        bcFlag << false;
        for (qint32 j = 0; j < namesuffixes.size(); j++){
            QList<Point> list;
            todraw["BC" + namesuffixes.at(j) + bcnamelist.at(i)] = list;
        }
    }

    for (qint32 i = 0; i < ucnamelist.size(); i++){
        ucFlag << false;
        for (qint32 j = 0; j < namesuffixes.size(); j++){
            QList<Point> list;
            todraw["UC" + namesuffixes.at(j) + ucnamelist.at(i)] = list;
        }
    }

    qWarning() << "Number of CREATED pictures to draw" << todraw.keys().size();
//    QStringList bcSet;
//    QStringList ucSet;

//    for (qint32 i = 1; i < lines.size(); i++){
//        QString line = lines.at(i);
//        lines.removeFirst();

//        line.replace("\"",""); // Getting rid of the quotation marks.
//        line.replace("\r",""); // Getting rid of the quotation marks.

//        QStringList cols = line.split(",",QString::SkipEmptyParts);
//        QString trialID = cols.at(COL_TRIAL_NAME);

//        if (cols.at(COL_BC_OR_UC) == "BC") {
//            qint32 index = bcnamelist.indexOf(trialID);
//            if (index == -1){
//                if (!bcSet.contains())
//            }
//        }
//        else if (cols.at(COL_BC_OR_UC) == "UC") {
//            if (!ucSet.contains(trialID)) {
//                ucSet << trialID;
//            }
//        }
//        else {
//            qWarning() << "Unknown binding type" << cols.at(COL_BC_OR_UC);
//            return;
//        }

//    }

//    return;

    // First line is the header.
    lines.pop_front();
    qreal totalp = lines.size();
    qreal counter = 0;

    emit(updatePercent(0));

    while (lines.size() > 0){
        QString line = lines.first();
        lines.removeFirst();

        line.replace("\"",""); // Getting rid of the quotation marks.
        line.replace("\r",""); // Getting rid of the quotation marks.

        QStringList cols = line.split(",",QString::SkipEmptyParts);
        if (cols.size() != COL_COUNT){
            qWarning() << "Line" << line << "has" << cols.size() << "columns. Expectign" << COL_COUNT;
            continue;
        }

        Point p;
        p.x = cols.at(COL_X).toDouble();
        p.y = cols.at(COL_Y).toDouble();
        p.condition = cols.at(COL_CONDITION);
        QString name;

        if (cols.at(COL_BC_OR_UC) == "BC") {
            name = "BC_";
            p.isbc = true;
        }
        else if (cols.at(COL_BC_OR_UC) == "UC") {
            name = "UC_";
            p.isbc = false;
        }
        else {
            qWarning() << "Unknown binding type" << cols.at(COL_BC_OR_UC);
            return;
        }

        if (cols.at(COL_EYE) == "0") name = name + "L_";
        else if (cols.at(COL_EYE) == "1") name = name + "R_";
        else {
            qWarning() << "Unknown eye type" << cols.at(COL_EYE);
            return;
        }


        if (cols.at(COL_TYPE) == COL_TYPE_ENC) {
            name = name + "ENC_";
            p.show = true;
        }
        else if (cols.at(COL_TYPE) == COL_TYPE_REC) {
            name = name + "REC_";
            p.show = false;
        }
        else {
            qWarning() << "Unknown trial type" << cols.at(COL_TYPE);
            return;
        }

        name = name + cols.at(COL_TRIAL_NAME);
        p.trialname = cols.at(COL_TRIAL_NAME);

        if (!todraw.contains(name)){
            qWarning() << "NAME: " << name << " is not contained in the picture list";
            continue;
        }

        todraw[name].append(p);
        counter++;
        emit(updatePercent(counter*100.0/totalp));

    }

    emit(updatePercent(0));
    // Starting the actual drawing

    QStringList allfilenames = todraw.keys();

    qWarning() << "Actual number of pictures to draw" << allfilenames.size();

    qint32 numpointcounter = 0;
    for (qint32 i = 0; i < allfilenames.size(); i++){
        QString filename = allfilenames.at(i);
        QList<Point> points = todraw.value(filename);

        if (points.isEmpty()){
            qWarning() << "FILE" << filename << "has no points";
            continue;
        }

        Point leadpoint = points.first();
        //qWarning() << "FILE: " << filename << "has" << points.size();
        if (leadpoint.isbc){
            if (!bc.drawFlags(leadpoint.trialname,leadpoint.show)){
                qWarning() << "Could not find BC trial" << leadpoint.trialname;
                continue;
            }
            QImage image(bc.getCanvas()->width(),bc.getCanvas()->height(),QImage::Format_RGB888);
            QPainter painter(&image);
            painter.setRenderHint(QPainter::Antialiasing);
            bc.getCanvas()->render(&painter);
            qreal R = 0.01*image.width();
            drawPointOnCanvas(&painter,points,R);
            image.save(outputdir + "/" + filename +  ".jpg");
        }
        else{
            if (!uc.drawFlags(leadpoint.trialname,leadpoint.show)){
                qWarning() << "Could not find UC trial" << leadpoint.trialname;
                continue;
            }
            QImage image(uc.getCanvas()->width(),uc.getCanvas()->height(),QImage::Format_RGB888);
            QPainter painter(&image);
            painter.setRenderHint(QPainter::Antialiasing);
            uc.getCanvas()->render(&painter);
            qreal R = 0.01*image.width();
            drawPointOnCanvas(&painter,points,R);
            image.save(outputdir + "/" + filename +  ".jpg");
        }

        numpointcounter = numpointcounter + points.size();

        emit(updatePercent(i*100.0/allfilenames.size()));
    }

    qWarning() << "Number of points drawn" << numpointcounter;

}

void BindingFixationDrawer::drawPointOnCanvas(QPainter *painter, QList<Point> points, qreal R){

    for (qint32 j = 0; j < points.size(); j++){
        QColor color;
        Point p = points.at(j);
        if (p.condition == COL_COND_DCL) color = QColor(255,0,0);
        else color = QColor(0,0,255);
        // Drawing the dot
        painter->setPen(QPen(color));
        QPointF px(p.x,p.y);
        painter->drawEllipse(px,R,R);
    }


}
