#ifndef GNGCONFIGURATOR_H
#define GNGCONFIGURATOR_H

#include "../studyconfigurator.h"

namespace RRS {

   namespace GNG {

      static const QString COLOR_CODE = "color";
      static const QString RIGHT      = "to_the_right";

   }

}

class GNGConfigurator: public StudyConfigurator
{
public:
    GNGConfigurator();

private:
    bool parseStudyDescription(int numberOfTrials);
    bool studySpecificConfiguration(const QVariantMap &studyConfig) override;
};

#endif // GNGCONFIGURATOR_H
