#ifndef BINDINGCONFIGURATOR_H
#define BINDINGCONFIGURATOR_H

#include "../studyconfigurator.h"

namespace RRS {

   namespace Binding {

      namespace Flag {
         static const QString ROW   = "row";
         static const QString COL   = "col";
         static const QString CROSS = "cross";
         static const QString BACK  = "back";
      }

      namespace Trial {
         static const QString ENCODE    = "encode";
         static const QString RETRIEVAL = "retrieval";
         static const QString NAME      = "name";
         static const QString IS_SAME   = "is_same";
      }

   }

}


class BindingConfigurator: public StudyConfigurator
{
public:
    BindingConfigurator(bool isBC, qint32 ntargets);

private:

    QList< QList<QStringList> > tokenMatrix;
    qint32 number_of_targets;
    bool isBC;

    bool parseStudyDescription();
    bool studySpecificConfiguration(const QVariantMap &studyConfig) override;

    bool createTokenMatrix();
    bool createBindingTrialFromTrialStringMatrix(const QList<QStringList> &matrix, QVariantMap *trial);
    bool parseFlagPositions(const QString &pos, QVariantMap *flag);
    QVariantMap createFlag(const QString &pos, const QString &back, const QString &cross);

};

#endif // BINDINGCONFIGURATOR_H
