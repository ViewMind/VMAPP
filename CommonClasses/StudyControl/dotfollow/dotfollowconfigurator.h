#ifndef DOTFOLLOWCONFIGURATOR_H
#define DOTFOLLOWCONFIGURATOR_H

#include "../studyconfigurator.h"

namespace RRS {

   namespace DOT_FOLLOW {

      static const QString MOVE_TYPE     = "move_type";
      static const QString START_POS     = "start_pos";
      static const QString END_POS       = "end_pos";
      static const QString HOLD_TIME     = "hold_time";

   }

}

class DotFollowConfigurator: public StudyConfigurator
{
public:
    DotFollowConfigurator();

private:
    bool parseStudyDescription(int numberOfMovements);
    bool studySpecificConfiguration(const QVariantMap &studyConfig) override;

    const qint32 DF_MIN_HOLD_TIME =                        500;
    const qint32 DF_MAX_HOLD_TIME =                        3000;
    const qint32 DF_MOVE_END_HOLD_TIME =                   500;
    const qint32 DF_MOVE_TIME =                            800;

};

#endif // DOTFOLLOWCONFIGURATOR_H
