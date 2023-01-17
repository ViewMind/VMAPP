#include "gonogosphereparser.h"

const QString GoNoGoSphereParser::COLORS       = "colors";
const QString GoNoGoSphereParser::ORIGINS      = "origins";
const QString GoNoGoSphereParser::HAND_CONTROL = "hand_control";
const QString GoNoGoSphereParser::SPEED_LIMIT  = "speed_limits";

GoNoGoSphereParser::GoNoGoSphereParser() {

}

bool GoNoGoSphereParser::parseGoNoGoSphereStudy(const QString &contents){
    error = "";
    study_description.clear();

//    if ((enableLeftHand == enableRightHand) && (enableLeftHand == false)){
//        error = "Cannot configure study with both hands disabled";
//        return false;
//    }

    // We first parse the contents.
    QJsonParseError json_error;
    QJsonDocument doc = QJsonDocument::fromJson(contents.toUtf8(),&json_error);
    if (doc.isNull()){
        error = "Error parsing the JSON file description: " + json_error.errorString();
        return false;
    }
    QVariantMap data = doc.object().toVariantMap();

    if (!data.contains(JFIELD_SPHERES)){
        error = "JSON Description does not contain 'spheres' key";
        return false;
    }

    QVariantList colors;
    QVariantList origins;

    QVariantList list = data.value(JFIELD_SPHERES).toList();
    for (qint32 i = 0; i < list.size(); i++){
        QVariantMap sphere = list.at(i).toMap();
        QString color_code = sphere.value(JFIELD_COLOR).toString();
        qint32  origin     = sphere.value(JFIELD_ORIGIN).toInt();
        if ((color_code != "R") && (color_code != "G")){
            error = "Description contains unexpected color code: '" + color_code + "'";
            return false;
        }
        if ((origin < 0) || (origin >= N_ORIGINS)){
            error = "Description contains invalid origin value of: '" + QString::number(origin) + "'";
            return false;
        }
        colors << color_code;
        origins << origin;
    }

    study_description[GoNoGoSphereParser::COLORS] = colors;
    study_description[GoNoGoSphereParser::ORIGINS] = origins;

//    if (min_speed < max_speed){
//        error = "Specified minimum speed (" + QString::number(min_speed) + ") is lower than the specified maxiumum speed (" + QString::number(max_speed) + ")";
//        return false;
//    }

//    QVariantList speeds; speeds << min_speed << max_speed;
//    QVariantList hands; hands << enableLeftHand << enableRightHand;

//    study_description[GoNoGoSphereParser::HAND_CONTROL] = hands;
//    study_description[GoNoGoSphereParser::SPEED_LIMIT] = speeds;


    return true;

}

QVariantMap GoNoGoSphereParser::getStudyDescriptionPayloadContent() const {
    return study_description;
}

qint32 GoNoGoSphereParser::getSize() const {
    return static_cast<qint32>(study_description.value(GoNoGoSphereParser::COLORS).toList().size());
}

QString GoNoGoSphereParser::getError() const {
    return error;
}
