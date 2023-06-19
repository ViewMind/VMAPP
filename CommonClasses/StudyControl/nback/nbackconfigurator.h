#ifndef NBACKCONFIGURATOR_H
#define NBACKCONFIGURATOR_H

#include "../studyconfigurator.h"
#include <QString>

namespace RRS {

   namespace NBack {

      namespace NBackTrial {
         static const QString ID = "id";
         static const QString SEQ = "sequence";
      }

      namespace NBackLightUp {
         static const QString ALL     = "all";
         static const QString NONE    = "none";
         static const QString CORRECT = "correct_only";
      }

   }

}


class NBackConfigurator: public StudyConfigurator
{
public:
    NBackConfigurator();

private:
    bool parseStudyDescription(int numberOfTrials);
    bool studySpecificConfiguration(const QVariantMap &studyConfig) override;

    const qint32 MAX_SEQUENCE_LENGTH =                          6;
    const qint32 REDUCED_TRIAL_SET   =                          64;

    const qint32 NBACKVS_MIN_HOLD_TIME =                        50;
    const qint32 NBACKVS_MAX_HOLD_TIME =                        250;
    const qint32 NBACKVS_STEP_HOLD_TIME =                       50;
    const qint32 NBACKVS_START_HOLD_TIME =                      250;
    const qint32 NBACKVS_NTRIAL_FOR_STEP_CHANGE =               2;
    const qint32 NBACKVS_RETRIEVAL_TIMEOUT =                    3000;
    const qint32 NBACKVS_TRANSITION_TIME =                      500;

};

#endif // NBACKCONFIGURATOR_H
