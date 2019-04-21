#include "bargrapher.h"

const QString BarGrapher::STR_CANVAS_IDS = "<!--CANVAS-->";
const QString BarGrapher::STR_RUN_CODE = "//<!--GENGRAPH-->";


BarGrapher::BarGrapher()
{

}

BarGrapher::BarGraphOptions::BarGraphOptions(){
    this->fontSize = 12;
    this->title = "TITLE";
    this->ylabel = "DATA";
    this->drawValuesOnBars = false;
}

QString BarGrapher::BarGraphOptions::verifyValues() const {
    qint32 M = this->dataSets.size();
    if (M == 0){
        return "No data sets";
    }
    if (M != this->colors.size()){
        return "The number of colors (" + QString::number(this->colors.size()) + ") does not match the number of data sets (" + QString::number(M) + ")";
    }
    qint32 N = this->dataSets.first().size();
    for (qint32 i = 0; i < M; i++){
        if (this->dataSets.at(i).size() != N){
            return "All datasets should be (" + QString::number(N) + ") but data set " + QString::number(i) + " is " + QString::number(this->dataSets.at(i).size()) + ")";
        }
    }
    if (this->xtext.size() != N){
        return "The number of labels for the x axis (" + QString::number(this->xtext.size()) + ") does not match the number of data points (" + QString::number(N) + ")";
    }
    if (this->legend.size() > 0){
        if (this->legend.size() != M){
            return "The number of legend texts (" + QString::number(this->legend.size()) + ") does not match the number of data sets (" + QString::number(M) + ")";
        }
    }
    return "";
}

QString BarGrapher::BarGraphOptions::generateJavaScriptObj(const QString &varName, const QString &tab) const{

    QString err = this->verifyValues();
    if (!err.isEmpty()){
        err = "Graph Title: " + this->title + "\\nFILE: " + this->associatedFileName + "\\nERROR: " + err;
        QString errret = tab + "generateMessage('" + varName +  "','" + err + "'," + QString::number(this->width) + "," + QString::number(this->height) + ")";
        return errret;
    }

    QStringList datasets;
    for (qint32 i = 0; i < this->dataSets.size(); i++){
        QStringList ds;
        for (qint32 j = 0; j < this->dataSets.at(i).size(); j++){
            ds << QString::number(this->dataSets.at(i).at(j)) ;
        }
        datasets << "[" + ds.join(",") + "]";
    }

    QString drawValuesOnBarsStr = "false";
    if (this->drawValuesOnBars) drawValuesOnBarsStr = "true";

    QStringList ans;
    ans << tab + "var " + varName + " = {";
    ans << tab + "   canvas: '" +  varName + "',";
    ans << tab + "   width: " +  QString::number(this->width) + ",";
    ans << tab + "   height: " +  QString::number(this->height) + ",";
    ans << tab + "   dataSet: [" + datasets.join(",") + "],";
    ans << tab + "   xtext: ['" + this->xtext.join("','") + "'],";
    ans << tab + "   colors: ['" + this->colors.join("','") + "'],";
    ans << tab + "   title: '" + this->title + "',";
    ans << tab + "   ylabel: '" + this->ylabel + "',";
    ans << tab + "   fontSize: " + QString::number(this->fontSize) + ",";
    if (this->legend.size() > 0){ // If the legend is not present, it is not a mistake as it is optional.
        ans << tab + "   legend: ['" + this->legend.join("', '") + "'],";
    }
    ans << tab + "   drawValueOnBars: " + drawValuesOnBarsStr;
    ans << tab + "}";
    ans << tab + "generateBarGraph(" + varName  + ")";

    return ans.join("\n");
}

void BarGrapher::addGraphToDo(BarGraphOptions bgo){
    graphs << bgo;
}

bool BarGrapher::createBarGraphHTML(const QString &fileName){
    error = "";

    if (graphs.size() == 0){
        error = "No graphs have been set";
        return false;
    }

    // Loading the resource
    QFile resFile(":/CommonClasses/DataAnalysis/BarGrapher/bargraph.html");
    if (!resFile.open(QFile::ReadOnly)){
        error = "Could not open the bargraph resource file for reading";
        return false;
    }
    QTextStream reader(&resFile);
    QString bargrapher = reader.readAll();
    resFile.close();


    QStringList canvas;
    QStringList doGraph;

    for (qint32 i = 0; i < graphs.size(); i++){
        QString id = "canvas" + QString::number(i);
        canvas << "    <canvas id='" + id + "'></canvas>";
        doGraph << graphs.at(i).generateJavaScriptObj(id,"   ");
    }

    bargrapher.replace(STR_CANVAS_IDS,canvas.join("\n"));
    bargrapher.replace(STR_RUN_CODE,doGraph.join("\n"));

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)){
        error = "Could not open file " + fileName + " for writing";
        return false;
    }

    QTextStream writer(&file);
    writer << bargrapher;
    file.close();

    return true;
}

