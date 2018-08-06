#include "bindingfilegenerator.h"

static const int NUMBER_OF_SAME_TEST_CASES_TO_GENERATE = 5;
static const int NUMBER_OF_DIFF_TEST_CASES_TO_GENERATE = 5;
static const int NUMBER_OF_DIFF_TRIALS_TO_GENERATE     = 16;
static const int NUMBER_OF_SAME_TRIALS_TO_GENERATE     = 16;

BindingFileGenerator::BindingFileGenerator()
{
    qsrand(QDateTime::currentSecsSinceEpoch());
}

void BindingFileGenerator::generateFile(const QString &filename, bool bc, bool numbered, int ntargets){


    // Generating the content.
    QString contents = QString::number(ntargets) + "\n";
    if ((ntargets < 2) || (ntargets > 4)){
        qWarning() << "N targets is invalid" << ntargets;
        return;
    }

    // Setting the grid Type and the min manhattan distance variable according to type.
    Slide::GridType gridType;
    bool minManhattan1;
    if (ntargets == 2){
        gridType = Slide::GT_3x2;
        minManhattan1 = false;
    }
    else{
        gridType = Slide::GT_3x3;
        minManhattan1 = true;
    }

    // Generating the which are same and which are different
    qint32 ntests = NUMBER_OF_DIFF_TEST_CASES_TO_GENERATE + NUMBER_OF_SAME_TEST_CASES_TO_GENERATE;
    qint32 ntrials = NUMBER_OF_DIFF_TRIALS_TO_GENERATE + NUMBER_OF_SAME_TRIALS_TO_GENERATE;

    QSet<qint32> sameTests;
    QSet<qint32> sameTrials;

    for (qint32 i = 0; i < NUMBER_OF_SAME_TEST_CASES_TO_GENERATE; i++) sameTests << qrand() % ntests;
    for (qint32 i = 0; i < NUMBER_OF_SAME_TRIALS_TO_GENERATE; i++) sameTrials << qrand() % ntrials;

    // This is all for the numbering system.
    qint32 baseNumer = 100;
    qint32 diffNumber = 3;
    qint32 textNumber = baseNumer + (ntests + ntrials)*diffNumber;

    // Need to generate 10 test cases and then 32 trials.
    QString prefix;
    QString trialType;
    QString name;
    qint32 trialCounter = 1;
    qint32 ntotal = ntests + ntrials;
    qint32 index;
    bool same;

    for (qint32 i = 0; i < ntotal; i++){

        if (i < ntests) {
            prefix = "0-test";
            index = i;
            same = sameTests.contains(index);
        }
        else {
            prefix = QString::number(trialCounter);
            if (prefix.size() < 2) prefix = "0" + prefix;
            prefix = prefix + "-trial";
            trialCounter++;
            index = i - ntests;
            same = sameTrials.contains(index);
        }
        name = "";

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

        qWarning() << "GENERATING" << name;

        // Creating the show slide and adding it to the contents.
        Slide show(ntargets,gridType,minManhattan1);
        show.generateRandomPositions();
        show.setColors();
        contents = contents + show.toDescription();

        // Generating an new set of postions
        Slide test(show);
        test.generateRandomPositions(true);

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

BindingFileGenerator::Slide::Slide(qint32 nt, GridType gt, bool mManhattanDof1){
    nTargets = nt;
    gridType = gt;
    minimumManhattanDistanceOf1 = mManhattanDof1;
}

BindingFileGenerator::Slide::Slide(const Slide &s){
    nTargets = s.nTargets;
    pos = s.pos;
    remainingColors = s.remainingColors;
    backs = s.backs;
    crosses = s.crosses;
    gridType = s.gridType;
    minimumManhattanDistanceOf1 = s.minimumManhattanDistanceOf1;
}

bool BindingFileGenerator::Slide::areEqual(const Slide &s){
    for (qint32 i = 0; i < nTargets; i++){
        if (crosses.at(i) != s.crosses.at(i)){
            //qWarning() << "different crosses" << crosses.at(i) << s.crosses.at(i);
            return false;
        }
        if (backs.at(i)  != s.backs.at(i)) {
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
    if (gridType == GT_3x3){
        remainingPos << "00" << "10" << "20"
                     << "01" << "11" << "21"
                     << "02" << "12" << "22";
    }
    else{
        // A 3x2 Grid.
        remainingPos << "00" << "10"
                     << "01" << "11"
                     << "02" << "12";
    }
}

void BindingFileGenerator::Slide::generateRandomPositions(bool avoidExistingPositions){

    QStringList positionsToAvoid;
    if (avoidExistingPositions) positionsToAvoid = pos;
    resetPositions();
    pos.clear();

    if (minimumManhattanDistanceOf1){

        qWarning() << "-----------ENTRANDO-------------";
        qWarning() << remainingPos;

        qint32 inx;
        QStringList toRemove;

        for (qint32 i = 0; i < nTargets; i++){

            qWarning() << "RPOS" << remainingPos;

            QStringList secCopyRPos = remainingPos;
            if (!positionsToAvoid.isEmpty()){
                secCopyRPos.removeOne(positionsToAvoid.at(i));
            }

            inx = qrand() % secCopyRPos.size();
            pos << secCopyRPos.at(inx);

            qWarning() << "SELECTED" << pos.last();

            // Removing only the selected index and those within a manhattan distance of 1.
            toRemove.clear();
            for (qint32 j = 0; j < secCopyRPos.size(); j++){
                if (j == inx) toRemove << secCopyRPos.at(j);
                else if (manhattanDistance(pos.last(),secCopyRPos.at(j)) == 1) toRemove << secCopyRPos.at(j);
            }

            // Checking if the position to avoid is also a Manhattan dinstance 1.
            if (!positionsToAvoid.isEmpty()){
                if (manhattanDistance(pos.last(),positionsToAvoid.at(i)) == 1){
                    toRemove << positionsToAvoid.at(i);
                }
            }

            // Actually removing it from the original copy
            for (qint32 j = 0; j < toRemove.size(); j++){
                remainingPos.removeOne(toRemove.at(j));
            }

        }



    }
    else{

        qint32 inx;
        for (qint32 i = 0; i < nTargets; i++){
            QStringList secCopyRPos = remainingPos;
            if (!positionsToAvoid.isEmpty()){
                secCopyRPos.removeOne(positionsToAvoid.at(i));
            }
            inx = qrand() % secCopyRPos.size();
            pos << secCopyRPos.at(inx);
            remainingPos.removeOne(pos.last());
        }
    }

}

qint32 BindingFileGenerator::Slide::manhattanDistance(const QString &a, const QString &b){

    QString ars = a.at(0);
    QString acs = a.at(1);
    QString brs = b.at(0);
    QString bcs = b.at(1);

    qint32 diffR = qAbs(ars.toInt() - brs.toInt());
    qint32 diffC = qAbs(acs.toInt() - bcs.toInt());

    return diffR + diffC;
}
