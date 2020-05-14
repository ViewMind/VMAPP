#include "eyetribeascextractor.h"

EyeTribeASCExtractor::EyeTribeASCExtractor()
{
    resolutionWidth = 1;
    resoltuionHeight = 1;
    normResolutionScale = 1;
}

void EyeTribeASCExtractor::run(){

    qint32 md;
    qint32 diff = (mdEnd - mdStart + 1);
    for (md = mdStart; md <= mdEnd; md++){
        if (!movingWindowOnASCFile(ascFileToSweep,md)) break;
        int p;
        if (diff > 0) p = (md - mdStart)*100/diff;
        else p = 0;
        emit(updateProgress(p));
    }

    if (md == mdEnd) emit(updateProgress(100));

}

bool EyeTribeASCExtractor::generateFixationFileFromASC(const QString &fileName, qreal resolutionX, qreal  resolutionY){

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)){
        error = "Could not open " + fileName + " for reading";
        return false;
    }

    QTextStream reader(&file);
    QStringList lines = reader.readAll().split("\n");
    file.close();

    QStringList fixationList;

    qreal scale = 1000.0;
    if ((resolutionX <= 0) || (resolutionY <= 0)){
        scale = 1.0;
        resolutionX = 1;
        resolutionY = 1;
    }

    for (int i = 0; i < lines.size(); i++){
        QString line = lines.at(i);

        line = line.replace('\t',' ');
        line = line.replace('\r',' ');

        QStringList tokens = line.split(" ",QString::SkipEmptyParts);

        if (tokens.size() == 8){
            if (tokens.at(0) == "EFIX"){

                qreal x = tokens.at(5).toDouble();
                qreal y = tokens.at(6).toDouble();
                x = x*scale/resolutionX;
                y = y*scale/resolutionY;

                if (tokens.at(1) == "R")
                    fixationList << QString::number(EYE_R) + "," + QString::number(x) + "," + QString::number(y) + "," + tokens.at(4);
                else if (tokens.at(1) == "L")
                    fixationList << QString::number(EYE_L) + "," + QString::number(x) + "," + QString::number(y) + "," + tokens.at(4);
            }
        }

    }

    fixationList.prepend("eyeID,x,y,dur");
    if (!saveStringList(fixationList,"fixlist.csv")) return false;
    return true;

}

bool EyeTribeASCExtractor::genrateCSVFixFileBySentences(const QString &fileName, qint32 maxDispersion){

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)){
        error = "Could not open " + fileName + " for reading";
        return false;
    }

    QTextStream reader(&file);
    QStringList ascLines = reader.readAll().split("\n");
    file.close();

    QString idCurrentSet = "";

    // Setting up moving window algorithm.
    MovingWindowParameters mwp;
    mwp.sampleFrequency       = 1000;
    mwp.maxDispersion         = maxDispersion;
    mwp.minimumFixationLength = 50;

    mwp.calculateWindowSize();
    MovingWindowAlgorithm rfix;
    MovingWindowAlgorithm lfix;

    rfix.parameters = mwp;
    lfix.parameters = mwp;

    rfix.finalizeOnlineFixationCalculation();
    lfix.finalizeOnlineFixationCalculation();

    QStringList output;
    // The CSV File Header.
    output << "trial,x,y,dur,ojoDI";
    QStringList rfixes;
    QStringList lfixes;

    Fixation r;
    Fixation l;

    for (int i = 0; i < ascLines.size(); i++){
        QString line = ascLines.at(i);

        line = line.replace('\t',' ');
        line = line.replace('\r',' ');

        QStringList tokens = line.split(" ",QString::SkipEmptyParts);

        if (tokens.size() == 4){
            if (tokens.at(2) == "EmpiezaLectura"){
                //qDebug() << "Empieza lectura found" << tokens.at(3);
                if (!idCurrentSet.isEmpty()){
                    r = rfix.finalizeOnlineFixationCalculation();
                    l = lfix.finalizeOnlineFixationCalculation();
                    if (r.isValid()){
                        rfixes << idCurrentSet + "," + QString::number(r.x) + ","  + QString::number(r.y) + "," + QString::number(r.duration) + "," + "1";
                    }
                    if (l.isValid()){
                        lfixes << idCurrentSet + "," + QString::number(l.x) + ","  + QString::number(l.y) + "," + QString::number(l.duration) + "," + "0";
                    }
                    output << lfixes << rfixes;
                    lfixes.clear();
                    rfixes.clear();
                }
                idCurrentSet = tokens.at(3);
            }
        }
        else if (!idCurrentSet.isEmpty()){
            if (tokens.size() == 7){
                bool ok = true;
                QList<qreal> values;
                for (qint32 j = 0; j < tokens.size(); j++){
                    values << tokens.at(j).toDouble(&ok);
                    if (!ok) break;
                }
                if (ok){
                    // This means all values where converted succesfully.
                    // The meaning of each value was inferred by averaging the columns on the ASC between a fixation and the comparing those value to the fixation location.
                    qreal timestamp = values.at(0);
                    qreal rx        = values.at(4);
                    qreal ry        = values.at(5);
                    qreal lx        = values.at(1);
                    qreal ly        = values.at(2);

                    r = rfix.calculateFixationsOnline(rx,ry,timestamp);
                    l = lfix.calculateFixationsOnline(lx,ly,timestamp);

                    if (r.isValid()){
                        //qDebug() << "Found R Fix";
                        rfixes << idCurrentSet + "," + QString::number(r.x) + ","  + QString::number(r.y) + "," + QString::number(r.duration) + "," + "1";
                    }
                    if (l.isValid()){
                        //qDebug() << "Found L Fix";
                        lfixes << idCurrentSet + "," + QString::number(l.x) + ","  + QString::number(l.y) + "," + QString::number(l.duration) + "," + "0";
                    }
                }
            }
        }
    }

    if (!saveStringList(output,"fix_csv_by_sentences.csv")) return false;

    return true;

}

