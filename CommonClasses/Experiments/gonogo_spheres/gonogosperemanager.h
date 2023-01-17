#ifndef GONOGOSPEREMANAGER_H
#define GONOGOSPEREMANAGER_H

#include "gonogosphereparser.h"
#include "../experimentdatapainter.h"

class GoNoGoSpereManager: public ExperimentDataPainter
{

public:

    GoNoGoSpereManager();

    bool parseExpConfiguration(const QString &contents) override;
    void init(qreal display_resolution_width, qreal display_resolution_height ) override;
    qint32 size() const override;
    void renderStudyExplanationScreen(qint32 screen_index) override;

    GoNoGoSphereParser getGNGSphereParser() const;

private:

    GoNoGoSphereParser gngsParser;

};

#endif // GONOGOSPEREMANAGER_H
