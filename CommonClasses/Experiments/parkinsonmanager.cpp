#include "parkinsonmanager.h"

ParkinsonManager::ParkinsonManager()
{

}

ParkinsonParser::ParkinsonMaze ParkinsonManager::getCurrentMaze() const{
    if ((currentMaze > 0) && (currentMaze <= mazes.size())){
        return mazes.at(currentMaze-1);
    }
    else return ParkinsonParser::ParkinsonMaze();
}

bool ParkinsonManager::parseExpConfiguration(const QString &contents){

    ParkinsonParser parser;
    if (!parser.parseParkinsonExperiment(contents,
                                         canvas->width(),canvas->height(),
                                         config->getReal(CONFIG_XPX_2_MM),config->getReal(CONFIG_YPX_2_MM))){
        error = parser.getError();
        return false;
    }

    mazes = parser.getMazes();
    versionString = parser.getVersionString();

    currentMaze = 0;

    if (config->getBool(CONFIG_DEMO_MODE)) enableDemoMode();
    return true;
}

void ParkinsonManager::init(ConfigurationManager *c){
    ExperimentDataPainter::init(c);

    // Setting the background brush.
    canvas->setBackgroundBrush(QBrush(Qt::gray));
}

bool ParkinsonManager::drawCurrentMaze(){
    if (currentMaze < mazes.size()){
        drawMaze(mazes.at(currentMaze));
        currentMaze++;
        return true;
    }
    return false;
}

void ParkinsonManager::drawMaze(const ParkinsonParser::ParkinsonMaze &maze){

    canvas->clear();

    qreal sTw = maze.squareW*.7;
    qreal sTh = maze.squareH*.7;
    qreal sToffX = maze.squareW*.15;
    qreal sToffY = maze.squareH*.15;

    for (qint32 i = 0; i < maze.mazeDescription.size(); i++){
        QList<ParkinsonParser::ParkinsonSquareType> row = maze.mazeDescription.at(i);
        for (qint32 j = 0; j < row.size(); j++){
            QGraphicsRectItem *square = canvas->addRect(0,0,maze.squareW,maze.squareH,QPen(Qt::black),QBrush(Qt::white));
            square->setPos(j*maze.squareW+maze.xOffset,i*maze.squareH+maze.yOffset);
            if (row.at(j) == ParkinsonParser::PST_WALL){
                square->setBrush(QBrush(Qt::black));
            }
            else if (row.at(j) == ParkinsonParser::PST_TARGET){
               QGraphicsEllipseItem *target = canvas->addEllipse(0,0,sTw,sTh,QPen(Qt::black),QBrush(Qt::red));
               target->setPos(j*maze.squareW+maze.xOffset+sToffX,i*maze.squareH+maze.yOffset+sToffY);
            }
            else if (row.at(j) == ParkinsonParser::PST_ARROW){
                square->setBrush(QBrush(QColor(247,95,28)));
            }
        }
    }
}

void ParkinsonManager::enableDemoMode(){
    /// THERE IS NO DIFFERENCE IN DEMO MODE FOR NOW.
}
