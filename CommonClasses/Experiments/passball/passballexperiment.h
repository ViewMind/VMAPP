#ifndef PASSBALLEXPERIMENT_H
#define PASSBALLEXPERIMENT_H

#include "../experiment.h"
#include "passballmanager.h"

class PassBallExperiment: public Experiment
{
public:
    PassBallExperiment(QObject *parent = nullptr, const QString &study_type = "");

    // Reimplementation of virtual functions
    bool startExperiment(const QString &workingDir, const QString &experimentFile,
                         const QVariantMap &studyConfig) override;

protected:
    void keyPressHandler(int keyPressed) override;
    PassBallManager *m;

private:

    // String keys for study messages
    static const QString MSG_PASSES_MISSED;
    static const QString MSG_PASSES_COMPLETED;

    void newDataForStudyMessageFor3DStudies(const QVariantList &msg) override;
    QString getExperimentDescriptionFile(const QVariantMap &studyConfig) override;
};

#endif // PASSBALLEXPERIMENT_H