void EyeTribeASCExtractor::setupSweep(const MDSweepParameters &params){
    ascFileToSweep = params.fileName;
    mdStart = params.startMD;
    mdEnd = params.endMD;
    if ((params.resX > 0) && (params.resY > 0)){
        resolutionWidth = params.resX;
        resoltuionHeight = params.resY;
        normResolutionScale = params.resScale;
    }
}

bool EyeTribeASCExtractor::movingWindowOnASCFile(const QString &fileName, qint32 maxDispersionToUse){

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)){
        error = "Could not open " + fileName + " for reading";
        return false;
    }

    QTextStream reader(&file);
    QStringList lines = reader.readAll().split("\n");
    file.close();

    bool isNormalized = ((resolutionWidth > 0) && (resoltuionHeight > 0));

    MovingWindowParameters mwp;
    mwp.sampleFrequency       = 1000;
    mwp.maxDispersion         = maxDispersionToUse;
    mwp.minimumFixationLength = 50;

    mwp.calculateWindowSize();
    MovingWindowAlgorithm rfix;
    MovingWindowAlgorithm lfix;

    rfix.parameters = mwp;
    lfix.parameters = mwp;

    rfix.finalizeOnlineFixationCalculation();
    lfix.finalizeOnlineFixationCalculation();

    QStringList fixationList;

    for (int i = 0; i < lines.size(); i++){
        QString line = lines.at(i);

        line = line.replace('\t',' ');
        line = line.replace('\r',' ');

        QStringList tokens = line.split(" ",QString::SkipEmptyParts);

        if (tokens.size() == 7){
            bool ok = true;
            QList<qreal> values;
            for (qint32 j = 0; j < tokens.size(); j++){
                values << tokens.at(j).toDouble(&ok);
                if (!ok) break;
            }
            if (ok){
                // This means all values where converted succesfully.
                // The meaning of each value was inferred by averaging the columns on the ASC between a fixation and the comparing those value to the fixation location.
                qreal timestamp = values.at(0);
                qreal rx        = values.at(4);
                qreal ry        = values.at(5);
                qreal lx        = values.at(1);
                qreal ly        = values.at(2);

                // Normalizing, if enabled.
                rx = rx*normResolutionScale/resolutionWidth;
                lx = lx*normResolutionScale/resolutionWidth;
                ry = ry*normResolutionScale/resoltuionHeight;
                ly = ly*normResolutionScale/resoltuionHeight;

                Fixation r = rfix.calculateFixationsOnline(rx,ry,timestamp);
                Fixation l = lfix.calculateFixationsOnline(lx,ly,timestamp);
                if (r.isValid()){
                    fixationList << QString::number(EYE_R) + "," + QString::number(r.x) + "," +  QString::number(r.y) + "," + QString::number(r.duration);
                }
                if (l.isValid()){
                    fixationList << QString::number(EYE_L) + "," + QString::number(l.x) + "," +  QString::number(l.y) + "," + QString::number(l.duration);
                }
            }
        }
    }

    fixationList.prepend("eyeID,x,y,dur");
    QString mdFileName = "mw_fixation_list_";
    if (isNormalized) mdFileName = mdFileName + "norm_to_" + QString::number(normResolutionScale) + "_";
    mdFileName = mdFileName + "md_";

    if (!saveStringList(fixationList, mdFileName + QString::number(mwp.maxDispersion) + ".csv")) return false;

    return true;

}

bool EyeTribeASCExtractor::saveStringList(const QStringList &list, const QString &outFileName){
    QFile outfile(outFileName);
    if (!outfile.open(QFile::WriteOnly)){
        error = "Could not open output file " + outFileName +  " for writing";
        return false;
    }
    QTextStream writerR(&outfile);
    writerR << list.join("\n");
    outfile.close();
    return true;
}

