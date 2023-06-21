#ifndef GNGSPHERESCONFIGURATOR_H
#define GNGSPHERESCONFIGURATOR_H

#include "../studyconfigurator.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace RRS {

   namespace GNGSpheres {

      namespace Sphere {
         static const QString ID      = "id";
         static const QString COLOR   = "color";
         static const QString ORIGIN  = "origin";
      }

   }

}



class GNGSpheresConfigurator: public StudyConfigurator
{
public:
    GNGSpheresConfigurator();

private:
    bool parseStudyDescription(int numberOfTrials);
    bool studySpecificConfiguration(const QVariantMap &studyConfig) override;

    // The fields in the study description.
    const QString JFIELD_SPHERES = "spheres";
    const QString JFIELD_COLOR   = "color";
    const QString JFIELD_ORIGIN  = "origin";

    // the number of origins.
    const qint32 N_ORIGINS = 9;

};

#endif // GNGSPHERESCONFIGURATOR_H
