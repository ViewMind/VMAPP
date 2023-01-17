#include "gonogosperemanager.h"

GoNoGoSpereManager::GoNoGoSpereManager()
{
    explanationListTextKey = STUDY_TEXT_KEY_GONOGO_3D;
    numberOfExplanationScreens = 4;
    versionString = "v1";
}


bool GoNoGoSpereManager::parseExpConfiguration(const QString &contents) {

    if (!gngsParser.parseGoNoGoSphereStudy(contents)){
        error = gngsParser.getError();
        return false;
    }

    return true;

}

void GoNoGoSpereManager::init(qreal display_resolution_width, qreal display_resolution_height ){
    ExperimentDataPainter::init(display_resolution_width,display_resolution_height);
}

qint32 GoNoGoSpereManager::size() const {
    if (shortModeEnabled){
        return 7;
    }
    else return gngsParser.getSize();
}

void GoNoGoSpereManager::renderStudyExplanationScreen(qint32 screen_index){
    Q_UNUSED(screen_index)
}

GoNoGoSphereParser GoNoGoSpereManager::getGNGSphereParser() const {
    return gngsParser;
}
