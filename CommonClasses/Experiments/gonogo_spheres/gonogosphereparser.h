#ifndef GONOGOSPHEREPARSER_H
#define GONOGOSPHEREPARSER_H

#include <QString>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
//#include "../../RenderServerClient/RenderServerPackets/RenderServerPacketNames.h"
#include "../../eyetracker_defines.h"

class GoNoGoSphereParser
{
public:

    static const QString COLORS;
    static const QString ORIGINS;
    static const QString HAND_CONTROL;
    static const QString SPEED_LIMIT;

    GoNoGoSphereParser();

    bool parseGoNoGoSphereStudy(const QString& contents);
    QVariantMap getStudyDescriptionPayloadContent() const;
    QString getError() const;
    qint32 getSize() const;


private:
    QString error;
    QVariantMap study_description;

    const QString JFIELD_SPHERES = "spheres";
    const QString JFIELD_COLOR   = "color";
    const QString JFIELD_ORIGIN  = "origin";

    const qint32 N_ORIGINS = 9;

};

#endif // GONOGOSPHEREPARSER_H
