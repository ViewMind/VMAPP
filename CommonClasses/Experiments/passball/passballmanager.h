#ifndef PASSBALLMANAGER_H
#define PASSBALLMANAGER_H

#include "passballparser.h"
#include "../experimentdatapainter.h"

class PassBallManager: public ExperimentDataPainter
{
public:
    PassBallManager();

    bool parseExpConfiguration(const QString &contents) override;
    void init(qreal display_resolution_width, qreal display_resolution_height ) override;
    qint32 size() const override;
    void renderStudyExplanationScreen(qint32 screen_index) override;

    PassBallParser getPBParser() const;


private:

    PassBallParser pbParser;
};

#endif // PASSBALLMANAGER_H
