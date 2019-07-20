#ifndef FIXATIONDRAWER_H
#define FIXATIONDRAWER_H

#include <QDir>
#include "../../../CommonClasses/Experiments/readingmanager.h"
#include "../../../CommonClasses/Experiments/bindingmanager.h"
#include "../../../CommonClasses/Experiments/fieldingmanager.h"
#include "../../../CommonClasses/DataAnalysis/EyeMatrixGenerator/movingwindowalgorithm.h"

class FixationDrawer
{
public:
    FixationDrawer();
    bool prepareToDrawFixations(const QString &expID, ConfigurationManager *c, const QString &expDescription, const QString &outDir);
    bool drawFixations(const FixationList &flist);
    QString getError() const {return error;}

private:

    // The drawing and experiment manager
    ExperimentDataPainter *manager;

    // Error message in case one ocurrs
    QString error;

    // Experiment ID
    QString experimentID;

    // Set the output director
    QString outputImageDirectory;

    // Actually drawing the fixation on the generated image.
    void drawFixationOnImage(const QString &outputBaseFileName, const Fixations &l, const Fixations &r);
};

#endif // FIXATIONDRAWER_H
