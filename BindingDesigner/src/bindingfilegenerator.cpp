#include "bindingfilegenerator.h"

BindingFileGenerator::BindingFileGenerator()
{
    qsrand(QDateTime::currentSecsSinceEpoch());
}

void BindingFileGenerator::generateFile(const QString &filename, bool bc, bool numbered, int ntargets){

    qint32 NTestCases = 10;
    qint32 NTrueTrials = 32;
    //    qint32 NTestCases = 1;
    //    qint32 NTrueTrials = 1;

    qint32 baseNumer = 100;
    qint32 diffNumber = 3;
    qint32 textNumber = baseNumer + (NTestCases + NTrueTrials)*diffNumber;


    // Generating the content.
    QString contents = QString::number(ntargets) + "\n";
    if ((ntargets < 2) || (ntargets > 4)){
        qWarning() << "N targets is invalid" << ntargets;
    }

    // Need to generate 10 test cases and then 32 trials.
    qint32 nTrials = NTestCases + NTrueTrials;
    QString prefix;
    QString trialType;
    QString name;
    qint32 trialCounter = 1;
    for (qint32 i = 0; i < nTrials; i++){

        if (i < NTestCases) {
            prefix = "0-test";
        }
        else {
            prefix = QString::number(trialCounter);
            if (prefix.size() < 2) prefix = "0" + prefix;
            prefix = prefix + "-trial";
            trialCounter++;
        }
        name = "";

        // Same or different.
        bool same = ((qrand() % 2)== 1);

        if (same) {
            name = name + prefix + "-" + QString(STR_SAME) + "-" + QString::number(i) + " ";
            trialType = "s";
        }
        else {
            name = name +  prefix + "-" + QString(STR_DIFFERENT) + "-" + QString::number(i) + " ";
            trialType = "d";
        }

        if (numbered) {
            name = name + QString::number(textNumber) + " ";
            textNumber = textNumber - diffNumber;
        }
        else name = name + "x ";
        name = name  + trialType;
        contents = contents  + name + "\n";

        // Creating the show slide and adding it to the contents.
        Slide show(ntargets);
        show.resetPositions();
        show.generateRandomPositions();
        show.setColors();
        contents = contents + show.toDescription();

        // Generating an new set of postions
        Slide test(show);
        test.generateRandomPositions();

        // Changing the colors according to specification, depending on bound or unbound if a different type trial
        if (!same){
            if (bc) test.boundChange();
            else test.unboundChange();
            if (show.areEqual(test)){
                qWarning() << "ERROR Different type trial is SAME on test:" << name << "for file" << filename;
            }
        }

        // Adding the the test slide of the trial.
        contents = contents + test.toDescription();

    }

    QFile file(filename);
    if (!file.open(QFile::WriteOnly)){
        qWarning() << "Could not open" << filename << "for writing";
        return;
    }

    QTextStream writer(&file);
    writer << contents;
    file.close();
    return;

}

//***************************************** SLIDE FUNCTIONS ********************************************

BindingFileGenerator::Slide::Slide(qint32 nt){
    nTargets = nt;
}

BindingFileGenerator::Slide::Slide(const Slide &s){
    nTargets = s.nTargets;
    pos = s.pos;
    remainingColors = s.remainingColors;
    backs = s.backs;
    crosses = s.crosses;
    remainingPos = s.remainingPos;
}

bool BindingFileGenerator::Slide::areEqual(const Slide &s){
    for (qint32 i = 0; i < nTargets; i++){
        if (crosses.at(i) != s.crosses.at(i)){
            //qWarning() << "different crosses" << crosses.at(i) << s.crosses.at(i);
            return false;
        }
        if (backs.at(i)   != s.backs.at(i)) {
            //qWarning() << "different backs" << backs.at(i) << s.backs.at(i);
            return false;
        }
    }
    return true;
}

QString BindingFileGenerator::Slide::toDescription() const{

    QString ans;
    for (qint32 i = 0; i < nTargets; i++){
        ans = ans + pos.at(i) + "     ";
    }
    ans = ans  + "\n";

    for (qint32 i = 0; i < nTargets; i++){
        ans = ans + backs.at(i) + " ";
    }
    ans = ans + "\n";

    for (qint32 i = 0; i < nTargets; i++){
        ans = ans + crosses.at(i) + " ";
    }
    ans = ans + "\n";

    return ans;

}

void BindingFileGenerator::Slide::setColors(){
    remainingColors.clear();
    remainingColors << "fa8071" << "ff01d5" << "fed700" << "4682b4"
                    << "279c27" << "00e8e8" << "81007f" << "7f8000"
                    << "992a1b" << "b49299";

    // Generate the back colors.
    backs.clear();
    qint32 inx;
    for (qint32 i = 0; i < nTargets; i++){
        inx = qrand() % remainingColors.size();
        backs << remainingColors.at(inx);
        remainingColors.removeAt(inx);
    }

    // Generate the cross colors.
    for (qint32 i = 0; i < nTargets; i++){
        inx = qrand() % remainingColors.size();
        crosses << remainingColors.at(inx);
        remainingColors.removeAt(inx);
    }
}

QList<qint32> BindingFileGenerator::Slide::select2RandomPositions(){
    QList<qint32> possibleIndexes;
    for (qint32 i = 0; i < nTargets; i++) possibleIndexes << i;
    qint32 id1 = qrand() % possibleIndexes.size();
    possibleIndexes.removeAt(id1);
    qint32 id2 = qrand() % possibleIndexes.size();
    id2 = possibleIndexes.at(id2);
    QList<qint32> ans;
    ans << id1 << id2;
    return ans;
}

void BindingFileGenerator::Slide::boundChange(){
    QList<qint32> id = select2RandomPositions();
    QString t = crosses.at(id.first());
    crosses[id.first()] = crosses.at(id.last());
    crosses[id.last()] = t;
}

void BindingFileGenerator::Slide::unboundChange(){
    QList<qint32> id = select2RandomPositions();
    backs[id.first()] = remainingColors.first();
    backs[id.last()] = remainingColors.last();
}

void BindingFileGenerator::Slide::resetPositions(){
    remainingPos.clear();
    remainingPos << "00" << "10" << "20"
                 << "01" << "11" << "21"
                 << "02" << "12" << "22";
}

void BindingFileGenerator::Slide::generateRandomPositions(){
    pos.clear();
    qint32 inx;
    for (qint32 i = 0; i < nTargets; i++){
        inx = qrand() % remainingPos.size();
        pos << remainingPos.at(inx);
        remainingPos.removeAt(inx);
    }

}
