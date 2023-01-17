#ifndef GONOGOSPHEREEXPERIMENT_H
#define GONOGOSPHEREEXPERIMENT_H

#include "../experiment.h"
#include "gonogosperemanager.h"

class GoNoGoSphereExperiment: public Experiment
{
public:

    GoNoGoSphereExperiment(QObject *parent = nullptr, const QString &study_type = "");

    // Reimplementation of virtual functions
    bool startExperiment(const QString &workingDir, const QString &experimentFile,
                         const QVariantMap &studyConfig) override;

protected:
    void keyPressHandler(int keyPressed) override;
    GoNoGoSpereManager *m;

private:

    // String keys for study messages
    static const QString MSG_GREEN_HIT;
    static const QString MSG_RED_HIT;

    void newDataForStudyMessageFor3DStudies(const QVariantList &msg) override;
    QString getExperimentDescriptionFile(const QVariantMap &studyConfig) override;

};

#endif // GONOGOSPHEREEXPERIMENT_H
