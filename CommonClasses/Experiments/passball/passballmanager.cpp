#include "passballmanager.h"

PassBallManager::PassBallManager()
{
    explanationListTextKey = STUDY_TEXT_KEY_PASS_BALL;
    numberOfExplanationScreens = 4;
    versionString = "v1";
}


bool PassBallManager::parseExpConfiguration(const QString &contents) {

    if (!pbParser.generatePassBallSequence()){
        error = pbParser.getError();
        return false;
    }

    return true;

}

void PassBallManager::init(qreal display_resolution_width, qreal display_resolution_height ){
    ExperimentDataPainter::init(display_resolution_width,display_resolution_height);
}

qint32 PassBallManager::size() const {
    if (shortModeEnabled){
        return 7;
    }
    else return pbParser.getSize();
}

void PassBallManager::renderStudyExplanationScreen(qint32 screen_index){
    Q_UNUSED(screen_index)
}

PassBallParser PassBallManager::getPBParser() const {
    return pbParser;
}
