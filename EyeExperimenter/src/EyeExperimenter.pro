QT += quick gui widgets network
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES += USE_IVIEW

CONFIG -= debug_and_release
MOC_DIR = MOCS
OBJECTS_DIR = OBJS
RCC_DIR = QRC

RESOURCES += qml.qrc \
    fonts.qrc \
    images.qrc \
    languages.qrc \
    expdata.qrc \
    demo_data.qrc \
    report_text.qrc \
    agreements.qrc

SOURCES += main.cpp \
    loader.cpp \
    ../../CommonClasses/ConfigurationManager/configurationmanager.cpp \
    ../../CommonClasses/Experiments/bindingmanager.cpp \
    ../../CommonClasses/Experiments/experimentdatapainter.cpp \
    ../../CommonClasses/Experiments/fieldingmanager.cpp \
    ../../CommonClasses/Experiments/readingmanager.cpp \
    ../../CommonClasses/LogInterface/loginterface.cpp \
    EyeTrackerInterface/eyetrackerdata.cpp \
    EyeTrackerInterface/eyetrackerinterface.cpp \
    EyeTrackerInterface/Mouse/calibrationarea.cpp \
    EyeTrackerInterface/Mouse/mouseinterface.cpp \
    flowcontrol.cpp \
    Experiments/experiment.cpp \
    Experiments/fieldingexperiment.cpp \
    Experiments/imageexperiment.cpp \
    Experiments/readingexperiment.cpp \
    monitorscreen.cpp \
    ../../CommonClasses/DataPacket/datapacket.cpp \
    EyeTrackerInterface/GazePoint/eventdetection.cpp \
    EyeTrackerInterface/GazePoint/opengazecommand.cpp \
    EyeTrackerInterface/GazePoint/opengazeinterface.cpp \
    ../../CommonClasses/PNGWriter/imagereportdrawer.cpp \
    sslclient/ssldataprocessingclient.cpp \
    sslclient/sslclient.cpp \
    sslclient/ssldbclient.cpp \
    ../../CommonClasses/Experiments/bindingparser.cpp \
    ../../CommonClasses/DatFileInfo/datfileinfoindir.cpp \
    ../../CommonClasses/PNGWriter/repfileinfo.cpp \
    ../../CommonClasses/PNGWriter/resultbar.cpp \
    ../../CommonClasses/LocalInformationManager/localinformationmanager.cpp \
    ../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.cpp \
    ../../CommonClasses/Experiments/readingparser.cpp \
    uiconfigmap.cpp

HEADERS += \
    loader.h \
    ../../CommonClasses/ConfigurationManager/configurationmanager.h \
    ../../CommonClasses/common.h \
    ../../CommonClasses/common.h \
    ../../CommonClasses/Experiments/bindingmanager.h \
    ../../CommonClasses/Experiments/experimentdatapainter.h \
    ../../CommonClasses/Experiments/fieldingmanager.h \
    ../../CommonClasses/Experiments/readingmanager.h \
    eye_experimenter_defines.h \
    ../../CommonClasses/common.h \
    ../../CommonClasses/LogInterface/loginterface.h \
    EyeTrackerInterface/eyetrackerdata.h \
    EyeTrackerInterface/eyetrackerinterface.h \
    EyeTrackerInterface/Mouse/calibrationarea.h \
    EyeTrackerInterface/Mouse/mouseinterface.h \
    flowcontrol.h \
    Experiments/experiment.h \
    Experiments/fieldingexperiment.h \
    Experiments/imageexperiment.h \
    Experiments/readingexperiment.h \
    monitorscreen.h \
    ../../CommonClasses/DataPacket/datapacket.h \
    EyeTrackerInterface/GazePoint/eventdetection.h \
    EyeTrackerInterface/GazePoint/opengazecommand.h \
    EyeTrackerInterface/GazePoint/opengazeinterface.h \
    ../../CommonClasses/PNGWriter/imagereportdrawer.h \
    countries.h \
    ../../CommonClasses/SQLConn/dbdescription.h \
    sslclient/sslclient.h \
    sslclient/ssldataprocessingclient.h \
    sslclient/ssldbclient.h \
    ../../CommonClasses/Experiments/bindingparser.h \
    ../../CommonClasses/DatFileInfo/datfileinfoindir.h \
    ../../CommonClasses/PNGWriter/repfileinfo.h \
    ../../CommonClasses/PNGWriter/resultbar.h \
    ../../CommonClasses/LocalInformationManager/localinformationmanager.h \
    ../../CommonClasses/DataAnalysis/FrequencyAnalsis/freqanalysis.h \
    ../../CommonClasses/Experiments/readingparser.h \
    uiconfigmap.h

contains(DEFINES, USE_IVIEW) {
  LIBS += -L$$PWD/EyeTrackerInterface/RED/ -liViewXAPI
  INCLUDEPATH += $$PWD/EyeTrackerInterface/RED
  DEPENDPATH += $$PWD/EyeTrackerInterface/RED
  SOURCES += EyeTrackerInterface/RED/redinterface.cpp
  HEADERS += EyeTrackerInterface/RED/iViewXAPI.h \
             EyeTrackerInterface/RED/redinterface.h

}


