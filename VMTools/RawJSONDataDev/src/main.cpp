#include <QCoreApplication>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>

#include "../../../CommonClasses/RawDataContainer/datfiletorawdatacontainer.h"
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    RawDataContainer rdc;
    RawDataContainer::StudyConfiguration studyConfiguration;
    studyConfiguration.insert(RawDataContainer::SCP_EYES,RawDataContainer::SCV_EYE_BOTH);
    studyConfiguration.insert(RawDataContainer::SCP_LANGUAGE,RawDataContainer::SCV_LANG_EN);
    if (rdc.setStudy(RawDataContainer::STUDY_READING,studyConfiguration)){
        std::cout << rdc.getJSONString().toStdString() << std::endl;
    }
    else{
        std::cout << "There was an error " << rdc.getError().toStdString() << std::endl;
    }


    return a.exec();
}
